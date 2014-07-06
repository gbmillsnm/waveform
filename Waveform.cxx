// @(#)root/test:$Id$
// Author: Rene Brun   19/08/96

////////////////////////////////////////////////////////////////////////
//
//                       Waveform classes
//                       ================
//
//  The Waveform class is an example of a waveform structure.
//
//
////////////////////////////////////////////////////////////////////////

#include "RVersion.h"
#include "TRandom.h"
#include "TProcessID.h"

#include "Waveform.h"

ClassImp(Waveform)

//______________________________________________________________________________
Waveform::Waveform() : fIsValid(kFALSE)
{
   // Create an Waveform object.
   // When the constructor is invoked for the first time, the class static
   // variable fgTraces is 0 and the TClonesArray fgTraces is created.

   fNtrace = 0;
   fWaveformName = 0;
}

//______________________________________________________________________________
Waveform::~Waveform()
{
   Clear();
   if (fWaveformName) delete [] fWaveformName;
}

//______________________________________________________________________________
void Waveform::Reset()
{
   Clear();
   if (fWaveformName) delete [] fWaveformName;
}

//______________________________________________________________________________
void Waveform::Build(Int_t run,Int_t ev, Int_t ntrace, Int_t nTimeStamps) {
  char etype[20];
  Float_t sigmat, sigmas;
  gRandom->Rannor(sigmat,sigmas);
  Float_t random = gRandom->Rndm(1);

  Clear();
  
  Int_t nch = 15;
  if (fWaveformName) delete [] fWaveformName;
  fWaveformName = new char[nch];
  snprintf(fWaveformName,nch,"Waveform %d_Run %d",ev,run);
  snprintf(etype,20,"type%d",ev%5);
  SetType(etype);
  SetHeader(ev, 200, 20140731);
  SetTemperature(random+20.);

  //  Create and Fill the Trace objects
  Int_t base = 10;
  UShort_t buffer[nTimeStamps];
  Trace tr;
  for (Int_t t = 0; t < ntrace; t++) {
    for (int i=0;i<nTimeStamps;i++) {
      gRandom->Rannor(sigmat,sigmas);
      buffer[i] = base + 10*sigmat;
    }
    tr.Set(t,nTimeStamps,buffer);
    AddTrace(tr);
  }
  SetSize();
}  

//______________________________________________________________________________
void Waveform::SetHeader(Int_t i, Int_t run, Int_t date)
{
   fEvtHdr.Set(i, run, date);
}

//______________________________________________________________________________
void Waveform::SetSize() {
  int len = fTraces.size();
  fEventSize = sizeof(Waveform);
  for (int i=0;i<len;i++) {
    fEventSize += len*sizeof(fTraces[i]);
  }
}
