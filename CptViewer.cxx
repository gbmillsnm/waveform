
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
//
//
// Notes:
//  This class creates a single window with two subwindows,
//  a dockable menu subwindow and a composite window that
//  contains a canvas, some slider controls, and control 
//  buttons. The sliders control the x,y,z viewing range 
//  of the histogram being viewed.
//
//  CptViewer inherits from TGMainFrame and is itself the 
//  top level window. The class is currently designed to 
//  read in a specific file format of TH2F of waveform 
//  channels. and display them in "surf1" format.
//
//  GBMills 7/13/2014
//

#include "CptViewer.h"


CptViewer::CptViewer(const TGWindow *win, const char * filename) : TGMainFrame(win,1000,600)
{

  fVerbocity = 0;

  SetCleanup(kDeepCleanup);

  const UInt_t max_size = 300;

  fFileOpened = kFALSE;
  fHistOpened = kFALSE;
  fNHist = 0;

  // Memory management isn't the best in this class, but make a half
  // hearted attempts not to gush too much....

  fCleanup = new TList;

  Connect("CloseWindow()", "CptViewer", this, "CloseWindow()");

  // Build the menu frame and popup menus
  fMenuDock = new TGDockableFrame(this);
  AddFrame(fMenuDock, new TGLayoutHints(kLHintsExpandX, 0, 0, 1, 0));
  fMenuDock->SetWindowName("CptViewer Menu");
  fCleanup->Add(fMenuDock);

  // a popup menu 
  fMenuFile = new TGPopupMenu(gClient->GetRoot()); 
 
  // adding menu entries 
  fMenuFile->AddEntry("&Open File...",M_FILE_OPEN); 
  fMenuFile->AddEntry("&Save",M_FILE_SAVE); 
  fMenuFile->AddEntry("S&ave as...",M_FILE_SAVEAS); 
  fMenuFile->AddEntry("&Close File",M_FILE_CLOSE); 
  fMenuFile->AddSeparator();
  fMenuFile->AddEntry("&Print", M_FILE_PRINT);
  fMenuFile->AddEntry("P&rint setup...", M_FILE_PRINTSETUP);
  fMenuFile->AddSeparator();
  fMenuFile->AddEntry("E&xit", M_FILE_EXIT);

  fMenuFile->DisableEntry(M_FILE_SAVEAS);
  fMenuFile->DisableEntry(M_FILE_SAVE);
  fMenuFile->DisableEntry(M_FILE_CLOSE);
  fMenuFile->HideEntry(M_FILE_PRINT);
  fCleanup->Add(fMenuFile);

  TGLayoutHints *fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
  TGLayoutHints *fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);

  // menu bar 
  fMenuBar = new TGMenuBar(fMenuDock,1,1,kHorizontalFrame);
  fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);

  fMenuDock->AddFrame(fMenuBar, fMenuBarLayout);

  char chandle[100];
  sprintf(chandle,"HandleMenu(=%d)",M_VIEW_UNDOCK);
  if (fVerbocity > 10) cerr << "chandle is " << chandle << endl;
  fMenuDock->Connect("Undocked()", "CptViewer", this,chandle);
  fMenuFile->Connect("Activated(Int_t)", "CptViewer", this,"HandleMenu(Int_t)");
  fMenuFile->Connect("PoppedUp()", "CptViewer", this, "HandlePopup()");
  fMenuFile->Connect("PoppedDown()", "CptViewer", this, "HandlePopdown()");

  // Build the composite window with 
  TGCompositeFrame *fComposite = new TGCompositeFrame(this,1000,600,kSunkenFrame);
  fCleanup->Add(fComposite);

  TGTableLayout* tlo = new TGTableLayout(fComposite, 10, 10);
  fComposite->SetLayoutManager(tlo);
  TGLayoutHints* loh = new TGLayoutHints(kLHintsTop|kLHintsLeft|
					 kLHintsExpandX|kLHintsExpandY);
  AddFrame(fComposite,loh);

  TGTableLayoutHints* tloh;
  TGTableLayoutHints* tlox;
  TGTableLayoutHints* tloy;
  TGTableLayoutHints* tloz;

  if (fVerbocity > 0) cerr << "Making the canvas" << endl;
  // The Canvas
  TRootEmbeddedCanvas* recanvas =
    new TRootEmbeddedCanvas("Shapes",fComposite,max_size,max_size);
  tloh = new TGTableLayoutHints(2,9,0,8,
				kLHintsExpandX|kLHintsExpandY |
				kLHintsShrinkX|kLHintsShrinkY |
				kLHintsFillX|kLHintsFillY);
  fComposite->AddFrame(recanvas,tloh);

  fCanvas = recanvas->GetCanvas();

  if (fVerbocity > 0) cerr << "Making the X slider" << endl;
  // The sliders
  sliderMax = 300;
  // X axis
  fXSlider = new TGDoubleHSlider(fComposite,sliderMax,kDoubleScaleBoth,100,
				 kHorizontalFrame,GetDefaultFrameBackground(),
				 kFALSE, kTRUE);
  tlox = new TGTableLayoutHints(2,9,8,9,
				kLHintsExpandX|kLHintsShrinkX|kLHintsFillX);
  fComposite->AddFrame(fXSlider,tlox);
  fXSlider->Connect("PositionChanged()","CptViewer",this,"DoSlider()");

  if (fVerbocity > 0) cerr << "Making the X slider" << endl;
  // Y axis
  fYSlider = new TGDoubleHSlider(fComposite,sliderMax,kDoubleScaleBoth,150,
				 kHorizontalFrame,GetDefaultFrameBackground(),
				 kFALSE, kTRUE);
  tloy = new TGTableLayoutHints(2,9,9,10,
				kLHintsExpandX|kLHintsShrinkX|kLHintsFillX);
  fComposite->AddFrame(fYSlider,tloy);
  fYSlider->Connect("PositionChanged()","CptViewer",this,"DoSlider()");

  if (fVerbocity > 0) cerr << "Making the Z slider" << endl;
  // Z axis
  fZSlider = new TGDoubleVSlider(fComposite,sliderMax,kDoubleScaleBoth,200,
				 kVerticalFrame,GetDefaultFrameBackground(),
				 kTRUE,kTRUE);
  tloz = new TGTableLayoutHints(0,1,0,8,
				kLHintsExpandY|kLHintsShrinkY|kLHintsFillY);
  fComposite->AddFrame(fZSlider,tloz);
  fZSlider->Connect("PositionChanged()","CptViewer",this,"DoSlider()");

  // The shape buttons
  const char* shape_button_name[] = {
    "Next", "Prev", "First", "Last", "Zoom Out", "Zoom In", "Reset", "Close"
  };

  if (fVerbocity > 0) cerr << "Making the shape buttons" << endl;
  UInt_t ind;
  for (ind = 0; ind < 8; ++ind) {
    TGTextButton* button =
      new TGTextButton(fComposite,shape_button_name[ind],ind);
    tloh = new TGTableLayoutHints(9,10,ind,ind+1,
				  kLHintsFillX|kLHintsFillX|
				  kLHintsShrinkY|kLHintsFillY );
    fComposite->AddFrame(button,tloh);
    //    button->Resize(100,button->GetDefaultHeight());
    button->Connect("Clicked()","CptViewer",this,"DoButton()");
  }

  if (fVerbocity > 0) cerr << "Done with buttons" << endl;

  //  fComposite->Layout();

  if (fVerbocity > 0) cerr << "Mapping Subwindows" << endl;
  MapSubwindows();
  if (fVerbocity > 0) cerr << "Doing the layout" << endl;
  Layout();
  if (fVerbocity > 0) cerr << "Mapping the window" << endl;
  MapWindow();

  if (fVerbocity > 0) cerr << "Making the histogram" << endl;

  gStyle->SetPalette(1);
  gStyle->SetOptStat("");

  fCanvas->cd();
  if (strcmp(filename,"") == 0) {
    fHist2 = new TH2F("htest","Test Histogram",20,0,100,20,0,100);
    fHist2->Draw("surf1");
  } else {
    if (fVerbocity > 0) cerr << "Opening the file " << filename << endl;
    ProcessFile(filename);
  }
  ResetHist(1);
  
}

CptViewer::~CptViewer()
{
  if (fVerbocity > 0) cerr << "~CprViewer called" << endl;

  //   fCanvas->Clear();
  //   delete fCanvas;
  fCleanup->Delete();
  delete fCleanup;
}
void CptViewer::CloseWindow()
{
  // Got close message for this MainFrame. Terminates the application.

  gApplication->Terminate();
}

void CptViewer::HandleMenu(Int_t id)
{
  // Handle menu items.

  switch (id) {

  case M_FILE_OPEN:
    OpenFile();
    break;

  case M_FILE_CLOSE:
    CloseFile();
    break;

  case M_FILE_SAVE:
    {
      if (fVerbocity > 0) printf("M_FILE_SAVE\n");
    }
    break;

  case M_FILE_PRINT:
    {
      if (fHistOpened) {
	fCanvas->SaveAs("wave.jpg");
      }
      if (fVerbocity > 0) printf("M_FILE_PRINT\n"); 
      if (fVerbocity > 0) printf("Hiding itself, select \"Print Setup...\" to enable again\n");
      fMenuFile->HideEntry(M_FILE_PRINT);
      break;
    }

  case M_FILE_PRINTSETUP:
    if (fVerbocity > 0) printf("M_FILE_PRINTSETUP\n");
    if (fVerbocity > 0) printf("Enabling \"Print\"\n");
    //          fMenuFile->EnableEntry(M_FILE_PRINT);
    break;

  case M_FILE_EXIT:
    CloseWindow();   // terminate theApp no need to use SendCloseMessage()
    break;

  case M_TEST_DLG:
    //          new TestDialog(gClient->GetRoot(), this, 400, 200);
    break;

  case M_TEST_MSGBOX:
    //          new TestMsgBox(gClient->GetRoot(), fMain, 400, 200);
    break;

  case M_TEST_SLIDER:
    //          new TestSliders(gClient->GetRoot(), fMain, 400, 200);
    break;

  case M_TEST_SHUTTER:
    //          new TestShutter(gClient->GetRoot(), fMain, 400, 200);
    break;

  case M_TEST_DIRLIST:
    //          new TestDirList(gClient->GetRoot(), fMain, 400, 200);
    break;

  case M_TEST_FILELIST:
    //          new TestFileList(gClient->GetRoot(), fMain, 400, 200);
    break;

  case M_TEST_PROGRESS:
    //          new TestProgress(gClient->GetRoot(), fMain, 600, 300);
    break;

  case M_TEST_NUMBERENTRY:
    //          new EntryTestDlg(gClient->GetRoot(), fMain);
    break;

  case M_TEST_FONTDIALOG:
    //          {
    //             TGFontDialog::FontProp_t prop;
    //             new TGFontDialog(gClient->GetRoot(), fMain, &prop);
    //             if (prop.fName != "")
    //                printf("Selected font: %s, size %d, italic %s, bold %s, color 0x%lx, align %u\n",
    //                       prop.fName.Data(), prop.fSize, prop.fItalic ? "yes" : "no",
    //                       prop.fBold ? "yes" : "no", prop.fColor, prop.fAlign);
    //          }
    break;

    //       case M_TEST_NEWMENU:
    //          {
    //             if (fMenuTest->IsEntryChecked(M_TEST_NEWMENU)) {
    //                HandleMenu(M_NEW_REMOVEMENU);
    //                return;
    //             }
    //             fMenuTest->CheckEntry(M_TEST_NEWMENU);
    //             TGPopupMenu *p = fMenuBar->GetPopup("Test");
    //             fMenuBar->AddPopup("New 1", fMenuNew1, fMenuBarItemLayout, p);
    //             p = fMenuBar->GetPopup("Help");
    //             fMenuBar->AddPopup("New 2", fMenuNew2, fMenuBarItemLayout, p);
    //             fMenuBar->MapSubwindows();
    //             fMenuBar->Layout();

    //             TGMenuEntry *e = fMenuTest->GetEntry("Add New Menus");
    //             fMenuTest->AddEntry("Remove New Menus", M_NEW_REMOVEMENU, 0, 0, e);
    //          }
    //          break;

    //    case M_NEW_REMOVEMENU:
    //          {
    //             fMenuBar->RemovePopup("New 1");
    //             fMenuBar->RemovePopup("New 2");
    //             fMenuBar->Layout();
    //             fMenuTest->DeleteEntry(M_NEW_REMOVEMENU);
    //             fMenuTest->UnCheckEntry(M_TEST_NEWMENU);
    //          }
    //          break;

  case M_VIEW_ENBL_DOCK:
    //          fMenuDock->EnableUndock(!fMenuDock->EnableUndock());
    //          if (fMenuDock->EnableUndock()) {
    //             fMenuView->CheckEntry(M_VIEW_ENBL_DOCK);
    //             fMenuView->EnableEntry(M_VIEW_UNDOCK);
    //          } else {
    //             fMenuView->UnCheckEntry(M_VIEW_ENBL_DOCK);
    //             fMenuView->DisableEntry(M_VIEW_UNDOCK);
    //          }
    break;

  case M_VIEW_ENBL_HIDE:
    //          fMenuDock->EnableHide(!fMenuDock->EnableHide());
    //          if (fMenuDock->EnableHide()) {
    //             fMenuView->CheckEntry(M_VIEW_ENBL_HIDE);
    //          } else {
    //             fMenuView->UnCheckEntry(M_VIEW_ENBL_HIDE);
    //          }
    break;

  case M_VIEW_DOCK:
    //          fMenuDock->DockContainer();
    //          fMenuView->EnableEntry(M_VIEW_UNDOCK);
    //          fMenuView->DisableEntry(M_VIEW_DOCK);
    break;

  case M_VIEW_UNDOCK:
    //          fMenuDock->UndockContainer();
    //          fMenuView->EnableEntry(M_VIEW_DOCK);
    //          fMenuView->DisableEntry(M_VIEW_UNDOCK);
    break;

  default:
    if (fVerbocity > 0) printf("Default Menu item %d selected\n", id);
    break;
  }
}

void CptViewer::DoButton()
{
  if (fVerbocity > 0) cerr << "DoButton called" << endl;
  TGButton* button = (TGButton*)gTQSender;
  UInt_t id = button->WidgetId();

  Float_t zoom = 2.;
  Float_t xmin, ymin, xmax, ymax, zmin, zmax;
  Float_t xdiff, ydiff, zdiff, xave, yave, zave;

  zmin = fHist2->GetMinimum();
  zmax = fHist2->GetMaximum();
  xmin = fHist2->GetXaxis()->GetBinLowEdge(fHist2->GetXaxis()->GetFirst());
  xmax = fHist2->GetXaxis()->GetBinUpEdge(fHist2->GetXaxis()->GetLast());
  ymin = fHist2->GetYaxis()->GetBinLowEdge(fHist2->GetYaxis()->GetFirst());
  ymax = fHist2->GetYaxis()->GetBinUpEdge(fHist2->GetYaxis()->GetLast());

  xdiff = xmax - xmin;
  ydiff = ymax - ymin;
  zdiff = zmax - zmin;
  xave = (xmax + xmin)/2.0;
  yave = (ymax + ymin)/2.0;
  zave = (zmax + zmin)/2.0;

  fCanvas->cd();

  switch (id) {
  case 0: { // Next
    NextHist();
    break;
  }

  case 1: {                   // Prev
    PreviousHist();
    break;
  }
  case 2: {                   // First
    FirstHist();
    break;
  }
  case 3: {                   // Last
    LastHist();
    break;
  }
  case 4:                     // Zoom out
    SetHistRange(xave-0.5*xdiff*zoom, yave-0.5*ydiff*zoom, zave-0.5*zdiff*zoom, 
		 xave+0.5*xdiff*zoom, yave+0.5*ydiff*zoom, zave+0.5*zdiff*zoom);
    break;
  case 5:                     // Zoom in
    SetHistRange(xave-0.5*xdiff/zoom, yave-0.5*ydiff/zoom, zave-0.5*zdiff/zoom, 
		 xave+0.5*xdiff/zoom, yave+0.5*ydiff/zoom, zave+0.5*zdiff/zoom);
    break;
  case 6:
    ResetHist(0);
    break;
  case 7:
    gApplication->Terminate(0);
    break;
  } // end switch(id)
}

void CptViewer::DoSlider()
{
  if (fVerbocity > 10) cerr << "DoSlider called" << endl;

  float xmin, ymin, zmin, xmax, ymax,zmax;
  float sxmin, symin, szmin, sxmax, symax,szmax;
 
  fXSlider->GetPosition(sxmin,sxmax);
  fYSlider->GetPosition(symin,symax);
  fZSlider->GetPosition(szmin,szmax);

  if (fVerbocity > 100) cerr << "   DoSlider: "  <<
    sxmin << " " << sxmax << " " <<  symin << " " << symax << 
    " " << szmin << " " << szmax << " " << endl;

  xmin = sxmin*(vmax0[0]-vmin0[0])/sliderMax + vmin0[0];
  xmax = sxmax*(vmax0[0]-vmin0[0])/sliderMax + vmin0[0];
  ymin = symin*(vmax0[1]-vmin0[1])/sliderMax + vmin0[1];
  ymax = symax*(vmax0[1]-vmin0[1])/sliderMax + vmin0[1];
  zmin = szmin*(vmax0[2]-vmin0[2])/sliderMax + vmin0[2];
  zmax = szmax*(vmax0[2]-vmin0[2])/sliderMax + vmin0[2];

  SetHistRange(xmin,ymin,zmin,xmax,ymax,zmax,kFALSE);
}


void CptViewer::SetHistRange(Float_t xmin, Float_t ymin, Float_t zmin, 
			     Float_t xmax, Float_t ymax, Float_t zmax,
			     Bool_t doSlider)
{
  if (fVerbocity > 10) cerr << "SetHistRange called: " << 
    xmin << " " << xmax << " " <<  ymin << " " << ymax << 
    " " << zmin << " " << zmax << " " << endl;

  fHist2->SetMinimum(zmin);
  fHist2->SetMaximum(zmax);
  fHist2->GetXaxis()->SetRangeUser(xmin,xmax);
  fHist2->GetYaxis()->SetRangeUser(ymin,ymax);

  if (doSlider) {
    SetSliderRange(xmin,ymin,zmin,xmax,ymax,zmax);
  } else {
    fCanvas->Modified();
    fCanvas->Update();
    fClient->ForceRedraw();  //prevent GUI from being starved of redraws
  }
}

void CptViewer::SetSliderRange(Float_t xmin, Float_t ymin, Float_t zmin, 
			       Float_t xmax, Float_t ymax, Float_t zmax)
{
  float sxmin,symin,szmin,sxmax,symax,szmax;

  if (fVerbocity > 10) cerr << "SetSliderRange called: " << endl;
  if (fVerbocity > 100) cerr << "   xysmin and xyzmax values: " << 
    xmin << " " << xmax << " " <<  ymin << " " << ymax << 
    " " << zmin << " " << zmax << " " << endl;
  if (fVerbocity > 100) cerr << "  vmin,vmax0: " <<
    vmin0[0] << " " << vmin0[1] << " " << vmin0[2] << " " << 
    vmax0[0] << " " << vmax0[1] << " " << vmax0[2] << " " << endl;
  
  sxmin = (xmin-vmin0[0])*sliderMax/(vmax0[0]-vmin0[0]);
  sxmax = (xmax-vmin0[0])*sliderMax/(vmax0[0]-vmin0[0]);
  symin = (ymin-vmin0[1])*sliderMax/(vmax0[1]-vmin0[1]);
  symax = (ymax-vmin0[1])*sliderMax/(vmax0[1]-vmin0[1]);
  szmin = (zmin-vmin0[2])*sliderMax/(vmax0[2]-vmin0[2]);
  szmax = (zmax-vmin0[2])*sliderMax/(vmax0[2]-vmin0[2]);

  if (fVerbocity > 100) cerr << "SetSliderRange setting: " << 
    sxmin << " " << sxmax << " " <<  symin << " " << symax << 
    " " << szmin << " " << szmax << " " << endl;
  
  fXSlider->SetPosition(sxmin,sxmax);
  fYSlider->SetPosition(symin,symax);
  fZSlider->SetPosition(szmin,szmax);

  fCanvas->Modified();
  fCanvas->Update();
  fClient->ForceRedraw();  //prevent GUI from being starved of redraws
}

void CptViewer::ResetHist(Int_t update) {
  if (fVerbocity > 10) cerr << "ResetHist called: " << endl;
  if ( update == 1 ) {
    vmin0[2] = fHist2->GetMinimum();
    vmax0[2] = fHist2->GetMaximum();
    vmin0[0] = fHist2->GetXaxis()->GetXmin();
    vmax0[0] = fHist2->GetXaxis()->GetXmax();
    vmin0[1] = fHist2->GetYaxis()->GetXmin();
    vmax0[1] = fHist2->GetYaxis()->GetXmax();
    if ( vmin0[2] == 0.0 && vmax0[2] == 0.0 )
      vmax0[2] = 1.0;
  }
  
  if (fVerbocity > 0) cerr << "Setting the Range" << endl;

  SetHistRange(vmin0[0],vmin0[1],vmin0[2],
	       vmax0[0],vmax0[1],vmax0[2]);
}

void CptViewer::SimulateHist() {
  if (fVerbocity > 10) cerr << "SimulateHist called: " << endl;
  float xmin, ymin, zmin, xmax, ymax,zmax;
  //if (fHist2) delete fHist2;
  delete gDirectory->FindObject("htest");
  fHist2 = new TH2F("htest","Test Histogram",20,0,100,20,0,100);
  Float_t sigmax,sigmay;
  for (int i = 0;i<10000;i++) {
    gRandom->Rannor(sigmax,sigmay);
    fHist2->Fill(50+10*sigmax,50+25*sigmay);
  }
  zmin = fHist2->GetMinimum();
  zmax = fHist2->GetMaximum();
  
  xmin = fHist2->GetXaxis()->GetXmin();
  xmax = fHist2->GetXaxis()->GetXmax();
  ymin = fHist2->GetYaxis()->GetXmin();
  ymax = fHist2->GetYaxis()->GetXmax();
  
  vmin0[0]=xmin;vmin0[1]=ymin;vmin0[2]=zmin;
  vmax0[0]=xmax;vmax0[1]=ymax;vmax0[2]=zmax;
  
  SetSliderRange(xmin,ymin,zmin,xmax,ymax,zmax);
  
  fCanvas->Clear();
  fHist2->Draw("surf1");
  fCanvas->Modified();
  fCanvas->Update();
}

void CptViewer::NextHist() {
  if (fVerbocity > 10) cerr << "NextHist called: " << endl;
  if (fHistOpened) {
    fHist2 = (TH2F *) fHistList->After(fHist2);
    if (fHist2 == 0) {
      fHist2 = (TH2F *) fHistList->Last();
      if (fVerbocity > 0) 
	cerr << "fHist2 set to Last()" << endl;
    }
    fCanvas->Clear();
    fHist2->Draw("surf1");
    ResetHist(1);
  }      
}

void CptViewer::FirstHist() {
  if (fVerbocity > 10) cerr << "FirstHist called: " << endl;
  if (fHistOpened) {
    fHist2 = (TH2F *) fHistList->First();
    fCanvas->Clear();
    fHist2->Draw("surf1");
    ResetHist(1);
  }      
}

void CptViewer::LastHist() {
  if (fVerbocity > 10) cerr << "LastHist called: " << endl;
  if (fHistOpened) {
    fHist2 = (TH2F *) fHistList->Last();
    fCanvas->Clear();
    fHist2->Draw("surf1");
    ResetHist(1);
  }
}

void CptViewer::PreviousHist() {
  if (fVerbocity > 10)cerr << "PreviousHist called: " << endl;
  if (fHistOpened) {
    fHist2 = (TH2F *) fHistList->Before(fHist2);
    if (fHist2 == 0) {
      fHist2 = (TH2F *) fHistList->First();
      if (fVerbocity > 10) 
	cerr << "fHist2 set to First()" << endl;
    }
    fCanvas->Clear();
    fHist2->Draw("surf1");
    ResetHist(1);
  }
}
void CptViewer::ProcessFile(const char * filename) {

  if (fVerbocity > 0) printf("Open file: %s \n", filename);
  fFile = new TFile(filename);
  fFileOpened = kTRUE;
  fNHist = 0;
  fMenuFile->EnableEntry(M_FILE_CLOSE);

  if (fVerbocity > 10) cerr << "Enabled Close: " << 
    fMenuFile->IsEntryEnabled(M_FILE_CLOSE) << endl;

  static TString th1f = "TH1F";
  static TString th2f = "TH2F";
  fHistList = new TList();
  TKey *key;
  TIter next(fFile->GetListOfKeys());
  while ((key=(TKey*)next())) {
    if (fVerbocity > 0) cerr << "key: " << key->GetName() << " points to an object of class: "
			     << key->GetClassName() << " at " << key->GetSeekKey() << endl;
    TString item = key->GetName();
    TString itemClass = key->GetClassName();
    if ( itemClass == th2f ) {
      if (fVerbocity > 0) cerr << "  Found TH2F " << key->GetName() << endl;
      fHistList->Add(gDirectory->Get(item.Data()));
      fHistOpened = kTRUE;
      fNHist++;
    }
  }
  if (fHistOpened) {
    fHist2 = (TH2F *) fHistList->First();
    fCanvas->Clear();
    fHist2->Draw("surf1");
    ResetHist(1);
  }
}

void CptViewer::OpenFile() {

  if (fVerbocity > 10) cerr << "OpenFile called: " << endl;
  static const char *filetypes[] = 
    { "All files",     "*",
      "ROOT files",    "*.root",
      "ROOT macros",   "*.C",
      "Text files",    "*.[tT][xX][tT]",
      0,               0 };

  static TString dir(".");
			
  TGFileInfo fi;
			
  fi.fFileTypes = filetypes;
  fi.fIniDir    = StrDup(dir);
  if (fVerbocity > 10) printf("fIniDir = %s\n", fi.fIniDir);
  new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fi);
  if (fVerbocity > 10) cerr << " fi is " << fi.fFilename <<endl ;
  dir = fi.fIniDir;
  if (fFileOpened)  {
    fFile->Close();
    fHistList->Clear();
    fNHist = 0;
  }
  if (strcmp(fi.fFilename,"") != 0) {
    ProcessFile(fi.fFilename);
  }
}

void CptViewer::CloseFile() {
  if (fVerbocity > 0) cerr << "CloseFile called: " << endl;
  if (fFileOpened) {
    fFile->Close();
    fFileOpened = kFALSE;
    fHistOpened = kFALSE;
  }
  fMenuFile->DisableEntry(M_FILE_CLOSE);
  if (fVerbocity > 10) cerr << "Disabled Close: " << 
    fMenuFile->IsEntryEnabled(M_FILE_CLOSE) << endl;
}
