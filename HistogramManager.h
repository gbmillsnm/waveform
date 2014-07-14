#ifndef ROOT_HistogramManager
#define ROOT_HistogramManager

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Histogram Manager                                                              //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "TMath.h"
#include "TDirectory.h"
#include "TH1.h"

#include "Waveform.h"


class TDirectory;

class HistogramManager {

private:
   TH1F  *hNtrace;
   TH1F  *hNTimeStamps;
   TH1F  *hTemperature;
   TH1F  *hMaxV;
   TH1F  *hMinV;
   TH1F  *hMean;
   TH1F  *hRMS;

public:
   HistogramManager(TDirectory *dir);
   virtual ~HistogramManager();

   void Hfill(Waveform *waveform);

   ClassDef(HistogramManager,1)  //Manages all histograms
};

#endif
