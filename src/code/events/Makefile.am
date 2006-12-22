## Makefile.am for the 'Events' module of the OSDL library.


# Convenience library for the 'Events' module :
noinst_LTLIBRARIES = libOSDLEvents.la

libOSDLEvents_la_SOURCES = $(EVENTS_INTERFACES) $(EVENTS_IMPLEMENTATIONS)

libOSDLEvents_la_CPPFLAGS = -I@OSDL_BASIC@ -I@OSDL_VIDEO@ -I@OSDL_VIDEO_TWO_DIMENSIONAL@ -I@OSDL_ENGINE@ 

libOSDLEventsIncludedir      = $(includedir)/OSDL
libOSDLEventsInclude_HEADERS = $(EVENTS_INTERFACES)


EVENTS_INTERFACES = \
	OSDLClassicalJoystick.h              \
	OSDLController.h                     \
	OSDLEventsCommon.h                   \
	OSDLEvents.h                         \
	OSDLEventsIncludes.h                 \
	OSDLInputDevice.h                    \
	OSDLInputDeviceHandler.h             \
	OSDLJoystickCommon.h                 \
	OSDLJoystick.h                       \
	OSDLJoystickHandler.h                \
	OSDLKeyboardHandler.h


EVENTS_IMPLEMENTATIONS = \
	OSDLClassicalJoystick.cc             \
	OSDLController.cc                    \
	OSDLEvents.cc                        \
	OSDLInputDevice.cc                   \
	OSDLInputDeviceHandler.cc            \
	OSDLJoystick.cc                      \
	OSDLJoystickCommon.cc                \
	OSDLJoystickHandler.cc               \
	OSDLKeyboardHandler.cc
