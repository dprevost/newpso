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

CLEANFILES = *~ core Memfile.mem

THREAD_TEST_CODE =   \
        Barrier.c    \
        Barrier.h    \
        ThreadWrap.c \
        ThreadWrap.h 

check_PROGRAMS =        \
        Acquire         \
        Fini            \
        Init            \
        LockConcurrency \
        LockTests       \
        Release         \
        TryAcq          

LockConcurrency_SOURCES      = LockConcurrency.c $(THREAD_TEST_CODE)

EXTRA_DIST =                  \
        LockConcurrency.sh    \
        LockConcurrencyTry.sh \
        Makefile.win32        \
        RunTests.vbs          

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                 \
        Acquire         \
        Fini            \
        Init            \
        Release         \
        TryAcq          \
        LockConcurrency \
        LockTests       

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/Common/libCommon.la

