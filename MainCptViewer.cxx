
#include "CptViewer.h"

#include "TROOT.h"
#include "TStyle.h"
#include "TApplication.h"

//---- Main program ------------------------------------------------------------

int main(int argc, char **argv)
{
  TString filename;
  if (argc > 0) {
    filename = argv[1];
  } else {
    filename = ""; 
  }
  TApplication theApp("CptViewerApp", &argc, argv);
  
  if (gROOT->IsBatch()) {
    fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
    return 1;
  }

  CptViewer cptviewer(gClient->GetRoot(),filename.Data());
    
  theApp.Run();

  return 0;
}
