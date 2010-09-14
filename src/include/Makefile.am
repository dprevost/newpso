# Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

CLEANFILES = *~ photon/*~ Doxygen/*~

C_HEADER_FILES =                                          \
        $(top_srcdir)/src/include/photon/photon.h         \
        $(top_srcdir)/src/include/photon/psoCommon.h      \
        $(top_srcdir)/src/include/photon/DataDefinition.h \
        $(top_srcdir)/src/include/photon/FastMap.h        \
        $(top_srcdir)/src/include/photon/Folder.h         \
        $(top_srcdir)/src/include/photon/HashMap.h        \
        $(top_srcdir)/src/include/photon/KeyDefinition.h  \
        $(top_srcdir)/src/include/photon/Lifo.h           \
        $(top_srcdir)/src/include/photon/Process.h        \
        $(top_srcdir)/src/include/photon/psoPhotonODBC.h  \
        $(top_srcdir)/src/include/photon/Queue.h          \
        $(top_srcdir)/src/include/photon/Session.h        

C_INSTALLED_FILES =             \
        photon/photon.h         \
        photon/psoCommon.h      \
        photon/psoErrors.h      \
        photon/DataDefinition.h \
        photon/FastMap.h        \
        photon/Folder.h         \
        photon/HashMap.h        \
        photon/KeyDefinition.h  \
        photon/Lifo.h           \
        photon/psoPhotonODBC.h  \
        photon/Process.h        \
        photon/Queue.h          \
        photon/Session.h        

CPP_INSTALLED_FILES =             \
        photon/photon             \
        photon/DataDefBuilderODBC \
        photon/DataDefBuilderUser \
        photon/DataDefinition     \
        photon/KeyDefBuilderODBC  \
        photon/KeyDefBuilderUser  \
        photon/KeyDefinition      \
        photon/Exception          \
        photon/FastMap            \
        photon/FastMapEditor      \
        photon/Folder             \
        photon/HashMap            \
        photon/Lifo               \
        photon/Process            \
        photon/Queue              \
        photon/Session            

CPP_DOXY_FILES =                                                \
        $(top_srcdir)/src/include/Doxygen/DataDefBuilderODBC.in \
        $(top_srcdir)/src/include/Doxygen/DataDefBuilderUser.in \
        $(top_srcdir)/src/include/Doxygen/DataDefinition.in     \
        $(top_srcdir)/src/include/Doxygen/KeyDefBuilderODBC.in  \
        $(top_srcdir)/src/include/Doxygen/KeyDefBuilderUser.in  \
        $(top_srcdir)/src/include/Doxygen/KeyDefinition.in      \
        $(top_srcdir)/src/include/Doxygen/psoException.in       \
        $(top_srcdir)/src/include/Doxygen/psoFastMap.in         \
        $(top_srcdir)/src/include/Doxygen/psoFastMapEditor.in   \
        $(top_srcdir)/src/include/Doxygen/psoFolder.in          \
        $(top_srcdir)/src/include/Doxygen/psoHashMap.in         \
        $(top_srcdir)/src/include/Doxygen/psoLifo.in            \
        $(top_srcdir)/src/include/Doxygen/psoProcess.in         \
        $(top_srcdir)/src/include/Doxygen/psoQueue.in           \
        $(top_srcdir)/src/include/Doxygen/psoSession.in         

CPP_MAN_PAGES =                      \
        $(top_builddir)/src/include/man3/psoDataDefBuilderODBC.3 \
        $(top_builddir)/src/include/man3/psoDataDefBuilderUser.3 \
        $(top_builddir)/src/include/man3/psoDataDefinition.3     \
        $(top_builddir)/src/include/man3/psoKeyDefBuilderODBC.3  \
        $(top_builddir)/src/include/man3/psoKeyDefBuilderUser.3  \
        $(top_builddir)/src/include/man3/psoKeyDefinition.3      \
        $(top_builddir)/src/include/man3/psoException.3          \
        $(top_builddir)/src/include/man3/psoFastMap.3            \
        $(top_builddir)/src/include/man3/psoFastMapEditor.3      \
        $(top_builddir)/src/include/man3/psoFolder.3             \
        $(top_builddir)/src/include/man3/psoHashMap.3            \
        $(top_builddir)/src/include/man3/psoLifo.3               \
        $(top_builddir)/src/include/man3/psoProcess.3            \
        $(top_builddir)/src/include/man3/psoQueue.3              \
        $(top_builddir)/src/include/man3/psoSession.3            

# Sadly, this list must be edited by hand (it will vary if functions are
# added or removed from the C API).
C_MAN_PAGES = \
        $(top_builddir)/src/include/man3/photon.h.3                \
        $(top_builddir)/src/include/man3/psoCommon.h.3             \
        $(top_builddir)/src/include/man3/psoDataDefinition_c.3     \
        $(top_builddir)/src/include/man3/psoDateStruct.3           \
        $(top_builddir)/src/include/man3/psoErrors.h.3             \
        $(top_builddir)/src/include/man3/psoFastMap_c.3            \
        $(top_builddir)/src/include/man3/psoFieldDefinition.3      \
        $(top_builddir)/src/include/man3/psoFolder_c.3             \
        $(top_builddir)/src/include/man3/psoFolderEntry.3          \
        $(top_builddir)/src/include/man3/psoHashMap_c.3            \
        $(top_builddir)/src/include/man3/psoInfo.3                 \
        $(top_builddir)/src/include/man3/psoKeyFieldDefinition.3   \
        $(top_builddir)/src/include/man3/psoKeyFieldDefinition_c.3 \
        $(top_builddir)/src/include/man3/psoLifo_c.3               \
        $(top_builddir)/src/include/man3/psoNumericStruct.3        \
        $(top_builddir)/src/include/man3/psoObjectDefinition.3     \
        $(top_builddir)/src/include/man3/psoObjStatus.3            \
        $(top_builddir)/src/include/man3/psoPhotonODBC.h.3         \
        $(top_builddir)/src/include/man3/psoProcess_c.3            \
        $(top_builddir)/src/include/man3/psoQueue_c.3              \
        $(top_builddir)/src/include/man3/psoSession_c.3            \
        $(top_builddir)/src/include/man3/psoTimeStampStruct.3      \
        $(top_builddir)/src/include/man3/psoTimeStruct.3           

man_MANS =               \
        $(C_MAN_PAGES)   \
        $(CPP_MAN_PAGES) 

EXTRA_DIST =              \
        Doxygen/pso_c.in  \
        $(C_MAN_PAGES)    \
        $(CPP_MAN_PAGES)  \
        $(CPP_DOXY_FILES) 

nobase_include_HEADERS =       \
        $(C_INSTALLED_FILES)   \
        $(CPP_INSTALLED_FILES) 

#
# Only rebuild the man pages if doxygen is present.
#
.PHONY: man3_dir
man3_dir: $(top_builddir)/src/include/man3
	-mkdir $(top_builddir)/src/include/man3

if COND_USE_DOXYGEN

$(top_builddir)/src/include/man3/psoDataDefBuilderODBC.3: $(top_srcdir)/src/include/Doxygen/DataDefBuilderODBC.in $(top_srcdir)/src/include/photon/DataDefBuilderODBC
	doxygen $(top_builddir)/src/include/Doxygen/DataDefBuilderODBC 
	cp dummy/man/man3/pso_DataDefBuilderODBC.3 man3/psoDataDefBuilderODBC.3
	-rm -rf dummy
$(top_builddir)/src/include/man3/psoDataDefBuilderUser.3: $(top_srcdir)/src/include/Doxygen/DataDefBuilderUser.in $(top_srcdir)/src/include/photon/DataDefBuilderUser
	doxygen $(top_builddir)/src/include/Doxygen/DataDefBuilderUser 
	cp dummy/man/man3/pso_DataDefBuilderUser.3 man3/psoDataDefBuilderUser.3
	-rm -rf dummy
$(top_builddir)/src/include/man3/psoDataDefinition.3: $(top_srcdir)/src/include/Doxygen/DataDefinition.in $(top_srcdir)/src/include/photon/DataDefinition
	doxygen $(top_builddir)/src/include/Doxygen/DataDefinition 
	cp dummy/man/man3/pso_DataDefinition.3 man3/psoDataDefinition.3
	-rm -rf dummy
$(top_builddir)/src/include/man3/psoKeyDefBuilderODBC.3: $(top_srcdir)/src/include/Doxygen/KeyDefBuilderODBC.in $(top_srcdir)/src/include/photon/KeyDefBuilderODBC
	doxygen $(top_builddir)/src/include/Doxygen/KeyDefBuilderODBC 
	cp dummy/man/man3/pso_KeyDefBuilderODBC.3 man3/psoKeyDefBuilderODBC.3
	-rm -rf dummy
$(top_builddir)/src/include/man3/psoKeyDefBuilderUser.3: $(top_srcdir)/src/include/Doxygen/KeyDefBuilderUser.in $(top_srcdir)/src/include/photon/KeyDefBuilderUser
	doxygen $(top_builddir)/src/include/Doxygen/KeyDefBuilderUser 
	cp dummy/man/man3/pso_KeyDefBuilderUser.3 man3/psoKeyDefBuilderUser.3
	-rm -rf dummy
$(top_builddir)/src/include/man3/psoKeyDefinition.3: $(top_srcdir)/src/include/Doxygen/KeyDefinition.in $(top_srcdir)/src/include/photon/KeyDefinition
	doxygen $(top_builddir)/src/include/Doxygen/KeyDefinition 
	cp dummy/man/man3/pso_KeyDefinition.3 man3/psoKeyDefinition.3
	-rm -rf dummy
$(top_builddir)/src/include/man3/psoException.3: $(top_srcdir)/src/include/Doxygen/psoException.in $(top_srcdir)/src/include/photon/Exception
	doxygen $(top_builddir)/src/include/Doxygen/psoException 
	cp dummy/man/man3/pso_Exception.3 man3/psoException.3
	-rm -rf dummy
$(top_builddir)/src/include/man3/psoFastMap.3: $(top_srcdir)/src/include/Doxygen/psoFastMap.in $(top_srcdir)/src/include/photon/FastMap
	doxygen $(top_builddir)/src/include/Doxygen/psoFastMap 
	cp dummy/man/man3/pso_FastMap.3 man3/psoFastMap.3
	-rm -rf dummy
$(top_builddir)/src/include/man3/psoFastMapEditor.3: $(top_srcdir)/src/include/Doxygen/psoFastMapEditor.in $(top_srcdir)/src/include/photon/FastMapEditor
	doxygen $(top_builddir)/src/include/Doxygen/psoFastMapEditor 
	cp dummy/man/man3/pso_FastMapEditor.3 man3/psoFastMapEditor.3
	-rm -rf dummy
$(top_builddir)/src/include/man3/psoFolder.3: $(top_srcdir)/src/include/Doxygen/psoFolder.in $(top_srcdir)/src/include/photon/Folder
	doxygen $(top_builddir)/src/include/Doxygen/psoFolder 
	cp dummy/man/man3/pso_Folder.3 man3/psoFolder.3
	-rm -rf dummy
$(top_builddir)/src/include/man3/psoHashMap.3: $(top_srcdir)/src/include/Doxygen/psoHashMap.in $(top_srcdir)/src/include/photon/HashMap
	doxygen $(top_builddir)/src/include/Doxygen/psoHashMap 
	cp dummy/man/man3/pso_HashMap.3 man3/psoHashMap.3
	-rm -rf dummy
$(top_builddir)/src/include/man3/psoLifo.3: $(top_srcdir)/src/include/Doxygen/psoLifo.in $(top_srcdir)/src/include/photon/Lifo
	doxygen $(top_builddir)/src/include/Doxygen/psoLifo 
	cp dummy/man/man3/pso_Lifo.3 man3/psoLifo.3
	-rm -rf dummy
$(top_builddir)/src/include/man3/psoProcess.3: $(top_srcdir)/src/include/Doxygen/psoProcess.in $(top_srcdir)/src/include/photon/Process
	doxygen $(top_builddir)/src/include/Doxygen/psoProcess 
	cp dummy/man/man3/pso_Process.3 man3/psoProcess.3
	-rm -rf dummy
$(top_builddir)/src/include/man3/psoQueue.3: $(top_srcdir)/src/include/Doxygen/psoQueue.in $(top_srcdir)/src/include/photon/Queue
	doxygen $(top_builddir)/src/include/Doxygen/psoQueue 
	cp dummy/man/man3/pso_Queue.3 man3/psoQueue.3
	-rm -rf dummy
$(top_builddir)/src/include/man3/psoSession.3: $(top_srcdir)/src/include/Doxygen/psoSession.in $(top_srcdir)/src/include/photon/Session
	doxygen $(top_builddir)/src/include/Doxygen/psoSession 
	cp dummy/man/man3/pso_Session.3 man3/psoSession.3
	-rm -rf dummy

# Warning: this doxygen run creates many more files which we do
# not want to distribute (a header file for each error code, for each
# data field in the data structures, etc.). All these files are links
# to the "meat" we are keeping anyway!
$(C_MAN_PAGES): $(C_HEADER_FILES) $(top_srcdir)/src/include/Doxygen/pso_c.in
	doxygen Doxygen/pso_c
	cp dummy/man/man3/pso*.3 man3/
	cp dummy/man/man3/photon*.3 man3/

#	-rm -rf dummy

endif

clean-local:
	-rm -rf dummy

#
# Only clean the man pages if doxygen is present.
#
if COND_USE_DOXYGEN

maintainer-clean-local:
	-rm -f man3/*.3

endif

