#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TRandom.h"
#include "TClassTable.h"
#include "TSystem.h"
#include "TROOT.h"
#if defined(__CINT__) && !defined(__MAKECINT__) 
#include "Waveform_cxx.so"
#include "WaveformHeader_cxx.so"
#include "Trace_cxx.so"
#else 
#include "Waveform.h"
#include "WaveformHeader.h"
#include "Trace.h"
#endif

// This example writes a tree with objects of . 
// This example can be run in many different ways:
//  way1:  .x waveform1.C    using the CINT interpreter
//  way2:  .L waveform1.C
//          waveform1()
//  way3:  .L Waveform.cxx++
//         .x waveform1.C++   using ACLIC
// One can also run the write and read parts in two separate sessions.
// For example following one of the sessions above, one can start the session:
//   .L waveform.C
//   waveform1r();

void waveform1w()
{
 
  //create a Tree file tree4.root
  TFile f("waveform1.root","RECREATE");
 
  // Create a ROOT Tree
  TTree w1("w1","A Tree with Waveforms");
    
  // Create a pointer to an Waveform object
  Waveform *waveform = new Waveform();
    
  // Create two branches, split one.
  w1.Branch("waveform", &waveform,16000,99);
 
  // a local variable for the waveform type 
  char etype[20];
  Int_t run = 1;
  // Fill the tree
  for (Int_t ev = 0; ev <1000; ev++) {
    waveform->Clear();
    Float_t sigmat, sigmas;
    gRandom->Rannor(sigmat,sigmas);
    Int_t ntrace   = 100;
    sprintf(etype,"type%d",ev%5);
    waveform->SetType(etype);
    waveform->SetHeader(ev, run, 20140731);
    waveform->SetTemperature(20.+sigmat*2);
        
    //  Create and fill the Trace objects
    Int_t nTimeStamps = 1000;
    Trace tr;
    Int_t base = 10;
    UShort_t buffer[nTimeStamps];
    for (Int_t t = 0; t < ntrace; t++) {
      UShort_t ib;
      for (int i=0;i<nTimeStamps;i++) {
	gRandom->Rannor(sigmat,sigmas);
	ib = base + 10*sigmat;
	if (ib >= 0 && ib < 4096)
	  buffer[i] = ib;
	else
	  buffer[i] = 0;
      }
      tr.Set(t,nTimeStamps,buffer);
      waveform->AddTrace(tr);
    }
    waveform->SetSize();

    // Fill the tree
    w1.Fill(); 
    
    // Clear the waveform before reloading it 
    waveform->Clear();
  }
  
  // Write the file header
  f.Write();
  
  // Print the tree contents
  w1.Print();
}


void waveform1r()
{
  // check to see if the waveform class is in the dictionary
  // if it is not load the definition in libWaveform.so
  if (!TClassTable::GetDict("Waveform")) {
    gSystem->Load("libWaveform");
  }    
   
  // read the tree generated with tree4w 
  
  //note that we use "new" to create the TFile and TTree objects !
  //because we want to keep these objects alive when we leave this function.
  TFile *f = new TFile("waveform1.root");
  TTree *w1 = (TTree*)f->Get("w1");
  
  // create a pointer to an waveform object. This will be used
  // to read the branch values.
  Waveform *waveform = new Waveform();
  
  // get two branches and set the branch address
  TBranch *bntrace = w1->GetBranch("fNtrace");
  TBranch *branch  = w1->GetBranch("waveform");

  branch->SetAddress(&waveform);
   
  Long64_t nwaveform = w1->GetEntries();
  Int_t nb = 0;
  for (Long64_t i=0;i<nwaveform;i++) {    
    //read branch "fNtrace"only
    bntrace->GetEntry(i);  
          
    //read complete accepted waveform in memory 
    nb += w1->GetEntry(i);                  

    //print the first accepted waveform
    if (i == 1) w1->Show();
    
    //clear traces array     
    waveform->Clear();                        
  }
   
  if (gROOT->IsBatch()) return;
  new TBrowser();
  w1->StartViewer();
}   

void waveform1() {
   waveform1w();
   waveform1r();
}
