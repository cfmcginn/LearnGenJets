//cpp dependencies
#include <iostream>
#include <string>
#include <vector>

//ROOT dependencies
#include "TDatime.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"

//FastJet dependencies
#include "include/fastjet/PseudoJet.hh"
#include "include/fastjet/ClusterSequence.hh"

#include "fastjet/contrib/SoftDrop.hh"

//Local dependencies
#include "include/checkMakeDir.h"

int simpleFastjetWithZg(const std::string inFileName)
{
  //Make sure input is valid root file
  if(!checkFile(inFileName) || inFileName.find(".root") == std::string::npos){
    std::cout << "Given input file \'" << inFileName << "\' is invalid. return 1" << std::endl;
    return 1;
  }

  //Grab todays date and create some quick output directories
  TDatime* date = new TDatime();
  const std::string dateStr = std::to_string(date->GetDate());
  delete date;  
  
  checkMakeDir("output");
  checkMakeDir("output/" + dateStr);

  //Lets base our output file name on input file name so dependencies are clearer
  std::string outFileName = inFileName.substr(0, inFileName.find(".root"));
  while(outFileName.find("/") != std::string::npos){outFileName.replace(0, outFileName.find("/")+1, "");}
  outFileName = "output/" + dateStr + "/" + outFileName + "_JETWITHZG_" + dateStr + ".root";

  
  //Open our output file and create the particle ttree we will write to
  TFile* outFile_p = new TFile(outFileName.c_str(), "RECREATE");
  TTree* jetTree_p = new TTree("jetTree", "");

  Float_t weight_;
  Float_t pthat_;
  
  //Use arrays - vectors introduce ambiguities with which variable is linked to which from ttree output
  const Int_t nMaxJets = 500;
  Int_t nref_;
  Float_t jtpt_[nMaxJets];
  Float_t jtphi_[nMaxJets];
  Float_t jteta_[nMaxJets];

  //Add some softdrop variables for the ttree
  Float_t jtptSD_[nMaxJets];
  Float_t jtphiSD_[nMaxJets];
  Float_t jtetaSD_[nMaxJets];

  Float_t zg_[nMaxJets];
  Float_t rg_[nMaxJets];
  
  jetTree_p->Branch("weight", &weight_, "weight/F");
  jetTree_p->Branch("pthat", &pthat_, "pthat/F");
  jetTree_p->Branch("nref", &nref_, "nref/I");
  jetTree_p->Branch("jtpt", jtpt_, "jtpt[nref]/F");
  jetTree_p->Branch("jtphi", jtphi_, "jtphi[nref]/F");
  jetTree_p->Branch("jteta", jteta_, "jteta[nref]/F");

  jetTree_p->Branch("jtptSD", jtptSD_, "jtptSD[nref]/F");
  jetTree_p->Branch("jtphiSD", jtphiSD_, "jtphiSD[nref]/F");
  jetTree_p->Branch("jtetaSD", jtetaSD_, "jtetaSD[nref]/F");

  jetTree_p->Branch("zg", zg_, "zg[nref]/F");
  jetTree_p->Branch("rg", rg_, "rg[nref]/F");

  //Now grab the input
  TFile* inFile_p = new TFile(inFileName.c_str(), "READ");
  TTree* particleTree_p = (TTree*)inFile_p->Get("particleTree");
  
  const Int_t nMaxParticles = 1000;
  Int_t nPart_;
  Float_t pt_[nMaxParticles];
  Float_t phi_[nMaxParticles];
  Float_t eta_[nMaxParticles];
  Float_t m_[nMaxParticles];

  //Turn off all branches then turn on the needed ones
  particleTree_p->SetBranchStatus("*", 0);
  particleTree_p->SetBranchStatus("weight", 1);
  particleTree_p->SetBranchStatus("pthat", 1);
  particleTree_p->SetBranchStatus("nPart", 1);
  particleTree_p->SetBranchStatus("pt", 1);
  particleTree_p->SetBranchStatus("phi", 1);
  particleTree_p->SetBranchStatus("eta", 1);
  particleTree_p->SetBranchStatus("m", 1);

  //Set address for needed branches
  particleTree_p->SetBranchAddress("weight", &weight_);
  particleTree_p->SetBranchAddress("pthat", &pthat_);
  particleTree_p->SetBranchAddress("nPart", &nPart_);
  particleTree_p->SetBranchAddress("pt", pt_);
  particleTree_p->SetBranchAddress("phi", phi_);
  particleTree_p->SetBranchAddress("eta", eta_);
  particleTree_p->SetBranchAddress("m", m_);

  const double rParam = 0.4; // choice of clustering resolution, typical QCD jet choice is 0.4, boosted heavy boson is 0.8
  fastjet::JetDefinition jet_defE(fastjet::antikt_algorithm, rParam, fastjet::E_scheme); //define our jets, antikt clustering w/ R=0.4, EScheme recombination (i.e. four vector sums). for details see fastjet manual. this set of choices is most common  

  //Define softdrop object
  const Double_t zgBeta = 0.0;
  const Double_t zgCut = 0.1;
  fastjet::contrib::SoftDrop sd(zgBeta, zgCut);
  
  
  const Int_t nEntries = particleTree_p->GetEntries();
  const Int_t nDiv = nEntries/20;

  std::cout << "Running jet clustering over " << nEntries << " events..." << std::endl;
  for(Int_t entry = 0; entry < nEntries; ++entry){
    if(entry%nDiv == 0) std::cout << " Entry " << entry << "/" << nEntries << std::endl;
    particleTree_p->GetEntry(entry);

    std::vector<fastjet::PseudoJet> particles; // We will create a vector of pseudojets from particles as clustering inputs
    TLorentzVector tL; //I use TL to do ptetaphim -> px py pz E, but whatever works for you

    //loop over particles and create input for jet clustering
    for(Int_t pI = 0; pI < nPart_; ++pI){
      tL.SetPtEtaPhiM(pt_[pI], eta_[pI], phi_[pI], m_[pI]);
      particles.push_back(fastjet::PseudoJet(tL.Px(), tL.Py(), tL.Pz(), tL.E()));
    }

    //lets define cluster sequence
    fastjet::ClusterSequence csE(particles, jet_defE);
    //grab inclusive jets sorted by pt from cluster sequence.
    std::vector<fastjet::PseudoJet> jetsE = fastjet::sorted_by_pt(csE.inclusive_jets());
    
    
    //Now lets fill our jet tree variables
    nref_ = 0;
    for(unsigned int jI = 0; jI < jetsE.size(); ++jI){
      jtpt_[nref_] = jetsE[jI].pt();
      jtphi_[nref_] = jetsE[jI].phi_std();
      jteta_[nref_] = jetsE[jI].eta();      

      //Adding softdrop processing
      fastjet::PseudoJet sdJet = sd(jetsE[jI]);

      jtptSD_[nref_] = sdJet.pt();
      jtphiSD_[nref_] = sdJet.phi_std();
      jtetaSD_[nref_] = sdJet.eta();

      zg_[nref_] = sdJet.structure_of<fastjet::contrib::SoftDrop>().symmetry();
      rg_[nref_] = sdJet.structure_of<fastjet::contrib::SoftDrop>().delta_R();

      ++nref_;
    }
        
    jetTree_p->Fill();
  }

  //Cleanup inputs
  inFile_p->Close();
  delete inFile_p;
  
  //Lets write the ttree to file and cleanup/close
  outFile_p->cd();

  jetTree_p->Write("", TObject::kOverwrite);
  delete jetTree_p;

  outFile_p->Close();
  delete outFile_p;
  
  return 0;
}

int main(int argc, char* argv[])
{
  if(argc != 2){
    std::cout << "Usage: ./bin/simpleFastjetWithZg.exe <inFileName>. return 1" << std::endl;
    return 1;
  }
  
  int retVal = 0;
  retVal += simpleFastjetWithZg(argv[1]);
  return retVal;
}
