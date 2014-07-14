//
// Author: Geoffrey Mills 
// July 2014
//
// CptViewer
//
// A data viewing tool for multiwire waveform data.
//
// to build: (with ROOTSYS defined as usual)
// > make CptViewer
//
// to run:
// > ./CptViewer
//

#include <cstdlib>
#include <cstring>

#include "TFile.h"
#include "TStyle.h"
#include "TList.h"
#include "TGMenu.h"
#include "TApplication.h"
#include "TROOT.h"
#include "TGTableLayout.h"
#include "TGFrame.h"
#include "TList.h"
#include "TGWindow.h"
#include "TRootEmbeddedCanvas.h"
#include "TEllipse.h"
#include "TBox.h"
#include "TArrow.h"
#include "TPolyLine.h"
#include "TGButton.h"
#include "TCanvas.h"
#include "TRandom.h"
#include "TGDoubleSlider.h"
#include "TGaxis.h"
#include "Riostream.h"
#include "TH1.h"
#include "TH2.h"
#include "TGDockableFrame.h"
#include <TGFileDialog.h>
#include <TKey.h>

enum ETestCommandIdentifiers {
   M_FILE_OPEN,
   M_FILE_SAVE,
   M_FILE_SAVEAS,
   M_FILE_PRINT,
   M_FILE_PRINTSETUP,
   M_FILE_CLOSE,
   M_FILE_EXIT,

   M_VIEW_ENBL_DOCK,
   M_VIEW_ENBL_HIDE,
   M_VIEW_DOCK,
   M_VIEW_UNDOCK,

   M_HELP_CONTENTS,
   M_HELP_SEARCH,
   M_HELP_ABOUT,

   M_TEST_DLG,
   M_TEST_MSGBOX,
   M_TEST_SLIDER,
   M_TEST_SHUTTER,
   M_TEST_DIRLIST,
   M_TEST_FILELIST,
   M_TEST_PROGRESS,
   M_TEST_NUMBERENTRY,
   M_TEST_FONTDIALOG,
   M_TEST_NEWMENU


};


class TList;
class TCanvas;
class TRootEmbeddedCanvas;
class TGaxis;
class TGDoubleSlider;


class CptViewer : public TGMainFrame {

private:
  TList               *fCleanup;
  TGDockableFrame     *fMenuDock;
  TGMenuBar           *fMenuBar;
  TGPopupMenu         *fMenuFile;
  TGCompositeFrame    *fComposite ;
  TCanvas             *fCanvas;
  TRootEmbeddedCanvas *fHScaleCanvas, *fVScaleCanvas;
  TGDoubleSlider      *fXSlider;
  TGDoubleSlider      *fYSlider;
  TGDoubleSlider      *fZSlider;
  Int_t                sliderMax;
  TH1F                *fHist1;
  TH2F                *fHist2;
  Float_t              vmin0[3];
  Float_t              vmax0[3];

  TFile               *fFile;
  TList               *fHistList;

  Bool_t               fFileOpened;
  Bool_t               fHistOpened;
  Int_t                fCurrentHist;
  Int_t                fNHist;
  Int_t                fCurrentFile;
  

  Int_t                fVerbocity;

public:
  CptViewer(const TGWindow *win, const char * filename);
  virtual ~CptViewer();
   // slots
  void CloseWindow();
  void DoButton();
  void HandleMenu(Int_t id);
  void HandlePopup() { if (fVerbocity > 0) printf("menu popped up\n"); }
  void HandlePopdown() { if (fVerbocity > 0) printf("menu popped down\n"); }
  
  void Created() { Emit("Created()"); }

  void OpenFile();
  void CloseFile();
  void ProcessFile(const char * filename);

  void ResetHist(Int_t update = 1);
  void NextHist();
  void FirstHist();
  void LastHist();
  void PreviousHist();
  void SimulateHist();

  void DoSlider();
  void SetHistRange(Float_t xmin, Float_t ymin, Float_t zmin, 
		Float_t xmax, Float_t ymax, Float_t zmax, 
		Bool_t do_slider = kTRUE);
  void SetSliderRange(Float_t sxmin, Float_t symin, Float_t szmin, 
		Float_t sxmax, Float_t symax, Float_t szmax);
  ClassDef(CptViewer,0) 

};
