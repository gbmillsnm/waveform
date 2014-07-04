

#include "Trace.h"

ClassImp(Trace)

//______________________________________________________________________________
Trace::Trace(const Trace &orig) : TObject(orig)
{
   // Copy a trace object

   fNTimeStamps = orig.fNTimeStamps;
   for (int i=0;i<fNTimeStamps;i++) {
     fADCValues[i] = origin.fADCValues[i];
   }
   fChannel = orig.fChannel;
   fASIC = orig.fASIC;
   fMotherboard = orig.fMotherboard;
   fCrate = orig.fCrate;
   fGlobal = orig.fGlobal;
}

//______________________________________________________________________________
Trace::Trace(Int_t chan, Int_t nTS, Short_t * Trace) : TObject()
{
  // Create a trace object.
  // Note that in this example, data members 
  //do not have any physical meaning.

  fChannel = chan;
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
   fChannel = orig.fChannel;
   fASIC = orig.fASIC;
   fMotherboard = orig.fMotherboard;
   fCrate = orig.fCrate;
   fGlobal = orig.fGlobal;
   
   return *this;
}

//______________________________________________________________________________
void Trace::Clear(Option_t * /*option*/)
{
   // Note that we intend on using TClonesArray::ConstructedAt, so we do not
   // need to delete any of the arrays.

   TObject::Clear();
}

//______________________________________________________________________________
void Trace::Set(Int_t chan, Int_t nTS, UShort_t * trace) 
{
  // Create a trace object.
  // Note that in this example, data members 
  //do not have any physical meaning.

  fChannel = chan;
  fNTimeStamps = nTS;
  for (int i=0; i<fNTimeStamps;i++) {
    fADCValues[i] = trace[i];
  }
}

Float_t Trace::GetIntegral() {
  fIntegral = 0;
  for (int i=0;i<fNTimeStamps;i++) {
    fIntegral += fADCValues[i];
  }
}

