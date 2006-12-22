## Makefile.am for the 'Basic' module of the OSDL library.


# Convenience library for the 'Basic' module :
noinst_LTLIBRARIES = libOSDLBasic.la

libOSDLBasic_la_SOURCES = $(BASIC_INTERFACES) $(BASIC_IMPLEMENTATIONS)

libOSDLBasic_la_CPPFLAGS = -I@OSDL_VIDEO@ -I@OSDL_VIDEO_TWO_DIMENSIONAL@ -I@OSDL_EVENTS@ -I@OSDL_AUDIO@

libOSDLBasicIncludedir      = $(includedir)/OSDL
libOSDLBasicInclude_HEADERS = $(BASIC_INTERFACES)


BASIC_INTERFACES = \
	OSDLBasic.h                          \
	OSDLBasicIncludes.h                  \
	OSDLCDROMDrive.h                     \
	OSDLCDROMDriveHandler.h              \
	OSDLException.h                      \
	OSDLTestException.h                  \
	OSDLTypes.h                          \
	OSDLUtils.h


BASIC_IMPLEMENTATIONS = \
	OSDLBasic.cc                         \
	OSDLCDROMDrive.cc                    \
	OSDLCDROMDriveHandler.cc             \
	OSDLException.cc                     \
	OSDLTestException.cc                 \
	OSDLUtils.cc
	

# DISABLED = OSDLDocMainPage.h
