## Makefile.am for the 'Data' module of the OSDL library.


# Convenience library for the 'Data' module:
noinst_LTLIBRARIES = libOSDLData.la

libOSDLData_la_SOURCES = $(DATA_INTERFACES) $(DATA_IMPLEMENTATIONS)

libOSDLData_la_CPPFLAGS = -I@OSDL_BASIC@ -I@OSDL_AUDIO@ -I@OSDL_VIDEO@ -I@OSDL_VIDEO_TWO_DIMENSIONAL@

libOSDLDataIncludedir      = $(includedir)/OSDL
libOSDLDataInclude_HEADERS = $(DATA_INTERFACES)


DATA_INTERFACES = \
	OSDLDataCommon.h                         \
	OSDLDataIncludes.h                       \
	OSDLResourceManager.h                             


DATA_IMPLEMENTATIONS = \
	OSDLDataCommon.cc                        \
	OSDLResourceManager.cc
	                             
