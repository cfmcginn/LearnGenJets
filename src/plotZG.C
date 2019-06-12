//cpp dependencies
#include <iostream>
#include <string>

//ROOT dependencies
#include "TCanvas.h"
#include "TDatime.h"
#include "TFile.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TTree.h"

//Local dependencies
#include "include/checkMakeDir.h"

void defineCanv(TCanvas* canv_p)
{
  canv_p->SetTopMargin(0.01);
  canv_p->SetRightMargin(0.01);
  canv_p->SetLeftMargin(0.1);
  canv_p->SetBottomMargin(0.1);
  
  return;
}

int plotZg(const std::string inFileName)
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

  //make sure our plot outdir exists
  checkMakeDir("pdfDir");
  checkMakeDir("pdfDir/" + dateStr);

  //Grab the input file and jet tree
  TFile* inFile_p = new TFile(inFileName.c_str(), "READ");
  TTree* jetTree_p = (TTree*)inFile_p->Get("jetTree");

  Float_t weight_;
  
  //Use arrays - vectors introduce ambiguities with which variable is linked to which from ttree output
  const Int_t nMaxJets = 500;
  Int_t nref_;
  Float_t jtpt_[nMaxJets];
  Float_t jteta_[nMaxJets];
  Int_t jtflavor_[nMaxJets];

  Float_t zg_[nMaxJets];
  Float_t rg_[nMaxJets];
  
  jetTree_p->SetBranchStatus("*", 0);

  jetTree_p->SetBranchStatus("weight", 1);
  jetTree_p->SetBranchStatus("nref", 1);
  jetTree_p->SetBranchStatus("jtpt", 1);
  jetTree_p->SetBranchStatus("jteta", 1);
  jetTree_p->SetBranchStatus("jtflavor", 1);
  jetTree_p->SetBranchStatus("zg", 1);
  jetTree_p->SetBranchStatus("rg", 1);

  jetTree_p->SetBranchAddress("weight", &weight_);
  jetTree_p->SetBranchAddress("nref", &nref_);
  jetTree_p->SetBranchAddress("jtpt", jtpt_);
  jetTree_p->SetBranchAddress("jteta", jteta_);
  jetTree_p->SetBranchAddress("jtflavor", jtflavor_);
  jetTree_p->SetBranchAddress("zg", zg_);
  jetTree_p->SetBranchAddress("rg", rg_);

  const Int_t nQG = 2;
  const std::string qgStr[nQG+1] = {"Inclusive", "Quark", "Gluon"};

  //Define some quick cuts
  const Double_t absEtaMax = 2.;
  const Double_t jtPtMin = 100.;

  bool doWeighted = jetTree_p->GetMaximum("weight") > 1.01 || jetTree_p->GetMinimum("weight") < 0.99;
  
  //Define zg, rg histograms;    
  TH1D* zg_h[nQG+1];
  TH1D* rg_h[nQG+1];

  for(Int_t qI = 0; qI < nQG+1; ++qI){
    zg_h[qI] = new TH1D(("zg_" + qgStr[qI] + "_h").c_str(), ";z_{g};Counts", 20, 0.0, 0.5);
    rg_h[qI] = new TH1D(("rg_" + qgStr[qI] + "_h").c_str(), ";r_{g};Counts", 20, 0.0, 0.4);

    if(doWeighted){
      zg_h[qI]->Sumw2();
      rg_h[qI]->Sumw2();
    }
  }
  
  const Int_t nEntries = jetTree_p->GetEntries();
  const Int_t nDiv = nEntries/20;

  std::cout << "Running plott over " << nEntries << " events..." << std::endl;
  for(Int_t entry = 0; entry < nEntries; ++entry){
    if(entry%nDiv == 0) std::cout << " Entry " << entry << "/" << nEntries << std::endl;
    jetTree_p->GetEntry(entry);

    //Lets loop over jets
    for(Int_t jI = 0; jI < nref_; ++jI){
      if(jtPtMin > jtpt_[jI]) continue;
      if(absEtaMax < TMath::Abs(jteta_[jI])) continue;
      if(zg_[jI] < 0) continue;

      if(doWeighted){
	zg_h[0]->Fill(zg_[jI], weight_);
	rg_h[0]->Fill(rg_[jI], weight_);

	if(TMath::Abs(jtflavor_[jI]) < 10){
	  zg_h[1]->Fill(zg_[jI], weight_);
	  rg_h[1]->Fill(rg_[jI], weight_);
	}
	else if(TMath::Abs(jtflavor_[jI]) == 21){
	  zg_h[2]->Fill(zg_[jI], weight_);
	  rg_h[2]->Fill(rg_[jI], weight_);
	}
      }
      else{
	zg_h[0]->Fill(zg_[jI]);
	rg_h[0]->Fill(rg_[jI]);

      	if(TMath::Abs(jtflavor_[jI]) < 10){
	  zg_h[1]->Fill(zg_[jI]);
	  rg_h[1]->Fill(rg_[jI]);
	}
	else if(TMath::Abs(jtflavor_[jI]) == 21){
	  zg_h[2]->Fill(zg_[jI]);
	  rg_h[2]->Fill(rg_[jI]);
	}
      }
    }
  }

  TLegend* leg_p = new TLegend(0.7, 0.7, 0.95, 0.95);
  leg_p->SetBorderSize(0);
  leg_p->SetFillColor(0);
  leg_p->SetFillStyle(0);
  leg_p->SetTextFont(43);
  leg_p->SetTextSize(12);

  //plot zg
  TCanvas* canv_p = new TCanvas("canv_p", "canv_p", 450, 450);
  defineCanv(canv_p);

  zg_h[0]->SetMarkerColor(1);
  zg_h[0]->SetLineColor(1);
  zg_h[0]->SetMarkerSize(1);
  zg_h[0]->SetMarkerStyle(20);

  zg_h[1]->SetMarkerColor(4);
  zg_h[1]->SetLineColor(1);
  zg_h[1]->SetMarkerSize(1);
  zg_h[1]->SetMarkerStyle(24);

  zg_h[2]->SetMarkerColor(2);
  zg_h[2]->SetLineColor(1);
  zg_h[2]->SetMarkerSize(1);
  zg_h[2]->SetMarkerStyle(25);

  zg_h[0]->GetXaxis()->CenterTitle();
  zg_h[0]->GetYaxis()->CenterTitle();
    
  zg_h[0]->DrawCopy("HIST E1");
  zg_h[1]->DrawCopy("HIST E1 SAME");
  zg_h[2]->DrawCopy("HIST E1 SAME");

  leg_p->AddEntry(zg_h[0], "Inclusive", "P L");
  leg_p->AddEntry(zg_h[1], "Quark", "P L");
  leg_p->AddEntry(zg_h[2], "Gluon", "P L");
  
  gStyle->SetOptStat(0);

  leg_p->Draw("SAME");
  
  canv_p->SaveAs(("pdfDir/" + dateStr + "/zg_" + dateStr + ".pdf").c_str());
  delete canv_p;

  //plot rg
  canv_p = new TCanvas("canv_p", "canv_p", 450, 450);
  defineCanv(canv_p);

  rg_h[0]->SetMarkerColor(1);
  rg_h[0]->SetLineColor(1);
  rg_h[0]->SetMarkerSize(1);
  rg_h[0]->SetMarkerStyle(20);

  rg_h[1]->SetMarkerColor(4);
  rg_h[1]->SetLineColor(1);
  rg_h[1]->SetMarkerSize(1);
  rg_h[1]->SetMarkerStyle(24);

  rg_h[2]->SetMarkerColor(2);
  rg_h[2]->SetLineColor(1);
  rg_h[2]->SetMarkerSize(1);
  rg_h[2]->SetMarkerStyle(25);

  rg_h[0]->GetXaxis()->CenterTitle();
  rg_h[0]->GetYaxis()->CenterTitle();

  rg_h[0]->DrawCopy("HIST E1");
  rg_h[1]->DrawCopy("HIST E1 SAME");
  rg_h[2]->DrawCopy("HIST E1 SAME");

  gStyle->SetOptStat(0);

  leg_p->Draw("SAME");

  canv_p->SaveAs(("pdfDir/" + dateStr + "/rg_" + dateStr + ".pdf").c_str());
  delete canv_p;

  delete leg_p;
  
  //cleanup hist
  for(Int_t qI = 0; qI < nQG+1; ++qI){
    delete zg_h[qI];
    delete rg_h[qI];
  }
  
  //Cleanup inputs
  inFile_p->Close();
  delete inFile_p;
    
  return 0;
}

int main(int argc, char* argv[])
{
  if(argc != 2){
    std::cout << "Usage: ./bin/plotZg.exe <inFileName>. return 1" << std::endl;
    return 1;
  }
  
  int retVal = 0;
  retVal += plotZg(argv[1]);
  return retVal;
}
