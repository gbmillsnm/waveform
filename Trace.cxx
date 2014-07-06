
#include "Trace.h"

ClassImp(Trace)

//______________________________________________________________________________
Trace::Trace(const Trace &orig)
{
   // Copy a trace object

   fNTimeStamps = orig.fNTimeStamps;
   fADCValues = new UShort_t[fNTimeStamps];
   for (int i=0;i<fNTimeStamps;i++) {
     fADCValues[i] = orig.fADCValues[i];
   }
   fChannel = orig.fChannel;
   fASIC = orig.fASIC;
   fMotherboard = orig.fMotherboard;
   fCrate = orig.fCrate;
   fGlobal = orig.fGlobal;
}

//______________________________________________________________________________
Trace::Trace(Int_t chan, Int_t nTS, UShort_t * buffer)
{
  // Create a trace object.
  fChannel = chan;
  fNTimeStamps = nTS;
  fADCValues = new UShort_t[fNTimeStamps];
  for (int i=0; i<fNTimeStamps;i++) {
    fADCValues[i] = buffer[i];
  }
  fChannel = chan;
  fASIC = chan % 8;
  fMotherboard = fASIC % 8;
  fCrate = fMotherboard % 20;
  fGlobal = chan;
  cout << "Created Trace" << endl;

}

//______________________________________________________________________________
Trace &Trace::operator=(const Trace &orig)
{
   // Copy a trace

  fNTimeStamps = orig.fNTimeStamps;
  fADCValues = new UShort_t[fNTimeStamps];
  fNTimeStamps = orig.fNTimeStamps;
  
  for (int i=0;i<fNTimeStamps;i++) {
    fADCValues[i] = orig.fADCValues[i];
  }
  fChannel = orig.fChannel;
  fASIC = orig.fASIC;
  fMotherboard = orig.fMotherboard;
  fCrate = orig.fCrate;
  fGlobal = orig.fGlobal;
   
   return *this;
}

//______________________________________________________________________________
void Trace::Clear(const Option_t * opt )
{
   // Note that we intend on using TClonesArray::ConstructedAt, so we do not
   // need to delete any of the arrays.

  //   TObject::Clear(opt);
}

//______________________________________________________________________________
void Trace::Set(Int_t chan, Int_t nTS, UShort_t * buffer) 
{
  // Create a trace object.
  fChannel = chan;
  fNTimeStamps = nTS;
  fADCValues = new UShort_t[fNTimeStamps];
  for (int i=0; i<fNTimeStamps;i++) {
    fADCValues[i] = buffer[i];
  }
  fChannel = chan;
  fASIC = chan % 8;
  fMotherboard = fASIC % 8;
  fCrate = fMotherboard % 20;
  fGlobal = chan;

  fIntegral = GetIntegral();
}

Float_t Trace::GetIntegral() {
  Float_t sum = 0;
  for (int i=0;i<fNTimeStamps;i++) {
    sum += fADCValues[i];
  }
  return sum;
}

