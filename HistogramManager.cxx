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

   fNtrace      = new TH1F("hNtrace",    "Ntrace",100,575,625);
   fNTimeStamps = new TH1F("hNseg",      "Nseg",1000,5000,10000);
   fTemperature = new TH1F("hTemperature","Temperature",100,19.5,20.5);

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

   fNtrace->Fill(waveform->GetNtrace());
   fNTimeStamps->Fill(waveform->GetNTimeStamps());
   fTemperature->Fill(waveform->GetTemperature());

   for (Int_t itrace = 0; itrace < waveform->GetNtrace(); itrace++) {
      Trace *trace = (Trace*)waveform->GetTraces()->UncheckedAt(itrace);
   }
}
