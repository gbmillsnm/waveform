# Makefile for the Waveform library
# This Makefile shows nicely how to compile and link applications
# using the ROOT libraries on all supported platforms.

RC     := root-config
ifeq ($(shell which $(RC) 2>&1 | sed -ne "s@.*/$(RC)@$(RC)@p"),$(RC))
MKARCH := $(wildcard $(shell $(RC) --etcdir)/Makefile.arch)
RCONFIG := $(wildcard $(shell $(RC) --incdir)/RConfigure.h)
endif
ifneq ($(MKARCH),)
include $(MKARCH)
else
ifeq ($(ROOTSYS),)
ROOTSYS = ..
endif
include $(ROOTSYS)/etc/Makefile.arch
endif

GLIBS := $(shell $(RC) --glibs)

#------------------------------------------------------------------------------

# here we lump all of the classes and their dictionaries together 
# into one shared object file libWaveform.so 
# (Trace, Waveform, HistogramManager)
WAVEFORMO        = Waveform.$(ObjSuf) WaveformDict.$(ObjSuf)
WAVEFORMS        = Waveform.$(SrcSuf) WaveformDict.$(SrcSuf)
WAVEFORMO        += Trace.$(ObjSuf) TraceDict.$(ObjSuf)
WAVEFORMS        += Trace.$(SrcSuf) Trace.$(SrcSuf)
WAVEFORMO        += WaveformHeader.$(ObjSuf) WaveformHeaderDict.$(ObjSuf)
WAVEFORMS        += WaveformHeader.$(SrcSuf) WaveformHeaderDict.$(SrcSuf)
WAVEFORMO        += HistogramManager.$(ObjSuf) HistogramManagerDict.$(ObjSuf)
WAVEFORMS        += HistogramManager.$(SrcSuf) HistogramManagerDict.$(SrcSuf)
WAVEFORMSO       = libWaveform.$(DllSuf)

CPTVIEWERO        = CptViewer.$(ObjSuf) CptViewerDict.$(ObjSuf)
CPTVIEWERS        = CptViewer.$(SrcSuf) CptViewerDict.$(SrcSuf)


# target executables
CPTVIEWER        = CptViewer$(ExeSuf)
WAVEFORM         = Waveform$(ExeSuf)
TRACE            = Trace$(ExeSuf)


ifeq ($(PLATFORM),win32)
WAVEFORMLIB      = libWaveform.lib
else
WAVEFORMLIB      = $(shell pwd)/$(WAVEFORMSO)
endif

# main() source and object files
MAINWAVEFORMO    = MainWaveform.$(ObjSuf)
MAINWAVEFORMS    = MainWaveform.$(SrcSuf)

MAINTRACEO    = MainTrace.$(ObjSuf)
MAINTRACES    = MainTrace.$(SrcSuf)

MAINCPTVIEWERO    = MainCptViewer.$(ObjSuf)
MAINCPTVIEWERS    = MainCptViewer.$(SrcSuf)

# object files and executable targets
OBJS          = $(WAVEFORMO) $(MAINWAVEFORMO)  $(CPTVIEWERO) $(MAINCPTVIEWERO)

PROGRAMS      =  $(WAVEFORM) $(TRACE) $(CPTVIEWER)

ifeq ($(ARCH),aix5)
MAKESHARED    = /usr/vacpp/bin/makeC++SharedLib
endif

#------------------------------------------------------------------------------

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)
.PHONY:

.$(SrcSuf).$(ObjSuf):
	$(CXX)  $(CXXFLAGS) -c $<

%.$(Objsuf): %.$(Srcsuf) %.h
	$(Cxx)  $(CXXFLAGS) -c $<

all:            $(PROGRAMS)

$(WAVEFORMSO):     $(WAVEFORMO)
ifeq ($(ARCH),aix5)
		$(MAKESHARED) $(OutPutOpt) $@ $(LIBS) -p 0 $^
else
ifeq ($(PLATFORM),macosx)
# We need to make both the .dylib and the .so
		$(LD) $(SOFLAGS)$@ $(LDFLAGS) $^ $(OutPutOpt) $@ $(EXPLLINKLIBS)
ifneq ($(subst $(MACOSX_MINOR),,1234),1234)
ifeq ($(MACOSX_MINOR),4)
		ln -sf $@ $(subst .$(DllSuf),.so,$@)
endif
endif
else
ifeq ($(PLATFORM),win32)
		bindexplib $* $^ > $*.def
		lib -nologo -MACHINE:IX86 $^ -def:$*.def \
		   $(OutPutOpt)$(WAVEFORMLIB)
		$(LD) $(SOFLAGS) $(LDFLAGS) $^ $*.exp $(LIBS) \
		   $(OutPutOpt)$@
		$(MT_DLL)
else
		$(LD) $(SOFLAGS) $(LDFLAGS) $^ $(OutPutOpt) $@ $(EXPLLINKLIBS)
endif
endif
endif
		@echo "$@ done"

		@echo "$@ done"

$(WAVEFORM):    $(WAVEFORMSO)  $(MAINWAVEFORMO)
		$(LD) $(LDFLAGS) $(MAINWAVEFORMO) $(WAVEFORMO) $(LIBS) $(OutPutOpt)$@
		$(MT_EXE)
		@echo "$@ done"

$(CPTVIEWER):    $(CPTVIEWERO)  $(MAINCPTVIEWERO)
		$(LD) $(LDFLAGS) $(MAINCPTVIEWERO) $(CPTVIEWERO) $(GLIBS) $(OutPutOpt)$@
		$(MT_EXE)
		@echo "$@ done"

$(TRACE)   :    $(WAVEFORMSO)  $(MAINTRACEO)
		$(LD) $(LDFLAGS) $(MAINTRACEO) $(WAVEFORMO) $(LIBS) $(OutPutOpt)$@
		$(MT_EXE)
		@echo "$@ done"


clean:
		@rm -f $(OBJS) core *Dict.*

distclean:      clean
		@rm -f $(PROGRAMS) $(WAVEFORMSO) $(WAVEFORMLIB) *Dict.* *.def *.exp \
		   *.root *.ps *.so *.lib *.dll *.d *.log .def so_locations \
		   files/* 


###
# The following define dependencies for 
# 1. the object file header dependencies
# 2. the root LinkDef files that generate class dictionaries for root

Waveform.$(ObjSuf): Waveform.h Trace.h

MainWaveform.$(ObjSuf): Waveform.h Trace.h HistogramManager.h 

MainWaveformDraw.$(ObjSuf): Waveform.h Trace.h HistogramManager.h 

MainCptViewer.$(ObjSuf): CptViewer.h  

CptViewerDict.$(SrcSuf): CptViewer.h CptViewerLinkDef.h
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ -c $^
WaveformDict.$(SrcSuf): Waveform.h WaveformLinkDef.h
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ -c $^
WaveformHeaderDict.$(SrcSuf): WaveformHeader.h WaveformHeaderLinkDef.h
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ -c $^
TraceDict.$(SrcSuf): Trace.h TraceLinkDef.h
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ -c $^
HistogramManagerDict.$(SrcSuf): HistogramManager.h HistogramManagerLinkDef.h
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ -c $^
