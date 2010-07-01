## Makefile.am for the main OSDL library.


SUFFIXES = .cc .h


# These are the subdirectories (i.e. modules) that are always built:
BUILT_SUBDIRS = \
	audio       \
	basic       \
	data        \
	engine      \
	events      \
	video



# These are the subdirectories which may be built:
EXTRA_SUBDIRS =	@OSDL_EXTRA_DIRS@

# Subdirectories taken into account but not especially built:
OTHER_SUBDIRS = scripts

# These are the subdirectories which will be taken into account now:
SUBDIRS = $(BUILT_SUBDIRS) $(OTHER_SUBDIRS) $(EXTRA_SUBDIRS)

# These are the subdirectories which will be distributed with 'make dist':
DIST_SUBDIRS = $(SUBDIRS)


# The OSDL library target:
lib_LTLIBRARIES = libOSDL.la

# All the library sources:
libOSDL_la_SOURCES = $(GENERAL_SRCS)

# Linker flag for the library:
libOSDL_la_LDFLAGS = \
	-no-undefined                                                           \
	-release $(OSDL_LT_RELEASE)                                             \
	-version-info $(OSDL_LT_CURRENT):$(OSDL_LT_REVISION):$(OSDL_LT_AGE)


# Submodule convenience libraries:
SUBLIBS = \
	audio/libOSDLAudio.la                                         \
	audio/audioRenderer/libOSDLAudioRenderer.la                   \
	basic/libOSDLBasic.la                                         \
	data/libOSDLData.la                                           \
	engine/libOSDLEngine.la                                       \
	events/libOSDLEvents.la                                       \
	video/libOSDLVideo.la                                         \
	video/graphicalUserInterface/libOSDLGraphicalUserInterface.la \
	video/twoDimensional/libOSDLTwoDimensional.la                 \
	video/videoRenderer/libOSDLVideoRenderer.la



EXTRA_SUBLIBS = @OSDL_EXTRA_LIBLTDL_SUBLIB@

# The overall library is made from submodule convenience libraries and al:
libOSDL_la_LIBADD	= \
	$(SUBLIBS)          \
	@OSDL_EXTRA_LIBS@   \
	@SYSTEM_LIBS@


libOSDL_la_DEPENDENCIES = \
	$(SUBLIBS)            \
	@OSDL_EXTRA_LIBS@


libOSDLIncludedir      = $(includedir)/OSDL
libOSDLInclude_HEADERS = OSDL.h OSDLConfig.h

DISTCLEANFILES = OSDLConfig.h


# Run ldconfig after installing the library:
install-exec-hook:
	-ldconfig

clean-local:
	-@/bin/rm -f stamp-h1 OSDLTemporaryConfig.h*
