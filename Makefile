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

#------------------------------------------------------------------------------

WAVEFORMO        = Waveform.$(ObjSuf) WaveformDict.$(ObjSuf)
WAVEFORMS        = Waveform.$(SrcSuf) WaveformDict.$(SrcSuf)
WAVEFORMSO       = libWaveform.$(DllSuf)
WAVEFORM         = Waveform$(ExeSuf)
ifeq ($(PLATFORM),win32)
WAVEFORMLIB      = libWaveform.lib
else
WAVEFORMLIB      = $(shell pwd)/$(WAVEFORMSO)
endif


MAINWAVEFORMO    = MainWaveform.$(ObjSuf)
MAINWAVEFORMS    = MainWaveform.$(SrcSuf)

OBJS          = $(WAVEFORMO) $(MAINWAVEFORMO)

PROGRAMS      = $(WAVEFORM) 

ifeq ($(ARCH),aix5)
MAKESHARED    = /usr/vacpp/bin/makeC++SharedLib
endif

WaveformDict.o : WaveformDict.$(SrcSuf)
	$(CXX) $(CXXFLAGS) -c $<

#------------------------------------------------------------------------------

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)
.PHONY:    Aclock Hello Tetris

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


clean:
		@rm -f $(OBJS) core *Dict.*

distclean:      clean
		@rm -f $(PROGRAMS) $(WAVEFORMSO) $(WAVEFORMLIB) *Dict.* *.def *.exp \
		   *.root *.ps *.so *.lib *.dll *.d *.log .def so_locations \
		   files/* 


###
 
Waveform.$(ObjSuf): Waveform.h

MainWaveform.$(ObjSuf): Waveform.h

WaveformDict.$(SrcSuf): Waveform.h WaveformLinkDef.h
	@echo "Generating dictionary $@..."
	$(ROOTCINT) -f $@ -c $^
