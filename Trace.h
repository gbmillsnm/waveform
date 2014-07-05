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

public:

  Trace() { }
  Trace(const Trace& orig);
  Trace(Int_t chan, Int_t nTS, UShort_t * values);
  virtual ~Trace() { }
  Trace &operator=(const Trace &orig);

  void Set(Int_t chan, Int_t nTS, UShort_t * values);
  void Clear(const Option_t * opt = "");

  Float_t       GetIntegral();
  Int_t         GetNTimeStamps() const { return fNTimeStamps; }
  Int_t         GetChannel() const { return fChannel; }
  Int_t         GetASIC() const { return fASIC; }
  Int_t         GetMotherboard() const { return fMotherboard;}
  Int_t         GetCrate() const { return fCrate; }
  Int_t         GetGlobal() const { return fGlobal; }

  ClassDef(Trace,1)  //A trace
};

#endif
