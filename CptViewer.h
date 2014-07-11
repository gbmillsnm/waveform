// @(#)root/test:$Id$
// Author: Brett Viren   04/15/2001

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
#include "TH2.h"

class TList;
class TCanvas;
class TRootEmbeddedCanvas;
class TGaxis;
class TGDoubleSlider;


class CptViewer : public TGMainFrame {

private:
  TList               *fCleanup;
  TCanvas             *fCanvas;
  TRootEmbeddedCanvas *fHScaleCanvas, *fVScaleCanvas;
  TGDoubleSlider      *fXSlider;
  TGDoubleSlider      *fYSlider;
  TGDoubleSlider      *fZSlider;
  Int_t                sliderMax;
  TH2F                *fHist;
  Float_t            vmin0[3];
  Float_t            vmax0[3];

  

public:
  CptViewer(const TGWindow *win);
  virtual ~CptViewer();
  void DoButton();
  void DoSlider();
  void SetHistRange(Float_t xmin, Float_t ymin, Float_t zmin, 
		Float_t xmax, Float_t ymax, Float_t zmax, 
		Bool_t do_slider = kTRUE);
  void SetSliderRange(Float_t sxmin, Float_t symin, Float_t szmin, 
		Float_t sxmax, Float_t symax, Float_t szmax);
  ClassDef(CptViewer,0) 

};
