# Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# Include this at the beginning to allow the current Makefile.am to
# overwrite any macros set in the common file
include $(top_srcdir)/src/Makefile.Common.am

CLEANFILES = *~ core

# The two "real" regression tests are ListTestSmall and ListTestBig.
# I put them at the end to make sure that the interface itself (to 
# the list) is ok. I also put Small before Big since the smaller one
# is doing a lot less iterations but doing many more consistency checks
# (I could have bundled it all together but the test would take too
# much clock time/cpu time). 

check_PROGRAMS =     \
        Fini         \
        GetFirst     \
        GetLast      \
        Init         \
        IsValid      \
        PeakFirst    \
        PeakLast     \
        PeakNext     \
        PeakPrevious \
        PutFirst     \
        PutLast      \
        Remove       \
        Replace      \
        Reset        \
        TestSmall    \
        TestBig      

LIST_H = ListTestCommon.h 

EXTRA_DIST =           \
        $(LIST_H)      \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =              \
        Fini         \
        GetFirst     \
        GetLast      \
        Init         \
        IsValid      \
        PeakFirst    \
        PeakLast     \
        PeakNext     \
        PeakPrevious \
        PutFirst     \
        PutLast      \
        Remove       \
        Replace      \
        Reset        \
        TestSmall    \
        TestBig      

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la

