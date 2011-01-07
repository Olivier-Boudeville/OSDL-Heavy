#ifndef _SRC_CODE_OSDLCONFIG_H
#define _SRC_CODE_OSDLCONFIG_H 1

/* src/code/OSDLConfig.h. Generated automatically at end of configure. */
/* src/code/OSDLTemporaryConfig.h.  Generated from OSDLTemporaryConfig.h.in by configure.  */
/* src/code/OSDLTemporaryConfig.h.in.  Generated from configure.ac by autoheader.  */

/* Define to 1 if the target platform is the Nintendo DS */
#ifndef OSDL_ARCH_NINTENDO_DS
#define OSDL_ARCH_NINTENDO_DS  0
#endif

/* Define to 1 if overall settings should be cached instead of being retrieved
   on the fly */
#ifndef OSDL_CACHE_OVERALL_SETTINGS
#define OSDL_CACHE_OVERALL_SETTINGS  0
#endif

/* Define to 1 if OpenGL calls should be checked for errors */
#ifndef OSDL_CHECK_OPENGL_CALLS
#define OSDL_CHECK_OPENGL_CALLS  1
#endif

/* Define to 1 if the `closedir' function returns void instead of `int'. */
/* #undef OSDL_CLOSEDIR_VOID */

/* Define to 1 if instance counting mode is to be enabled */
#ifndef OSDL_COUNT_INSTANCES
#define OSDL_COUNT_INSTANCES  0
#endif

/* Define to 1 if generic debug mode is to be enabled */
#ifndef OSDL_DEBUG
#define OSDL_DEBUG  1
#endif

/* Define to 1 if debug mode for sound/music playback is to be enabled */
#ifndef OSDL_DEBUG_AUDIO_PLAYBACK
#define OSDL_DEBUG_AUDIO_PLAYBACK  0
#endif

/* Define to 1 if debug mode for audio renderer is to be enabled */
#ifndef OSDL_DEBUG_AUDIO_RENDERER
#define OSDL_DEBUG_AUDIO_RENDERER  0
#endif

/* Define to 1 if debug mode for blit operations is to be enabled */
#ifndef OSDL_DEBUG_BLIT
#define OSDL_DEBUG_BLIT  0
#endif

/* Define to 1 if debug mode for bounding boxes is to be enabled */
#ifndef OSDL_DEBUG_BOUNDING_BOX
#define OSDL_DEBUG_BOUNDING_BOX  0
#endif

/* Define to 1 if debug mode for cached states is to be enabled */
#ifndef OSDL_DEBUG_CACHED_STATES
#define OSDL_DEBUG_CACHED_STATES  1
#endif

/* Define to 1 if debug mode for colors is to be enabled */
#ifndef OSDL_DEBUG_COLOR
#define OSDL_DEBUG_COLOR  0
#endif

/* Define to 1 if debug mode for conics is to be enabled */
#ifndef OSDL_DEBUG_CONICS
#define OSDL_DEBUG_CONICS  0
#endif

/* Define to 1 if debug mode for events is to be enabled */
#ifndef OSDL_DEBUG_EVENTS
#define OSDL_DEBUG_EVENTS  0
#endif

/* Define to 1 if debug mode for font management is to be enabled */
#ifndef OSDL_DEBUG_FONT
#define OSDL_DEBUG_FONT  0
#endif

/* Define to 1 if debug mode for the graphical user interface is to be enabled
   */
#ifndef OSDL_DEBUG_GUI
#define OSDL_DEBUG_GUI  1
#endif

/* Define to 1 if debug mode for image management is to be enabled */
#ifndef OSDL_DEBUG_IMAGE
#define OSDL_DEBUG_IMAGE  0
#endif

/* Define to 1 if debug mode for Nintendo DS is to be enabled */
#ifndef OSDL_DEBUG_NINTENDO_DS
#define OSDL_DEBUG_NINTENDO_DS  0
#endif

/* Define to 1 if debug mode for palette operations is to be enabled */
#ifndef OSDL_DEBUG_PALETTE
#define OSDL_DEBUG_PALETTE  0
#endif

/* Define to 1 if debug mode for pixel-level operations is to be enabled */
#ifndef OSDL_DEBUG_PIXEL
#define OSDL_DEBUG_PIXEL  0
#endif

/* Define to 1 if debug mode for rectangle management is to be enabled */
#ifndef OSDL_DEBUG_RECTANGLES
#define OSDL_DEBUG_RECTANGLES  0
#endif

/* Define to 1 if debug mode for general renderer is to be enabled */
#ifndef OSDL_DEBUG_RENDERER
#define OSDL_DEBUG_RENDERER  0
#endif

/* Define to 1 if debug mode for resource manager is to be enabled */
#ifndef OSDL_DEBUG_RESOURCE_MANAGER
#define OSDL_DEBUG_RESOURCE_MANAGER  1
#endif

/* Define to 1 if debug mode for scheduler is to be enabled */
#ifndef OSDL_DEBUG_SCHEDULER
#define OSDL_DEBUG_SCHEDULER  0
#endif

/* Define to 1 if debug mode for sprites is to be enabled */
#ifndef OSDL_DEBUG_SPRITE
#define OSDL_DEBUG_SPRITE  0
#endif

/* Define to 1 if debug mode for surfaces is to be enabled */
#ifndef OSDL_DEBUG_SURFACE
#define OSDL_DEBUG_SURFACE  0
#endif

/* Define to 1 if debug mode for general video is to be enabled */
#ifndef OSDL_DEBUG_VIDEO
#define OSDL_DEBUG_VIDEO  0
#endif

/* Define to 1 if debug mode for video renderer is to be enabled */
#ifndef OSDL_DEBUG_VIDEO_RENDERER
#define OSDL_DEBUG_VIDEO_RENDERER  1
#endif

/* Define to 1 if debug mode for widgets is to be enabled */
#ifndef OSDL_DEBUG_WIDGET
#define OSDL_DEBUG_WIDGET  0
#endif

/* Use the Apple OpenGL framework. */
/* #undef OSDL_USES_APPLE_OPENGL_FRAMEWORK */

/* Define to 1 if you have the <Ceylan.h> header file. */
#ifndef OSDL_USES_CEYLAN_H
#define OSDL_USES_CEYLAN_H  1
#endif

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#ifndef OSDL_USES_DIRENT_H
#define OSDL_USES_DIRENT_H  1
#endif

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef OSDL_USES_DLFCN_H
#define OSDL_USES_DLFCN_H  1
#endif

/* Define to 1 if you have the `fork' function. */
#ifndef OSDL_USES_FORK
#define OSDL_USES_FORK  1
#endif

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef OSDL_USES_INTTYPES_H
#define OSDL_USES_INTTYPES_H  1
#endif

/* Define to 1 if you have the <memory.h> header file. */
#ifndef OSDL_USES_MEMORY_H
#define OSDL_USES_MEMORY_H  1
#endif

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef OSDL_USES_NDIR_H */

/* Define if you have POSIX threads libraries and header files. */
#ifndef OSDL_USES_PTHREAD
#define OSDL_USES_PTHREAD  1
#endif

/* Define to 1 if `stat' has the bug that it succeeds when given the
   zero-length file name argument. */
/* #undef OSDL_USES_STAT_EMPTY_STRING_BUG */

/* Define to 1 if stdbool.h conforms to C99. */
#ifndef OSDL_USES_STDBOOL_H
#define OSDL_USES_STDBOOL_H  1
#endif

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef OSDL_USES_STDINT_H
#define OSDL_USES_STDINT_H  1
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef OSDL_USES_STDLIB_H
#define OSDL_USES_STDLIB_H  1
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef OSDL_USES_STRINGS_H
#define OSDL_USES_STRINGS_H  1
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef OSDL_USES_STRING_H
#define OSDL_USES_STRING_H  1
#endif

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef OSDL_USES_SYS_DIR_H */

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef OSDL_USES_SYS_NDIR_H */

/* Define to 1 if you have the <sys/select.h> header file. */
#ifndef OSDL_USES_SYS_SELECT_H
#define OSDL_USES_SYS_SELECT_H  1
#endif

/* Define to 1 if you have the <sys/socket.h> header file. */
#ifndef OSDL_USES_SYS_SOCKET_H
#define OSDL_USES_SYS_SOCKET_H  1
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef OSDL_USES_SYS_STAT_H
#define OSDL_USES_SYS_STAT_H  1
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef OSDL_USES_SYS_TYPES_H
#define OSDL_USES_SYS_TYPES_H  1
#endif

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
#ifndef OSDL_USES_SYS_WAIT_H
#define OSDL_USES_SYS_WAIT_H  1
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef OSDL_USES_UNISTD_H
#define OSDL_USES_UNISTD_H  1
#endif

/* Define to 1 if you have the <utime.h> header file. */
#ifndef OSDL_USES_UTIME_H
#define OSDL_USES_UTIME_H  1
#endif

/* Define to 1 if `utime(file, NULL)' sets file's timestamp to the present. */
#ifndef OSDL_USES_UTIME_NULL
#define OSDL_USES_UTIME_NULL  1
#endif

/* Define to 1 if you have the `vfork' function. */
#ifndef OSDL_USES_VFORK
#define OSDL_USES_VFORK  1
#endif

/* Define to 1 if you have the <vfork.h> header file. */
/* #undef OSDL_USES_VFORK_H */

/* Define to 1 if you have the <windows.h> header file. */
/* #undef OSDL_USES_WINDOWS_H */

/* Define to 1 if `fork' works. */
#ifndef OSDL_USES_WORKING_FORK
#define OSDL_USES_WORKING_FORK  1
#endif

/* Define to 1 if `vfork' works. */
#ifndef OSDL_USES_WORKING_VFORK
#define OSDL_USES_WORKING_VFORK  1
#endif

/* Define to 1 if the system has the type `_Bool'. */
#ifndef OSDL_USES__BOOL
#define OSDL_USES__BOOL  1
#endif

/* Current Libtool version for the OSDL library */
#ifndef OSDL_LIBTOOL_VERSION
#define OSDL_LIBTOOL_VERSION  "0.5.0"
#endif

/* Define to 1 if `lstat' dereferences a symlink specified with a trailing
   slash. */
#ifndef OSDL_LSTAT_FOLLOWS_SLASHED_SYMLINK
#define OSDL_LSTAT_FOLLOWS_SLASHED_SYMLINK  1
#endif

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#ifndef OSDL_LT_OBJDIR
#define OSDL_LT_OBJDIR  ".libs/"
#endif

/* Name of package */
#ifndef OSDL_PACKAGE
#define OSDL_PACKAGE  "osdl"
#endif

/* Define to the address where bug reports for this package should be sent. */
#ifndef OSDL_PACKAGE_BUGREPORT
#define OSDL_PACKAGE_BUGREPORT  "osdl-bugs@lists.sourceforge.net"
#endif

/* Define to the full name of this package. */
#ifndef OSDL_PACKAGE_NAME
#define OSDL_PACKAGE_NAME  "OSDL"
#endif

/* Define to the full name and version of this package. */
#ifndef OSDL_PACKAGE_STRING
#define OSDL_PACKAGE_STRING  "OSDL 0.5"
#endif

/* Define to the one symbol short name of this package. */
#ifndef OSDL_PACKAGE_TARNAME
#define OSDL_PACKAGE_TARNAME  "osdl"
#endif

/* Define to the version of this package. */
#ifndef OSDL_PACKAGE_VERSION
#define OSDL_PACKAGE_VERSION  "0.5"
#endif

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef OSDL_PTHREAD_CREATE_JOINABLE */

/* Define as the return type of signal handlers (`int' or `void'). */
#ifndef OSDL_RETSIGTYPE
#define OSDL_RETSIGTYPE  void
#endif

/* Define to the type of arg 1 for `select'. */
#ifndef OSDL_SELECT_TYPE_ARG1
#define OSDL_SELECT_TYPE_ARG1  int
#endif

/* Define to the type of args 2, 3 and 4 for `select'. */
#ifndef OSDL_SELECT_TYPE_ARG234
#define OSDL_SELECT_TYPE_ARG234  (fd_set *)
#endif

/* Define to the type of arg 5 for `select'. */
#ifndef OSDL_SELECT_TYPE_ARG5
#define OSDL_SELECT_TYPE_ARG5  (struct timeval *)
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef OSDL_STDC_HEADERS
#define OSDL_STDC_HEADERS  1
#endif

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#ifndef OSDL_TIME_WITH_SYS_TIME
#define OSDL_TIME_WITH_SYS_TIME  1
#endif

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
/* #undef OSDL_TM_IN_SYS_TIME */

/* Define to 1 if OSDL is to use Agar */
#ifndef OSDL_USES_AGAR
#define OSDL_USES_AGAR  1
#endif

/* Define to 1 if OSDL is to use libpng */
#ifndef OSDL_USES_LIBPNG
#define OSDL_USES_LIBPNG  1
#endif

/* Define to 1 if OSDL is to use OpenGL and GLU */
#ifndef OSDL_USES_OPENGL
#define OSDL_USES_OPENGL  1
#endif

/* Define to 1 if OSDL is to use PhysicsFS */
#ifndef OSDL_USES_PHYSICSFS
#define OSDL_USES_PHYSICSFS  1
#endif

/* Define to 1 if OSDL is to use SDL */
#ifndef OSDL_USES_SDL
#define OSDL_USES_SDL  1
#endif

/* Define to 1 if OSDL is to use SDL_gfx */
#ifndef OSDL_USES_SDL_GFX
#define OSDL_USES_SDL_GFX  1
#endif

/* Define to 1 if OSDL is to use SDL_image */
#ifndef OSDL_USES_SDL_IMAGE
#define OSDL_USES_SDL_IMAGE  1
#endif

/* Define to 1 if OSDL is to use SDL_mixer */
#ifndef OSDL_USES_SDL_MIXER
#define OSDL_USES_SDL_MIXER  1
#endif

/* Define to 1 if OSDL is to use SDL_ttf */
#ifndef OSDL_USES_SDL_TTF
#define OSDL_USES_SDL_TTF  1
#endif

/* Define to 1 if OSDL is to use Zlib */
#ifndef OSDL_USES_ZLIB
#define OSDL_USES_ZLIB  1
#endif

/* Define to 1 if verbose mode for audio module is to be enabled */
#ifndef OSDL_VERBOSE_AUDIO_MODULE
#define OSDL_VERBOSE_AUDIO_MODULE  0
#endif

/* Define to 1 if verbose mode for basic module is to be enabled */
#ifndef OSDL_VERBOSE_BASIC_MODULE
#define OSDL_VERBOSE_BASIC_MODULE  0
#endif

/* Define to 1 if verbose mode for controller is to be enabled */
#ifndef OSDL_VERBOSE_CONTROLLER
#define OSDL_VERBOSE_CONTROLLER  0
#endif

/* Define to 1 if verbose mode for data module is to be enabled */
#ifndef OSDL_VERBOSE_DATA_MODULE
#define OSDL_VERBOSE_DATA_MODULE  0
#endif

/* Define to 1 if verbose mode for engine module is to be enabled */
#ifndef OSDL_VERBOSE_ENGINE_MODULE
#define OSDL_VERBOSE_ENGINE_MODULE  1
#endif

/* Define to 1 if verbose mode for events module is to be enabled */
#ifndef OSDL_VERBOSE_EVENTS_MODULE
#define OSDL_VERBOSE_EVENTS_MODULE  1
#endif

/* Define to 1 if verbose mode for joystick is to be enabled */
#ifndef OSDL_VERBOSE_JOYSTICK
#define OSDL_VERBOSE_JOYSTICK  0
#endif

/* Define to 1 if verbose mode for joystick handler is to be enabled */
#ifndef OSDL_VERBOSE_JOYSTICK_HANDLER
#define OSDL_VERBOSE_JOYSTICK_HANDLER  0
#endif

/* Define to 1 if verbose mode for keyboard handler is to be enabled */
#ifndef OSDL_VERBOSE_KEYBOARD_HANDLER
#define OSDL_VERBOSE_KEYBOARD_HANDLER  0
#endif

/* Define to 1 if verbose mode for mouse is to be enabled */
#ifndef OSDL_VERBOSE_MOUSE
#define OSDL_VERBOSE_MOUSE  0
#endif

/* Define to 1 if verbose mode for mouse handler is to be enabled */
#ifndef OSDL_VERBOSE_MOUSE_HANDLER
#define OSDL_VERBOSE_MOUSE_HANDLER  0
#endif

/* Define to 1 if verbose mode for video module is to be enabled */
#ifndef OSDL_VERBOSE_VIDEO_MODULE
#define OSDL_VERBOSE_VIDEO_MODULE  1
#endif

/* Version number of package */
#ifndef OSDL_VERSION
#define OSDL_VERSION  "0.5"
#endif

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef _osdl_const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef _osdl_inline */
#endif

/* Define to `int' if <sys/types.h> does not define. */
/* #undef _osdl_pid_t */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef _osdl_size_t */

/* Define as `fork' if `vfork' does not work. */
/* #undef _osdl_vfork */

/* Define to empty if the keyword `volatile' does not work. Warning: valid
   code using `volatile' can become incorrect without. Disable with care. */
/* #undef _osdl_volatile */

/* once: _SRC_CODE_OSDLCONFIG_H */
#endif
/* Note: NEVER commit (check-in) this generated file! */
