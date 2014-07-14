#include "HistogramManager.h"

ClassImp(HistogramManager)

//______________________________________________________________________________
HistogramManager::HistogramManager(TDirectory *dir)
{
   // Create histogram manager object. Histograms will be created
   // in the "dir" directory.

   // Save current directory and cd to "dir".
   TDirectory *saved = gDirectory;
   dir->cd();

   // Histogram definitions (Note corresponding property in .h file)
   hNtrace      = new TH1F("hNtrace",    "Ntrace",100,575,625);
   hNTimeStamps = new TH1F("hNTimeStamps","NTimeStamp",0,5000,500);
   hTemperature = new TH1F("hTemperature","Temperature",100,15.,25.);
   hMaxV = new TH1F("hMaxV","Maximum Voltage",4095,0,4095);
   hMinV = new TH1F("hMinV","Minimum Voltage",200,0,200);
   hMean = new TH1F("hMean","Mean Voltage",500,0,500);
   hRMS = new TH1F("hRMS","RMS Voltage",500,0,500);

   // cd back to original directory
   saved->cd();
}

//______________________________________________________________________________
HistogramManager::~HistogramManager()
{
   // Clean up all histograms.

   // Nothing to do. Histograms will be deleted when the directory
   // in which tey are stored is closed.
}

//______________________________________________________________________________
void HistogramManager::Hfill(Waveform *waveform)
{
   // Fill histograms.
  
   hNtrace->Fill(waveform->GetNtrace());
   hTemperature->Fill(waveform->GetTemperature());
   std::vector<Trace> *traces = waveform->GetTraces();
   std::vector<Trace>::iterator itrace;
   for (itrace = traces->begin(); itrace != traces->end(); itrace++) {
     Int_t nTS = itrace->GetNTimeStamps();
     const UShort_t * buffer = itrace->GetADCValues();
     hNTimeStamps->Fill(nTS);
     Int_t imax = 0;
     Int_t imin = 0;
     Float_t vmax = -1;
     Float_t vmin = -1;
     for (int its = 0; its<nTS;its++) {
       if (buffer[its]>vmax) {
	 imax = its;
	 vmax = buffer[its];
       } else if (buffer[its]<vmin){	 
	 imin = its;
	 vmin = buffer[its];
       }
     }
     hMaxV->Fill(vmax);
     hMinV->Fill(vmin);
     hMean->Fill(itrace->GetMean());
     hRMS->Fill(itrace->GetRMS());
   }
}
