#ifndef ROOT_WaveformHeader
#define ROOT_WaveformHeader

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

#endif
