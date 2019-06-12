//cpp dependencies
#include <iostream>
#include <string>

//ROOT dependencies
#include "TDatime.h"
#include "TDirectoryFile.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TNamed.h"
#include "TTree.h"

//PYTHIA8 dependencies
#include "Pythia8/Pythia.h"

//Local dependencies
#include "include/checkMakeDir.h"
#include "include/pdgToMassInGeV.h"

int advancedPYTHIA(const int nEvt, const double comE = 5020., bool doPthatWeight = false, const double weightPower = 4.5, const double pthatMin = 15., const double pthatMax = 10000., const int seed = 0)
{
  //Grab todays date and create some quick output directories
  TDatime* date = new TDatime();
  const std::string dateStr = std::to_string(date->GetDate());
  delete date;

  checkMakeDir("output");
  checkMakeDir("output/" + dateStr);

  //declare class to grab pdg mass of particles;
  pdgToMassInGeV pdgToM;

  //define a few kinematic cuts for event content appropriate to CMS
  const Double_t minPartPt = 0.5;
  const Double_t maxPartAbsEta = 5.1;
  
  //Open our output file and create the particle ttree we will write to
  TFile* outFile_p = new TFile(("output/" + dateStr + "/advancedPYTHIA8_Seed" + std::to_string(seed) + "_" + dateStr + ".root").c_str(), "RECREATE");
  TTree* particleTree_p = new TTree("particleTree", "");
  TDirectory* dir_p = outFile_p->mkdir("paramDir");
  
  Float_t weight_ = 1.;
  Float_t pthat_;
  
  //Use arrays - vectors introduce ambiguities with which variable is linked to which from ttree output
  const Int_t nQG = 2;
  Float_t qgPt_[nQG];
  Float_t qgEta_[nQG];
  Float_t qgPhi_[nQG];
  Float_t qgM_[nQG];
  Int_t qgPDG_[nQG];

  const Int_t nMaxParticles = 1000;
  Int_t nPart_;
  Float_t pt_[nMaxParticles];
  Float_t phi_[nMaxParticles];
  Float_t eta_[nMaxParticles];
  Float_t m_[nMaxParticles];
  Int_t pdg_[nMaxParticles];

  particleTree_p->Branch("weight", &weight_, "weight/F");
  particleTree_p->Branch("pthat", &pthat_, "pthat/F");

  particleTree_p->Branch("qgPt", qgPt_, ("qgPt[" + std::to_string(nQG) + "]/F").c_str());
  particleTree_p->Branch("qgPhi", qgPhi_, ("qgPhi[" + std::to_string(nQG) + "]/F").c_str());
  particleTree_p->Branch("qgEta", qgEta_, ("qgEta[" + std::to_string(nQG) + "]/F").c_str());
  particleTree_p->Branch("qgM", qgM_, ("qgM[" + std::to_string(nQG) + "]/F").c_str());
  particleTree_p->Branch("qgPDG", qgPDG_, ("qgPDG[" + std::to_string(nQG) + "]/I").c_str());

  particleTree_p->Branch("nPart", &nPart_, "nPart/I");
  particleTree_p->Branch("pt", pt_, "pt[nPart]/F");
  particleTree_p->Branch("phi", phi_, "phi[nPart]/F");
  particleTree_p->Branch("eta", eta_, "eta[nPart]/F");
  particleTree_p->Branch("m", m_, "m[nPart]/F");
  particleTree_p->Branch("pdg", pdg_, "pdf[nPart]/I");

  //Lets define PYTHIA8 generator
  Pythia8::Pythia pythia;
  pythia.readString(("Beams:eCM = " + std::to_string(comE) + "").c_str());//COM is in GeV so this is matched to PbPb Run2
  pythia.readString("HardQCD:all = on");//Define processes, this is default hard qcd physics, what you would run for inclusive jet analysis (think 'everything')
  pythia.readString("Random:setSeed = on");//We will set the seed defining our pseudo-random number generator
  pythia.readString(("Random:seed = " + std::to_string(seed)).c_str());//Seed 0 gives unique random number tied to current time

  pythia.readString(("PhaseSpace:pTHatMin = " + std::to_string(pthatMin)).c_str()); // set the minimum scale of the hardscattering in GeV - here pick 80 just for quick jetphysics testing
  pythia.readString(("PhaseSpace:pTHatMax = " + std::to_string(pthatMax)).c_str()); //set the maximum scale of the hardscattering in GeV

  if(doPthatWeight){
    pythia.readString("PhaseSpace:bias2Selection = on");
    pythia.readString("PhaseSpace:bias2SelectionPow = " + std::to_string(weightPower));
    pythia.readString("PhaseSpace:bias2SelectionRef = " + std::to_string(pthatMin));
  }
  
  pythia.init(); // with all parameters set, initialize pythia
  
  //Loop to run PYTHIA and fill TTree
  const Int_t nDiv = nEvt/20; //setup to do printouts per 5% progress

  std::cout << "Running PYTHIA for " << nEvt << " events..." << std::endl;
  while(particleTree_p->GetEntries() < nEvt){
    if(!pythia.next()) continue; 

    pthat_ = pythia.info.pTHat(); // grab pthat
    if(doPthatWeight) weight_ = pythia.info.weight();
    nPart_ = 0; // initialize number of particles

    //Lets grab the initial hard scattering
    TLorentzVector tL;
    for(Int_t qI = 0; qI < nQG; ++qI){
      tL.SetPxPyPzE(pythia.event[qI+5].px(), pythia.event[qI+5].py(), pythia.event[qI+5].pz(), pythia.event[qI+5].e());

      qgPt_[qI] = tL.Pt();
      qgPhi_[qI] = tL.Phi();
      qgEta_[qI] = tL.Eta();
      qgM_[qI] = tL.M();
      qgPDG_[qI] = pythia.event[qI+5].id();
    }

    for(int i = 0; i < pythia.event.size(); ++i){//iterate over particles
      if(!pythia.event[i].isFinal()) continue; // only take stable final state particles
      if(pythia.event[i].pT() < minPartPt) continue; // assuming gev
      if(TMath::Abs(pythia.event[i].eta()) > maxPartAbsEta) continue; // cut assuming rough detector geometry     

      //We will skip the neutrinos as we really have no chance of working with them at colliders
      if(TMath::Abs(pythia.event[i].id()) == 12) continue;
      if(TMath::Abs(pythia.event[i].id()) == 14) continue;
      if(TMath::Abs(pythia.event[i].id()) == 16) continue;

      pt_[nPart_] = pythia.event[i].pT();
      phi_[nPart_] = pythia.event[i].phi();
      eta_[nPart_] = pythia.event[i].eta();
      pdg_[nPart_] = pythia.event[i].id();
      m_[nPart_] = pdgToM.getMassFromID(TMath::Abs(pythia.event[i].id()));

      ++nPart_;
    }

    //silly to keep an empty event, so lets just ignore those
    if(nPart_ == 0) continue;

    if(particleTree_p->GetEntries()%nDiv == 0) std::cout << " Event " << particleTree_p->GetEntries() << "/" << nEvt << std::endl;
    
    particleTree_p->Fill();
  }
    
  //Lets write the ttree to file and cleanup/close
  outFile_p->cd();

  particleTree_p->Write("", TObject::kOverwrite);
  delete particleTree_p;

  dir_p->cd();
  TNamed nEvtName("nEvt", std::to_string(nEvt).c_str());
  nEvtName.Write("", TObject::kOverwrite);
  TNamed comEName("comE", std::to_string(comE).c_str());
  comEName.Write("", TObject::kOverwrite);
  TNamed doPthatWeightName("doPthatWeight", std::to_string(doPthatWeight).c_str());
  doPthatWeightName.Write("", TObject::kOverwrite);
  TNamed weightPowerName("weightPower", std::to_string(weightPower).c_str());
  weightPowerName.Write("", TObject::kOverwrite);
  TNamed pthatMinName("pthatMin", std::to_string(pthatMin).c_str());
  pthatMinName.Write("", TObject::kOverwrite);
  TNamed pthatMaxName("pthatMax", std::to_string(pthatMax).c_str());
  pthatMaxName.Write("", TObject::kOverwrite);
  TNamed seedName("seed", std::to_string(seed).c_str());
  seedName.Write("", TObject::kOverwrite);

  
  outFile_p->Close();
  delete outFile_p;
  
  return 0;
}

int main(int argc, char* argv[])
{
  if(argc > 8 || argc < 2){
    std::cout << "Usage: ./bin/advancedPYTHIA.exe <nEvt> <comE-default5020> <doPthatWeight-defaultFalse> <weightPower-default4.5> <pthatMin-default15> <pthatMax-default10000> <seed-default0>" << std::endl;
    return 1;
  }

  
  int retVal = 0;

  if(argc == 2) retVal += advancedPYTHIA(std::stoi(argv[1]));
  else if(argc == 3) retVal += advancedPYTHIA(std::stoi(argv[1]), std::stod(argv[2]));
  else if(argc == 4) retVal += advancedPYTHIA(std::stoi(argv[1]), std::stod(argv[2]), std::stoi(argv[3]));
  else if(argc == 5) retVal += advancedPYTHIA(std::stoi(argv[1]), std::stod(argv[2]), std::stoi(argv[3]), std::stod(argv[4]));
  else if(argc == 6) retVal += advancedPYTHIA(std::stoi(argv[1]), std::stod(argv[2]), std::stoi(argv[3]), std::stod(argv[4]), std::stod(argv[5]));
  else if(argc == 7) retVal += advancedPYTHIA(std::stoi(argv[1]), std::stod(argv[2]), std::stoi(argv[3]), std::stod(argv[4]), std::stod(argv[5]), std::stod(argv[6]));
  else if(argc == 8) retVal += advancedPYTHIA(std::stoi(argv[1]), std::stod(argv[2]), std::stoi(argv[3]), std::stod(argv[4]), std::stod(argv[5]), std::stod(argv[6]), std::stoi(argv[7]));

  return retVal;
}
