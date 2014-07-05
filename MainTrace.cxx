// @(#)root/test:$Id$
// Author: Rene Brun   19/01/97

////////////////////////////////////////////////////////////////////////
//
//             A simple example with a ROOT tree
//             =================================
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
#include "TH1.h"

#include "Trace.h"


//______________________________________________________________________________
int main(int argc, char **argv)
{
  Int_t ntrace = 400;     // by default create 400 waveforms
  Int_t write  = 1;       // by default the tree is filled
  Int_t read   = 0;
  Int_t punzip = 0;
  Int_t comp   = 1;       // by default file is compressed

  

  TFile *hfile;
  TTree *tree;

  Trace *tr = 0;


  // Fill trace, header and tracks with some random numbers
  //   Create a timer object to benchmark this loop
  TStopwatch timer;
  timer.Start();
  Long64_t nb = 0;
  Int_t ev;
  Int_t bufsize;
  Double_t told = 0;
  Double_t tnew = 0;
  Int_t printev = 100;

  //         Read case
  if (read) {
    hfile = new TFile("Trace.root");
    tree = (TTree*)hfile->Get("T");
    TBranch *branch = tree->GetBranch("trace");
    branch->SetAddress(&tr);
    Int_t nentries = (Int_t)tree->GetEntries();
    ntrace = TMath::Min(ntrace,nentries);
    if (read == 1) {  //read sequential
      //by setting the read cache to -1 we set it to the AutoFlush value when writing
      Int_t cachesize = -1; 
      if (punzip) tree->SetParallelUnzip();
      tree->SetCacheSize(cachesize);
      tree->SetCacheLearnEntries(1); //one entry is sufficient to learn
      tree->SetCacheEntryRange(0,ntrace);
      for (ev = 0; ev < ntrace; ev++) {
	tree->LoadTree(ev);  //this call is required when using the cache
	if (ev%printev == 0) {
	  tnew = timer.RealTime();
	  printf("trace:%d, rtime=%f s\n",ev,tnew-told);
	  told=tnew;
	  timer.Continue();
	}
	nb += tree->GetEntry(ev);        //read complete trace in memory
      }
    } else {    //read random
      Int_t evrandom;
      for (ev = 0; ev < ntrace; ev++) {
	if (ev%printev == 0) cout<<"trace="<<ev<<endl;
	evrandom = Int_t(ntrace*gRandom->Rndm(1));
	nb += tree->GetEntry(evrandom);  //read complete trace in memory
      }
    }
  } else {
    //         Write case
    // Create a new ROOT binary machine independent file.
    // Note that this file may contain any kind of ROOT objects, histograms,
    // pictures, graphics objects, detector geometries, tracks, traces, etc..
    // This file is now becoming the current directory.

    const Int_t nTimeStamps = 1000;

    hfile = new TFile("Trace.root","RECREATE","TTree benchmark ROOT file");
    hfile->SetCompressionLevel(comp);
  
    // Create histogram to show write_time in function of time
    Float_t curtime = -0.5;
    Int_t ntime = ntrace/printev;
    TH1F *htime = new TH1F("htime","Real-Time to write versus time",ntime,0,ntime);
  
    // Create a ROOT Tree and one superbranch
    tree = new TTree("T","An example of a ROOT tree");
    tree->SetAutoSave(1000000000); // autosave when 1 Gbyte written
    tree->SetCacheSize(10000000);  // set a 10 MBytes cache (useless when writing local files)
    bufsize = 64000;

    UShort_t buffer[nTimeStamps];

    
    tr = new Trace(1,nTimeStamps,buffer); // By setting the value, we own the pointer and must delete it.

    TBranch *branch = tree->Branch("trace", &tr, bufsize,1);
    branch->SetAutoDelete(kFALSE);
  
    for (ev = 0; ev < ntrace; ev++) {
      if (ev%printev == 0) {
	tnew = timer.RealTime();
	printf("trace:%d, rtime=%f s\n",ev,tnew-told);
	htime->Fill(curtime,tnew-told);
	curtime += 1;
	told=tnew;
	timer.Continue();
      }
      Int_t base = 100;
      Float_t sigmat,sigmas;
      for (int i=0;i<nTimeStamps;i++) {
	gRandom->Rannor(sigmat,sigmas);
	buffer[i] = base + 10*sigmat;
      }
      tr->Set(ev, nTimeStamps, buffer);
    
      if (write) nb += tree->Fill();  //fill the tree
    
    }
    if (write) {
      hfile = tree->GetCurrentFile(); //just in case we switched to a new file
      hfile->Write();
      tree->Print();
    }
  }
  // We own the trace (since we set the branch address explicitly), we need to delete it.
  delete tr;  tr = 0;

  //  Stop timer and print results
  timer.Stop();
  Float_t mbytes = 0.000001*nb;
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();


  printf("\n%d traces and %lld bytes processed.\n",ntrace,nb);
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  if (read) {
    tree->PrintCacheStats();
    printf("You read %f Mbytes/Realtime seconds\n",mbytes/rtime);
    printf("You read %f Mbytes/Cputime seconds\n",mbytes/ctime);
  } else {
    printf("You write %f Mbytes/Realtime seconds\n",mbytes/rtime);
    printf("You write %f Mbytes/Cputime seconds\n",mbytes/ctime);
  }
  hfile->Close();
  return 0;
}
