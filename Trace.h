#ifndef ROOT_Trace
#define ROOT_Trace

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Trace                                                                //
//                                                                      //
// Description of the trace parameters                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"


class TDirectory;

class Trace : public TObject {

private:
  Int_t        fNTimeStamps;   //Number of points for this trace
  UShort_t  *  fADCValues;    //[fNTimeStamps]
  Int_t        fChannel; //Channel
  Int_t        fASIC;    //ASIC
  Int_t        fMotherboard;//Motherboard
  Int_t        fCrate;//Crate
  Int_t        fGlobal;//
  Float_t      fIntegral;

public:

  Trace() { fNTimeStamps = 0; fChannel =0; fASIC=0; 
    fCrate =0; fMotherboard=0;fGlobal = 0; }
  Trace(const Trace& orig);
  Trace(Int_t chan, Int_t nTS, UShort_t * values);
  virtual ~Trace() {Clear();}
  Trace &operator=(const Trace &orig);

  Float_t       GetIntegral();
  Int_t         GetNTimeStamps() const { return fNTimeStamps; }
  Int_t         GetChannel() const { return fChannel; }
  Int_t         GetASIC() const { return fASIC; }
  Int_t         GetMotherboard() const { return fMotherboard;}
  Int_t         GetCrate() const { return fCrate; }
  Int_t         GetGlobal() const { return fGlobal; }

  ClassDef(Trace,2)  //A trace
};

#endif
