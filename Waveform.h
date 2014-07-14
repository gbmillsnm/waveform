#ifndef ROOT_Waveform
#define ROOT_Waveform

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Waveform                                                                //
//                                                                      //
// Description of the waveform and trace parameters                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <vector>

#include "TObject.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TBits.h"
#include "TMath.h"

#include "WaveformHeader.h"
#include "Trace.h"
 
class Waveform {

private:
  char                fType[20];          //waveform type
  char*               fWaveformName;         //run+waveform number in character format
  Int_t               fNtrace;            //Number of traces
  Double32_t          fTemperature;       
  WaveformHeader      fEvtHdr;
  std::vector<Trace>  fTraces;            //vector with all traces
  TBits               fTriggerBits;       //Bits triggered by this waveform.
  Bool_t              fIsValid;           //
  int                 fEventSize;
  TCanvas *           fCanvas;

public:
  Waveform();
  virtual ~Waveform();
  void          Clear() { fNtrace=0;fTraces.clear(); }
  void          SetSize();
  Bool_t        IsValid() const { return fIsValid; }
  void          Reset();
  void          SetTemperature(Double32_t t) { fTemperature = t; }
  void          SetType(char *type) {strcpy(fType,type);}
  void          SetHeader(Int_t i, Int_t run, Int_t date);

  void          AddTrace(const Trace & tr) {fTraces.push_back(tr);fNtrace++;}
  void          Build(Int_t run, Int_t ev, Int_t ntrace, Int_t nTimeStamps);
  void          DrawChannel(Int_t chan);

  char         *GetType() {return fType;}
  Int_t         GetNtrace() { return fNtrace; }
  Double32_t    GetTemperature() { return fTemperature; }
  WaveformHeader  *GetHeader() { return &fEvtHdr; }
  std::vector<Trace> *GetTraces() {return & fTraces;}

  ClassDef(Waveform,1)  //Waveform structure

};

#endif
