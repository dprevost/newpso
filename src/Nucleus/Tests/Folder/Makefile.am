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

check_PROGRAMS =        \
        CommitEdit      \
        CreateFolder    \
        CreateMap       \
        CreateQueue     \
        Delete          \
        Destroy         \
        Edit            \
        Fini            \
        Get             \
        GetDefinition   \
        GetFirst        \
        GetNext         \
        GetStatus       \
        Init            \
        InsertFolder    \
        InsertMap       \
        InsertQueue     \
        Release         \
        Remove          \
        Resize          \
        RollbackEdit    \
        Status          \
        Tests           \
        Validate        

COMMON_HEADER = folderTest.h

EXTRA_DIST =           \
        Makefile.win32 \
        RunTests.vbs   

TESTS_ENVIRONMENT =                  \
        srcdir=$(srcdir)             \
        top_builddir=$(top_builddir) \
        verbose=1                    \
        PYTHON=$(PYTHON)             

TESTS =                 \
        CommitEdit      \
        CreateFolder    \
        CreateMap       \
        CreateQueue     \
        Delete          \
        Destroy         \
        Edit            \
        Fini            \
        Get             \
        GetDefinition   \
        GetFirst        \
        GetNext         \
        GetStatus       \
        Init            \
        InsertFolder    \
        InsertMap       \
        InsertQueue     \
        Release         \
        Remove          \
        Resize          \
        RollbackEdit    \
        Status          \
        Tests           \
        Validate        

XFAIL_TESTS = 

LDADD = $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la

