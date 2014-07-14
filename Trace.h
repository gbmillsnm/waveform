#ifndef ROOT_Trace
#define ROOT_Trace

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Trace                                                                //
//                                                                      //
// Description of the trace parameters                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "Rtypes.h"
#include "Riostream.h"
#include "TMath.h"

class Trace {

private:
  Int_t        fNTimeStamps;   //Number of points for this trace
  UShort_t  *  fADCValues;    //[fNTimeStamps] (rootcint array size)
  Int_t        fChannel; //Channel
  Int_t        fASIC;    //ASIC
  Int_t        fMotherboard;//Motherboard
  Int_t        fCrate;//Crate
  Int_t        fGlobal;//
  Float_t      fIntegral;
  Float_t      fMean;
  Float_t      fRMS;

public:

  Trace() { fADCValues = 0; fNTimeStamps = 0; }
  Trace(const Trace& orig);
  Trace(Int_t chan, Int_t nTS, UShort_t * values);
  virtual ~Trace() { if (fADCValues) delete[] fADCValues; }
  Trace &operator=(const Trace &orig);

  void Set(Int_t chan, Int_t nTS, UShort_t * values);
  void Clear();

  Float_t       GetIntegral();
  Float_t       GetMean() { return fMean;}
  Float_t       GetRMS() { return fRMS;}
  Int_t         GetNTimeStamps() { return fNTimeStamps; }
  Int_t         GetChannel() { return fChannel; }
  Int_t         GetASIC() { return fASIC; }
  Int_t         GetMotherboard() { return fMotherboard;}
  Int_t         GetCrate() { return fCrate; }
  Int_t         GetGlobal() { return fGlobal; }
  const UShort_t* GetADCValues() { return fADCValues; }
  ClassDef(Trace,1)  //A trace
};

#endif
