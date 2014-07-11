// @(#)root/test:$Id$
// Author: Brett Viren   04/15/2001

// guiviewer.cxx: GUI test program showing TGTableLayout widget manager,
// embedded ROOT canvases, axis and sliders.
// To run it do: make guiviewer; guiviewer

#include "CptViewer.h"


CptViewer::CptViewer(const TGWindow *win) : TGMainFrame(win,500,500)
{
  const UInt_t max_size = 300;

  // Memory management isn't the best in this class, but make a half
  // hearted attempts not to gush too much....
  fCleanup = new TList;

  TGCompositeFrame *table = new TGCompositeFrame(this,600,600,kSunkenFrame);
  fCleanup->Add(table);

  TGTableLayout* tlo = new TGTableLayout(table, 10, 6);
  table->SetLayoutManager(tlo);
  TGLayoutHints* loh = new TGLayoutHints(kLHintsTop|kLHintsLeft|
					 kLHintsExpandX|kLHintsExpandY);
  AddFrame(table,loh);

  TGTableLayoutHints* tloh;
  TGTableLayoutHints* tlox;
  TGTableLayoutHints* tloy;
  TGTableLayoutHints* tloz;

  cerr << "Making the canvas" << endl;
  // The Canvas
  TRootEmbeddedCanvas* recanvas =
    new TRootEmbeddedCanvas("Shapes",table,max_size,max_size);
  tloh = new TGTableLayoutHints(2,5,3,7,
				kLHintsExpandX|kLHintsExpandY |
				kLHintsShrinkX|kLHintsShrinkY |
				kLHintsFillX|kLHintsFillY);
  table->AddFrame(recanvas,tloh);

  fCanvas = recanvas->GetCanvas();

  cerr << "Making the X slider" << endl;
  // The sliders
  sliderMax = 300;
  // X axis
  fXSlider = new TGDoubleHSlider(table,sliderMax,kDoubleScaleBoth,100,
				 kHorizontalFrame,GetDefaultFrameBackground(),
				 kFALSE, kTRUE);
  tlox = new TGTableLayoutHints(2,5,0,1,
				kLHintsExpandX|kLHintsShrinkX|kLHintsFillX);
  table->AddFrame(fXSlider,tlox);
  fXSlider->Connect("PositionChanged()","CptViewer",this,"DoSlider()");

  cerr << "Making the X slider" << endl;
  // Y axis
  fYSlider = new TGDoubleHSlider(table,sliderMax,kDoubleScaleBoth,150,
				 kHorizontalFrame,GetDefaultFrameBackground(),
				 kFALSE, kTRUE);
  tloy = new TGTableLayoutHints(2,5,1,2,
				kLHintsExpandX|kLHintsShrinkX|kLHintsFillX);
  table->AddFrame(fYSlider,tloy);
  fYSlider->Connect("PositionChanged()","CptViewer",this,"DoSlider()");

  cerr << "Making the Z slider" << endl;
  // Z axis
  fZSlider = new TGDoubleVSlider(table,sliderMax,kDoubleScaleBoth,200,
				 kVerticalFrame,GetDefaultFrameBackground(),
				 kTRUE,kTRUE);
  tloz = new TGTableLayoutHints(0,1,3,7,
				kLHintsExpandY|kLHintsShrinkY|kLHintsFillY);
  table->AddFrame(fZSlider,tloz);
  fZSlider->Connect("PositionChanged()","CptViewer",this,"DoSlider()");

    // The shape buttons
   const char* shape_button_name[] = {
       "Next", "Prev", "First", "Last", "Zoom Out", "Zoom In", "Close"
   };

  cerr << "Making the shape buttons" << endl;
   UInt_t ind;
   for (ind = 0; ind < 7; ++ind) {
      TGTextButton* button =
          new TGTextButton(table,shape_button_name[ind],ind);
      tloh = new TGTableLayoutHints(5,6,ind+3,ind+1+3,
                                    kLHintsExpandX|kLHintsExpandY |
                                    kLHintsShrinkX|kLHintsShrinkY |
                                    kLHintsFillX|kLHintsFillY);
      table->AddFrame(button,tloh);
      button->Resize(100,button->GetDefaultHeight());
      button->Connect("Clicked()","CptViewer",this,"DoButton()");
   }

  cerr << "Making the control buttons" << endl;

   const char* ctrl_button_name[3][3] = {
      { "NW", "N", "NE" },
      { "W" , "C", "E" },
      { "SW", "S", "SE" }
   };
   UInt_t indx, indy;
   for (indx = 0; indx < 3; ++indx) for (indy = 0; indy < 3; ++indy) {
      TGTextButton* button = new TGTextButton(table,
                                              ctrl_button_name[indy][indx],
                                              (indx+1)*10 + indy +1);
      tloh = new TGTableLayoutHints(indx+2,indx+1+2, 7+indy,7+indy+1,
                                    kLHintsExpandX|kLHintsExpandY |
                                    kLHintsShrinkX|kLHintsShrinkY |
                                    kLHintsFillX|kLHintsFillY);
      table->AddFrame(button,tloh);
      button->Resize(100,button->GetDefaultHeight());
      button->Connect("Clicked()","CptViewer",this,"DoButton()");
   }

  cerr << "Done with buttons" << endl;

  table->Layout();

  // exit on close window
  Connect("CloseWindow()","TApplication",gApplication,"Terminate(=0)");

  MapSubwindows();
  Layout();
  MapWindow();

  cerr << "Making the X histogram" << endl;

  fHist = new TH2F("htest","Test Histogram",20,0,100,20,0,100);

  fCanvas->cd();
  fHist->Draw("surf1");

  vmin0[2] = fHist->GetMinimum();
  vmax0[2] = 100.;
  vmin0[0] = fHist->GetXaxis()->GetXmin();
  vmax0[0] = fHist->GetXaxis()->GetXmax();
  vmin0[1] = fHist->GetYaxis()->GetXmin();
  vmax0[1] = fHist->GetYaxis()->GetXmax();
  
  cerr << "Setting the Range" << endl;
  // Call this after sliders are setup
  SetHistRange(0,0,0,100,100,100);

}

CptViewer::~CptViewer()
{
  cerr << "~CprViewer called" << endl;

  fCanvas->Clear();
  delete fCanvas;
  fCleanup->Delete();
  delete fCleanup;
}

void CptViewer::DoButton()
{
  cerr << "DoButton called" << endl;
  TGButton* button = (TGButton*)gTQSender;
  UInt_t id = button->WidgetId();

  Float_t xmin, ymin, xmax, ymax, zmin, zmax;
  Float_t xdiff, ydiff, zdiff;

  zmin = fHist->GetMinimum();
  zmax = fHist->GetMaximum();
  xmin = fHist->GetXaxis()->GetXmin();
  xmax = fHist->GetXaxis()->GetXmax();
  ymin = fHist->GetYaxis()->GetXmin();
  ymax = fHist->GetYaxis()->GetXmax();

  xdiff = xmax - xmin;
  ydiff = ymax - ymin;
  zdiff = zmax - zmin;

  fCanvas->cd();

  switch (id) {
  case 0: {                   // Next
    //if (fHist) delete fHist;
    delete gDirectory->FindObject("htest");
    fHist = new TH2F("htest","Test Histogram",20,0,100,20,0,100);
    Float_t sigmax,sigmay;
    for (int i = 0;i<10000;i++) {
      gRandom->Rannor(sigmax,sigmay);
      fHist->Fill(50+10*sigmax,50+25*sigmay);
    }
    zmin = fHist->GetMinimum();
    zmax = fHist->GetMaximum();
    
    xmin = fHist->GetXaxis()->GetXmin();
    xmax = fHist->GetXaxis()->GetXmax();
    ymin = fHist->GetYaxis()->GetXmin();
    ymax = fHist->GetYaxis()->GetXmax();

    vmin0[0]=xmin;vmin0[1]=ymin;vmin0[2]=zmin;
    vmax0[0]=xmax;vmax0[1]=ymax;vmax0[2]=zmax;

    SetSliderRange(xmin,ymin,zmin,xmax,ymax,zmax);

    fCanvas->Clear();
    fHist->Draw("surf1");
    fCanvas->Modified();
    fCanvas->Update();
    break;
  }
  case 1: {                   // Prev
    //if (fHist) delete fHist;
    delete gDirectory->FindObject("htest");
    fHist = new TH2F("htest","Test Histogram",20,0,100,20,0,100);
    Float_t sigmax,sigmay;
    for (int i = 0;i<10000;i++) {
      gRandom->Rannor(sigmax,sigmay);
      fHist->Fill(50+10*sigmax,50+25*sigmay);
    }
    zmin = fHist->GetMinimum();
    zmax = fHist->GetMaximum();
    
    xmin = fHist->GetXaxis()->GetXmin();
    xmax = fHist->GetXaxis()->GetXmax();
    ymin = fHist->GetYaxis()->GetXmin();
    ymax = fHist->GetYaxis()->GetXmax();

    vmin0[0]=xmin;vmin0[1]=ymin;vmin0[2]=zmin;
    vmax0[0]=xmax;vmax0[1]=ymax;vmax0[2]=zmax;

    SetSliderRange(xmin,ymin,zmin,xmax,ymax,zmax);
//     fXSlider->SetRange(xmin,xmax);
//     fYSlider->SetRange(ymin,ymax);
//     fZSlider->SetRange(zmin,zmax);

    fCanvas->Clear();
    fHist->Draw("surf1");
    fCanvas->Modified();
    fCanvas->Update();
    break;
  }
  case 2: {                   // First
    //if (fHist) delete fHist;
    delete gDirectory->FindObject("htest");
    fHist = new TH2F("htest","Test Histogram",20,0,100,20,0,100);
    Float_t sigmax,sigmay;
    for (int i = 0;i<10000;i++) {
      gRandom->Rannor(sigmax,sigmay);
      fHist->Fill(50+10*sigmax,50+25*sigmay);
    }
    zmin = fHist->GetMinimum();
    zmax = fHist->GetMaximum();
    
    xmin = fHist->GetXaxis()->GetXmin();
    xmax = fHist->GetXaxis()->GetXmax();
    ymin = fHist->GetYaxis()->GetXmin();
    ymax = fHist->GetYaxis()->GetXmax();

    vmin0[0]=xmin;vmin0[1]=ymin;vmin0[2]=zmin;
    vmax0[0]=xmax;vmax0[1]=ymax;vmax0[2]=zmax;

    SetSliderRange(xmin,ymin,zmin,xmax,ymax,zmax);

    fCanvas->Clear();
    fHist->Draw("surf1");
    fCanvas->Modified();
    fCanvas->Update();
    break;
  }
  case 3: {                   // Last
    //if (fHist) delete fHist;
    delete gDirectory->FindObject("htest");
    fHist = new TH2F("htest","Test Histogram",20,0,100,20,0,100);
    Float_t sigmax,sigmay;
    for (int i = 0;i<10000;i++) {
      gRandom->Rannor(sigmax,sigmay);
      fHist->Fill(50+10*sigmax,50+25*sigmay);
    }
    zmin = fHist->GetMinimum();
    zmax = fHist->GetMaximum();    
    xmin = fHist->GetXaxis()->GetXmin();
    xmax = fHist->GetXaxis()->GetXmax();
    ymin = fHist->GetYaxis()->GetXmin();
    ymax = fHist->GetYaxis()->GetXmax();

    SetSliderRange(xmin,ymin,zmin,xmax,ymax,zmax);

    vmin0[0]=xmin;vmin0[1]=ymin;vmin0[2]=zmin;
    vmax0[0]=xmax;vmax0[1]=ymax;vmax0[2]=zmax;

    fCanvas->Clear();
    fHist->Draw("surf1");
    fCanvas->Modified();
    fCanvas->Update();
    break;
  }
  case 4:                     // Zoom out
    SetHistRange(xmin-xdiff*.1, ymin-ydiff*.1, zmin-zdiff*.1, 
	     xmax+xdiff*.1, ymax+ydiff*.1, zmax+zdiff*.1);
    break;
  case 5:                     // Zoom in
    SetHistRange(xmin+xdiff*.1, ymin+ydiff*.1, zmin+zdiff*.1, 
	     xmax-xdiff*.1, ymax-ydiff*.1, zmax-zdiff*.1);
    break;
  case 6:
    gApplication->Terminate(0);
    break;
  case 11:                    // nw
    SetHistRange(xmin-xdiff*.1, ymin+ydiff*.1, zmin, 
	     xmax-xdiff*.1, ymax+ydiff*.1, zmax);
    break;
  case 12:                    // w
    SetHistRange(xmin-xdiff*.1, ymin, zmin, 
	     xmax-xdiff*.1, ymax, zmax);
    break;
  case 13:                    // sw
    SetHistRange(xmin-xdiff*.1, ymin-ydiff*.1, zmin, 
	     xmax-xdiff*.1, ymax-ydiff*.1, zmax);
    break;
  case 23:                    // s
    SetHistRange(xmin, ymin-ydiff*.1, zmin, 
	     xmax, ymax-ydiff*.1, zmax);
    break;
  case 33:                    // se
    SetHistRange(xmin+xdiff*.1, ymin-ydiff*.1, zmin, 
	     xmax+xdiff*.1, ymax-ydiff*.1, zmax);
    break;
  case 32:                    // e
    SetHistRange(xmin+xdiff*.1, ymin, zmin, 
	     xmax+xdiff*.1, ymax, zmax);
    break;
  case 31:                    // ne
    SetHistRange(xmin+xdiff*.1, ymin+ydiff*.1, zmin, 
	     xmax+xdiff*.1, ymax+ydiff*.1, zmax);
    break;
  case 21:                    // n
    SetHistRange(xmin, ymin+ydiff*.1, zmin, 
	     xmax, ymax+ydiff*.1, zmax);
    break;
  case 22:                    // c
    SetHistRange(vmin0[0],vmin0[1],vmin0[2],vmax0[0],vmax0[1],vmax0[2]);
    break;
  } // end switch(id)
}

void CptViewer::DoSlider()
{
  cerr << "DoSlider called" << endl;

  float xmin, ymin, zmin, xmax, ymax,zmax;
  float sxmin, symin, szmin, sxmax, symax,szmax;
 
  fXSlider->GetPosition(sxmin,sxmax);
  fYSlider->GetPosition(symin,symax);
  fZSlider->GetPosition(szmin,szmax);

  cerr << "   DoSlider: "  <<
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
  cerr << "SetHistRange called: " << 
    xmin << " " << xmax << " " <<  ymin << " " << ymax << 
    " " << zmin << " " << zmax << " " << endl;

  fHist->SetMinimum(zmin);
  fHist->SetMaximum(zmax);
  fHist->GetXaxis()->SetRangeUser(xmin,xmax);
  fHist->GetYaxis()->SetRangeUser(ymin,ymax);

  if (doSlider) 
    SetSliderRange(xmin,ymin,zmin,xmax,ymax,zmax);

  fCanvas->Modified();
  fCanvas->Update();
  fClient->ForceRedraw();  //prevent GUI from being starved of redraws

}

void CptViewer::SetSliderRange(Float_t xmin, Float_t ymin, Float_t zmin, 
			 Float_t xmax, Float_t ymax, Float_t zmax)
{
  float sxmin,symin,szmin,sxmax,symax,szmax;

  cerr << "SetSliderRange called: " << 
    xmin << " " << xmax << " " <<  ymin << " " << ymax << 
    " " << zmin << " " << zmax << " " << endl;
  cerr << "SetSliderRange vmin,vmax0: " <<
    vmin0[0] << " " << vmin0[1] << " " << vmin0[2] << " " << 
    vmax0[0] << " " << vmax0[1] << " " << vmax0[2] << " " << endl;
  
  sxmin = (xmin-vmin0[0])*sliderMax/(vmax0[0]-vmin0[0]);
  sxmax = (xmax-vmin0[0])*sliderMax/(vmax0[0]-vmin0[0]);
  symin = (ymin-vmin0[1])*sliderMax/(vmax0[1]-vmin0[1]);
  symax = (ymax-vmin0[1])*sliderMax/(vmax0[1]-vmin0[1]);
  szmin = (zmin-vmin0[2])*sliderMax/(vmax0[2]-vmin0[2]);
  szmax = (zmax-vmin0[2])*sliderMax/(vmax0[2]-vmin0[2]);

  cerr << "SetSliderRange setting: " << 
    sxmin << " " << sxmax << " " <<  symin << " " << symax << 
    " " << szmin << " " << szmax << " " << endl;
  
  fXSlider->SetPosition(sxmin,sxmax);
  fYSlider->SetPosition(symin,symax);
  fZSlider->SetPosition(szmin,szmax);

  fCanvas->Modified();
  fCanvas->Update();
  fClient->ForceRedraw();  //prevent GUI from being starved of redraws

}

