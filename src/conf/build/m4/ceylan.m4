# Autoconf macro specifically written for Ceylan users.

# This file is therefore not generated, and should be kept as it is.

# This macro is of no use for the Ceylan library itself. It is provided to
# help user code, be it other libraries or programs, to have their 
# 'configure.ac' use correctly the Ceylan library.

# This m4 macro tests for the Ceylan users everything needed, including
# appropriate Ceylan headers and library. It then records the relevant 
# settings for the build of the user program or library.

# More precisely, this macro checks that the Ceylan header (Ceylan.h) is
# available, i.e. that it exists and can be preprocessed and compiled.
# Then a check is performed to ensure the Ceylan headers and the Ceylan 
# library that will be used match, i.e. that the library implements the API
# described in the headers (depending on the version, newer headers mixed
# with an obsolete library would fail at the link step).
# Finally, a test program is built and run to check that the full tool chain 
# behaves flawlessly, including with respect to the user-specified Ceylan
# version expected by the code being built after this configuration step
# (versions are compared according to the libtool rules about specified
# downward-compatibility).


# Ceylan users can execute 'aclocal' (ex: 'aclocal -I . --output=aclocal.m4')
# to have our macros added to their 'aclocal.m4', provided that 
# CEYLAN_PATH is called from the user configure.ac.

# Note that this file (ceylan.m4) depends on pkg.m4 (it is available from the
# same directory).

# This macro will take advantage of locations specified to the configure
# script, ex: ./configure --with-ceylan-prefix=~/myPrefixedCeylanInstall

#
# CEYLAN_PATH( major, revision, age, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]] )
# 
# The three arguments designate the minimum Ceylan version to be searched for,
# according to libtool conventions:
# 	major   : the major number for this version 
#	revision: the revision number for this version 
#   age     : the interface age for this version 
# Note: age should not be needed if the libtool numbering scheme was really
# used.
#
# A Ceylan library will be searched for, and its version will be checked 
# against specified minimum version.
#
# Checks that the Ceylan library can be used, stops with AC_MSG_ERROR if 
# it failed, otherwise defines the CEYLAN_CPPFLAGS and CEYLAN_LIBS variables
# accordingly.
#
# The CEYLAN_PREFIX variable will contain the actual prefix of Ceylan
# installation.
#
# Example: in configure.ac, searching for Ceylan 0.4.0 is:
# CEYLAN_PATH( 0,4,0 )
#
AC_DEFUN([CEYLAN_PATH],
[
  # Here is specified for each given Ceylan release what are the most ancient
  # versions that are still compatible with this current version:
  CEYLAN_OLDEST_SUPPORTED_MAJOR=0
  CEYLAN_OLDEST_SUPPORTED_MINOR=6  
  
  # Setting the install path of the Ceylan library:
  CEYLAN_LIBS=" -lCeylan "
  AC_ARG_WITH(ceylan-prefix,
    AS_HELP_STRING([--with-ceylan-prefix=PFX],[Prefix where Ceylan is installed (optional)]),
      [
        CEYLAN_CPPFLAGS="-I${withval}/include/Ceylan"
        CEYLAN_LIBS="-L${withval}/lib -lCeylan"
		CEYLAN_PREFIX="${withval}"
		# Get ready for configure tests:
		LD_LIBRARY_PATH="${withval}/lib:$LD_LIBRARY_PATH"
		export LD_LIBRARY_PATH
      ],[
		default_install_path=/usr/local
	  	# No path specified, trying to guess it:
		# Is pkg-config available?
		
		# Note: see the top of our pkg.m4 to understand why this file is 
		# needed.
		
		AC_PATH_PROG(PKG_CONFIG,pkg-config,[no]) 
		if test $PKG_CONFIG = "no" ; then 
			AC_MSG_WARN([pkg-config tool not found, using default install path ${default_install_path}])
        	CEYLAN_CPPFLAGS="-I${default_install_path}/include/Ceylan"
        	CEYLAN_LIBS="-L${default_install_path}/lib -lCeylan"
			CEYLAN_PREFIX="${default_install_path}"
		else 
			# Here pkg-config is available.
			# Use it if possible, otherwise choose default values:
			# PKG_CONFIG_PATH should contain /usr/local/lib/pkgconfig, use
			# export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH
			# Check if Ceylan pkg-config configuration file is found:
			AC_MSG_CHECKING([for ceylan-$1.$2.pc (Ceylan pkg-config configuration file)]) 
			if $PKG_CONFIG --exists ceylan-$1.$2 ; then
				AC_MSG_RESULT(yes) 
				PKG_CHECK_MODULES(CEYLAN,ceylan-$1.$2 >= $CEYLAN_OLDEST_SUPPORTED_MAJOR.$CEYLAN_OLDEST_SUPPORTED_MINOR,
				[
					# pkg-config succeeded and set CEYLAN_CFLAGS and 
					# CEYLAN_LIBS:
					CEYLAN_CPPFLAGS="${CEYLAN_CFLAGS} ${CEYLAN_CPPFLAGS}"
					# CEYLAN_LIBS left as set (overwritten but was empty here)
					CEYLAN_PREFIX=`$PKG_CONFIG ceylan-$1.$2 --variable=prefix`
					# Get ready for configure tests:
					LD_LIBRARY_PATH=`$PKG_CONFIG ceylan-$1.$2 --variable=libdir`:$LD_LIBRARY_PATH
					export LD_LIBRARY_PATH
				],
				[
					AC_MSG_WARN([pkg-config failed for ceylan-$1.$2, defaulting to install path $default_install_path])
        			CEYLAN_CPPFLAGS="-I${default_install_path}/include/Ceylan"
        			CEYLAN_LIBS="-L${default_install_path}/lib -lCeylan"
					CEYLAN_PREFIX="${default_install_path}"
				])
			else
				AC_MSG_WARN([pkg-config did not find ceylan-$1.$2.pc, using default install path ${default_install_path}])
        		CEYLAN_CPPFLAGS="-I${default_install_path}/include/Ceylan"
        		CEYLAN_LIBS="-L${default_install_path}/lib -lCeylan"
				CEYLAN_PREFIX="${default_install_path}"
			fi
		fi			
     	])
  # Select relevant system libraries needed by Ceylan (pthread, network, etc.):
  case "$target" in
      *-*-bsdi*)
    	  pthread_cflags="-D_REENTRANT -D_THREAD_SAFE"
    	  pthread_lib=""
    	  ;;
      *-*-darwin*)
    	  pthread_cflags="-D_THREAD_SAFE"
    	  # causes Carbon.p complaints?
    	  # pthread_cflags="-D_REENTRANT -D_THREAD_SAFE"
    	  ;;
      *-*-freebsd*)
    	  pthread_cflags="-D_REENTRANT -D_THREAD_SAFE"
    	  pthread_lib="-pthread"
    	  ;;
      *-*-netbsd*)
	  	  # -I/usr/pkg/include removed since its pthread.h had 
		  # conflicting types with /usr/include/pthread_types.h:
    	  pthread_cflags="-D_REENTRANT"
		  # -lsem removed, it does not exist with NetBSD 2.0.2:
    	  pthread_lib="-L/usr/pkg/lib -lpthread"
		  # Needed for AC_RUN_IFELSE:
		  LD_LIBRARY_PATH="/usr/pkg/lib:$LD_LIBRARY_PATH"
		  export LD_LIBRARY_PATH
    	  ;;
      *-*-openbsd*)
    	  pthread_cflags="-D_REENTRANT"
    	  pthread_lib="-pthread"
    	  ;;
      *-*-solaris*)
    	  pthread_cflags="-D_REENTRANT"
		  # Network is managed here as well:
    	  pthread_lib="-lpthread -lposix4 -lresolv -lsocket -lnsl"
    	  ;;
      *-*-sysv5*)
    	  pthread_cflags="-D_REENTRANT -Kthread"
    	  pthread_lib=""
    	  ;;
      *-*-irix*)
    	  pthread_cflags="-D_SGI_MP_SOURCE"
    	  pthread_lib="-lpthread"
    	  ;;
      *-*-aix*)
    	  pthread_cflags="-D_REENTRANT -mthreads"
    	  pthread_lib="-lpthread"
    	  ;;
      *-*-hpux11*)
    	  pthread_cflags="-D_REENTRANT"
    	  pthread_lib="-L/usr/lib -lpthread"
    	  ;;
      *-*-qnx*)
    	  pthread_cflags=""
    	  pthread_lib=""
    	  ;;
      *-*-osf*)
    	  pthread_cflags="-D_REENTRANT"
    	  if test x$ac_cv_prog_gcc = xyes; then
    		  pthread_lib="-lpthread -lrt"
    	  else
    		  pthread_lib="-lpthread -lexc -lrt"
    	  fi
    	  ;;
      *)
    	  pthread_cflags="-D_REENTRANT"
    	  pthread_lib="-lpthread"
    	  ;;
  esac
  
  if test x$ac_cv_cxx_compiler_gnu = xyes; then
    # Needed to avoid gcc warning triggered by a needed Visual C++ pragma:
    CEYLAN_CPPFLAGS="$CEYLAN_CPPFLAGS -Wno-unknown-pragmas"
  fi
   
  # CEYLAN_DLL section.


  # For all builds, CEYLAN_DLL will be actually defined
  # thanks to command-line preprocessor options (-DCEYLAN_DLL="...")
  # so that headers can avoid #including platform-specific configuration
  # files.

  # Needed when building a library on Windows platforms:
  CEYLAN_DLL_IMPORT_WINDOWS="__declspec( dllimport )"

  CEYLAN_DLL=

  case "$target" in
	
    *-*-win32*) 
        CEYLAN_DLL="${CEYLAN_DLL_IMPORT_WINDOWS}"
        ;;

	*-*-cygwin*)
        CEYLAN_CPPFLAGS="$CEYLAN_CPPFLAGS -I/usr/include/mingw -DWIN32 -Uunix -mno-cygwin"
        CEYLAN_LIBS="$CEYLAN_LIBS -mno-cygwin"
        CEYLAN_DLL="${CEYLAN_DLL_IMPORT_WINDOWS}"
        ;;
		
	*-*-mingw32*)
        CEYLAN_LIBS="-lmingw32 $CEYLAN_LIBS -mwindows"
        CEYLAN_DLL="${CEYLAN_DLL_IMPORT_WINDOWS}"
        ;;
													
  esac
	  

  # Wanting CEYLAN_DLL to be replaced by nothing, not "", on UNIX:	  
  if test "${CEYLAN_DLL}" = "" ; then
 	 CEYLAN_CPPFLAGS="$CEYLAN_CPPFLAGS $pthread_cflags -DCEYLAN_DLL="
  else	 
 	 CEYLAN_CPPFLAGS="$CEYLAN_CPPFLAGS $pthread_cflags -DCEYLAN_DLL=\"${CEYLAN_DLL}\""
  fi

  CEYLAN_LIBS="$CEYLAN_LIBS $pthread_lib"
  
  # Uncomment to debug:
  #AC_MSG_NOTICE([CEYLAN_CPPFLAGS = $CEYLAN_CPPFLAGS])
  #AC_MSG_NOTICE([CEYLAN_LIBS     = $CEYLAN_LIBS])  

  AC_SUBST(CEYLAN_CPPFLAGS)
  AC_SUBST(CEYLAN_LIBS)
  
  # Checking for Ceylan header files:
  have_ceylan=yes
  AC_MSG_NOTICE([checking for the Ceylan library])
  # AC_CHECK_HEADERS checks better than AC_PREPROC_IFELSE:
  # (compilable more interesting than only preprocessable)
  AC_LANG_PUSH([C++])
  
  # Update the build flags for testing:
  ac_save_CPPFLAGS="$CPPFLAGS"
  ac_save_LIBS="$LIBS"
  CPPFLAGS="$CPPFLAGS $CEYLAN_CPPFLAGS"
  LIBS="$LIBS $CEYLAN_LIBS"
  
  AC_CHECK_HEADERS([Ceylan.h],[],[have_ceylan=no])
  if test x$have_ceylan = xno; then
    AC_MSG_ERROR([No usable Ceylan header file (Ceylan.h) found. If the Ceylan library has been installed in a non-standard location (ex: when configuring it, --prefix=$HOME/myCeylanInstall was added), use the --with-ceylan-prefix option, ex: --with-ceylan-prefix=$HOME/myCeylanInstall.])
  fi
  # Now we must check that we will link indeed to the expected version
  # of Ceylan library, as specified by CEYLAN_PATH arguments:
  # Note: 
  #   - this test will be disabled if cross-compiling
  #   - we do not use AC_CACHE_CHECK and al on purpose, to avoid the
  # traps of cached entries.
  have_working_ceylan_library=yes
  AC_RUN_IFELSE([
  	AC_LANG_PROGRAM(
	  [[#include "Ceylan.h"]],[[Ceylan::Uint32 x;]])],
	[],[have_working_ceylan_library=no],[AC_MSG_WARN([No Ceylan library sanity test run, since cross-compiling])])
  AC_MSG_CHECKING([whether the Ceylan library can be linked to])
  if test x$have_working_ceylan_library = xyes; then
  	# Cross-compilation goes here, too.
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([The Ceylan library could not be successfully linked to.])  
  fi
  # We can link to the Ceylan library, but do the headers we see can work
  # with the library we link to?
  have_ceylan_headers_matching_library_version=yes
  AC_RUN_IFELSE([
  	AC_LANG_PROGRAM(
	  [[#include "Ceylan.h"]],[[CHECK_CEYLAN_VERSIONS();]])],
	[],[have_ceylan_headers_matching_library_version=no],[AC_MSG_WARN([No Ceylan library and headers matching test run, since cross-compiling])])
  AC_MSG_CHECKING([whether the Ceylan headers match the library version])
  if test x$have_ceylan_headers_matching_library_version = xyes; then
  	# Cross-compilation goes here, too.
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([The Ceylan installation does not seem to be clean, since headers did not match the library version.])  
  fi
  # We can link to the library, and it is compatible with the Ceylan headers
  # being used, but is this pack compatible with the Ceylan version the user
  # specified? 
  have_compatible_ceylan_version=yes
  AC_RUN_IFELSE([
  	AC_LANG_PROGRAM(
	  [[#include "Ceylan.h"]],[[Ceylan::LibtoolVersion targetVersion( "$1.$2.$3" ) ; if ( ! Ceylan::GetVersion().isCompatibleWith( targetVersion ) ) return 1 ;]])],
	[],[have_compatible_ceylan_version=no],[AC_MSG_WARN([No Ceylan version compatibility test run, since cross-compiling])])
  AC_MSG_CHECKING([whether we are linked to a $1.$2.$3-compatible Ceylan version])
  if test x$have_compatible_ceylan_version = xyes; then
    AC_MSG_RESULT([yes])
  else
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([The Ceylan library we would link to is not compatible with the one expected by the program being configured])  
  fi  
  
  # Restore original settings, now that CEYLAN_* counterparts are validated:
  CPPFLAGS="$ac_save_CPPFLAGS"
  LIBS="$ac_save_LIBS"

  AC_LANG_POP([C++]) 
])
