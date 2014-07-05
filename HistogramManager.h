#ifndef ROOT_HistogramManager
#define ROOT_HistogramManager

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Histogram Manager                                                              //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include "TDirectory.h"
#include "TH1.h"

#include "Waveform.h"


class TDirectory;

class HistogramManager {

private:
   TH1F  *fNtrace;
   TH1F  *fNTimeStamps;
   TH1F  *fTemperature;

public:
   HistogramManager(TDirectory *dir);
   virtual ~HistogramManager();

   void Hfill(Waveform *waveform);

   ClassDef(HistogramManager,1)  //Manages all histograms
};

#endif
