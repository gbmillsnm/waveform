#ifndef ROOT_Waveform
#define ROOT_Waveform

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Waveform                                                                //
//                                                                      //
// Description of the waveform and trace parameters                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TRefArray.h"
#include "TRef.h"
#include "TH1.h"
#include "TBits.h"
#include "TMath.h"


class TDirectory;

class Trace : public TObject {

private:
  Int_t        fNTimeStamps;   //Number of points for this trace
  Short_t      *fADCValues;    //[fNTimeStamps]
  Int_t        fChannelNumber; //Channel
  Int_t        fASICNumber;    //ASIC
  Int_t        fMotherboardNumber;//Motherboard
  Int_t        fCrateNumber;//Channel
  Int_t        fGlobalNumber;//Channel

public:
  Trace() { fNTimeStamps = 0; fChannelNumber =0; fASICNumber=0; 
    fCrateNumber =0; fMotherboardNumber=0;fGlobalNumber = 0; }
  Trace(const Trace& orig);
  Trace(Int_t chan, Int_t nTS, Short_t * Trace);
  virtual ~Trace() {Clear();}
  Trace &operator=(const Trace &orig);

  Float_t       GetIntegral();
  Int_t         GetNpoint() const { return fNpoint; }
  TBits&        GetTriggerBits() { return fTriggerBits; }
  Int_t         GetN() const { return fNsp; }
  Double32_t    GetPointValue(Int_t i=0) const { return (i<fNsp)?fPointValue[i]:0; }

  ClassDef(Trace,2)  //A trace
};

class WaveformHeader {

private:
   Int_t   fEvtNum;
   Int_t   fRun;
   Int_t   fDate;

public:
   WaveformHeader() : fEvtNum(0), fRun(0), fDate(0) { }
   virtual ~WaveformHeader() { }
   void   Set(Int_t i, Int_t r, Int_t d) { fEvtNum = i; fRun = r; fDate = d; }
   Int_t  GetEvtNum() const { return fEvtNum; }
   Int_t  GetRun() const { return fRun; }
   Int_t  GetDate() const { return fDate; }

   ClassDef(WaveformHeader,1)  //Waveform Header
};


class Waveform : public TObject {

private:
   char           fType[20];          //waveform type
   char          *fWaveformName;         //run+waveform number in character format
   Int_t          fNtrace;            //Number of traces
   Int_t          fNTimeStamps;       //Number of time stamps per trace
   UInt_t         fFlag;
   Double32_t     fTemperature;       
   WaveformHeader    fEvtHdr;
   TClonesArray  *fTraces;            //->array with all traces
   TRefArray     *fPulsePeaks;            //array of peak pulse candidates
   TRef           fLastTrace;         //reference pointer to last trace
   TBits          fTriggerBits;       //Bits triggered by this waveform.
   Bool_t         fIsValid;           //

   static TClonesArray *fgTraces;
   static TH1F         *fgHist;

public:
   Waveform();
   virtual ~Waveform();
   void          Build(Int_t ev, Int_t arg5=600, Float_t ptmin=1);
   void          Clear(Option_t *option ="");
   Bool_t        IsValid() const { return fIsValid; }
   static void   Reset(Option_t *option ="");
   void          ResetHistogramPointer() {fH=0;}
   void          SetNTimeStamps(Int_t n) { fNTimeStamps = n; }
   void          SetNtrace(Int_t n) { fNtrace = n; }
   void          SetFlag(UInt_t f) { fFlag = f; }
   void          SetTemperature(Double32_t t) { fTemperature = t; }
   void          SetType(char *type) {strcpy(fType,type);}
   void          SetHeader(Int_t i, Int_t run, Int_t date, Float_t random);
   Trace        *AddTrace(Float_t random, Float_t ptmin=1);
}
   void          SetRandomVertex();

   char         *GetType() {return fType;}
   Int_t         GetNtrace() const { return fNtrace; }
   Int_t         GetNTimeStamps() const { return fNTimeStamps; }
   UInt_t        GetFlag() const { return fFlag; }
   Double32_t    GetTemperature() const { return fTemperature; }
   WaveformHeader  *GetHeader() { return &fEvtHdr; }
   TClonesArray *GetTraces() const {return fTraces;}
   TRefArray    *GetPulsePeaks() const {return fPulsePeaks;}
   Trace        *GetLastTrace() const {return (Trace*)fLastTrace.GetObject();}
   TH1F         *GetHistogram() const {return fH;}
   TBits&        GetTriggerBits() { return fTriggerBits; }

   ClassDef(Waveform,1)  //Waveform structure
};


class HistogramManager {

private:
   TH1F  *fNtrace;
   TH1F  *fNTimeStamps;
   TH1F  *fTemperature;

public:
   HistogramManager(TDirectory *dir);
   virtual ~HistogramManager();

   void Hfill(Waveform *waveform);

   ClassDef(HistogramManager,1)  //Manages all histograms
};

#endif
