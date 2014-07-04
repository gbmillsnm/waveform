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
////////////////////////////////////////////////////////////////////////

#include "RVersion.h"
#include "TRandom.h"
#include "TProcessID.h"

#include "Waveform.h"

ClassImp(Waveform)

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
   fWaveformName = 0;
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
void Waveform::Build(Int_t ev, Int_t ntrace, Int_t nTimeStamps) {
  char etype[20];
  Float_t sigmat, sigmas;
  gRandom->Rannor(sigmat,sigmas);
  Float_t random = gRandom->Rndm(1);

  //Save current Object count
  Int_t ObjectNumber = TProcessID::GetObjectCount();
  Clear();
  
  Int_t nch = 15;
  if (fWaveformName) delete [] fWaveformName;
  fWaveformName = new char[nch];
  snprintf(fWaveformName,nch,"Waveform%d_Run%d",ev,200);
  snprintf(etype,20,"type%d",ev%5);
  SetType(etype);
  SetHeader(ev, 200, 960312, random);
  SetNvertex(Int_t(1+20*gRandom->Rndm()));
  SetTemperature(random+20.);

  //  Create and Fill the Trace objects
  UShort_t buffer[nTimeStamps];
  for (Int_t t = 0; t < ntrace; t++) {
    for (int i=0;i<nTimeStamps) {
      gRandom->Rannor(sigmat,sigmas)
      buffer[i] = 
    }
    AddTrace(random,ptmin);
  }
  //Restore Object count 
  //To save space in the table keeping trace of all referenced objects
  //we assume that our waveforms do not address each other. We reset the 
  //object count to what it was at the beginning of the waveform.
  TProcessID::SetObjectCount(ObjectNumber);
}  

//______________________________________________________________________________
Trace *Waveform::AddTrace(Int_t chan, Int_t nTS, UShort_t * values)
{
   // Add a new trace to the list of traces for this waveform.
   // To avoid calling the very time consuming operator new for each trace,
   // the standard but not well know C++ operator "new with placement"
   // is called. If traces[i] is 0, a new Trace object will be created
   // otherwise the previous Trace[i] will be overwritten.
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,32,0)
   Trace *trace = (Trace*)fTraces->ConstructedAt(fNtrace++);
   trace->Set(chan,nTS,values);
#else
   TClonesArray &traces = *fTraces;
   Trace *trace = new(traces[fNtrace++]) Trace(chan,nTS,values);
#endif
   //Save reference to last Trace in the collection of Traces
   fLastTrace = trace;
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
void Waveform::SetHeader(Int_t i, Int_t run, Int_t date)
{
   fEvtHdr.Set(i, run, date);
}

