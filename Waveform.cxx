// @(#)root/test:$Id$
// Author: Rene Brun   19/08/96

////////////////////////////////////////////////////////////////////////
//
//                       Waveform and Trace classes
//                       =======================
//
//  The Waveform class is a naive/simple example of a waveform structure.
//
// private:
//    char           fType[20];          //waveform type
//    char          *fWaveformName;         //run+waveform number in character format
//    Int_t          fNtrace;            //Number of traces
//    Int_t          fNTimeStamps;       //Number of time stamps per trace
//    UInt_t         fFlag;
//    Double32_t     fTemperature;       
//    WaveformHeader    fEvtHdr;
//    TClonesArray  *fTraces;            //->array with all traces
//    TRefArray     *fPulsePeaks;            //array of peak pulse candidates
//    TRef           fLastTrace;         //reference pointer to last trace
//    TBits          fTriggerBits;       //Bits triggered by this waveform.
//    Bool_t         fIsValid;           //

//    static TClonesArray *fgTraces;
//    static TH1F         *fgHist;

// public:
//    Waveform();
//    virtual ~Waveform();
//    void          Build(Int_t ev, Int_t arg5=600, Float_t ptmin=1);
//    void          Clear(Option_t *option ="");
//    Bool_t        IsValid() const { return fIsValid; }
//    static void   Reset(Option_t *option ="");
//    void          ResetHistogramPointer() {fH=0;}
//    void          SetNTimeStamps(Int_t n) { fNTimeStamps = n; }
//    void          SetNtrace(Int_t n) { fNtrace = n; }
//    void          SetFlag(UInt_t f) { fFlag = f; }
//    void          SetTemperature(Double32_t t) { fTemperature = t; }
//    void          SetType(char *type) {strcpy(fType,type);}
//    void          SetHeader(Int_t i, Int_t run, Int_t date, Float_t random);
//    Trace        *AddTrace(Float_t random, Float_t ptmin=1);
//   The WaveformHeader class has 3 data members (integers):
//     public:
//        Int_t          fEvtNum;
//        Int_t          fRun;
//        Int_t          fDate;
//
//
//   The Waveform data member fTraces is a pointer to a TClonesArray.
//   It is an array of a variable number of traces per waveform.
//   Each element of the array is an object of class Trace with the members:
//
// private:
//    Int_t        fNTimeStamps;   //Number of points for this trace
//    Short_t      *fADCValues;    //[fNTimeStamps]
//    Int_t        fChannelNumber; //Channel
//    Int_t        fASICNumber;    //ASIC
//    Int_t        fMotherboardNumber;//Motherboard
//    Int_t        fCrateNumber;//Channel
//    Int_t        fGlobalNumber;//Channel

// public:
//   Trace() { fNTimeStamps = 0; fChannelNumber =0; fASICNumber=0; 
//     fCrateNumber =0; fMotherboardNumber=0;fGlobalNumber = 0; }
//    Trace(const Trace& orig);
//   Trace(Int_t nTS, Short_t * Trace);
//    virtual ~Trace() {Clear();}
//    Trace &operator=(const Trace &orig);

//   Float_t       Integral();
//    Int_t         GetNpoint() const { return fNpoint; }
//    TBits&        GetTriggerBits() { return fTriggerBits; }
//    Short_t       GetValid()  const { return fValid; }
//    virtual void  SetValid(Int_t valid=1) { fValid = valid; }
//    Int_t         GetN() const { return fNsp; }
//    Double32_t    GetPointValue(Int_t i=0) const { return (i<fNsp)?fPointValue[i]:0; }
//
//   An example of a batch program to use the Waveform/Trace classes is given
//   in this directory: MainWaveform.
//   Look also in the same directory at the following macros:
//     - waveforma.C  an example how to read the tree
//     - waveformb.C  how to read waveforms conditionally
//
//   During the processing of the waveform (optionally) also a large number
//   of histograms can be filled. The creation and handling of the
//   histograms is taken care of by the HistogramManager class.
//
//   Note:  This version of the class Waveform (see WaveformMT.h and WaveformMT.cxx
//   for an alternative) uses static variables to improve performance (by
//   reducing the number of memory allocations).  Consequently, only one
//   instance of the class Waveform should be in use at a time (a 2nd instance 
//   would share the array of Traces with the first instance).
//
////////////////////////////////////////////////////////////////////////

#include "RVersion.h"
#include "TRandom.h"
#include "TDirectory.h"
#include "TProcessID.h"

#include "Waveform.h"


ClassImp(WaveformHeader)
ClassImp(Waveform)
ClassImp(Trace)
ClassImp(HistogramManager)

TClonesArray *Waveform::fgTraces = 0;
TH1F *Waveform::fgHist = 0;

//______________________________________________________________________________
Waveform::Waveform() : fIsValid(kFALSE)
{
   // Create an Waveform object.
   // When the constructor is invoked for the first time, the class static
   // variable fgTraces is 0 and the TClonesArray fgTraces is created.

   if (!fgTraces) fgTraces = new TClonesArray("Trace", 1000);
   fTraces = fgTraces;
   fHighPt = new TRefArray;
   fNtrace = 0;
   fH      = 0;
   Int_t i0,i1;
   for (i0 = 0; i0 < 4; i0++) {
      for (i1 = 0; i1 < 4; i1++) {
         fMatrix[i0][i1] = 0.0;
      }
   }
   for (i0 = 0; i0 <10; i0++) fMeasures[i0] = 0;
   for (i0 = 0; i0 <20; i0++) fType[i0] = 0;
   fClosestDistance = 0;
   fWaveformName = 0;
   fWebHistogram.SetAction(this);
}

//______________________________________________________________________________
Waveform::~Waveform()
{
   Clear();
   if (fH == fgHist) fgHist = 0;
   delete fH; fH = 0;
   delete fHighPt; fHighPt = 0;
   delete fMuons;  fMuons = 0;
   delete [] fClosestDistance;
   if (fWaveformName) delete [] fWaveformName;
}

//______________________________________________________________________________
void Waveform::Build(Int_t ev, Int_t arg5, Float_t ptmin) {
  fIsValid = kTRUE;
  char etype[20];
  Float_t sigmat, sigmas;
  gRandom->Rannor(sigmat,sigmas);
  Int_t ntrace   = Int_t(arg5 +arg5*sigmat/120.);
  Float_t random = gRandom->Rndm(1);

  //Save current Object count
  Int_t ObjectNumber = TProcessID::GetObjectCount();
  Clear();
  fHighPt->Delete();
  fMuons->Delete();
  
  Int_t nch = 15;
  if (ev >= 100)   nch += 3;
  if (ev >= 10000) nch += 3;
  if (fWaveformName) delete [] fWaveformName;
  fWaveformName = new char[nch];
  snprintf(fWaveformName,nch,"Waveform%d_Run%d",ev,200);
  snprintf(etype,20,"type%d",ev%5);
  SetType(etype);
  SetHeader(ev, 200, 960312, random);
  SetNseg(Int_t(10*ntrace+20*sigmas));
  SetNvertex(Int_t(1+20*gRandom->Rndm()));
  SetFlag(UInt_t(random+0.5));
  SetTemperature(random+20.);

  for(UChar_t m = 0; m < 10; m++) {
     SetMeasure(m, Int_t(gRandom->Gaus(m,m+1)));
  }
  for(UChar_t i0 = 0; i0 < 4; i0++) {
    for(UChar_t i1 = 0; i1 < 4; i1++) {
       SetMatrix(i0,i1,gRandom->Gaus(i0*i1,1));
    }
  }

  fTriggerBits.SetBitNumber((UInt_t)(64*gRandom->Rndm(1)));
  fTriggerBits.SetBitNumber((UInt_t)(64*gRandom->Rndm(1)));
  fTriggerBits.SetBitNumber((UInt_t)(64*gRandom->Rndm(1)));

  //  Create and Fill the Trace objects
  for (Int_t t = 0; t < ntrace; t++) AddTrace(random,ptmin);
  
  //Restore Object count 
  //To save space in the table keeping trace of all referenced objects
  //we assume that our waveforms do not address each other. We reset the 
  //object count to what it was at the beginning of the waveform.
  TProcessID::SetObjectCount(ObjectNumber);
}  

//______________________________________________________________________________
Trace *Waveform::AddTrace(Float_t random, Float_t ptmin)
{
   // Add a new trace to the list of traces for this waveform.
   // To avoid calling the very time consuming operator new for each trace,
   // the standard but not well know C++ operator "new with placement"
   // is called. If traces[i] is 0, a new Trace object will be created
   // otherwise the previous Trace[i] will be overwritten.
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,32,0)
   Trace *trace = (Trace*)fTraces->ConstructedAt(fNtrace++);
   trace->Set(random);
#else
   TClonesArray &traces = *fTraces;
   Trace *trace = new(traces[fNtrace++]) Trace(random);
#endif
   //Save reference to last Trace in the collection of Traces
   fLastTrace = trace;
   //Save reference in fHighPt if trace is a high Pt trace
   if (trace->GetPt() > ptmin)   fHighPt->Add(trace);
   //Save reference in fMuons if trace is a muon candidate
   if (trace->GetMass2() < 0.11) fMuons->Add(trace);
   return trace;
}

//______________________________________________________________________________
void Waveform::Clear(Option_t * /*option*/)
{
   fTraces->Clear("C"); //will also call Trace::Clear
   fHighPt->Delete();
   fMuons->Delete();
   fTriggerBits.Clear();
}

//______________________________________________________________________________
void Waveform::Reset(Option_t * /*option*/)
{
// Static function to reset all static objects for this waveform
//   fgTraces->Delete(option);

   delete fgTraces; fgTraces = 0;
   fgHist   = 0;
}

//______________________________________________________________________________
void Waveform::SetHeader(Int_t i, Int_t run, Int_t date, Float_t random)
{
   fNtrace = 0;
   fEvtHdr.Set(i, run, date);
   if (!fgHist) fgHist = new TH1F("hstat","Waveform Histogram",100,0,1);
   fH = fgHist;
   fH->Fill(random);
}

//______________________________________________________________________________
void Waveform::SetMeasure(UChar_t which, Int_t what) {
   if (which<10) fMeasures[which] = what;
}

//______________________________________________________________________________
void Waveform::SetRandomVertex() {
   // This delete is to test the relocation of variable length array
   if (fClosestDistance) delete [] fClosestDistance;
   if (!fNvertex) {
      fClosestDistance = 0;
      return;
   }
   fClosestDistance = new Double32_t[fNvertex];
   for (Int_t k = 0; k < fNvertex; k++ ) {
      fClosestDistance[k] = gRandom->Gaus(1,1);
   }
}

//______________________________________________________________________________
Trace::Trace(const Trace &orig) : TObject(orig)
{
   // Copy a trace object

   fNTimeStamps = orig.fNTimeStamps;
   for (int i=0;i<fNTimeStamps;i++) {
     fADCValues[i] = origin.fADCValues[i];
   }
   fChannelNumber = orig.fChannelNumber;
   fASICNumber = orig.fASICNumber;
   fMotherboardNumber = orig.fMotherboardNumber;
   fCrateNumber = orig.fCrateNumber;
   fGlobalNumber = orig.fGlobalNumber;
}

//______________________________________________________________________________
Trace::Trace(Int_t chan, Int_t nTS, Short_t * Trace) : TObject()
{
  // Create a trace object.
  // Note that in this example, data members 
  //do not have any physical meaning.

  fChannelNumber = chan;
  fNTimeStamps = nTS;
  for (int i=0; i<fNTimeStamps;i++) {
    fADCValues[i] = Trace[i];
  }
}

//______________________________________________________________________________
Trace &Trace::operator=(const Trace &orig)
{
   // Copy a trace
 
   TObject::operator=(orig);

   fNTimeStamps = orig.fNTimeStamps;
   for (int i=0;i<fNTimeStamps;i++) {
     fADCValues[i] = origin.fADCValues[i];
   }
   fChannelNumber = orig.fChannelNumber;
   fASICNumber = orig.fASICNumber;
   fMotherboardNumber = orig.fMotherboardNumber;
   fCrateNumber = orig.fCrateNumber;
   fGlobalNumber = orig.fGlobalNumber;
   
   return *this;
}

//______________________________________________________________________________
void Trace::Clear(Option_t * /*option*/)
{
   // Note that we intend on using TClonesArray::ConstructedAt, so we do not
   // need to delete any of the arrays.

   TObject::Clear();
   fTriggerBits.Clear(); 
}

//______________________________________________________________________________
void Trace::Set(Float_t random)
{
   // Set the values of the Trace data members.
   
   Float_t a,b,px,py;
   gRandom->Rannor(px,py);
   fPx = px;
   fPy = py;
   fPz = TMath::Sqrt(px*px+py*py);
   fRandom = 1000*random;
   if (fRandom < 10) fMass2 = 0.106;
   else if (fRandom < 100) fMass2 = 0.8;
   else if (fRandom < 500) fMass2 = 4.5;
   else if (fRandom < 900) fMass2 = 8.9;
   else  fMass2 = 9.8;
   gRandom->Rannor(a,b);
   fBx = 0.1*a;
   fBy = 0.1*b;
   fMeanCharge = 0.01*gRandom->Rndm(1);
   gRandom->Rannor(a,b);
   fXfirst = a*10;
   fXlast  = b*10;
   gRandom->Rannor(a,b);
   fYfirst = a*12;
   fYlast  = b*16;
   gRandom->Rannor(a,b);
   fZfirst = 50 + 5*a;
   fZlast  = 200 + 10*b;
   fCharge = Double32_t(Int_t(3*gRandom->Rndm(1)) - 1);
   
   fTriggerBits.SetBitNumber((UInt_t)(64*gRandom->Rndm(1)));
   fTriggerBits.SetBitNumber((UInt_t)(64*gRandom->Rndm(1)));
   fTriggerBits.SetBitNumber((UInt_t)(64*gRandom->Rndm(1)));
   
   fVertex[0] = gRandom->Gaus(0,0.1);
   fVertex[1] = gRandom->Gaus(0,0.2);
   fVertex[2] = gRandom->Gaus(0,10);
   fNpoint = Int_t(60+10*gRandom->Rndm(1));
   Int_t newNsp = Int_t(3*gRandom->Rndm(1));
   if (fNsp > newNsp) {
      fNsp = newNsp;
      if (fNsp == 0) {
         delete [] fPointValue;
         fPointValue = 0;
      } else {
         for(int i=0; i<fNsp; i++) {
            fPointValue[i] = i+1;
         }         
      }
      
   } else {
      if (fNsp) {
         delete [] fPointValue;
      }
      fNsp = newNsp;
      if (fNsp) {
         fPointValue = new Double32_t[fNsp];
         for(int i=0; i<fNsp; i++) {
            fPointValue[i] = i+1;
         }
      } else {
         fPointValue = 0;
      }
   }
   fValid  = Int_t(0.6+gRandom->Rndm(1));
}

//______________________________________________________________________________
HistogramManager::HistogramManager(TDirectory *dir)
{
   // Create histogram manager object. Histograms will be created
   // in the "dir" directory.

   // Save current directory and cd to "dir".
   TDirectory *saved = gDirectory;
   dir->cd();

   fNtrace      = new TH1F("hNtrace",    "Ntrace",100,575,625);
   fNseg        = new TH1F("hNseg",      "Nseg",100,5800,6200);
   fTemperature = new TH1F("hTemperature","Temperature",100,19.5,20.5);
   fPx          = new TH1F("hPx",        "Px",100,-4,4);
   fPy          = new TH1F("hPy",        "Py",100,-4,4);
   fPz          = new TH1F("hPz",        "Pz",100,0,5);
   fRandom      = new TH1F("hRandom",    "Random",100,0,1000);
   fMass2       = new TH1F("hMass2",     "Mass2",100,0,12);
   fBx          = new TH1F("hBx",        "Bx",100,-0.5,0.5);
   fBy          = new TH1F("hBy",        "By",100,-0.5,0.5);
   fMeanCharge  = new TH1F("hMeanCharge","MeanCharge",100,0,0.01);
   fXfirst      = new TH1F("hXfirst",    "Xfirst",100,-40,40);
   fXlast       = new TH1F("hXlast",     "Xlast",100,-40,40);
   fYfirst      = new TH1F("hYfirst",    "Yfirst",100,-40,40);
   fYlast       = new TH1F("hYlast",     "Ylast",100,-40,40);
   fZfirst      = new TH1F("hZfirst",    "Zfirst",100,0,80);
   fZlast       = new TH1F("hZlast",     "Zlast",100,0,250);
   fCharge      = new TH1F("hCharge",    "Charge",100,-1.5,1.5);
   fNpoint      = new TH1F("hNpoint",    "Npoint",100,50,80);
   fValid       = new TH1F("hValid",     "Valid",100,0,1.2);

   // cd back to original directory
   saved->cd();
}

//______________________________________________________________________________
HistogramManager::~HistogramManager()
{
   // Clean up all histograms.

   // Nothing to do. Histograms will be deleted when the directory
   // in which tey are stored is closed.
}

//______________________________________________________________________________
void HistogramManager::Hfill(Waveform *waveform)
{
   // Fill histograms.

   fNtrace->Fill(waveform->GetNtrace());
   fNseg->Fill(waveform->GetNseg());
   fTemperature->Fill(waveform->GetTemperature());

   for (Int_t itrace = 0; itrace < waveform->GetNtrace(); itrace++) {
      Trace *trace = (Trace*)waveform->GetTraces()->UncheckedAt(itrace);
      fPx->Fill(trace->GetPx());
      fPy->Fill(trace->GetPy());
      fPz->Fill(trace->GetPz());
      fRandom->Fill(trace->GetRandom());
      fMass2->Fill(trace->GetMass2());
      fBx->Fill(trace->GetBx());
      fBy->Fill(trace->GetBy());
      fMeanCharge->Fill(trace->GetMeanCharge());
      fXfirst->Fill(trace->GetXfirst());
      fXlast->Fill(trace->GetXlast());
      fYfirst->Fill(trace->GetYfirst());
      fYlast->Fill(trace->GetYlast());
      fZfirst->Fill(trace->GetZfirst());
      fZlast->Fill(trace->GetZlast());
      fCharge->Fill(trace->GetCharge());
      fNpoint->Fill(trace->GetNpoint());
      fValid->Fill(trace->GetValid());
   }
}
