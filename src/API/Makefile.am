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

CLEANFILES = *~
lib_LTLIBRARIES = libphoton.la

SUBDIRS = . Tests

# See http://www.gnu.org/software/libtool/manual.html#SEC34
# for more info on version-info. 
# Hints:
#  - the version is not in synch with the version of the whole package
#  - format:  currentversion:revision:age  (age is the tricky one here)

libphoton_la_LDFLAGS = -version-info 0:0:0

include_HEADERS = 

libphoton_la_SOURCES =   \
        api.c            \
        api.h            \
        CommonObject.c   \
        CommonObject.h   \
        Connector.c      \
        Connector.h      \
        FastMap.c        \
        FastMap.h        \
        Folder.c         \
        Folder.h         \
        HashMap.c        \
        HashMap.h        \
        Lifo.c           \
        Lifo.h           \
        ListReaders.h    \
        Process.c        \
        Process.h        \
        QuasarCommon.h   \
        Queue.c          \
        Queue.h          \
        Session.c        \
        Session.h        \
        Standalone.c     

libphoton_la_LIBADD =                             \
        $(top_builddir)/src/Nucleus/libNucleus.la \
        $(top_builddir)/src/Common/libCommon.la   

EXTRA_DIST =              \
        API.vs2005.vcproj \
        API.vs2008.vcproj \
        Makefile.win32    

