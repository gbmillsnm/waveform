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
#else 
#include "Waveform.h"
#endif

// This example writes a tree with objects of . 
// It is a simplified version of $ROOTSYS/test/MainWaveform.cxx to 
// write the tree, and $ROOTSYS/test/waveformb.C
// It shows:
//   -how to fill a Tree with an waveform class containing these 
//    data members:
//     char           fType[20];
//     Int_t          fNtrack;
//     Int_t          fNseg;
//     Int_t          fNvertex;
//     UInt_t         fFlag;
//     Float_t        fTemperature;
//     WaveformHeader    fEvtHdr;
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
  w1.Branch("waveform_split", &waveform,16000,99);
  w1.Branch("waveform_not_split", &waveform,16000,0);
 
  // a local variable for the waveform type 
  char etype[20];

  // Fill the tree
  for (Int_t ev = 0; ev <100; ev++) {
    Float_t sigmat, sigmas;
    gRandom->Rannor(sigmat,sigmas);
    Int_t ntrack   = Int_t(600 + 600 *sigmat/120.);
    Float_t random = gRandom->Rndm(1);
    sprintf(etype,"type%d",ev%5);
    waveform->SetType(etype);
    waveform->SetHeader(ev, 200, 960312, random);
    waveform->SetNseg(Int_t(10*ntrack+20*sigmas));
    waveform->SetNvertex(Int_t(1+20*gRandom->Rndm()));
    waveform->SetFlag(UInt_t(random+0.5));
    waveform->SetTemperature(random+20.);
    
    for(UChar_t m = 0; m < 10; m++) {
      waveform->SetMeasure(m, Int_t(gRandom->Gaus(m,m+1)));
    }
    
    // fill the matrix
    for(UChar_t i0 = 0; i0 < 4; i0++) {
      for(UChar_t i1 = 0; i1 < 4; i1++) {
        waveform->SetMatrix(i0,i1,gRandom->Gaus(i0*i1,1));
      }
    }

    //  Create and fill the Track objects
    for (Int_t t = 0; t < ntrack; t++) waveform->AddTrack(random);
    
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
  TBranch *bntrack = w1->GetBranch("fNtrack");
  TBranch *branch  = w1->GetBranch("waveform_split");
  branch->SetAddress(&waveform);
   
  Long64_t nwaveform = w1->GetEntries();
  Int_t nselected = 0;
  Int_t nb = 0;
  for (Long64_t i=0;i<nwaveform;i++) {    
    //read branch "fNtrack"only
    bntrack->GetEntry(i);  
          
    //reject waveforms with more than 587 tracks          
    if (waveform->GetNtrack() > 587)continue;
    
    //read complete accepted waveform in memory 
    nb += w1->GetEntry(i);                  
    nselected++;
    
    //print the first accepted waveform
    if (nselected == 1) w1->Show();
    
    //clear tracks array     
    waveform->Clear();                        
  }
   
  if (gROOT->IsBatch()) return;
  new TBrowser();
  w1->StartViewer();
}   

void waveform1() {
   Waveform::Reset(); // Allow for re-run this script by cleaning static variables.
   waveform1w();
   Waveform::Reset(); // Allow for re-run this script by cleaning static variables.
   waveform1r();
}
