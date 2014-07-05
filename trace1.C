#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TRandom.h"
#include "TClassTable.h"
#include "TSystem.h"
#include "TROOT.h"
#if defined(__CINT__) && !defined(__MAKECINT__) 
#include "Trace_cxx.so"
#else 
#include "Trace.h"
#endif

// This example writes a tree with Trace objects. 
// It is a simplified version of $ROOTSYS/test/MainTrace.cxx to 
// write the tree, and $ROOTSYS/test/traceb.C
// It shows:
//   -how to fill a Tree with an trace class containing these 
//    data members:
//     char           fType[20];
//     Int_t          fNtrack;
//     Int_t          fNseg;
//     Int_t          fNvertex;
//     UInt_t         fFlag;
//     Float_t        fTemperature;
//     TraceHeader    fEvtHdr;
//     TClonesArray  *fTracks;            //->
//     TH1F          *fH;                 //->
//     Int_t          fMeasures[10];
//     Float_t        fMatrix[4][4];
//     Float_t       *fClosestDistance;   //[fNvertex] 
//
//   -the difference in splitting or not splitting a branch
//   -how to read selected branches of the tree,
//    and print the first entry with less than 587 tracks.
//   -how to browse and analyze the Tree via the TBrowser and TTreeViewer

// This example can be run in many different ways:
//  way1:  .x trace1.C    using the CINT interpreter
//  way2:  .L trace1.C
//          trace1()
//  way3:  .L Trace.cxx++
//         .x trace1.C++   using ACLIC
// One can also run the write and read parts in two separate sessions.
// For example following one of the sessions above, one can start the session:
//   .L trace.C
//   trace1r();

void trace1w()
{
 
  //create a Tree file tree4.root
  TFile f("trace1.root","RECREATE");
 
  // Create a ROOT Tree
  TTree t1("t1","A Tree with Traces");
    
  // Create a pointer to an Trace object
  Int_t chan = 1;
  Int_t nTimeStamps = 1000;
  UShort_t adcValues[nTimeStamps];

  Trace *trace = new Trace(chan,nTimeStamps,adcValues);

  // Create two branches, split one.
  t1.Branch("trace", &trace,16000,99);
 
  // Fill the tree
  Int_t base = 100 ;
  for (Int_t ev = 0; ev <100; ev++) {
    Float_t sigmat, sigmas;
    for (int i=0;i<nTimeStamps;i++) {
      gRandom->Rannor(sigmat,sigmas);
      adcValues[i] = base + 10*sigmat;
    }
    trace->Set(chan,nTimeStamps,adcValues);

    t1.Fill(); 
    
    // Clear the trace before reloading it 
    trace->Clear();
  }
  
  // Write the file header
  f.Write();
  
  // Print the tree contents
  t1.Print();
}


void trace1r()
{
  // check to see if the trace class is in the dictionary
  // if it is not load the definition in libTrace.so
  if (!TClassTable::GetDict("Trace")) {
    gSystem->Load("libTrace");
  }    
   
  // read the tree generated with tree4w 
  
  //note that we use "new" to create the TFile and TTree objects !
  //because we want to keep these objects alive when we leave this function.
  TFile *f = new TFile("trace1.root");
  TTree *t1 = (TTree*)f->Get("t1");
  
  // create a pointer to an trace object. This will be used
  // to read the branch values.
  Trace *trace = new Trace();
  
  // get two branches and set the branch address
  TBranch *branch  = t1->GetBranch("trace");
  branch->SetAddress(&trace);
   
  Long64_t ntrace = t1->GetEntries();
  Int_t nb = 0;

  for (Long64_t i=0;i<ntrace;i++) {    
    //read complete accepted trace in memory 
    nb += t1->GetEntry(i);
    
    //print the first accepted trace
    if (nb < 10) t1->Show();
    
    //clear tracks array     
    trace->Clear("");                        
  }
   
  if (gROOT->IsBatch()) return;
  new TBrowser();
  t1->StartViewer();
}   

void trace1() {
  //   Trace::Reset(); // Allow for re-run this script by cleaning static variables.
   trace1w();
   //   Trace::Reset(); // Allow for re-run this script by cleaning static variables.
   trace1r();
}
