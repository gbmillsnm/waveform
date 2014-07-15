// Geoff Mills   July 2014
// Modified by Alex Mills  7.12.2014

// Data format: (these files are probably written by using the boost serialization macro's)
// a useful unix tool is hexdump
//       hex      decimal 
// Note: 0x4000 = 16384 
//       0x5000 = 20480
//       0xffff = 65535
//       0xf000 = 61440
// 
// line       d0      d1      d2      d3      d4      d5      d6      d7   Comments
//
// 0000000    ffff    ffff    ffff    f007    f095    fd32    f000    f001 0xe (12) header words,0xf001 => ev1
// 0000010    f000    f001    fef2    f22c    4000    0802    0803    0804 0x4000 word marks ch 0 start, ev1   
// 0000020    0804    0804    0803    0802    0802    0802    0802    0801 (the header words marked 0xf... )
// 0000030    0802    0801    0803    0802    0802    0803    0802    0803
// 0000040    0803    0802    0802    0803    0802    0802    0803    0803
// 0000050    0803    0803    0802    0803    0803    0802    0803    0802
// 0000060    0803    0803    0802    0802    0802    0803    0802    0800
//              ...     ...            ...             ...
// 0004af0    0803    0803    0803    0802    0802    0802    0801    0803
// 0004b00    0802    0802    0803    0802    0802    0802    0802    0803
// 0004b10    5000    4001    07ff    07ff    0800    0800    0800    07ff 0x5000: Ch0 End, 0x4001: Ch1 Start
// 0004b20    0800    0800    07fe    07ff    07ff    07fe    07fe    07ff
// 0004b30    07ff    07fe    07ff    07ff    0800    07ff    0800    07ff
// 0004b40    07ff    0801    0800    07fe    0800    0800    07ff    07fe
//  ch2 through ch63 data ...
// 012ba50    0802    0802    0802    0803    0802    0802    0803    0803
// 012ba60    0803    0802    0802    0802    0802    0801    0801    0802
// 012ba70    0801    503f    0000    0000    0000    e000    ffff    ffff 0x503f: Ch63 end, 0x0000: ev1 end
// 012ba80    ffff    f007    f095    fd2d    f000    f002    f000    f001 the 0xf002 signifies ev2
// 012ba90    feee    f9b5    4000    0803    0803    0802    0803    0802 0x4000: ch 0, ev2 start
// 012baa0    0800    0803    0804    0804    0800    07ff    0803    0801
// etc.
//  until the last event: (0xf006 in this case)
// 05da440    0802    0801    0801    0803    0805    0803    0800    07ff
// 05da450    0801    0802    0802    0802    0803    0801    0800    0802
// 05da460    0803    0802    503f    0000    0000    e000    ffff    ffff
// 05da470    ffff    f007    f095    fd2c    f000    f006    f000    f001 the 0xf006 signifies ev6
// 05da480    feed    f590    4000    0802    0802    0802    0803    0801
// 05da490    0802    0803    0801    0801    0800    0801    0803    0803
//                   .....
// 05db8b0    0801    0801    0802    0804    0803    0804    0803    0803
// 05db8c0    0802    0801    0803    0801    0801    0803    07ff    0800
// 05db8d0    0802    0803    0803    0803    0802    0802    0803    0801
// 05db8e0    0802    0803    0803    0802    0802    0801    0801    0801
// 05db8f0    0802    0802    0802    0802    0804    0804    0802    0801
// 05db900    0801    0801    0800    0801    0802    0801                
// 05db90c
//
// the last event in this file is truncated so that only ch0 is included
//

#include <cstdio>
#include <vector>
#include <iostream>
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TMath.h"

// extracts the top byte (4 bits) of a 16 bit word
inline UShort_t topByte(UShort_t s)
{
  return ((s&0xf000) >> 12);
}

// extracts the lower 3 bytes (12 bits) of a 16 bit word
inline UShort_t lowerBytes(UShort_t s)
{
  return (s&0x0fff);
}

// class EventHeader {
// public:
//   int length;
//   int * data;
// };

// class Channel {
// public:
//   int chNumber;
//   int nData;
//   unsigned short * data;
// };

// class Event {
// public:
//   int nChannels;
//   EventHeader header;
//   std::vector<Channel> vChannel;
// };

void ReadEvents(int run, int verbose = 0)
{
  char file_name[200];
  //  sprintf(file_name, "data/xmit_exttrig_bin_%d.dat", run);
  sprintf(file_name, "data/xmit_exttrig_bin_%d.dat", run);
  if (verbose > 0)  cout<<"opening "<<file_name<<"\n";

  FILE *finput;
  finput = fopen(file_name, "rb");

  if (!finput) {
    printf("Could not open.");
    return;
  }

  // get length of file
  fseek (finput, 0 , SEEK_END);
  long int lSize = ftell (finput);
  rewind (finput);

  if (verbose > 0)  cout << "File size is " << lSize << " bytes" << endl;

  UShort_t word_16bit;

  // Scan through first couple of events to get number of channels and header size

  size_t bytes;

  long int headerBytes = 0;
  long int byteCount = 0;
  long int nDataTotal = 0;
  long int chBegin = 0;
  long int chEnd = 0;
  long int chLength = 0;
  long int evEnd = 0;
  long int evEndOld = 0;
  long int evLength = 0;

  int nData = 0;
  int nDataOld = 0;
  int nChannels = 0;
  int nChannelsOld = 0;
  int nEvents = 0;
  int channel = 0;
  int event = 0;
  int rmDataPt = 0;
  int wordSize = sizeof(UShort_t);

  bool validData = false;

  double sum = 0;
  double sumsquare = 0;

  vector<double> baselines;
  vector<double> RMSs;

  rewind (finput);
  if (verbose>0)  cout << "Starting at " << ftell(finput)<< " in file " << file_name << endl;

  while (byteCount < lSize) {
    bytes = fread(&word_16bit, sizeof(UShort_t), 1, finput);
    if (bytes != 1) {
      if (verbose>0)  cout << "fread returns " << bytes << " bytes!!" << endl;
    }
    byteCount += wordSize*bytes;
    UShort_t tb = topByte(word_16bit);
    UShort_t lb = lowerBytes(word_16bit);

    // header words have top byte 0xf
    if (tb == 0xf) {
      headerBytes += wordSize;
      if (headerBytes == 16) {
	event = lb;
	if (verbose > 0) {
	  cout << "Found event " << event << endl; 
	}
      }

      // channel begin words have top byte 0x4
    } else if (tb == 0x4) {
      channel = lb;
      chBegin = ftell(finput);
      if (verbose > 10) {
	cout << "Found channel " << channel << " begining at " << chBegin << endl; 
      }
      nChannels++;
      validData = true;

      // channel end words have top byte 0x4
    } else if (tb == 0x5) {
      channel = lb;
      chEnd = ftell(finput);
      chLength = chEnd - wordSize - chBegin;
      if (verbose > 10) {
	cout << "End of data for channel " << channel << " ending at " << chEnd << endl; 
	cout << " Channel length for " << channel << " is " << chLength << endl; 
      }
      int corNData = nData - rmDataPt;
      double mu = sum/(corNData*1.0);
      baselines.push_back(mu);
      RMSs.push_back(sqrt(sumsquare/(corNData*1.0) - mu*mu));
      nDataOld = nData;
      validData = false;
      nData = 0;
      sum = 0;
      sumsquare = 0;
      rmDataPt = 0;

      // end of event words have top byte 0xe
    } else if (tb == 0xe) {
      nEvents++;
      evEndOld = evEnd;
      evEnd = ftell(finput);
      evLength = evEnd - evEndOld;
      if (verbose > 0) {
	cout << "Found end of event " << event << " ending at " << evEnd << endl; 
	cout << "    length of " << evLength << " bytes " << endl; 
	cout << "    with " << nChannels << " channels " << endl; 
      }
      nChannelsOld = nChannels;
      nChannels = 0;
      headerBytes = 0;

      // data words have top byte 0xe
    } else if (tb == 0x0 && validData) {
      nData++;
      nDataTotal++;
      sum += 1.0*lb;
      sumsquare += 1.0*lb*lb;
      double mean = sum/(1.0*nData);
      if ((nData > 10 && TMath::Abs(lb*1.0 - mean) > 50) || lb <= 1000) {
        sum -= lb*1.0;
        sumsquare -= 1.0*lb*lb;
        rmDataPt++;
      }
    }
    if (verbose > 0) {
      if (feof(finput)) {
	cout << "End of file reached at " << ftell(finput) << endl;
	break;
      }
    }
  }

  if (verbose > 0) {
    cout << endl << "Ending at " << ftell(finput) << " in file " << file_name << endl;
    
    cout << endl 
	 << "File parameters:          " << endl;
    cout << " Size:                      " << lSize - 1 << endl;
    cout << " Number of complete events: " << nEvents << endl;
    cout << " Event Length:              " << evLength << endl;
    cout << " Header Length:             " << headerBytes << endl;
    cout << " Number of channels:        " << nChannelsOld << endl;
    cout << " Channel data length:       " << nDataOld*wordSize << endl; 
    cout << " Remainder channels:        " << nChannels << endl;
    cout << " Remainder data length      " << nData*wordSize << endl; 
    
    long int fileSize = 
      nEvents*(headerBytes + (3 + 2*nChannelsOld)*wordSize)+ headerBytes + (nDataTotal+1)*wordSize;
    cout << " Calculated file size     : " << fileSize << endl; 
  }


  //
  // Now read file and make 2D histogram display of events
  //

  Int_t nDataBuffer = nDataOld;
  Int_t nEvChannels = nChannelsOld;
  Int_t nFullEvents = nEvents;

  int RMSL = RMSs.size();

  char histoName[100];
  char histoTitle[100];
  char histoFileName[100];

  sprintf(histoFileName,"Run%dHistograms.root",run);

  TFile hFile(histoFileName,"RECREATE");
  TH1F * RMSHist[nFullEvents];
  TH2F  * histoArray[nFullEvents];       // Array of histogram pointers
  TH2F * histoArrayPSub[nFullEvents];

  for (int ev = 0; ev < nFullEvents; ev++) {
    sprintf(histoName, "Run%dEvent%d", run, ev);
    sprintf(histoTitle, "Run %d Event %d", run, ev);
    histoArray[ev] = new TH2F(histoName, histoTitle, nEvChannels, 0, nEvChannels, nDataBuffer, 0, nDataBuffer);
    sprintf(histoName, "Run%dEvent%dPedestalSubtracted", run, ev);
    sprintf(histoTitle, "Run %d Event %d with Pedestal Subtraction", run, ev);
    histoArrayPSub[ev] = new TH2F(histoName, histoTitle, nEvChannels, 0, nEvChannels, nDataBuffer, 0, nDataBuffer);
    sprintf(histoName, "Run%dEvent%dRMS", run, ev);
    sprintf(histoTitle, "Run %d Event %d RMS", run, ev);
    RMSHist[ev] = new TH1F(histoName, histoTitle, nEvChannels, 0, nEvChannels);
    for (int ch = 0; ch < nEvChannels; ch++) {
      if (ch+nEvChannels*ev < RMSL) RMSHist[ev]->SetBinContent(ch+1, RMSs.at(ch+nEvChannels*ev));
    }
    if (verbose > 0) {
      cout << "Booking histogram " << histoTitle << " " << ev << endl;
    }
  }

  bytes = 0;
  headerBytes = 0;
  byteCount = 0;

  nData = 0;
  nDataOld = 0;
  nDataTotal = 0;
  nChannels = 0;
  nChannelsOld = 0;
  nEvents = 0;
  channel = 0;
  event = 0;

  chBegin = 0;
  chEnd = 0;
  chLength = 0;

  evEnd = 0;
  evEndOld = 0;
  evLength = 0;

  validData = false;

  int pedIndex = 0;
  double pedestal = baselines.at(pedIndex);
  int lCheck = baselines.size();

  rewind (finput);

  while (byteCount < lSize && event < nFullEvents) {

    bytes = fread(&word_16bit, sizeof(UShort_t), 1, finput);
    if (bytes != 1) {
      if (verbose > 0)  cout << "fread returns " << bytes << " bytes!!" << endl;
    }
    byteCount += wordSize*bytes;

    UShort_t tb = topByte(word_16bit);
    UShort_t lb = lowerBytes(word_16bit);

    // header words have top byte 0xf
    if (tb == 0xf) {
      headerBytes += wordSize;
      if (headerBytes == 16) {
	event = lb;
	if (verbose > 0) {
	  cout << "Found event " << event << endl; 
	}
      }

      // channel begin words have top byte 0x4
    } else if (tb == 0x4) {
      channel = lb;
      chBegin = ftell(finput);
      if (verbose > 10) {
	cout << "Found channel " << channel << " begining at " << chBegin << endl; 
      }
      nChannels++;
      validData = true;

      // channel end words have top byte 0x4
    } else if (tb == 0x5) {
      channel = lb;
      chEnd = ftell(finput);
      chLength = chEnd - wordSize - chBegin;
      if (verbose > 10) {
	cout << "End of data for channel " << channel << " ending at " << chEnd << endl; 
	cout << " Channel length for " << channel << " is " << chLength << endl; 
      }
      nDataOld = nData;
      validData = false;
      nData = 0;
      pedIndex++;
      if (lCheck > pedIndex) pedestal = baselines.at(pedIndex);

      // end of event words have top byte 0xe
    } else if (tb == 0xe) {
      nEvents++;
      evEndOld = evEnd;
      evEnd = ftell(finput);
      evLength = evEnd - evEndOld;
      if (verbose > 0) {
	cout << "Found end of event " << event << " ending at " << evEnd << endl; 
	cout << "    length of " << evLength << " bytes " << endl; 
	cout << "    with " << nChannels << " channels " << endl; 
      }
      nChannelsOld = nChannels;
      nChannels = 0;
      headerBytes = 0;

      // data words have top byte 0xe
    } else if (tb == 0x0 && validData) {
      nData++;
      nDataTotal++;
      if (nData<nDataBuffer && (channel+1) <= nEvChannels) {
	Int_t bin = histoArray[event-1]->GetBin(channel+1, nData);
	histoArray[event-1]->SetBinContent(bin, lb);
	Int_t binPS = histoArrayPSub[event-1]->GetBin(channel+1, nData);
	histoArrayPSub[event-1]->SetBinContent(binPS, 1.0*lb - pedestal);
      }
    }
    if (verbose > 0) {
      if (feof(finput)) {
	cout << "End of file reached at " << ftell(finput) << endl;
	break;
      }
    }
  }

  hFile.Write();
  hFile.Close();

  if (verbose >= 0) {
    printf("Run %i successful\n",run);
  }

}
