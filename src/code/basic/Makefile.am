## Makefile.am for the 'Basic' module of the OSDL library.


# Convenience library for the 'Basic' module:
noinst_LTLIBRARIES = libOSDLBasic.la

libOSDLBasic_la_SOURCES = $(BASIC_INTERFACES) $(BASIC_IMPLEMENTATIONS)

libOSDLBasic_la_CPPFLAGS = -I@OSDL_VIDEO@ -I@OSDL_VIDEO_TWO_DIMENSIONAL@ -I@OSDL_EVENTS@ -I@OSDL_AUDIO@

libOSDLBasicIncludedir      = $(includedir)/OSDL
libOSDLBasicInclude_HEADERS = $(BASIC_INTERFACES)


# OSDLCommandManager* not even listed here.

BASIC_INTERFACES = \
	OSDLARM7Codes.h                      \
	OSDLBasic.h                          \
	OSDLBasicIncludes.h                  \
	OSDLCDROMDrive.h                     \
	OSDLCDROMDriveHandler.h              \
	OSDLEmbeddedDirectory.h              \
	OSDLEmbeddedFile.h                   \
	OSDLEmbeddedFileSystemManager.h      \
	OSDLException.h                      \
	OSDLFileTags.h                       \
	OSDLGUI.h                            \
	OSDLHeaderVersion.h                  \
	OSDLIncludeCorrecter.h               \
	OSDLIPCCommands.h                    \
	OSDLTestException.h                  \
	OSDLTypes.h                          \
	OSDLUtils.h


BASIC_IMPLEMENTATIONS = \
	OSDLBasic.cc                         \
	OSDLCDROMDrive.cc                    \
	OSDLCDROMDriveHandler.cc             \
	OSDLEmbeddedDirectory.cc             \
	OSDLEmbeddedFile.cc                  \
	OSDLEmbeddedFileSystemManager.cc     \
	OSDLException.cc                     \
	OSDLFileTags.cc                      \
	OSDLGUI.cc                           \
	OSDLTestException.cc                 \
	OSDLUtils.cc


# DISABLED = OSDLDocMainPage.h
