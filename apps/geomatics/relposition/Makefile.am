# $Id$
INCLUDES = -I$(srcdir)/../../../src -I$(srcdir)/../../../lib/geomatics

LDADD = ../../../lib/geomatics/libgeomatics.la ../../../src/libgpstk.la
DDBase_LDADD = ../../../lib/geomatics/libgeomatics.la ../../../src/libgpstk.la ./libbase.la

lib_LTLIBRARIES = libbase.la
libbase_la_LDFLAGS = -version-number @GPSTK_SO_VERSION@
libbase_la_SOURCES = ClockModel.cpp CommandInput.cpp ComputeRAIMSolution.cpp \
Configure.cpp DataOutput.cpp DataStructures.cpp DoubleDifference.cpp \
EditDDs.cpp EditRawDataBuffers.cpp ElevationMask.cpp EphemerisImprovement.cpp \
Estimation.cpp ProcessRawData.cpp ReadObsFiles.cpp ReadRawData.cpp \
StochasticModels.cpp Synchronization.cpp Timetable.cpp

bin_PROGRAMS = DDBase ddmerge 

DDBase_SOURCES = DDBase.cpp
ddmerge_SOURCES = ddmerge.cpp
