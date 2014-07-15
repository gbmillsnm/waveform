// Geoff Mills   July 2014
// Modified by Alex Mills 7.12.2014

#include "TStyle.h"
#include "TString.h"
#include "TString.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH2.h"
#include <cstdio>
#include <iostream>

void DrawEvent(Int_t run = 114, Int_t event = 0, int ld = 1)
{
  char cRun[100];
  sprintf(cRun,"Run%d",run);

  char cEvent[100];
  sprintf(cEvent,"Event%d",event);

  TString fileName = cRun;
  fileName.Append("Histograms.root");

  TString hName = cRun;
  hName.Append(cEvent);

  TString cName = cRun;

  char cTitle[100];
  sprintf(cTitle,"Run %i, Line Drivers %i & %i, Event %i: 7.11.2014",run,ld,ld+1,event);

  TString jpgFile = hName.Data();
  jpgFile.Append(".jpg");

  TFile fRun(fileName.Data());

  TH2F * hRun0 = (TH2F *) fRun.Get(hName.Data());

  TH2F * hRun = new TH2F(*hRun0);

  TCanvas * cEventDisplay = new TCanvas(cName.Data(),cTitle,16,27,766,672);

  gStyle->SetOptStat(0);
  gStyle->SetPalette(1);

  cEventDisplay->SetFillColor(0);
  cEventDisplay->SetBorderMode(0);
  cEventDisplay->SetBorderSize(2);
  cEventDisplay->SetTheta(48.30508);
  cEventDisplay->SetPhi(52.5);
  cEventDisplay->SetFrameBorderMode(0);

  Float_t textSize = 0.025;
  Float_t hMin = 1550;
  Float_t hMax = 2450;
  Float_t tLo = 1;
  Float_t tHi = 8395;
  Float_t titleOffset = 2; 

  hRun->GetXaxis()->SetTitle("Channel");
  hRun->GetXaxis()->SetLabelFont(42);
  hRun->GetXaxis()->SetLabelSize(textSize);
  hRun->GetXaxis()->SetTitleSize(textSize);
  hRun->GetXaxis()->SetTitleOffset(titleOffset);
  hRun->GetXaxis()->SetTitleFont(42);
  hRun->GetYaxis()->SetTitle("Time Stamp");
  hRun->GetYaxis()->SetRange(tLo,tHi);
  hRun->GetYaxis()->SetLabelFont(42);
  hRun->GetYaxis()->SetLabelSize(textSize);
  hRun->GetYaxis()->SetTitleSize(textSize);
  hRun->GetYaxis()->SetTitleOffset(titleOffset);
  hRun->GetYaxis()->SetTitleFont(42);
  hRun->GetZaxis()->SetTitle("ADC Value");
  hRun->GetZaxis()->SetLabelFont(42);
  hRun->GetZaxis()->SetLabelSize(textSize);
  hRun->GetZaxis()->SetTitleSize(textSize);
  hRun->GetZaxis()->SetTitleOffset(titleOffset);
  hRun->GetZaxis()->SetTitleFont(42);
  hRun->SetTitle(cTitle);
  hRun->SetMinimum(hMin);
  hRun->SetMaximum(hMax);
  hRun->Draw("surf1");

  cEventDisplay->Update();
  cEventDisplay->Print(jpgFile.Data());
  cEventDisplay->Close("");

//   cout << "done? (y)" ;
//   char temp[100];
//   cin >> temp;
//   delete cEventDisplay;
}
