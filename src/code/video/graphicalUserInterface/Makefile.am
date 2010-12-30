## Makefile.am for the submodule 'Graphical User Interface' of the 'Video'
## module of the OSDL library.


# Convenience library for the 'Graphical User Interface' submodule:
noinst_LTLIBRARIES = libOSDLGraphicalUserInterface.la

libOSDLGraphicalUserInterface_la_SOURCES = \
	$(GRAPHICAL_USER_INTERFACE_INTERFACES)      \
	$(GRAPHICAL_USER_INTERFACE_IMPLEMENTATIONS)


libOSDLGraphicalUserInterface_la_CPPFLAGS = -I@OSDL_BASIC@


libOSDLGraphicalUserInterfaceIncludedir = $(includedir)/OSDL

libOSDLGraphicalUserInterfaceInclude_HEADERS = \
	$(GRAPHICAL_USER_INTERFACE_INTERFACES)


GRAPHICAL_USER_INTERFACE_INTERFACES = \
	OSDLBaseGUIView.h                     \
	OSDLGraphicalUserInterfaceIncludes.h


GRAPHICAL_USER_INTERFACE_IMPLEMENTATIONS = \
	OSDLBaseGUIView.cc
