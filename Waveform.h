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



class Waveform : public TObject {

private:
   char           fType[20];          //waveform type
   char          *fWaveformName;         //run+waveform number in character format
   Int_t          fNtrace;            //Number of traces
   Double32_t     fTemperature;       
   WaveformHeader fEvtHdr;
   TClonesArray  *fTraces;            //->array with all traces
   TRef           fLastTrace;         //reference pointer to last trace
   TBits          fTriggerBits;       //Bits triggered by this waveform.
   Bool_t         fIsValid;           //

   static TClonesArray *fgTraces;
   static TH1F         *fgHist;

public:
   Waveform();
   virtual ~Waveform();
   void          Clear(Option_t *option ="");
   Bool_t        IsValid() const { return fIsValid; }
   static void   Reset(Option_t *option ="");
   void          ResetHistogramPointer() {fH=0;}
   void          SetNtrace(Int_t n) { fNtrace = n; }
   void          SetFlag(UInt_t f) { fFlag = f; }
   void          SetTemperature(Double32_t t) { fTemperature = t; }
   void          SetType(char *type) {strcpy(fType,type);}
   void          SetHeader(Int_t i, Int_t run, Int_t date, Float_t random);
   Trace        *AddTrace(Float_t random, Float_t ptmin=1);

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

   ClassDef(Waveform,1)  //Waveform structure
};

#endif
