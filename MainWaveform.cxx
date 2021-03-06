
////////////////////////////////////////////////////////////////////////
//
//             A simple example with a ROOT tree
//             =================================
//
//
////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "Riostream.h"
#include "TROOT.h"
#include "TFile.h"
#include "TNetFile.h"
#include "TRandom.h"
#include "TTree.h"
#include "TBranch.h"
#include "TClonesArray.h"
#include "TStopwatch.h"

#include "Waveform.h"
#include "HistogramManager.h"


//______________________________________________________________________________
int main(int argc, char **argv)
{
  Int_t run    = 1;        // run number
  Int_t nwaveform = 400;     // by default create 400 waveforms
  Int_t comp   = 1;       // by default file is compressed
  Int_t split  = 1;       // by default, split Waveform in sub branches
  Int_t write  = 1;       // by default the tree is filled
  Int_t hfill  = 0;       // by default histograms are not filled
  Int_t read   = 0;
  Int_t arg4   = 1;
  Int_t arg5   = 600;     //default number of tracks per waveform
  Int_t netf   = 0;
  Int_t punzip = 0;

  if (argc > 1)  nwaveform = atoi(argv[1]);
  if (argc > 2)  comp   = atoi(argv[2]);
  if (argc > 3)  split  = atoi(argv[3]);
  if (argc > 4)  arg4   = atoi(argv[4]);
  if (argc > 5)  arg5   = atoi(argv[5]);
  if (arg4 ==  0) { write = 0; hfill = 0; read = 1;}
  if (arg4 ==  1) { write = 1; hfill = 0;}
  if (arg4 ==  2) { write = 0; hfill = 0;}
  if (arg4 == 10) { write = 0; hfill = 1;}
  if (arg4 == 11) { write = 1; hfill = 1;}
  if (arg4 == 20) { write = 0; read  = 1;}  //read sequential
  if (arg4 == 21) { write = 0; read  = 1;  punzip = 1;}  //read sequential + parallel unzipping
  if (arg4 == 25) { write = 0; read  = 2;}  //read random
  if (arg4 >= 30) { netf  = 1; }            //use TNetFile
  if (arg4 == 30) { write = 0; read  = 1;}  //netfile + read sequential
  if (arg4 == 35) { write = 0; read  = 2;}  //netfile + read random
  if (arg4 == 36) { write = 1; }            //netfile + write sequential
  Int_t branchStyle = 1; //new style by default
  if (split < 0) {branchStyle = 0; split = -1-split;}

  TFile *hfile;
  TTree *tree;
  Waveform *waveform = 0;

  // Fill waveform, header and tracks with some random numbers
  //   Create a timer object to benchmark this loop
  TStopwatch timer;
  timer.Start();
  Long64_t nb = 0;
  Int_t ev;
  Int_t bufsize;
  Double_t told = 0;
  Double_t tnew = 0;
  Int_t printev = 100;
  if (arg5 < 100) printev = 1000;
  if (arg5 < 10)  printev = 10000;

  //         Read case
  if (read) {
    if (netf) {
      hfile = new TNetFile("root://localhost/root/test/WaveformNet.root");
    } else
      hfile = new TFile("Waveform.root");
    tree = (TTree*)hfile->Get("T");

    TBranch *branch = tree->GetBranch("waveform");

    branch->SetAddress(&waveform);
    Int_t nentries = (Int_t)tree->GetEntries();
    nwaveform = TMath::Min(nwaveform,nentries);

    if (read == 1) {  //read sequential
      //by setting the read cache to -1 we set it to the AutoFlush value when writing
      Int_t cachesize = -1; 
      if (punzip) tree->SetParallelUnzip();
      tree->SetCacheSize(cachesize);
      tree->SetCacheLearnEntries(1); //one entry is sufficient to learn
      tree->SetCacheEntryRange(0,nwaveform);
      for (ev = 0; ev < nwaveform; ev++) {
	tree->LoadTree(ev);  //this call is required when using the cache
	if (ev%printev == 0) {
	  tnew = timer.RealTime();
	  printf("waveform:%d, rtime=%f s\n",ev,tnew-told);
	  told=tnew;
	  timer.Continue();
	}
	nb += tree->GetEntry(ev);        //read complete waveform in memory
      }
    } else {    //read random
      Int_t evrandom;
      for (ev = 0; ev < nwaveform; ev++) {
	if (ev%printev == 0) cout<<"waveform="<<ev<<endl;
	evrandom = Int_t(nwaveform*gRandom->Rndm(1));
	nb += tree->GetEntry(evrandom);  //read complete waveform in memory
      }
    }
  } else {
    //         Write case
    // Create a new ROOT binary machine independent file.
    // Note that this file may contain any kind of ROOT objects, histograms,
    // pictures, graphics objects, detector geometries, tracks, waveforms, etc..
    // This file is now becoming the current directory.
    if (netf) {
      hfile = new TNetFile("root://localhost/root/test/WaveformNet.root","RECREATE","TTree benchmark ROOT file");
    } else
      hfile = new TFile("Waveform.root","RECREATE","TTree benchmark ROOT file");
    hfile->SetCompressionLevel(comp);

    // Create histogram to show write_time in function of time
    Float_t curtime = -0.5;
    Int_t ntime = nwaveform/printev;
    TH1F *htime = new TH1F("htime","Real-Time to write versus time",ntime,0,ntime);
    HistogramManager *hm = 0;
    if (hfill) {
      TDirectory *hdir = new TDirectory("histograms", "all histograms");
      hm = new HistogramManager(hdir);
    }

    // Create a ROOT Tree and one superbranch
    tree = new TTree("T","An example of a ROOT tree");
    tree->SetAutoSave(1000000000); // autosave when 1 Gbyte written
    tree->SetCacheSize(10000000);  // set a 10 MBytes cache (useless when writing local files)
    bufsize = 64000;
    if (split)  bufsize /= 4;
    waveform = new Waveform();           // By setting the value, we own the pointer and must delete it.
    TTree::SetBranchStyle(branchStyle);
    TBranch *branch = tree->Branch("waveform", &waveform, bufsize,split);
    branch->SetAutoDelete(kFALSE);
    if(split >= 0 && branchStyle) tree->BranchRef();

    for (ev = 0; ev < nwaveform; ev++) {
      if (ev%printev == 0) {
	tnew = timer.RealTime();
	printf("waveform:%d, rtime=%f s\n",ev,tnew-told);
	htime->Fill(curtime,tnew-told);
	curtime += 1;
	told=tnew;
	timer.Continue();
      }

      waveform->Build(run, ev, 64, 1000);

      if (write) nb += tree->Fill();  //fill the tree

      if (hm) hm->Hfill(waveform);      //fill histograms
    }
    if (write) {
      hfile = tree->GetCurrentFile(); //just in case we switched to a new file
      hfile->Write();
      tree->Print();
    }
  }
  // We own the waveform (since we set the branch address explicitly), we need to delete it.
  delete waveform;  waveform = 0;

  //  Stop timer and print results
  timer.Stop();
  Float_t mbytes = 0.000001*nb;
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();


  printf("\n%d waveforms and %lld bytes processed.\n",nwaveform,nb);
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  if (read) {
    tree->PrintCacheStats();
    printf("You read %f Mbytes/Realtime seconds\n",mbytes/rtime);
    printf("You read %f Mbytes/Cputime seconds\n",mbytes/ctime);
  } else {
    printf("compression level=%d, split=%d, arg4=%d\n",comp,split,arg4);
    printf("You write %f Mbytes/Realtime seconds\n",mbytes/rtime);
    printf("You write %f Mbytes/Cputime seconds\n",mbytes/ctime);
    //printf("file compression factor = %f\n",hfile.GetCompressionFactor());
  }
  hfile->Close();
  return 0;
}
