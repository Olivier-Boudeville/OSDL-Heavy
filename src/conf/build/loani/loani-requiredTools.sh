# This script is made to be sourced by LOANI when retrieving required tools.
# Therefore, all tools managed here should be strict LOANI prerequisites.

# Creation date: 2004, February 22.
# Author: Olivier Boudeville (olivier.boudeville@esperide.com)


# VCS tags to select versions to retrieve (if use_current_vcs not selected):
latest_stable_ceylan="release-0.7.0"
latest_stable_osdl="release-0.5.0"


################################################################################

#TRACE "[loani-requiredTools] Begin"

# Required tools section.
# guichan/guichan_win not listed, since has been deprecated here by Agar.

# Note: if this list is to be updated, update as well:
# osdl/OSDL/trunk/src/conf/build/loani/update-LOANI-mirror.sh


if [ $is_windows -eq 0 ] ; then

  # Always remember that, on Windows, DLL are searched through the PATH, not the
  # LD_LIBRARY_PATH.

  # Windows special case:
  REQUIRED_TOOLS="SDL_win zlib_win libjpeg_win libpng_win SDL_image_win SDL_gfx_win freetype_win SDL_ttf_win libogg_win libvorbis_win SDL_mixer_win PhysicsFS_win PCRE_win FreeImage_win CEGUI_win"

  if [ $manage_only_third_party_tools -eq 1 ] ; then

	REQUIRED_TOOLS="${REQUIRED_TOOLS} Ceylan_win OSDL_win"

  fi

  # For Ceylan and OSDL:
  use_svn=0

  # Where LOANI-specific package solutions are stored:
  WINDOWS_SOLUTIONS_ROOT="$repository/visual-express"

  if [ ! -d "${WINDOWS_SOLUTIONS_ROOT}" ] ; then

	echo "Error, unable to find the directory where Windows Visual Express solutions are stored (searched for ${WINDOWS_SOLUTIONS_ROOT})." 1>&2
	exit 40

  fi


  # Build type for libraries:
  #  - either debug or release
  #  - either classical or multithread (mt)
  library_build_type="debug-mt"

  dll_install_dir="${alternate_prefix}/OSDL-libraries/${library_build_type}/dll"
  ${MKDIR} -p ${dll_install_dir}

  lib_install_dir="${alternate_prefix}/OSDL-libraries/${library_build_type}/lib"
  ${MKDIR} -p ${lib_install_dir}

else

  if [ $target_nds -eq 1 ] ; then

	# All non-windows non-DS platforms should build everything from sources:

	# Note: as of Tuesday, November 30, 2010, we removed 'zlib libpng' as when
	# their respective versions (1.2.5 and 1.4.4) are used, programs using them
	# crash.
	# Ex: > ./darm.exe
	# Checkpoint [1]: Launching DARM.
	# Checkpoint [2]: Initializing sound adjustment model.
	# Checkpoint [3]: Initializing sound adjustment view.
	# libpng warning: Image height exceeds user limit in IHDR
	# libpng error: Invalid IHDR data
	# Segmentation fault
	#
	# whereas morse-icon.tex2D is a normal PNG image, 300 x 286, 8-bit/color
	# RGBA, non-interlaced
	#
	# SDL_gfx (which was just after SDL_image) was removed, due to build
	# problems with the 2.0.22 version (m4-related).
	REQUIRED_TOOLS="libtool SDL libjpeg SDL_image freetype SDL_ttf libogg libvorbis SDL_mixer PCRE FreeImage CEGUI PhysicsFS"

	if [ $manage_only_third_party_tools -eq 1 ] ; then

	  REQUIRED_TOOLS="${REQUIRED_TOOLS} Ceylan OSDL"

	fi

  else

	# Not on Windows and aiming at the Nintendo DS:

	REQUIRED_TOOLS="dlditool"

  fi

fi

# Maybe libmikmod should be added for SDL_mixer, if MOD music was to be
# supported.


# TIFF library was removed from the list, since its build is a nonsense and that
# image format is not compulsory: PNG and JPEG are better choices and should be
# used instead.


# Creating retrieve list.
target_list="$REQUIRED_TOOLS"


LOG_STATUS()
{
	DEBUG "$*"
	echo 1>>"$LOG_OUTPUT"
	echo 1>>"$LOG_OUTPUT"
	echo "--------------> $*" 1>>"$LOG_OUTPUT"
}


LOG_STATUS "Scheduling retrieval of required tools ($REQUIRED_TOOLS)."

GenerateWithVisualExpress()
# Launches Visual Express 2005 with specified solution so that the user
# regenerates it.
#
# Usage: GenerateWithVisualExpress <package name> <solution path>
# Ex: GenerateWithVisualExpress SDL a/dir/SDL.sln
{

  PACKAGE_NAME="$1"
  SOLUTION_PATH="$2"

  if [ ! -f "${SOLUTION_PATH}" ] ; then
		ERROR "Unable to find solution ${SOLUTION_PATH} for ${PACKAGE_NAME}."
		exit 20
  fi

  WIN_SOLUTION_PATH=`cygpath -w ${SOLUTION_PATH}`

  if [ $be_quiet -eq 1 ] ; then
	DISPLAY ""
	DISPLAY "Visual Express will be launched now to generate ${PACKAGE_NAME}, choose regenerate the full solution, and exit on build success."
	#waitForKey "< Hit enter and wait for the IDE to be launched >"
  fi

  DEBUG "Loading solution in ${WIN_SOLUTION_PATH}"

  PREVIOUS_PATH=`pwd`

  cd "${VISUAL_ROOT}"

  ./${VISUAL_CMD} "${WIN_SOLUTION_PATH}"

	if [ $? != 0 ] ; then
		ERROR "Unable to generate ${PACKAGE_NAME} from solution ."${SOLUTION_PATH}
		exit 21
	fi

  cd "${PREVIOUS_PATH}"

}



################################################################################
################################################################################
# SDL
################################################################################
################################################################################


#TRACE "[loani-requiredTools] SDL"

################################################################################
# SDL for non-Windows platforms:
################################################################################


getSDL()
{
	LOG_STATUS "Getting SDL..."
	launchFileRetrieval SDL
	return $?
}



prepareSDL()
{

	LOG_STATUS "Preparing SDL..."
	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunzipped."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "SDL        "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${SDL_ARCHIVE} ${SDL_ARCHIVE}.save && ${GUNZIP} -f ${SDL_ARCHIVE} && ${TAR} -xvf "SDL-${SDL_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${SDL_ARCHIVE}."
		LOG_STATUS "Restoring ${SDL_ARCHIVE}."
		${MV} -f ${SDL_ARCHIVE}.save ${SDL_ARCHIVE}
		exit 10
	fi

	${MV} -f ${SDL_ARCHIVE}.save ${SDL_ARCHIVE}
	${RM} -f "SDL-${SDL_VERSION}.tar"

	printOK

}



generateSDL()
{

	LOG_STATUS "Generating SDL..."


	cd "SDL-${SDL_VERSION}"

	printItem "configuring"

	if [ -n "$prefix" ] ; then
	{

		if [ $is_windows -eq 0 ] ; then
			SDL_PREFIX=`cygpath -w ${prefix}/SDL-${SDL_VERSION} | ${SED} 's|\\\|/|g'`
		else
			SDL_PREFIX="${prefix}/SDL-${SDL_VERSION}"
		fi


		${MKDIR} -p ${SDL_PREFIX}

		# DirectFB disabled, as build will fail on openSuse if corresponding
		# package is not installed (Ubuntu can cope with this situation though).

		# Note also that, at least on Ubuntu Maverick, the libpulse-dev package
		# must be installed beforehand, otherwise PulseAudio support will be
		# deactivated, and SDL will say: No available audio device.
		setBuildEnv ./configure --enable-video-directfb=no --disable-rpath --prefix=${SDL_PREFIX} --exec-prefix=${SDL_PREFIX}

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ./configure
	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure SDL."
		exit 11
	fi

	printOK

	printItem "building"

	{

		 setBuildEnv ${MAKE} LDFLAGS="-lgcc_s"

	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build SDL."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{

		echo "# SDL section." >> ${OSDL_ENV_FILE}

		echo "SDL_PREFIX=${SDL_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export SDL_PREFIX" >> ${OSDL_ENV_FILE}
		echo "PATH=\$SDL_PREFIX/bin:\${PATH}" >> ${OSDL_ENV_FILE}

		echo "LD_LIBRARY_PATH=\$SDL_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		PATH=${SDL_PREFIX}/bin:${PATH}
		export PATH

		LD_LIBRARY_PATH=${SDL_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		if [ $is_windows -eq 0 ] ; then

			PATH=${SDL_PREFIX}/lib:${PATH}
			export PATH

			echo "PATH=\$SDL_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}

		LIBPATH="-L${SDL_PREFIX}/lib"

		# Do not ever imagine that to avoid bad nedit syntax highlighting
		# you could change:
		# include/*.h to "include/*.h" in next line.
		# It would fail at runtime with "include/*.h" not found...

		setBuildEnv ${MAKE} install && ${CP} -f include/*.h ${SDL_PREFIX}/include/SDL

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install SDL."
		exit 13
	fi

	if [ $is_windows -eq 0 ] ; then
			${MV} -f ${SDL_PREFIX}/bin/*.dll ${SDL_PREFIX}/lib
	fi

	printOK

	printEndList

	LOG_STATUS "SDL successfully installed."

	cd "$initial_dir"

}



cleanSDL()
{
	LOG_STATUS "Cleaning SDL build tree..."
	${RM} -rf "SDL-${SDL_VERSION}"
}




################################################################################
# SDL build thanks to Visual Express.
################################################################################


getSDL_win()
{
	LOG_STATUS "Getting SDL for windows..."
	launchFileRetrieval SDL_win
	return $?
}


prepareSDL_win()
{

	LOG_STATUS "Preparing SDL for windows.."
	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "SDL        "

	printItem "extracting"

	cd $repository

	{
		${UNZIP} -o ${SDL_win_ARCHIVE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${SDL_win_ARCHIVE}."
		exit 10
	fi

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/SDL-from-LOANI" "SDL-${SDL_win_VERSION}"

	if [ $? != 0 ] ; then
		ERROR "Unable to copy SDL solution in build tree."
		exit 11
	fi

	printOK

}


generateSDL_win()
{

	LOG_STATUS "Generating SDL for windows..."

	cd "SDL-${SDL_win_VERSION}"

	printItem "configuring"
	printOK

	sdl_solution=`pwd`"/SDL-from-LOANI/SDL-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress SDL ${sdl_solution}
	printOK

	printItem "installing"

	# Take care of the exported header files (API):
	sdl_install=${alternate_prefix}/SDL-${SDL_win_VERSION}
	${MKDIR} -p ${sdl_install}
	${CP} -rf include ${sdl_install}
	printOK

	printEndList

	LOG_STATUS "SDL successfully installed."

	cd "$initial_dir"

}


cleanSDL_win()
{
	LOG_STATUS "Cleaning SDL build tree..."
	${RM} -rf "SDL-${SDL_win_VERSION}"
}




################################################################################
################################################################################
# SDL_image prerequisites
################################################################################
################################################################################


# Prerequisites of SDL_image: JPEG library, PNG library, zlib library.
# TIFF support is disabled for the moment.
# For Visual Express builds, these prerequisites are managed from SDL_image_win.

################################################################################
################################################################################
# JPEG (libjpeg)
################################################################################
################################################################################


#TRACE "[loani-requiredTools] JPEG"


# This will not rightly compile on windows since this ltconfig does not support
# cygwin or mingw.
#
# To overcome this issue, prebuilt binaries will be installed instead.  In the
# future, one might create its own Makefile for JPEG and maybe make use of
# libtool 1.5.2 to directly generate the DLL without the configure nightmare.
#
# Maybe one could be inspired by the pure cygwin makefile.

# However, even on Windows, the building of the static library is still
# maintained, since one of its byproducts is a generated header file which is
# needed by SDL_image's own building.


getlibjpeg()
{
		LOG_STATUS "Getting JPEG library..."
		launchFileRetrieval libjpeg
		return $?
}


preparelibjpeg()
{

	LOG_STATUS "Preparing JPEG library..."
	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunzipped."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "libjpeg    "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${libjpeg_ARCHIVE} ${libjpeg_ARCHIVE}.save && ${GUNZIP} -f ${libjpeg_ARCHIVE} && ${TAR} -xvf "jpegsrc.v${libjpeg_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${libjpeg_ARCHIVE}."
		LOG_STATUS "Restoring ${libjpeg_ARCHIVE}."
		${MV} -f ${libjpeg_ARCHIVE}.save ${libjpeg_ARCHIVE}
		exit 10
	fi

	${MV} -f ${libjpeg_ARCHIVE}.save ${libjpeg_ARCHIVE}
	${RM} -f "jpegsrc.v${libjpeg_VERSION}.tar"

	printOK

}


generatelibjpeg()
{

	LOG_STATUS "Generating libjpeg..."

	cd "jpeg-${libjpeg_VERSION}"

	# Everything is always built since, even on Windows, the jpeg.dll cannot be
	# used without jconfig.h which ... is only generated by the build process!

	printItem "configuring"

	# Non windows: standard way of building.

	# On some platforms, libtool is unable to guess the correct host type:
	# config.guess fails to detect anything as soon as --enable-shared is added.
	#
	# One attempt is being made in case of failure, to test whether it cannot be
	# the most common platform used for OSDL by far, i686-pc-linux-gnu.

	# if ! ./ltconfig ltmain.sh 1>>"$LOG_OUTPUT" 2>&1 ; then

	#		LOG_STATUS "ltconfig host detection will fail in the configure step."

	#			if [ $is_linux -eq 0 ] ; then
	#				WORK_AROUND_PLATFORM=i686-pc-linux-gnu
	#			LOG_STATUS "Linux detected, trying workaround of most common platform (${WORK_AROUND_PLATFORM})."
	#				${CAT} configure | ${SED} "s|ltmain.sh$|ltmain.sh ${WORK_AROUND_PLATFORM}|1" > configure.tmp
	#				${MV} -f configure.tmp configure
	#				${CHMOD} +x configure
	#			else
	#				echo
	#				ERROR "Unable to pre-configure libjpeg, host detection failed and there is no host work-around for your platform."
	#				exit 11
	#			fi
	# fi

	if [ -n "$prefix" ] ; then
	{


		libjpeg_PREFIX=${prefix}/jpeg-${libjpeg_VERSION}

		if [ $is_mingw -eq 0 ] ; then
			OLD_LD=$LD
			LD=${MINGW_PATH}/ld
			export LD
			DEBUG "Selected LD is $LD."

			if [ ! -x "$LD" ] ; then
				ERROR "No executable ld found (tried $LD)."
				exit 14
			fi

		fi

		setBuildEnv --exportEnv ./configure --prefix=${libjpeg_PREFIX} --exec-prefix=${libjpeg_PREFIX} --enable-shared

	 } 1>>"$LOG_OUTPUT" 2>&1
	 else
	 {
		setBuildEnv ./configure --enable-shared
	 } 1>>"$LOG_OUTPUT" 2>&1
	 fi

	 if [ $? != 0 ] ; then
			echo
			ERROR "Unable to configure libjpeg."
			exit 12
	 fi


	printOK


	printItem "building"

	# On Windows (Cygwin/MinGW), the configure step will pretend shared
	# libraries cannot be generated: checking for ld used by GCC... no ltconfig:
	# error: no acceptable ld found in $PATH so jpeg dll won't be created.
	#
	# Nevertheless some other generated files (ex: jconfig.h) will be needed to
	# build libraries using JPEG, such as SDL_image: it remains useful.

	{
		#setBuildEnv ${MAKE} LDFLAGS="-lgcc_s"
		setBuildEnv ${MAKE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build libjpeg."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{

		LIBFLAG="-L${libjpeg_PREFIX}/lib ${LIBFLAG}"

		echo "# libjpeg section." >> ${OSDL_ENV_FILE}

		echo "libjpeg_PREFIX=${libjpeg_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export libjpeg_PREFIX" >> ${OSDL_ENV_FILE}
		echo "LD_LIBRARY_PATH=\$libjpeg_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		# In order SDL_image configure does not fail:
		LD_LIBRARY_PATH=${libjpeg_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		if [ $is_windows -eq 0 ] ; then

			PATH=${libjpeg_PREFIX}/lib:${PATH}
			export PATH

			echo "PATH=\$libjpeg_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}

		fi

		echo "" >> ${OSDL_ENV_FILE}


		${MKDIR} -p ${libjpeg_PREFIX}/include
		${MKDIR} -p ${libjpeg_PREFIX}/bin
		${MKDIR} -p ${libjpeg_PREFIX}/lib
		${MKDIR} -p ${libjpeg_PREFIX}/man/man1

		setBuildEnv ${MAKE} install prefix=${libjpeg_PREFIX}

		# Note: for some unknown reason, this build is to create a library like
		# jpeg-8c/lib/libjpeg.so.8.3.0, whereas user tools (ex: Agar, emacs,
		# etc.) will request libjpeg.so.62.0.0 instead (which is surprinsingly
		# different).
		#
		# A good work-around is simply to build them against the right (jpeg-8c)
		# headers, and to create a fake 0.62 version thanks to a symbolic link
		# like: ln -s libjpeg.so.8.3.0 libjpeg.so.62
		#
		# Moreover, for Agar, one may have to previously hide the system libjpeg
		# libraries (ex: /usr/lib/libjpeg.so.62.0.0, for example with
		# /root/hide-jpeg.sh) otherwise the -L/usr/lib64 put at the beginning of
		# the link command-line will pick them up instead (this will break the
		# build as the compile step will have recorded that it expects for
		# example 80, not 62).

		if [ $? != 0 ] ; then
			ERROR "Unable to install libjpeg."
			exit 13
		fi

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
			setBuildEnv ${MAKE} install

			if [ $? != 0 ] ; then
				echo
				ERROR "Unable to install libjpeg."
				exit 13
			fi

	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	printOK

	printEndList

	LOG_STATUS "libjpeg successfully installed."

	cd "$initial_dir"

}


cleanlibjpeg()
{
	LOG_STATUS "Cleaning JPEG library build tree..."
	${RM} -rf "jpeg-${libjpeg_VERSION}"
}



################################################################################
# libjpeg build thanks to Visual Express.
################################################################################


getlibjpeg_win()
{
	LOG_STATUS "Getting libjpeg for windows..."
	launchFileRetrieval libjpeg_win
	return $?
}


preparelibjpeg_win()
{

	LOG_STATUS "Preparing libjpeg for windows.."
	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunzipped."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "libjpeg    "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${libjpeg_win_ARCHIVE} ${libjpeg_win_ARCHIVE}.save && ${GUNZIP} -f ${libjpeg_win_ARCHIVE} && ${TAR} -xvf "jpegsrc.v${libjpeg_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${libjpeg_win_ARCHIVE}."
		exit 10
	fi

	${MV} -f ${libjpeg_win_ARCHIVE}.save ${libjpeg_win_ARCHIVE}
	${RM} -f "jpegsrc.v${libjpeg_win_VERSION}.tar"

	libjpeg_source_dir="$repository/jpeg-${libjpeg_win_VERSION}"

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/libjpeg-from-LOANI" "${WINDOWS_SOLUTIONS_ROOT}/libjpeg-from-LOANI/jmorecfg.h" ${libjpeg_source_dir}
	if [ $? != 0 ] ; then
		ERROR "Unable to copy libjpeg solution in build tree."
		exit 11
	fi

	# Prefer Visual-based config:
	${CP} -f ${libjpeg_source_dir}/jconfig.vc ${libjpeg_source_dir}/jconfig.h
	if [ $? != 0 ] ; then
		ERROR "Unable to update libjpeg config header in build tree."
		exit 12
	fi

	printOK

}


generatelibjpeg_win()
{

	LOG_STATUS "Generating libjpeg for windows..."

	cd ${libjpeg_source_dir}

	printItem "configuring"
	printOK

	libjpeg_solution=`pwd`"/libjpeg-from-LOANI/libjpeg-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress libjpeg ${libjpeg_solution}
	printOK

	printItem "installing"

	# Take care of the exported header files (API):
	libjpeg_install=${alternate_prefix}/libjpeg-${libjpeg_win_VERSION}
	libjpeg_header_install=${libjpeg_install}/include
	${MKDIR} -p ${libjpeg_header_install}
	${CP} -f jpeglib.h jconfig.h jmorecfg.h jerror.h ${libjpeg_header_install}
	printOK

	printEndList

	LOG_STATUS "libjpeg successfully installed."

	cd "$initial_dir"

}


cleanlibjpeg_win()
{
	LOG_STATUS "Cleaning libjpeg build tree..."
	${RM} -rf "jpeg-${libjpeg_VERSION}"
}




################################################################################
################################################################################
# zlib
################################################################################
################################################################################


#TRACE "[loani-requiredTools] zlib"


getzlib()
{
	LOG_STATUS "Getting zlib library..."
	launchFileRetrieval zlib
	return $?
}


preparezlib()
{

	LOG_STATUS "Preparing zlib library..."

	if findTool bunzip2 ; then
		BUNZIP2=$returnedString
	else
		ERROR "No bunzip2 tool found, whereas some files have to be bunzip2-ed."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "libzlib    "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${zlib_ARCHIVE} ${zlib_ARCHIVE}.save && ${BUNZIP2} -f ${zlib_ARCHIVE} && ${TAR} -xvf "zlib-${zlib_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${zlib_ARCHIVE}."
		LOG_STATUS "Restoring ${zlib_ARCHIVE}."
		${MV} -f ${zlib_ARCHIVE}.save ${zlib_ARCHIVE}
		exit 10
	fi

	${MV} -f ${zlib_ARCHIVE}.save ${zlib_ARCHIVE}
	${RM} -f "zlib-${zlib_VERSION}.tar"

	printOK

}


generatezlib()
{

	LOG_STATUS "Generating zlib..."

	if [ -d "zlib" ] ; then
		${RM} -rf "zlib"
	fi

	# Not all platforms have symbolic links:
	${MV} -f "zlib-${zlib_VERSION}" zlib
	cd zlib

	printItem "configuring"

	if [ -n "$prefix" ] ; then
	{

		# Note: at least with zlib-1.2.5, following patch must be applied:
		# http://sources.gentoo.org/cgi-bin/viewvc.cgi/gentoo-x86/sys-libs/zlib/files/zlib-1.2.5-lfs-decls.patch?revision=1.2

		# Otherwise: LOANI-installations/zlib-1.2.5/include/zlib.h:1583: error:
		# declaration of C function 'off_t gzseek64(void*, off_t, int)'
		# conflicts with previous declaration 'off64_t gzseek64(void*, off64_t,
		# int)'
		#
		# However after some testing, it appeared that this built zlib resulted
		# in crashed (ex: tested an otherwise functional eog).
		# Thus currently we install it in a 'deactivated' directory.
		zlib_PREFIX=${prefix}/zlib-${zlib_VERSION}

		setBuildEnv ./configure --shared --prefix=${zlib_PREFIX}

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ./configure --shared
	} 1>>"$LOG_OUTPUT" 2>&1
	fi
	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure zlib."
		exit 11
	fi

	printOK

	printItem "building"

	{

		# LDFLAGS="-lgcc_s" not added.
		setBuildEnv ${MAKE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build zlib."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{
		LIBFLAG="-L${zlib_PREFIX}/lib ${LIBFLAG}"

		echo "# zlib section." >> ${OSDL_ENV_FILE}

		echo "zlib_PREFIX=${zlib_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export zlib_PREFIX" >> ${OSDL_ENV_FILE}
		echo "LD_LIBRARY_PATH=\$zlib_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		# In order SDL_image configure does not fail:
		LD_LIBRARY_PATH=${zlib_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		if [ $is_windows -eq 0 ] ; then

			PATH=${zlib_PREFIX}/lib:${PATH}
			export PATH

			echo "PATH=\$zlib_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}

		# Currently (1.2.5) causes crashes, hence deactivated:
		setBuildEnv ${MAKE} install prefix=${zlib_PREFIX}-deactivated

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{

		setBuildEnv ${MAKE} install

	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install zlib."
		exit 13
	fi

	printOK

	printEndList

	LOG_STATUS "zlib successfully installed."

	cd "$initial_dir"

}


cleanzlib()
{
	LOG_STATUS "Cleaning zlib library build tree..."
	${RM} -rf zlib
}



################################################################################
# zlib build thanks to Visual Express.
################################################################################

#TRACE "[loani-requiredTools] zlib for Visual Express targets"


getzlib_win()
{
	LOG_STATUS "Getting zlib for windows..."
	launchFileRetrieval zlib_win
	return $?
}


preparezlib_win()
{

	LOG_STATUS "Preparing zlib for windows.."

	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "zlib       "

	printItem "extracting"

	cd $repository

	# Archive content is not contained into a unique root directory !

	zlib_source_dir="zlib-${zlib_win_VERSION}"
	${MKDIR} -p ${zlib_source_dir}
	${CP} -f ${zlib_win_ARCHIVE} ${zlib_source_dir}
	cd ${zlib_source_dir}

	{
		${UNZIP} -o ${zlib_win_ARCHIVE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${zlib_win_ARCHIVE}."
		exit 10
	fi

	zlib_install_dir="${prefix}/zlib-${zlib_win_VERSION}"
	${MKDIR} -p ${zlib_install_dir}

	cd $repository

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/zlib-from-LOANI" "zlib-${zlib_win_VERSION}"

	if [ $? != 0 ] ; then
		ERROR "Unable to copy zlib solution in build tree."
		exit 11
	fi

	printOK

}


generatezlib_win()
{


	LOG_STATUS "Generating zlib for windows..."

	cd "zlib-${zlib_win_VERSION}"

	printItem "configuring"
	printOK

	zlib_solution=`pwd`"/zlib-from-LOANI/zlib-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress zlib ${zlib_solution}
	printOK

	printItem "installing"

	# Take care of the exported header files (API for zlib):
	zlib_include_install_dir="${zlib_install_dir}/include"
	${MKDIR} -p ${zlib_include_install_dir}
	${CP} -f *.h ${zlib_include_install_dir}

	printOK

	printEndList

	LOG_STATUS "zlib successfully installed."

	cd "$initial_dir"

}


cleanzlib_win()
{
	LOG_STATUS "Cleaning SDL build tree..."
	${RM} -rf "${zlib_source_dir}"
}




################################################################################
################################################################################
# libpng (PNG library)
################################################################################
################################################################################


#TRACE "[loani-requiredTools] PNG"

getlibpng()
{
	LOG_STATUS "Getting PNG library..."
	launchFileRetrieval libpng
	return $?
}


preparelibpng()
{

	LOG_STATUS "Preparing PNG library..."

	if findTool bunzip2 ; then
		BUNZIP2=$returnedString
	else
		ERROR "No bunzip2 tool found, whereas some files have to be bunzip2-ed."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "libPNG     "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${libpng_ARCHIVE} ${libpng_ARCHIVE}.save && ${BUNZIP2} -f ${libpng_ARCHIVE} && ${TAR} -xvf "libpng-${libpng_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${libpng_ARCHIVE}."
		LOG_STATUS "Restoring ${libpng_ARCHIVE}."
		${MV} -f ${libpng_ARCHIVE}.save ${libpng_ARCHIVE}
		exit 10
	fi

	${MV} -f ${libpng_ARCHIVE}.save ${libpng_ARCHIVE}
	${RM} -f "libpng-${libpng_VERSION}.tar"

	printOK

}


generatelibpng()
{

	LOG_STATUS "Generating libpng..."

	if [ -d "libpng" ] ; then
		${RM} -rf "libpng"
	fi

	# Not all platforms support symbolic links:
	${MV} -f "libpng-${libpng_VERSION}" libpng
	cd libpng

	printItem "configuring"

	if [ -n "$prefix" ] ; then
		libpng_PREFIX=${prefix}/PNG-${libpng_VERSION}
	fi

	if [ $is_linux -eq 0 ] ; then
		${CP} -f scripts/makefile.linux makefile
	fi

	if [ $is_solaris -eq 0 ] ; then
		WARNING "generatelibpng is selecting makefile.solaris, but there is also makefile.so9 and makefile.sunos"
		${CP} -f scripts/makefile.solaris makefile
	fi

	if [ $is_aix -eq 0 ] ; then
		${CP} -f scripts/makefile.aix makefile
	fi

	if [ $is_freebsd -eq 0 ] ; then
		${CP} -f scripts/makefile.freebsd makefile
	fi

	if [ $is_netbsd -eq 0 ] ; then
		${CP} -f scripts/makefile.netbsd makefile
	fi

	if [ $is_openbsd -eq 0 ] ; then
		${CP} -f scripts/makefile.openbsd makefile
	fi

	if [ $is_pure_cygwin -eq 0 ] ; then
		${CP} -f scripts/makefile.cygwin makefile
	fi

	if [ $is_cygwin_mingw -eq 0 ] ; then
		${CP} -f scripts/makefile.cygwin makefile
	fi

	if [ $use_msys -eq 0 ] ; then
		WARNING "Assuming cygwin settings for libpng for this Windows MSYS/Mingw platform."
		${CP} -f scripts/makefile.cygwin makefile
	fi

	if [ $is_macosx -eq 0 ] ; then
		${CP} -f scripts/makefile.macosx makefile
	fi

	if [ ! -f "makefile" ] ; then
		ERROR "Your platform does not seem to be supported by LOANI. If you want to continue nevertheless, you must select in "`pwd`"/scripts the makefile.XXX where XXX matches your platform, and copy and rename it in "`pwd`": cd "`pwd`"; cp -f scripts/makefile.XXX makefile"
		if ! askDefaultYes "Do you want to continue ? [press y only when the relevant makefile has been copied, n or CTRL-C to stop " ; then
			ERROR "Installation cancelled."
			exit 0
		fi

		if [ ! -f "makefile" ] ; then
			ERROR "No makefile found in "`pwd`", stopping installation."
			exit 11
		fi
	fi

	# No configure for libpng

	printOK

	printItem "building"

	PNG_NUMBER=12

	{
		if [ $is_mingw -eq 0 ] ; then
			# zLib Library version could be used.
			setBuildEnv ${MAKE} ${BUILD_LOCATIONS} prefix=${libpng_PREFIX} MINGW_CCFLAGS=${MINGW_CFLAGS} MINGW_LDFLAGS=${MINGW_LFLAGS} SHAREDLIB=libpng${PNG_NUMBER}.dll ZLIBINC=../zlib ZLIBLIB=../zlib
		else
			# LDFLAGS="-lgcc_s" not added.
			setBuildEnv ${MAKE} ${BUILD_LOCATIONS} prefix=${libpng_PREFIX}
		fi

	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build libpng."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{

		LIBFLAG="-L${libpng_PREFIX}/lib ${LIBFLAG}"

		echo "# libpng section." >> ${OSDL_ENV_FILE}

		echo "libpng_PREFIX=${libpng_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export libpng_PREFIX" >> ${OSDL_ENV_FILE}

		echo "LD_LIBRARY_PATH=\$libpng_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		# In order SDL_image configure does not fail:
		LD_LIBRARY_PATH=${libpng_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		if [ $is_windows -eq 0 ] ; then

			PATH=${libpng_PREFIX}/lib:${PATH}
			export PATH

			echo "PATH=\$libpng_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}

		${MKDIR} -p ${libpng_PREFIX}/include
		${MKDIR} -p ${libpng_PREFIX}/lib
		${MKDIR} -p ${libpng_PREFIX}/man
		${MKDIR} -p ${libpng_PREFIX}/bin

		if [ $is_mingw -eq 0 ] ; then

			PNG_SHARED_LIB=libpng${PNG_NUMBER}.dll
			setBuildEnv ${MAKE} install prefix=${libpng_PREFIX} SHAREDLIB=${PNG_SHARED_LIB}
			${CP} -f ${libpng_PREFIX}/bin/${PNG_SHARED_LIB} ${libpng_PREFIX}/lib/libpng.dll
			${MV} -f ${libpng_PREFIX}/include/libpng${PNG_NUMBER}/* ${libpng_PREFIX}/include

		else

			setBuildEnv ${MAKE} install prefix=${libpng_PREFIX}

			# Apparently, some libraries (notably Agar examples) expect to find
			# a PNG library like libpng14.so.14 whereas the only shared
			# libraries installed are libpng14.so, libpng14.so.14.4 and
			# libpng.so. Thus we create this "missing" link:

			full_png_name=`/bin/ls ${libpng_PREFIX}/lib/libpng*.so.*.*`
			# Ex: full_png_name=libpng14.so.14.4

			# Remove the rightmost dot and characters after it:
			target_png_link=`echo ${full_png_name} | ${AWK} -F"." '{print $1,$2,$3}' OFS="."`
			# Ex: target_png_link=libpng14.so.14 (not using sed as would be
			# greedy)
			${LN} -sf ${full_png_name} ${target_png_link}

		fi

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install libpng."
		exit 13
	fi

	printOK

	printEndList

	LOG_STATUS "libpng successfully installed."

	cd "$initial_dir"

}


cleanlibpng()
{
	LOG_STATUS "Cleaning PNG library build tree..."
	${RM} -rf libpng
}


################################################################################
# libpng build thanks to Visual Express.
#
# Its libpng and libpng prerequisites are managed seperatly, whereas libjpeg is
# managed here (this second order library is not built, it is just downloaded
# with the libpng package).
################################################################################

#TRACE "[loani-requiredTools] libpng for Visual Express targets"

getlibpng_win()
{
	LOG_STATUS "Getting libpng for windows..."
	launchFileRetrieval libpng_win
	return $?
}


preparelibpng_win()
{

	LOG_STATUS "Preparing libpng for windows.."

	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "libpng  "

	printItem "extracting"

	cd $repository

	libpng_source_dir="libpng-${libpng_win_VERSION}"
	${RM} -rf ${libpng_source_dir} 2>/dev/null

	{
		${UNZIP} -o ${libpng_win_ARCHIVE} && ${MV} -f lpng${libpng_win_archive_VERSION} ${libpng_source_dir}
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${libpng_win_ARCHIVE}."
		exit 10
	fi

	libpng_install_dir="${prefix}/libpng-${libpng_win_VERSION}"
	${MKDIR} -p ${libpng_install_dir}

	cd $repository

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/libpng-from-LOANI" "libpng-${libpng_win_VERSION}"

	if [ $? != 0 ] ; then
		ERROR "Unable to copy libpng solution in build tree."
		exit 11
	fi

	printOK

}


generatelibpng_win()
{

	LOG_STATUS "Generating libpng for windows..."

	cd "libpng-${libpng_win_VERSION}"

	printItem "configuring"
	printOK

	libpng_solution=`pwd`"/libpng-from-LOANI/libpng-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress libpng ${libpng_solution}
	printOK

	printItem "installing"

	# Take care of the exported header files (API for libpng):
	libpng_include_install_dir="${libpng_install_dir}/include"
	${MKDIR} -p ${libpng_include_install_dir}
	${CP} -f *.h  ${libpng_include_install_dir}

	printOK

	printEndList

	LOG_STATUS "libpng successfully installed."

	cd "$initial_dir"

}


cleanlibpng_win()
{
	LOG_STATUS "Cleaning SDL build tree..."
	${RM} -rf "libpng-${libpng_win_VERSION}"
}



################################################################################
################################################################################
# libtiff (TIFF library)
# Currently disabled.
################################################################################
################################################################################


#TRACE "[loani-requiredTools] TIFF"

getlibtiff()
{
	LOG_STATUS "Getting TIFF library..."
	launchFileRetrieval libtiff
	return $?
}


preparelibtiff()
{

	LOG_STATUS "Preparing TIFF library..."

	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunzipped."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "libTIFF    "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${libtiff_ARCHIVE} ${libtiff_ARCHIVE}.save && ${GUNZIP} -f ${libtiff_ARCHIVE} && ${TAR} -xvf "tiff-v${libtiff_VERSION}-tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${libtiff_ARCHIVE}."
		LOG_STATUS "Restoring ${libtiff_ARCHIVE}."
		${MV} -f ${libtiff_ARCHIVE}.save ${libtiff_ARCHIVE}
		exit 10
	fi

	${MV} -f ${libtiff_ARCHIVE}.save ${libtiff_ARCHIVE}
	${RM} -f "tiff-v${libtiff_VERSION}-tar"

	printOK

}


generatelibtiff()
{

	LOG_STATUS "Generating libtiff..."

	cd "tiff-v${libtiff_VERSION}"

	printItem "configuring"

	############################################################################
	# Not gone any further, since tiff support is not required and
	# the makefile generated from ./configure --noninteractive
	# is totally unusable.
	ERROR "TIFF support not implemented yet."
	exit 1
	############################################################################

	printOK

	printItem "building"

	if [ -n "${CC}" ] ; then
	{
			# LDFLAGS="-lgcc_s" not added.
			${MAKE} CC=${CC}
	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
			${MAKE}
	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build libtiff."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{

		libtiff_PREFIX=${prefix}/TIFF-${libtiff_VERSION}

		${MKDIR} -p ${libtiff_PREFIX}/include
		${MKDIR} -p ${libtiff_PREFIX}/lib
		${MKDIR} -p ${libtiff_PREFIX}/man
		${MKDIR} -p ${libtiff_PREFIX}/bin
		${MAKE} install prefix=${libtiff_PREFIX}

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
			${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install libtiff."
		exit 13
	fi

	printOK

	printEndList

	LOG_STATUS "libtiff successfully installed."

	cd "$initial_dir"

}


cleanlibtiff()
{
	LOG_STATUS "Cleaning TIFF library build tree..."
	${RM} -rf "tiff-v${libtiff_VERSION}"
}



################################################################################
################################################################################
# SDL_image
################################################################################
################################################################################


#TRACE "[loani-requiredTools] SDL_image"


getSDL_image()
{
	LOG_STATUS "Getting SDL_image..."
	launchFileRetrieval SDL_image
	return $?
}


prepareSDL_image()
{

	LOG_STATUS "Preparing SDL_image..."
	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunzipped."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "SDL_image  "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${SDL_image_ARCHIVE} ${SDL_image_ARCHIVE}.save && ${GUNZIP} -f ${SDL_image_ARCHIVE} && ${TAR} -xvf "SDL_image-${SDL_image_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${SDL_image_ARCHIVE}."
		LOG_STATUS "Restoring ${SDL_image_ARCHIVE}."
		${MV} -f ${SDL_image_ARCHIVE}.save ${SDL_image_ARCHIVE}
		exit 10
	fi

	${MV} -f ${SDL_image_ARCHIVE}.save ${SDL_image_ARCHIVE}
	${RM} -f "SDL_image-${SDL_image_VERSION}.tar"

	printOK

}


generateSDL_image()
{

	LOG_STATUS "Generating SDL_image..."

	if [ $is_windows -eq 0 ] ; then

		DEBUG "Correcting sdl-config for SDL_image."

		sdl_config=${SDL_PREFIX}/bin/sdl-config

		DEBUG "Correcting ${sdl_config}"
		prefix_one=`cygpath -w ${SDL_PREFIX} | ${SED} 's|\\\|/|g'`
		prefix_two=`cygpath -w ${SDL_PREFIX} | ${SED} 's|\\\|/|g'`

		${CAT} ${sdl_config} | ${SED} "s|^prefix=.*$|prefix=$prefix_one|1" > sdl-config.tmp && ${CAT} sdl-config.tmp | ${SED} "s|^exec_prefix=.*$|exec_prefix=$prefix_two|1" > sdl-config.tmp2 && ${RM} -f ${sdl_config} sdl-config.tmp && ${MV} -f sdl-config.tmp2 ${sdl_config}

		if [ $? -ne 0 ] ; then
			ERROR "Unable to correct sdl-config so that SDL_image can use it."
			exit 10
		fi
	fi


	cd "SDL_image-${SDL_image_VERSION}"

	printItem "configuring"


	if [ -f "config.cache" ] ; then
		${RM} -f config.cache
	fi

	if [ -n "$prefix" ] ; then
	{

		SDL_image_PREFIX=${prefix}/SDL_image-${SDL_image_VERSION}

		# For debug purpose (should be set from other targets):

		#LIBFLAG="-L${SDL_PREFIX}/lib"
		#LIBFLAG="-L${libjpeg_PREFIX}/lib ${LIBFLAG}"
		#LIBFLAG="-L${zlib_PREFIX}/lib ${LIBFLAG}"
		#LIBFLAG="-L${libpng_PREFIX}/lib ${LIBFLAG}"

		if [ $is_windows -eq 0 ] ; then

			LIBFLAG="-L`cygpath -w ${SDL_PREFIX}/lib`"
			LIBFLAG="-L`cygpath -w ${libjpeg_PREFIX}/lib ${LIBFLAG}`"
			LIBFLAG="-L`cygpath -w ${zlib_PREFIX}/lib ${LIBFLAG}`"
			LIBFLAG="-L`cygpath -w ${libpng_PREFIX}/lib ${LIBFLAG}`"

			LIBFLAG=`echo $LIBFLAG | ${SED} 's|\\\|/|g'`

			# DLL are searched from PATH on Windows.

			PATH=${SDL_PREFIX}/lib:${PATH}
			PATH=${libjpeg_PREFIX}/lib:${PATH}
			PATH=${zlib_PREFIX}/lib:${PATH}
			PATH=${libpng_PREFIX}/lib:${PATH}

			export PATH

		fi

		PATH=${SDL_PREFIX}/bin:${PATH}
		export PATH

		LD_LIBRARY_PATH=${SDL_PREFIX}/lib:${LD_LIBRARY_PATH}
		LD_LIBRARY_PATH=${libjpeg_PREFIX}/lib:${LD_LIBRARY_PATH}
		LD_LIBRARY_PATH=${zlib_PREFIX}/lib:${LD_LIBRARY_PATH}
		LD_LIBRARY_PATH=${libpng_PREFIX}/lib:${LD_LIBRARY_PATH}

		export LD_LIBRARY_PATH


		LOG_STATUS "PATH for SDL_image configure is <${PATH}>."
		LOG_STATUS "LD_LIBRARY_PATH for SDL_image configure is <${LD_LIBRARY_PATH}>."

		if [ $is_mingw -eq 0 ] ; then

			LOG_STATUS "Using SDL prefix $SDL_PREFIX for SDL_image."

			OLD_LDFLAGS=${LDFLAGS}

			LDFLAGS=${LIBFLAG}
			export LDFLAGS

			setBuildEnv ./configure --with-sdl-prefix=${SDL_PREFIX} --disable-tif "LDFLAGS=${LDFLAGS}"

			LDFLAGS=${OLD_LDFLAGS}
			export LDFLAGS

		else

			# --disable-sdltest added since configure tries to compile a test
			# without letting the system libraries locations to be
			# redefined. Therefore a wrong libstdc++.so could be chosen, leading
			# to errors such as: "undefined reference to
			# `_Unwind_Resume_or_Rethrow@GCC_3.3'"

			setBuildEnv ./configure --with-sdl-prefix=${SDL_PREFIX} --disable-tif --disable-sdltest LDFLAGS="${LIBFLAG} -lz" CPPFLAGS="-I${libjpeg_PREFIX}/include -I${libpng_PREFIX}/include -I${zlib_PREFIX}/include"

		fi

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ./configure
	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure SDL_image."
		exit 11
	fi

	printOK

	printItem "building"

	if [ -n "$prefix" ] ; then
	{

		#setBuildEnv ${MAKE} "CFLAGS=-O2 -I${SDL_PREFIX}/include/SDL -D_REENTRANT -DLOAD_BMP -DLOAD_GIF -DLOAD_LBM -DLOAD_PCX -DLOAD_PNM -DLOAD_TGA -DLOAD_XPM -DLOAD_JPG -DLOAD_PNG" "LDFLAGS=${LIBFLAG} -ljpeg -lpng -lz" "IMG_LIBS=-ljpeg -lpng -lz"

		if [ $is_windows -eq 0 ] ; then

			JPEG_INC=`cygpath -w ${libjpeg_PREFIX}/include | ${SED} 's|\\\|/|g'`
			PNG_INC=`cygpath -w ${libpng_PREFIX}/include | ${SED} 's|\\\|/|g'`
			ZLIB_INC=`cygpath -w ${zlib_PREFIX}/include | ${SED} 's|\\\|/|g'`

		else

			JPEG_INC="${libjpeg_PREFIX}/include"
			ZLIB_INC="${zlib_PREFIX}/include"
			PNG_INC="${libpng_PREFIX}/include"

		fi

		setBuildEnv ${MAKE} LDFLAGS="-lgcc_s ${LIBFLAG}" CPPFLAGS="-I${JPEG_INC} -I${ZLIB_INC} -I${PNG_INC}"

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE} LDFLAGS="-lgcc_s"

	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build SDL_image."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{
		echo "# SDL_image section." >> ${OSDL_ENV_FILE}

		echo "SDL_image_PREFIX=${SDL_image_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export SDL_image_PREFIX" >> ${OSDL_ENV_FILE}
		echo "LD_LIBRARY_PATH=\$SDL_image_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		LD_LIBRARY_PATH=${SDL_image_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		if [ $is_windows -eq 0 ] ; then

			PATH=${SDL_image_PREFIX}/lib:${PATH}
			export PATH

			echo "PATH=\$SDL_image_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}

		${MKDIR} -p ${SDL_image_PREFIX}

		setBuildEnv ${MAKE} install prefix=${SDL_image_PREFIX}

		if [ $? != 0 ] ; then
			echo
			ERROR "Unable to install SDL_image."
			exit 13
		fi

		# Rename 'libSDL_image.la', to prevent libtool from detecting it when
		# linking OSDL and issuing very annoying messages twice, such as:
		# "libtool: link: warning: library `[...]/libSDL_image.la' was moved."

		${MV} -f ${SDL_image_PREFIX}/lib/libSDL_image.la ${SDL_image_PREFIX}/lib/libSDL_image.la-hidden-by-LOANI

		if [ $? != 0 ] ; then
			echo
			ERROR "Unable to post-install SDL_image (correction for libtool)."
			exit 13
		fi


	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install SDL_image."
		exit 13
	fi


	printOK

	printEndList

	LOG_STATUS "SDL_image successfully installed."

	cd "$initial_dir"

}


cleanSDL_image()
{
	LOG_STATUS "Cleaning SDL_image library build tree..."
	${RM} -rf "SDL_image-${SDL_image_VERSION}"
}



################################################################################
# SDL_image build thanks to Visual Express.
#
# Its libpng and zlib prerequisites are managed seperatly, whereas libjpeg is
# managed here (this second order library is not built, it is just downloaded
# with the SDL_image package).
################################################################################

#TRACE "[loani-requiredTools] SDL_image for Visual Express targets"

getSDL_image_win()
{
	LOG_STATUS "Getting SDL_image for windows..."
	launchFileRetrieval SDL_image_win
	return $?
}


prepareSDL_image_win()
{

	LOG_STATUS "Preparing SDL_image for windows.."

	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "SDL_image  "

	printItem "extracting"

	cd $repository

	{
		${UNZIP} -o ${SDL_image_win_ARCHIVE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${SDL_image_win_ARCHIVE}."
		exit 10
	fi

	cd "SDL_image-${SDL_image_win_VERSION}"

	sdl_image_install_dir="${prefix}/SDL_image-${SDL_image_win_VERSION}"

	${MKDIR} -p ${sdl_image_install_dir}

	cd $repository

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/SDL_image-from-LOANI" "SDL_image-${SDL_image_win_VERSION}"

	if [ $? != 0 ] ; then
		ERROR "Unable to copy SDL_image solution in build tree."
		exit 11
	fi

	printOK

}


generateSDL_image_win()
{

	LOG_STATUS "Generating SDL_image for windows..."

	cd "SDL_image-${SDL_image_win_VERSION}"

	printItem "configuring"
	printOK

	sdl_image_solution=`pwd`"/SDL_image-from-LOANI/SDL_image-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress SDL_image ${sdl_image_solution}
	printOK

	printItem "installing"

	# Take care of the exported header files (API for SDL_image and libjpeg):
	sdl_image_include_install_dir="${sdl_image_install_dir}/include"
	${MKDIR} -p ${sdl_image_include_install_dir}
	${CP} -f *.h ${sdl_image_include_install_dir}

	printOK

	printEndList

	LOG_STATUS "SDL_image successfully installed."

	cd "$initial_dir"

}


cleanSDL_image_win()
{
	LOG_STATUS "Cleaning SDL build tree..."
	${RM} -rf "SDL_image-${SDL_image_win_VERSION}"
}



################################################################################
# SDL_image_*_precompiled targets.
################################################################################

# Windows binaries.

getSDL_image_win_precompiled()
{
		LOG_STATUS "Getting prebuilt JPEG library for windows (SDL_image package)..."
		launchFileRetrieval SDL_image_win_precompiled
		return $?
}


prepareSDL_image_win_precompiled()
{

	LOG_STATUS "Preparing prebuilt JPEG library..."
	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "Precompiled"

	printItem "extracting"

	cd $repository

	SDL_image_prebuilt_dir="SDL_image-${SDL_image_VERSION}-prebuilt"

	# Prevent archive from disappearing because of unzip.
	# Create SDL_image-x.y.z-prebuilt directory with precompiled DLL in it.
	{
		${CP} -f ${SDL_image_win_precompiled_ARCHIVE} ${SDL_image_win_precompiled_ARCHIVE}.save && ${UNZIP} -o ${SDL_image_win_precompiled_ARCHIVE} && ${MV} -f SDL_image-${SDL_image_VERSION} ${SDL_image_prebuilt_dir}
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${SDL_image_win_precompiled_ARCHIVE}."
		LOG_STATUS "Restoring ${SDL_image_win_precompiled_ARCHIVE}."
		${MV} -f ${SDL_image_win_precompiled_ARCHIVE}.save ${SDL_image_win_precompiled_ARCHIVE}
		exit 10
	fi

	${MV} -f ${SDL_image_win_precompiled_ARCHIVE}.save ${SDL_image_win_precompiled_ARCHIVE}

	printOK

}


generateSDL_image_win_precompiled()
{

	LOG_STATUS "Generating SDL_image_win_precompiled ..."

	cd "${SDL_image_prebuilt_dir}"

	printItem "configuring"

	# Nothing to do!

	printOK

	printItem "building"

	# Nothing to do!

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{

		# Let's suppose the precompiled version has for version libjpeg_VERSION.

		# The libjpeg target should already have:
		#       - created ${libjpeg_PREFIX}/lib
		#       - defined:
		# LIBFLAG="-L${libjpeg_PREFIX}/lib ${LIBFLAG}"
		#       - added the libjpeg section." in ${OSDL_ENV_FILE}
		#       - updated LD_LIBRARY_PATH *and* PATH with ${libjpeg_PREFIX}/lib

		${CP} -f lib/jpeg.dll ${libjpeg_PREFIX}/lib

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		if [ $is_windows -eq 1 ] ; then
			setBuildEnv ${MAKE} install
		else
			DEBUG "Using ${windows_dll_location} as target Windows DLL location."
			${CP} -f lib/jpeg.dll ${windows_dll_location}
		fi

	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install precompiled binaries."
		exit 13
	fi

	printOK

	printEndList

	LOG_STATUS "Precompiled binaries successfully installed."

	cd "$initial_dir"

}


cleanSDL_image_win_precompiled()
{
	LOG_STATUS "Cleaning SDL_image precompiled for windows build tree..."
	${RM} -rf "${SDL_image_prebuilt_dir}"
}




################################################################################
################################################################################
################################################################################
# libogg
################################################################################
################################################################################


#TRACE "[loani-requiredTools] libogg"


getlibogg()
{
	LOG_STATUS "Getting libogg..."
	launchFileRetrieval libogg
	return $?
}


preparelibogg()
{

	LOG_STATUS "Preparing libogg..."

	if findTool bunzip2 ; then
		BUNZIP2=$returnedString
	else
		ERROR "No bunzip2 tool found, whereas some files have to be bunzip2-ed."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "libogg     "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${libogg_ARCHIVE} ${libogg_ARCHIVE}.save && ${XZ} --decompress -f ${libogg_ARCHIVE} && ${TAR} -xvf "libogg-${libogg_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${libogg_ARCHIVE}."
		LOG_STATUS "Restoring ${libogg_ARCHIVE}."
		${MV} -f ${libogg_ARCHIVE}.save ${libogg_ARCHIVE}
		exit 10
	fi

	${MV} -f ${libogg_ARCHIVE}.save ${libogg_ARCHIVE}
	${RM} -f "libogg-${libogg_VERSION}.tar"

	printOK

}


generatelibogg()
{

	LOG_STATUS "Generating libogg..."


	cd "libogg-${libogg_VERSION}"

	printItem "configuring"


	if [ -f "config.cache" ] ; then
		${RM} -f config.cache
	fi

	if [ -n "$prefix" ] ; then
	{

		libogg_PREFIX=${prefix}/libogg-${libogg_VERSION}

		setBuildEnv ./configure --prefix=${libogg_PREFIX} --exec-prefix=${libogg_PREFIX}

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ./configure
	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure libogg."
		exit 11
	fi

	printOK

	printItem "building"

	if [ -n "$prefix" ] ; then
	{

		setBuildEnv ${MAKE}

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE}

	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build libogg."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{

		LIBFLAG="-L${libogg_PREFIX}/lib ${LIBFLAG}"
		INCLUDE
		echo "# libogg section." >> ${OSDL_ENV_FILE}

		echo "libogg_PREFIX=${libogg_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export libogg_PREFIX" >> ${OSDL_ENV_FILE}
		echo "LD_LIBRARY_PATH=\$libogg_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		LD_LIBRARY_PATH=${libogg_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		if [ $is_windows -eq 0 ] ; then

			PATH=${libogg_PREFIX}/lib:${PATH}
			export PATH

			echo "PATH=\$libogg_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}

		${MKDIR} -p ${libogg_PREFIX}

		setBuildEnv ${MAKE} install prefix=${libogg_PREFIX}

		if [ $? != 0 ] ; then
			echo
			ERROR "Unable to install libogg."
			exit 13
		fi

		# Rename 'libogg.la', to prevent libtool from detecting it when
		# linking OSDL and issuing very annoying messages twice, such as:
		# "libtool: link: warning: library `[...]/libogg.la' was moved."

		# Disabled since it would prevent SDL_mixer build:
		#${MV} -f ${libogg_PREFIX}/lib/libogg.la ${libogg_PREFIX}/lib/libogg.la-hidden-by-LOANI
		#
		#if [ $? != 0 ] ; then
		#	echo
		#	ERROR "Unable to post-install libogg (correction for libtool)."
		#	exit 13
		#fi


	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install libogg."
		exit 13
	fi


	printOK

	printEndList

	LOG_STATUS "libogg successfully installed."

	cd "$initial_dir"

}


cleanlibogg()
{
	LOG_STATUS "Cleaning libogg library build tree..."
	${RM} -rf "libogg-${libogg_VERSION}"
}


################################################################################
# libogg build thanks to Visual Express.
################################################################################

#TRACE "[loani-requiredTools] SDL_image for Visual Express targets"

getlibogg_win()
{
	LOG_STATUS "Getting libogg for windows..."
	launchFileRetrieval libogg_win
	return $?
}


preparelibogg_win()
{

	LOG_STATUS "Preparing libogg for windows.."

	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "libogg     "

	printItem "extracting"

	cd $repository

	{
		${UNZIP} -o ${libogg_win_ARCHIVE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${libogg_win_ARCHIVE}."
		exit 10
	fi

	cd "libogg-${libogg_win_VERSION}"

	libogg_install_dir="${prefix}/libogg-${libogg_win_VERSION}"

	${MKDIR} -p ${libogg_install_dir}

	cd $repository

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/libogg-from-LOANI" "libogg-${libogg_win_VERSION}"

	if [ $? != 0 ] ; then
		ERROR "Unable to copy libogg solution in build tree."
		exit 11
	fi

	printOK

}


generatelibogg_win()
{

	LOG_STATUS "Generating libogg for windows..."

	cd "libogg-${libogg_win_VERSION}"

	printItem "configuring"
	printOK

	libogg_solution=`pwd`"/libogg-from-LOANI/libogg-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress libogg ${libogg_solution}
	printOK

	printItem "installing"

	# Take care of the exported header files (API for libogg and libjpeg):
	libogg_include_install_dir="${libogg_install_dir}/include/ogg"
	${MKDIR} -p ${libogg_include_install_dir}

	# vorbis includes are in the form "ogg/x.h":
	${CP} -f include/ogg/*.h ${libogg_include_install_dir}

	printOK

	printEndList

	LOG_STATUS "libogg successfully installed."

	cd "$initial_dir"

}


cleanlibogg_win()
{
	LOG_STATUS "Cleaning SDL build tree..."
	${RM} -rf "libogg-${libogg_win_VERSION}"
}




################################################################################
################################################################################
# libvorbis
################################################################################
################################################################################

#TRACE "[loani-requiredTools] libvorbis"


getlibvorbis()
{
	LOG_STATUS "Getting libvorbis..."
	launchFileRetrieval libvorbis
	return $?
}


preparelibvorbis()
{

	LOG_STATUS "Preparing libvorbis..."

	if findTool bunzip2 ; then
		BUNZIP2=$returnedString
	else
		ERROR "No bunzip2 tool found, whereas some files have to be bunzip2-ed."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "libvorbis  "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${libvorbis_ARCHIVE} ${libvorbis_ARCHIVE}.save && ${XZ} --decompress ${libvorbis_ARCHIVE} && ${TAR} -xvf "libvorbis-${libvorbis_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${libvorbis_ARCHIVE}."
		LOG_STATUS "Restoring ${libvorbis_ARCHIVE}."
		${MV} -f ${libvorbis_ARCHIVE}.save ${libvorbis_ARCHIVE}
		exit 10
	fi

	${MV} -f ${libvorbis_ARCHIVE}.save ${libvorbis_ARCHIVE}
	${RM} -f "libvorbis-${libvorbis_VERSION}.tar"

	printOK

}


generatelibvorbis()
{

	LOG_STATUS "Generating libvorbis..."


	cd "libvorbis-${libvorbis_VERSION}"

	printItem "configuring"


	if [ -f "config.cache" ] ; then
		${RM} -f config.cache
	fi

	if [ -n "$prefix" ] ; then
	{

		libvorbis_PREFIX=${prefix}/libvorbis-${libvorbis_VERSION}

		setBuildEnv ./configure --prefix=${libvorbis_PREFIX} --exec-prefix=${libvorbis_PREFIX} --with-ogg=${libogg_PREFIX}

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ./configure
	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure libvorbis."
		exit 11
	fi

	printOK

	printItem "building"

	if [ -n "$prefix" ] ; then
	{

		setBuildEnv ${MAKE}

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE}

	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build libvorbis."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{
		echo "# libvorbis section." >> ${OSDL_ENV_FILE}

		echo "libvorbis_PREFIX=${libvorbis_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export libvorbis_PREFIX" >> ${OSDL_ENV_FILE}
		echo "LD_LIBRARY_PATH=\$libvorbis_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		LD_LIBRARY_PATH=${libvorbis_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		if [ $is_windows -eq 0 ] ; then

			PATH=${libvorbis_PREFIX}/lib:${PATH}
			export PATH

			echo "PATH=\$libvorbis_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}

		${MKDIR} -p ${libvorbis_PREFIX}

		setBuildEnv ${MAKE} install prefix=${libvorbis_PREFIX}

		if [ $? != 0 ] ; then
			echo
			ERROR "Unable to install libvorbis."
			exit 13
		fi

		# Rename 'libvorbis.la', to prevent libtool from detecting it when
		# linking OSDL and issuing very annoying messages twice, such as:
		# "libtool: link: warning: library `[...]/libvorbis.la' was moved."

		# Disabled since would prevent SDL_mixer build: ${MV} -f
		#${libvorbis_PREFIX}/lib/libvorbis.la
		#${libvorbis_PREFIX}/lib/libvorbis.la-hidden-by-LOANI
		#
		#if [ $? != 0 ] ; then
		#	echo
		#	ERROR "Unable to post-install libvorbis (correction for libtool)."
		#	exit 13
		#fi


	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install libvorbis."
		exit 13
	fi


	printOK

	printEndList

	LOG_STATUS "libvorbis successfully installed."

	cd "$initial_dir"

}


cleanlibvorbis()
{
	LOG_STATUS "Cleaning libvorbis library build tree..."
	${RM} -rf "libvorbis-${libvorbis_VERSION}"
}



################################################################################
# libvorbis build thanks to Visual Express.
################################################################################


#TRACE "[loani-requiredTools] libvorbis for Visual Express targets"

getlibvorbis_win()
{
	LOG_STATUS "Getting libvorbis for windows..."
	launchFileRetrieval libvorbis_win
	return $?
}


preparelibvorbis_win()
{

	LOG_STATUS "Preparing libvorbis for windows.."

	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "libvorbis  "

	printItem "extracting"

	cd $repository

	{
		${UNZIP} -o ${libvorbis_win_ARCHIVE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${libvorbis_win_ARCHIVE}."
		exit 10
	fi

	cd "libvorbis-${libvorbis_win_VERSION}"

	libvorbis_install_dir="${prefix}/libvorbis-${libvorbis_win_VERSION}"

	${MKDIR} -p ${libvorbis_install_dir}

	cd $repository

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/libvorbis-from-LOANI" "libvorbis-${libvorbis_win_VERSION}"

	if [ $? != 0 ] ; then
		ERROR "Unable to copy libvorbis solution in build tree."
		exit 11
	fi

	printOK

}


generatelibvorbis_win()
{

	LOG_STATUS "Generating libvorbis for windows..."

	cd "libvorbis-${libvorbis_win_VERSION}"

	printItem "configuring"
	printOK

	libvorbis_solution=`pwd`"/libvorbis-from-LOANI/libvorbis-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress libvorbis ${libvorbis_solution}
	printOK

	printItem "installing"

	# Take care of the exported header files (API for libvorbis and libjpeg):
	libvorbis_include_install_dir="${libvorbis_install_dir}/include/vorbis"
	${MKDIR} -p ${libvorbis_include_install_dir}
	${CP} -f include/vorbis/*.h ${libvorbis_include_install_dir}

	printOK

	printEndList

	LOG_STATUS "libvorbis successfully installed."

	cd "$initial_dir"

}


cleanlibvorbis_win()
{
	LOG_STATUS "Cleaning SDL build tree..."
	${RM} -rf "libvorbis-${libvorbis_win_VERSION}"
}




################################################################################
################################################################################
# SDL_mixer
################################################################################
################################################################################


################################################################################
# SDL_mixer for non-Windows platforms
################################################################################

#TRACE "[loani-requiredTools] SDL_mixer"


getSDL_mixer()
{
	LOG_STATUS "Getting SDL_mixer..."
	launchFileRetrieval SDL_mixer
	return $?
}


prepareSDL_mixer()
{

	LOG_STATUS "Preparing SDL_mixer..."
	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunzipped."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "SDL_mixer  "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${SDL_mixer_ARCHIVE} ${SDL_mixer_ARCHIVE}.save && ${GUNZIP} -f ${SDL_mixer_ARCHIVE} && ${TAR} -xvf "SDL_mixer-${SDL_mixer_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${SDL_mixer_ARCHIVE}."
		LOG_STATUS "Restoring ${SDL_mixer_ARCHIVE}."
		${MV} -f ${SDL_mixer_ARCHIVE}.save ${SDL_mixer_ARCHIVE}
		exit 10
	fi

	${MV} -f ${SDL_mixer_ARCHIVE}.save ${SDL_mixer_ARCHIVE}
	${RM} -f "SDL_mixer-${SDL_mixer_VERSION}.tar"

	printOK

}


generateSDL_mixer()
{

	LOG_STATUS "Generating SDL_mixer..."


	cd "SDL_mixer-${SDL_mixer_VERSION}"

	printItem "configuring"


	if [ -f "config.cache" ] ; then
		${RM} -f config.cache
	fi

	if [ -n "$prefix" ] ; then
	{

		SDL_mixer_PREFIX=${prefix}/SDL_mixer-${SDL_mixer_VERSION}

		#LDFLAGS="-L${libogg_PREFIX}/lib -L${libvorbis_PREFIX}/lib ${LDFLAGS}"
		#export LDFLAGS

		#CFLAGS="-I${libogg_PREFIX}/include -I${libvorbis_PREFIX}/include"
		#export CFLAGS

		# Following features are deactivated automatically (their companion
		# library is not installed by LOANI yet, and they are currently
		# considered as less useful than the ones of the core selection):
		#
		#  - MOD support (including libmikmod),
		#  - MIDI support (including native and timidity),
		#  - MP3 support (including SMPEG)
		#
		# The recommended sound encodings are:
		#  - WAVE (for short samples)
		#  - OggVorbis (for longer ones, including music).
		#

		# Saturday, October 20, 2007: there is a problem with SDL_mixer (1.2.8)
		# and libvorbis (1.2.0): at first SDL_mixer configure disables vorbis
		# support due to a lacking symbol. Fixing it (by downgrading libvorbis
		# to 1.1.2 re-enable vorbis support in SDL_mixer configure but still at
		# runtime says 'Unrecognized sound file type' with .ogg.
		#
		# Only solution seems to downgrade SDL_mixer to 1.2.7 with either of the
		# vorbis versions.


		setBuildEnv ./configure --prefix=${SDL_mixer_PREFIX} --exec-prefix=${SDL_mixer_PREFIX} -with-sdl-prefix=${SDL_PREFIX}  --disable-static --disable-music-mod --disable-music-midi --disable-music-timidity-midi --disable-music-native-midi --disable-music-native-midi-gpl --disable-music-mp3 --disable-smpegtest --enable-music-wave LDFLAGS="-L${libogg_PREFIX}/lib -L${libvorbis_PREFIX}/lib" CFLAGS="-I${libogg_PREFIX}/include  -I${libvorbis_PREFIX}/include"

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ./configure
	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure SDL_mixer."
		exit 11
	fi

	printOK

	printItem "building"

	if [ -n "$prefix" ] ; then
	{

		setBuildEnv ${MAKE}

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE}

	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build SDL_mixer."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{
		echo "# SDL_mixer section." >> ${OSDL_ENV_FILE}

		echo "SDL_mixer_PREFIX=${SDL_mixer_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export SDL_mixer_PREFIX" >> ${OSDL_ENV_FILE}
		echo "LD_LIBRARY_PATH=\$SDL_mixer_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		LD_LIBRARY_PATH=${SDL_mixer_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		if [ $is_windows -eq 0 ] ; then

			PATH=${SDL_mixer_PREFIX}/lib:${PATH}
			export PATH

			echo "PATH=\$SDL_mixer_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}

		${MKDIR} -p ${SDL_mixer_PREFIX}

		setBuildEnv ${MAKE} install prefix=${SDL_mixer_PREFIX}

		if [ $? != 0 ] ; then
			echo
			ERROR "Unable to install SDL_mixer."
			exit 13
		fi

		# Rename 'libSDL_mixer.la', to prevent libtool from detecting it when
		# linking OSDL and issuing very annoying messages twice, such as:
		# "libtool: link: warning: library `[...]/libSDL_mixer.la' was moved."

		${MV} -f ${SDL_mixer_PREFIX}/lib/libSDL_mixer.la ${SDL_mixer_PREFIX}/lib/libSDL_mixer.la-hidden-by-LOANI

		if [ $? != 0 ] ; then
			echo
			ERROR "Unable to post-install SDL_mixer (correction for libtool)."
			exit 13
		fi


	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install SDL_mixer."
		exit 13
	fi


	printOK

	printEndList

	LOG_STATUS "SDL_mixer successfully installed."

	cd "$initial_dir"

}


cleanSDL_mixer()
{
	LOG_STATUS "Cleaning SDL_mixer library build tree..."
	${RM} -rf "SDL_mixer-${SDL_mixer_VERSION}"
}



################################################################################
# SDL_mixer build thanks to Visual Express.
# Its prerequisites (Ogg/Vorbis libraries, including VorbisFile) are managed
# here as well (these second order libraries are not built, they are just
# downloaded with the SDL_mixer package).
################################################################################


getSDL_mixer_win()
{
	LOG_STATUS "Getting SDL_mixer for windows..."
	launchFileRetrieval SDL_mixer_win
	return $?
}


prepareSDL_mixer_win()
{

	LOG_STATUS "Preparing SDL_mixer for windows.."

	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "SDL_mixer  "

	printItem "extracting"

	cd $repository

	{
		${UNZIP} -o ${SDL_mixer_win_ARCHIVE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${SDL_mixer_win_ARCHIVE}."
		exit 10
	fi

	cd "SDL_mixer-${SDL_mixer_win_VERSION}"

	sdl_mixer_install_dir="${prefix}/SDL_mixer-${SDL_mixer_win_VERSION}"

	${MKDIR} -p ${sdl_mixer_install_dir}

	cd $repository

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/SDL_mixer-from-LOANI" "SDL_mixer-${SDL_mixer_win_VERSION}"

	if [ $? != 0 ] ; then
		ERROR "Unable to copy SDL_mixer solution in build tree."
		exit 11
	fi

	printOK

}


generateSDL_mixer_win()
{

	LOG_STATUS "Generating SDL_mixer for windows..."

	cd "SDL_mixer-${SDL_mixer_win_VERSION}"

	printItem "configuring"
	printOK

	sdl_mixer_solution=`pwd`"/SDL_mixer-from-LOANI/SDL_mixer-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress SDL_mixer ${sdl_mixer_solution}
	printOK

	printItem "installing"

	sdl_mixer_install_include_dir=${sdl_mixer_install_dir}/include
	${MKDIR} -p ${sdl_mixer_install_include_dir}
	${CP} -f SDL_mixer.h ${sdl_mixer_install_include_dir}

	printOK

	printEndList

	LOG_STATUS "SDL_mixer successfully installed."

	cd "$initial_dir"

}


cleanSDL_mixer_win()
{
	LOG_STATUS "Cleaning SDL_mixer build tree..."
	${RM} -rf "SDL-${SDL_mixer_win_VERSION}"
}



################################################################################
################################################################################
# Guichan (libguichan, libguichan_sdl, libguichan_opengl)
################################################################################
################################################################################


#TRACE "[loani-requiredTools] guichan"

################################################################################
# Guichan for non-Windows platforms:
################################################################################

getguichan()
{
	LOG_STATUS "Getting guichan..."
	launchFileRetrieval guichan
	return $?
}


prepareguichan()
{

	LOG_STATUS "Preparing guichan..."
	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunzipped."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "guichan    "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${guichan_ARCHIVE} ${guichan_ARCHIVE}.save && ${GUNZIP} -f ${guichan_ARCHIVE} && ${TAR} -xvf "guichan-${guichan_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${guichan_ARCHIVE}."
		LOG_STATUS "Restoring ${guichan_ARCHIVE}."
		${MV} -f ${guichan_ARCHIVE}.save ${guichan_ARCHIVE}
		exit 10
	fi

	${MV} -f ${guichan_ARCHIVE}.save ${guichan_ARCHIVE}
	${RM} -f "guichan-${guichan_VERSION}.tar"

	printOK

}


generateguichan()
{

	LOG_STATUS "Generating guichan..."


	cd "guichan-${guichan_VERSION}"

	printItem "configuring"

	if [ -n "$prefix" ] ; then
	{

		if [ $is_windows -eq 0 ] ; then
			guichan_PREFIX=`cygpath -w ${prefix}/guichan-${guichan_VERSION} | ${SED} 's|\\\|/|g'`
		else
			guichan_PREFIX="${prefix}/guichan-${guichan_VERSION}"
		fi


		${MKDIR} -p ${guichan_PREFIX}

		setBuildEnv ./configure --prefix=${guichan_PREFIX} --enable-sdl --enable-sdlimage --enable-opengl CPPFLAGS="-I${SDL_PREFIX}/include/SDL -I${SDL_image_PREFIX}/include/SDL" LDFLAGS="-L${SDL_PREFIX}/lib -L${SDL_image_PREFIX}/lib"

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ./configure
	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure guichan."
		exit 11
	fi

	printOK

	printItem "building"

	{

		 setBuildEnv ${MAKE}

	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build guichan."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{

		echo "# guichan section." >> ${OSDL_ENV_FILE}

		echo "guichan_PREFIX=${guichan_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export guichan_PREFIX" >> ${OSDL_ENV_FILE}
		echo "PATH=\$guichan_PREFIX/bin:\${PATH}" >> ${OSDL_ENV_FILE}

		echo "LD_LIBRARY_PATH=\$guichan_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		PATH=${guichan_PREFIX}/bin:${PATH}
		export PATH

		LD_LIBRARY_PATH=${guichan_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		if [ $is_windows -eq 0 ] ; then

			PATH=${guichan_PREFIX}/lib:${PATH}
			export PATH

			echo "PATH=\$guichan_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}

		LIBPATH="-L${guichan_PREFIX}/lib"

		# Do not ever imagine that to avoid bad nedit syntax highlighting
		# you could change:
		# include/*.h to "include/*.h" in next line.
		# It would fail at runtime with "include/*.h" not found...

		setBuildEnv ${MAKE} install && ${CP} -f include/*.h ${guichan_PREFIX}/include/guichan

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install guichan."
		exit 13
	fi

	if [ $is_windows -eq 0 ] ; then
			${MV} -f ${guichan_PREFIX}/bin/*.dll ${guichan_PREFIX}/lib
	fi

	printOK

	printEndList

	LOG_STATUS "guichan successfully installed."

	cd "$initial_dir"

}


cleanguichan()
{
	LOG_STATUS "Cleaning guichan build tree..."
	${RM} -rf "guichan-${guichan_VERSION}"
}




################################################################################
# Guichan build thanks to Visual Express.
################################################################################


getguichan_win()
{
	LOG_STATUS "Getting guichan for windows..."
	launchFileRetrieval guichan_win
	return $?
}


prepareguichan_win()
{


	LOG_STATUS "Preparing guichan for windows.."
	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunzipped."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "guichan    "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${guichan_win_ARCHIVE} ${guichan_win_ARCHIVE}.save && ${GUNZIP} -f ${guichan_win_ARCHIVE} && ${TAR} -xvf "guichan-${guichan_win_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${guichan_win_ARCHIVE}."
		LOG_STATUS "Restoring ${guichan_win_ARCHIVE}."
		${MV} -f ${guichan_win_ARCHIVE}.save ${guichan_win_ARCHIVE}
		exit 10
	fi

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/guichan-from-LOANI" "guichan-${guichan_win_VERSION}"

	${MV} -f ${guichan_win_ARCHIVE}.save ${guichan_win_ARCHIVE}
	${RM} -f "guichan-${guichan_win_VERSION}.tar"

	printOK

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/guichan-from-LOANI" "guichan-${guichan_win_VERSION}"

	if [ $? != 0 ] ; then
		ERROR "Unable to copy guichan solution in build tree."
		exit 11
	fi

	printOK

}


generateguichan_win()
{

	LOG_STATUS "Generating guichan for windows..."

	cd "guichan-${guichan_win_VERSION}"

	printItem "configuring"
	printOK

	sdl_solution=`pwd`"/guichan-from-LOANI/guichan-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress guichan ${sdl_solution}
	printOK

	printItem "installing"

	# Take care of the exported header files (API):
	sdl_install=${alternate_prefix}/guichan-${guichan_win_VERSION}
	${MKDIR} -p ${sdl_install}
	${CP} -rf include ${sdl_install}
	printOK

	printEndList

	LOG_STATUS "guichan successfully installed."

	cd "$initial_dir"

}


cleanguichan_win()
{
	LOG_STATUS "Cleaning guichan build tree..."
	${RM} -rf "guichan-${guichan_win_VERSION}"
}




################################################################################
################################################################################
# Agar (libag_core/libag_gui)
################################################################################
################################################################################


#TRACE "[loani-requiredTools] Agar"

################################################################################
# Agar for non-Windows platforms:
################################################################################

getAgar()
{
	LOG_STATUS "Getting Agar..."
	launchFileRetrieval Agar
	return $?
}


prepareAgar()
{

	LOG_STATUS "Preparing Agar..."

	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunzipped."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "Agar       "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${Agar_ARCHIVE} ${Agar_ARCHIVE}.save && ${GUNZIP} -f ${Agar_ARCHIVE} && ${TAR} -xvf "agar-${Agar_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${Agar_ARCHIVE}."
		LOG_STATUS "Restoring ${Agar_ARCHIVE}."
		${MV} -f ${Agar_ARCHIVE}.save ${Agar_ARCHIVE}
		exit 10
	fi

	${MV} -f ${Agar_ARCHIVE}.save ${Agar_ARCHIVE}
	${RM} -f "agar-${Agar_VERSION}.tar"

	printOK

}


generateAgar()
{

	LOG_STATUS "Generating Agar..."


	cd "agar-${Agar_VERSION}"

	printItem "configuring"

	AGAR_CONFIGURE_OPT="--disable-network --disable-threads --disable-server --enable-gui --enable-utf8 --with-gl --with-jpeg --with-freetype "

	if [ -n "$prefix" ] ; then
	{

		if [ $is_windows -eq 0 ] ; then
			Agar_PREFIX=`cygpath -w ${prefix}/agar-${Agar_VERSION} | ${SED} 's|\\\|/|g'`
		else
			Agar_PREFIX="${prefix}/agar-${Agar_VERSION}"
		fi


		${MKDIR} -p ${Agar_PREFIX}

		# Currently the Agar configure cannot override the default location
		# for libjpeg (CFLAGS="-I${libjpeg_PREFIX}/include" is rejected):
		setBuildEnv ./configure --prefix=${Agar_PREFIX} ${AGAR_CONFIGURE_OPT}

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ./configure	${AGAR_CONFIGURE_OPT}
	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure Agar."
		exit 11
	fi

	printOK

	printItem "building"

	{

		 setBuildEnv ${MAKE} depend all

	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build Agar."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{

		echo "# Agar section." >> ${OSDL_ENV_FILE}

		echo "Agar_PREFIX=${Agar_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export Agar_PREFIX" >> ${OSDL_ENV_FILE}
		echo "PATH=\$Agar_PREFIX/bin:\${PATH}" >> ${OSDL_ENV_FILE}
		echo "LD_LIBRARY_PATH=\$Agar_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		PATH=${Agar_PREFIX}/bin:${PATH}
		export PATH

		LD_LIBRARY_PATH=${Agar_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		if [ $is_windows -eq 0 ] ; then

			PATH=${Agar_PREFIX}/lib:${PATH}
			export PATH

			echo "PATH=\$Agar_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}

		LIBPATH="-L${Agar_PREFIX}/lib"


		setBuildEnv ${MAKE} install

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install Agar."
		exit 13
	fi

	if [ $is_windows -eq 0 ] ; then
		${MV} -f ${Agar_PREFIX}/bin/*.dll ${Agar_PREFIX}/lib
	fi

	printOK

	printEndList

	LOG_STATUS "Agar successfully installed."

	cd "$initial_dir"

}


cleanAgar()
{
	LOG_STATUS "Cleaning Agar build tree..."
	${RM} -rf "Agar-${Agar_VERSION}"
}




################################################################################
# Agar build thanks to Visual Express.
# Static libraries have been removed.
# Paths and properties have been updated in agar*vcproj files.
################################################################################



getAgar_win()
{
	LOG_STATUS "Getting Agar for windows..."
	launchFileRetrieval Agar_win
	return $?
}



prepareAgar_win()
{


	LOG_STATUS "Preparing Agar for windows.."

	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "Agar       "

	printItem "extracting"

	cd $repository

	{
		${UNZIP} -o ${Agar_win_ARCHIVE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${Agar_win_ARCHIVE}."
		LOG_STATUS "Restoring ${Agar_win_ARCHIVE}."
		${MV} -f ${Agar_win_ARCHIVE}.save ${Agar_win_ARCHIVE}
		exit 10
	fi

	# Now let's use our own solution:

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/Agar-from-LOANI" "agar-${Agar_win_VERSION}"

	if [ $? != 0 ] ; then
		ERROR "Unable to copy Agar solution in build tree."
		exit 12
	fi

	cd "agar-${Agar_win_VERSION}"

	AGAR_FLAVOUR_ARCHIVE="vs2005-windows-nothreads.zip"

	# Now we use the project prebuilt files as a base:
	{

		${CP} -f "ProjectFiles/${AGAR_FLAVOUR_ARCHIVE}" . && ${UNZIP} -o "${AGAR_FLAVOUR_ARCHIVE}"
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${AGAR_FLAVOUR_ARCHIVE}."
		exit 11
	fi


	printOK


}



generateAgar_win()
{

	LOG_STATUS "Generating Agar for windows..."


	printItem "configuring"
	printOK

	Agar_solution=`pwd`"/Agar-from-LOANI/Agar-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress Agar ${Agar_solution}
	printOK

	printItem "installing"

	# Take care of the exported header files (API):
	Agar_install=${alternate_prefix}/agar-${Agar_win_VERSION}
	${MKDIR} -p ${Agar_install}
	${CP} -rf include ${Agar_install}
	printOK

	printEndList

	LOG_STATUS "Agar successfully installed."

	cd "$initial_dir"

}



cleanAgar_win()
{
	LOG_STATUS "Cleaning Agar build tree..."
	${RM} -rf "agar-${Agar_win_VERSION}"
}





################################################################################
################################################################################
# PCRE (Perl Compatible Regular Expressions, http://www.pcre.org/)
# Needed by CEGUI.
################################################################################
################################################################################


################################################################################
# PCRE for non-Windows platforms
################################################################################

#TRACE "[loani-requiredTools] PCRE"


getPCRE()
{
	LOG_STATUS "Getting PCRE..."
	launchFileRetrieval PCRE
	return $?
}


preparePCRE()
{

	LOG_STATUS "Preparing PCRE..."

	if findTool bunzip2 ; then
		BUNZIP2=$returnedString
	else
		ERROR "No bunzip2 tool found, whereas some files have to be bunzip2-ed."
		exit 14
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "PCRE       "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of bunzip2.
	{
		${CP} -f ${PCRE_ARCHIVE} ${PCRE_ARCHIVE}.save && ${BUNZIP2} -f ${PCRE_ARCHIVE} && tar -xvf "pcre-${PCRE_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${PCRE_ARCHIVE}."
		LOG_STATUS "Restoring ${PCRE_ARCHIVE}."
		${MV} -f ${PCRE_ARCHIVE}.save ${PCRE_ARCHIVE}
		exit 10
	fi

	${MV} -f ${PCRE_ARCHIVE}.save ${PCRE_ARCHIVE}
	${RM} -f "pcre-${PCRE_VERSION}.tar"

	printOK

}


generatePCRE()
{

	LOG_STATUS "Generating PCRE..."


	cd "pcre-${PCRE_VERSION}"

	printItem "configuring"


	if [ -f "config.cache" ] ; then
		${RM} -f config.cache
	fi

	if [ -n "$prefix" ] ; then
	{

		PCRE_PREFIX=${prefix}/PCRE-${PCRE_VERSION}

		setBuildEnv ./configure --prefix=${PCRE_PREFIX} --enable-unicode-properties

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ./configure
	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure PCRE."
		exit 11
	fi

	printOK

	printItem "building"

	if [ -n "$prefix" ] ; then
	{

		setBuildEnv ${MAKE}

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE}

	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build PCRE."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{
		echo "# PCRE section." >> ${OSDL_ENV_FILE}

		echo "PCRE_PREFIX=${PCRE_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export PCRE_PREFIX" >> ${OSDL_ENV_FILE}
		echo "LD_LIBRARY_PATH=\$PCRE_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		LD_LIBRARY_PATH=${PCRE_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		if [ $is_windows -eq 0 ] ; then

			PATH=${PCRE_PREFIX}/lib:${PATH}
			export PATH

			echo "PATH=\$PCRE_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}

		${MKDIR} -p ${PCRE_PREFIX}

		setBuildEnv ${MAKE} install prefix=${PCRE_PREFIX}

		if [ $? != 0 ] ; then
			echo
			ERROR "Unable to install PCRE."
			exit 13
		fi


	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install PCRE."
		exit 13
	fi


	printOK

	printEndList

	LOG_STATUS "PCRE successfully installed."

	cd "$initial_dir"

}


cleanPCRE()
{
	LOG_STATUS "Cleaning PCRE library build tree..."
	${RM} -rf "pcre-${PCRE_VERSION}"
}



################################################################################
# PCRE build thanks to Visual Express.
################################################################################


getPCRE_win()
{
	LOG_STATUS "Getting PCRE for windows..."
	launchFileRetrieval PCRE_win
	return $?
}


preparePCRE_win()
{

	LOG_STATUS "Preparing PCRE for windows.."

	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "PCRE  "

	printItem "extracting"

	cd $repository

	{
		${UNZIP} -o ${PCRE_win_ARCHIVE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${PCRE_win_ARCHIVE}."
		exit 10
	fi

	cd "PCRE-${PCRE_win_VERSION}"

	PCRE_install_dir="${prefix}/PCRE-${PCRE_win_VERSION}"

	${MKDIR} -p ${PCRE_install_dir}

	cd $repository

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/PCRE-from-LOANI" "PCRE-${PCRE_win_VERSION}"

	if [ $? != 0 ] ; then
		ERROR "Unable to copy PCRE solution in build tree."
		exit 11
	fi

	printOK

}


generatePCRE_win()
{

	LOG_STATUS "Generating PCRE for windows..."

	cd "pcre-${PCRE_win_VERSION}"

	printItem "configuring"
	printOK

	PCRE_solution=`pwd`"/PCRE-from-LOANI/PCRE-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress PCRE ${PCRE_solution}
	printOK

	printItem "installing"

	PCRE_install_include_dir=${PCRE_install_dir}/include
	${MKDIR} -p ${PCRE_install_include_dir}
	${CP} -f pcre.h ${PCRE_install_include_dir}

	printOK

	printEndList

	LOG_STATUS "PCRE successfully installed."

	cd "$initial_dir"

}


cleanPCRE_win()
{
	LOG_STATUS "Cleaning PCRE build tree..."
	${RM} -rf "pcre-${PCRE_win_VERSION}"
}



################################################################################
################################################################################
# FreeImage (http://freeimage.sourceforge.net)
# Needed by CEGUI.
################################################################################
################################################################################


################################################################################
# FreeImage for non-Windows platforms
################################################################################

#TRACE "[loani-requiredTools] FreeImage"


getFreeImage()
{
	LOG_STATUS "Getting FreeImage..."
	launchFileRetrieval FreeImage
	return $?
}


prepareFreeImage()
{

	LOG_STATUS "Preparing FreeImage..."

	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "FreeImage  "

	printItem "extracting"

	cd $repository

	{
		${UNZIP} -o "${FreeImage_ARCHIVE}" && ${MV} -f FreeImage "FreeImage-${FreeImage_VERSION}"
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ ! $? -eq 0 ] ; then
		ERROR "Unable to extract ${FreeImage_ARCHIVE}."
		exit 10
	fi

	printOK

}


generateFreeImage()
{

	LOG_STATUS "Generating FreeImage..."

	cd "FreeImage-${FreeImage_VERSION}"

	printItem "configuring"

	# Nothing to do here.

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure FreeImage."
		exit 11
	fi

	printOK

	printItem "building"

	if [ -n "$prefix" ] ; then
	{

		setBuildEnv ${MAKE}

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE}

	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build FreeImage."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{

		FreeImage_PREFIX="${prefix}/FreeImage-${FreeImage_VERSION}"

		echo "# FreeImage section." >> ${OSDL_ENV_FILE}

		echo "FreeImage_PREFIX=${FreeImage_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export FreeImage_PREFIX" >> ${OSDL_ENV_FILE}
		echo "LD_LIBRARY_PATH=\$FreeImage_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		LD_LIBRARY_PATH=${FreeImage_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		if [ $is_windows -eq 0 ] ; then

			PATH=${FreeImage_PREFIX}/lib:${PATH}
			export PATH

			echo "PATH=\$FreeImage_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}

		${MKDIR} -p ${FreeImage_PREFIX}

		# The default install make target cannot be used, as it requires root
		# privileges.

		#setBuildEnv ${MAKE} install INSTALLDIR=${FreeImage_PREFIX}

		# Custom-made install instead:
		FreeImage_inc="${FreeImage_PREFIX}/include"
		FreeImage_lib="${FreeImage_PREFIX}/lib"

		# Ex: libfreeimage-3.15.0.so
		FreeImage_shared_lib="libfreeimage-${FreeImage_VERSION}.so"

		# Ex: libfreeimage.so.3
		FreeImage_shared_lib_short="libfreeimage.so."`echo ${FreeImage_VERSION} | sed 's|\..*||1'`

		install -d ${FreeImage_inc} ${FreeImage_lib} && install -m 644 Source/FreeImage.h ${FreeImage_inc} && install -m 644 libfreeimage.a  ${FreeImage_lib} && install -m 755 ${FreeImage_shared_lib} ${FreeImage_lib} && ln -sf ${FreeImage_shared_lib} ${FreeImage_lib}/${FreeImage_shared_lib_short} && ln -sf ${FreeImage_shared_lib_short} ${FreeImage_lib}/libfreeimage.so

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install FreeImage."
		exit 13
	fi


	printOK

	printEndList

	LOG_STATUS "FreeImage successfully installed."

	cd "$initial_dir"

}


cleanFreeImage()
{
	LOG_STATUS "Cleaning FreeImage library build tree..."
	${RM} -rf "FreeImage-${FreeImage_VERSION}"
}



################################################################################
# FreeImage build thanks to Visual Express.
################################################################################


getFreeImage_win()
{
	LOG_STATUS "Getting FreeImage for windows..."
	launchFileRetrieval FreeImage_win
	return $?
}


prepareFreeImage_win()
{

	LOG_STATUS "Preparing FreeImage for windows.."

	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "FreeImage  "

	printItem "extracting"

	cd $repository

	{
		${UNZIP} -o ${FreeImage_win_ARCHIVE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${FreeImage_win_ARCHIVE}."
		exit 10
	fi

	cd "FreeImage-${FreeImage_win_VERSION}"

	FreeImage_install_dir="${prefix}/FreeImage-${FreeImage_win_VERSION}"

	${MKDIR} -p ${FreeImage_install_dir}

	cd $repository

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/FreeImage-from-LOANI" "FreeImage-${FreeImage_win_VERSION}"

	if [ $? != 0 ] ; then
		ERROR "Unable to copy FreeImage solution in build tree."
		exit 11
	fi

	printOK

}


generateFreeImage_win()
{

	LOG_STATUS "Generating FreeImage for windows..."

	cd "FreeImage-${FreeImage_win_VERSION}"

	printItem "configuring"
	printOK

	FreeImage_solution=`pwd`"/FreeImage-from-LOANI/FreeImage-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress FreeImage ${FreeImage_solution}
	printOK

	printItem "installing"

	FreeImage_install_include_dir=${FreeImage_install_dir}/include
	${MKDIR} -p ${FreeImage_install_include_dir}
	${CP} -f FreeImage.h ${FreeImage_install_include_dir}

	printOK

	printEndList

	LOG_STATUS "FreeImage successfully installed."

	cd "$initial_dir"

}


cleanFreeImage_win()
{
	LOG_STATUS "Cleaning FreeImage build tree..."
	${RM} -rf "FreeImage-${FreeImage_win_VERSION}"
}





################################################################################
################################################################################
# CEGUI
################################################################################
################################################################################


#TRACE "[loani-requiredTools] CEGUI"

################################################################################
# CEGUI for non-Windows platforms:
################################################################################

getCEGUI()
{
	LOG_STATUS "Getting CEGUI..."
	launchFileRetrieval CEGUI
	return $?
}


prepareCEGUI()
{

	LOG_STATUS "Preparing CEGUI..."

	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunzipped."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "CEGUI      "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${CEGUI_ARCHIVE} ${CEGUI_ARCHIVE}.save && ${GUNZIP} -f ${CEGUI_ARCHIVE} && ${TAR} -xvf "CEGUI-${CEGUI_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${CEGUI_ARCHIVE}."
		LOG_STATUS "Restoring ${CEGUI_ARCHIVE}."
		${MV} -f ${CEGUI_ARCHIVE}.save ${CEGUI_ARCHIVE}
		exit 10
	fi

	${MV} -f ${CEGUI_ARCHIVE}.save ${CEGUI_ARCHIVE}
	${RM} -f "CEGUI-${CEGUI_VERSION}.tar"

	printOK

}


generateCEGUI()
{

	LOG_STATUS "Generating CEGUI..."


	cd "CEGUI-${CEGUI_VERSION}"

	printItem "configuring"

	# Telling where the CEGUI dependencies are to be found:

	export pcre_CFLAGS="-I${PCRE_PREFIX}/include"
	export pcre_LIBS="-L${PCRE_PREFIX}/lib -lpcre"

	export freetype2_CFLAGS="-I${freetype_PREFIX}/include -I${freetype_PREFIX}/include/freetype2"
	export freetype2_LIBS="-L${freetype_PREFIX}/lib -lfreetype"


	# --disable-pcre does not work (thus removed), as CEGUI still requires PCRE:
	# In file included from elements/CEGUIEditbox.cpp:38:0:
	# ../../cegui/include/CEGUIPCRERegexMatcher.h:33:18: fatal error: pcre.h: No
	# such file or directory
	#
	# Note: the glut package is not required for CEGUI itself, but is required
	# for the samples: 'apt-get install libglut3-dev libglut3'.

	# --with-zlib=DIR could be used as well:
	CEGUI_CONFIGURE_OPT="--enable-debug --disable-lua-module --disable-python-module"

	if [ -n "$prefix" ] ; then
	{

		if [ $is_windows -eq 0 ] ; then
			CEGUI_PREFIX=`cygpath -w ${prefix}/CEGUI-${CEGUI_VERSION} | ${SED} 's|\\\|/|g'`
		else
			CEGUI_PREFIX="${prefix}/CEGUI-${CEGUI_VERSION}"
		fi


		${MKDIR} -p ${CEGUI_PREFIX}

		# Currently the CEGUI configure cannot override the default location
		# for libjpeg (CFLAGS="-I${libjpeg_PREFIX}/include" is rejected):
		setBuildEnv ./configure --prefix=${CEGUI_PREFIX} ${CEGUI_CONFIGURE_OPT}

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ./configure ${CEGUI_CONFIGURE_OPT}
	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure CEGUI."
		exit 11
	fi

	printOK

	printItem "building"

	{

		 setBuildEnv ${MAKE} all

	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build CEGUI."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{

		echo "# CEGUI section." >> ${OSDL_ENV_FILE}

		echo "CEGUI_PREFIX=${CEGUI_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export CEGUI_PREFIX" >> ${OSDL_ENV_FILE}
		echo "PATH=\$CEGUI_PREFIX/bin:\${PATH}" >> ${OSDL_ENV_FILE}
		echo "LD_LIBRARY_PATH=\$CEGUI_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		PATH=${CEGUI_PREFIX}/bin:${PATH}
		export PATH

		LD_LIBRARY_PATH=${CEGUI_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		if [ $is_windows -eq 0 ] ; then

			PATH=${CEGUI_PREFIX}/lib:${PATH}
			export PATH

			echo "PATH=\$CEGUI_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}

		LIBPATH="-L${CEGUI_PREFIX}/lib"


		setBuildEnv ${MAKE} install

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install CEGUI."
		exit 13
	fi

	if [ $is_windows -eq 0 ] ; then
		${MV} -f ${CEGUI_PREFIX}/bin/*.dll ${CEGUI_PREFIX}/lib
	fi

	printOK

	printEndList

	LOG_STATUS "CEGUI successfully installed."

	cd "$initial_dir"

}


cleanCEGUI()
{
	LOG_STATUS "Cleaning CEGUI build tree..."
	${RM} -rf "CEGUI-${CEGUI_VERSION}"
}




################################################################################
# CEGUI build thanks to Visual Express.
# Static libraries have been removed.
# Paths and properties have been updated in CEGUI*vcproj files.
################################################################################



getCEGUI_win()
{
	LOG_STATUS "Getting CEGUI for windows..."
	launchFileRetrieval CEGUI_win
	return $?
}



prepareCEGUI_win()
{


	LOG_STATUS "Preparing CEGUI for windows.."

	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "CEGUI       "

	printItem "extracting"

	cd $repository

	{
		${UNZIP} -o ${CEGUI_win_ARCHIVE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${CEGUI_win_ARCHIVE}."
		LOG_STATUS "Restoring ${CEGUI_win_ARCHIVE}."
		${MV} -f ${CEGUI_win_ARCHIVE}.save ${CEGUI_win_ARCHIVE}
		exit 10
	fi

	# Now let's use our own solution:

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/CEGUI-from-LOANI" "CEGUI-${CEGUI_win_VERSION}"

	if [ $? != 0 ] ; then
		ERROR "Unable to copy CEGUI solution in build tree."
		exit 12
	fi

	cd "CEGUI-${CEGUI_win_VERSION}"

	CEGUI_FLAVOUR_ARCHIVE="vs2005-windows-nothreads.zip"

	# Now we use the project prebuilt files as a base:
	{

		${CP} -f "ProjectFiles/${CEGUI_FLAVOUR_ARCHIVE}" . && ${UNZIP} -o "${CEGUI_FLAVOUR_ARCHIVE}"
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${CEGUI_FLAVOUR_ARCHIVE}."
		exit 11
	fi


	printOK


}



generateCEGUI_win()
{

	LOG_STATUS "Generating CEGUI for windows..."


	printItem "configuring"
	printOK

	CEGUI_solution=`pwd`"/CEGUI-from-LOANI/CEGUI-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress CEGUI ${CEGUI_solution}
	printOK

	printItem "installing"

	# Take care of the exported header files (API):
	CEGUI_install=${alternate_prefix}/CEGUI-${CEGUI_win_VERSION}
	${MKDIR} -p ${CEGUI_install}
	${CP} -rf include ${CEGUI_install}
	printOK

	printEndList

	LOG_STATUS "CEGUI successfully installed."

	cd "$initial_dir"

}



cleanCEGUI_win()
{
	LOG_STATUS "Cleaning CEGUI build tree..."
	${RM} -rf "CEGUI-${CEGUI_win_VERSION}"
}





################################################################################
################################################################################
# PhysicsFS
################################################################################
################################################################################


#TRACE "[loani-requiredTools] PhysicsFS"

################################################################################
# PhysicsFS for non-Windows platforms:
################################################################################

getPhysicsFS()
{
	LOG_STATUS "Getting PhysicsFS..."
	launchFileRetrieval PhysicsFS
	return $?
}


preparePhysicsFS()
{

	LOG_STATUS "Preparing PhysicsFS..."

	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunzipped."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "PhysicsFS  "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${PhysicsFS_ARCHIVE} ${PhysicsFS_ARCHIVE}.save && ${GUNZIP} -f ${PhysicsFS_ARCHIVE} && ${TAR} -xvf "physfs-${PhysicsFS_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${PhysicsFS_ARCHIVE}."
		LOG_STATUS "Restoring ${PhysicsFS_ARCHIVE}."
		${MV} -f ${PhysicsFS_ARCHIVE}.save ${PhysicsFS_ARCHIVE}
		exit 10
	fi

	${MV} -f ${PhysicsFS_ARCHIVE}.save ${PhysicsFS_ARCHIVE}
	${RM} -f "physfs-${PhysicsFS_VERSION}.tar"

	printOK

}


generatePhysicsFS()
{

	LOG_STATUS "Generating PhysicsFS..."


	cd "physfs-${PhysicsFS_VERSION}"

	printItem "configuring"

	PHYSICSFS_CMAKE_OPT=""

	if [ -n "$prefix" ] ; then

		PhysicsFS_PREFIX="${alternate_prefix}/PhysicsFS-${PhysicsFS_VERSION}"

PHYSICSFS_CMAKE_OPT="-DCMAKE_INSTALL_PREFIX=${PhysicsFS_PREFIX}"

	fi

	{

		setBuildEnv ${CMAKE} . ${PHYSICSFS_CMAKE_OPT}

	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to pre-configure PhysicsFS."
		exit 11
	fi

	{

		setBuildEnv echo cqe | ${CCMAKE} .

	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure PhysicsFS."
		exit 12
	fi

	printOK

	printItem "building"

	{

		 setBuildEnv ${MAKE}

	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build PhysicsFS."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{

		echo "# PhysicsFS section." >> ${OSDL_ENV_FILE}

		echo "PhysicsFS_PREFIX=${PhysicsFS_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export PhysicsFS_PREFIX" >> ${OSDL_ENV_FILE}
		echo "PATH=\$PhysicsFS_PREFIX/bin:\${PATH}" >> ${OSDL_ENV_FILE}
		echo "LD_LIBRARY_PATH=\$PhysicsFS_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		PATH=${PhysicsFS_PREFIX}/bin:${PATH}
		export PATH

		LD_LIBRARY_PATH=${PhysicsFS_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		if [ $is_windows -eq 0 ] ; then

			PATH=${PhysicsFS_PREFIX}/lib:${PATH}
			export PATH

			echo "PATH=\$PhysicsFS_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}

		LIBPATH="-L${PhysicsFS_PREFIX}/lib"

		setBuildEnv ${MAKE} install


	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install PhysicsFS."
		exit 13
	fi


	if [ $is_windows -eq 0 ] ; then
		${MV} -f ${PhysicsFS_PREFIX}/bin/*.dll ${PhysicsFS_PREFIX}/lib
	fi

	printOK

	printEndList

	LOG_STATUS "PhysicsFS successfully installed."

	cd "$initial_dir"

}


cleanPhysicsFS()
{
	LOG_STATUS "Cleaning PhysicsFS build tree..."
	${RM} -rf "PhysicsFS-${PhysicsFS_VERSION}"
}




################################################################################
# PhysicsFS build thanks to Visual Express.
# Static libraries have been removed.
# Paths and properties have been updated in agar*vcproj files.
################################################################################


getPhysicsFS_win()
{
	LOG_STATUS "Getting PhysicsFS for windows..."
	launchFileRetrieval PhysicsFS_win
	return $?
}


preparePhysicsFS_win()
{


	LOG_STATUS "Preparing PhysicsFS for windows.."

	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunzipped."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "PhysicsFS       "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${PhysicsFS_win_ARCHIVE} ${PhysicsFS_win_ARCHIVE}.save && ${GUNZIP} -f ${PhysicsFS_win_ARCHIVE} && ${TAR} -xvf "physfs-${PhysicsFS_win_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${PhysicsFS_win_ARCHIVE}."
		LOG_STATUS "Restoring ${PhysicsFS_win_ARCHIVE}."
		${MV} -f ${PhysicsFS_win_ARCHIVE}.save ${PhysicsFS_win_ARCHIVE}
		exit 10
	fi


	# Now let's use our own solution:

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/PhysicsFS-from-LOANI" "physfs-${PhysicsFS_win_VERSION}"

	if [ $? != 0 ] ; then
		ERROR "Unable to copy PhysicsFS solution in build tree."
		exit 12
	fi

	printOK


}


generatePhysicsFS_win()
{

	LOG_STATUS "Generating PhysicsFS for windows..."

	cd "physfs-${PhysicsFS_win_VERSION}"

	printItem "configuring"
	printOK

	PhysicsFS_solution=`pwd`"/PhysicsFS-from-LOANI/PhysicsFS-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress PhysicsFS ${PhysicsFS_solution}
	printOK

	printItem "installing"

	# Take care of the exported header files (API):
	PhysicsFS_install=${alternate_prefix}/PhysicsFS-${PhysicsFS_win_VERSION}
	${MKDIR} -p ${PhysicsFS_install}/include
	${CP} -f *.h ${PhysicsFS_install}/include
	printOK

	printEndList

	LOG_STATUS "PhysicsFS successfully installed."

	cd "$initial_dir"

}


cleanPhysicsFS_win()
{
	LOG_STATUS "Cleaning PhysicsFS build tree..."
	${RM} -rf "agar-${PhysicsFS_win_VERSION}"
}



################################################################################
################################################################################
# SDL_gfx
################################################################################
################################################################################


################################################################################
# SDL_gfx for non-Windows platforms:
################################################################################

#TRACE "[loani-requiredTools] SDL_gfx"

getSDL_gfx()
{
	LOG_STATUS "Getting SDL_gfx..."
	launchFileRetrieval SDL_gfx
	return $?
}


prepareSDL_gfx()
{

	LOG_STATUS "Preparing SDL_gfx..."
	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunzipped."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "SDL_gfx    "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${SDL_gfx_ARCHIVE} ${SDL_gfx_ARCHIVE}.save && ${GUNZIP} -f ${SDL_gfx_ARCHIVE} && ${TAR} -xvf "SDL_gfx-${SDL_gfx_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${SDL_gfx_ARCHIVE}."
		LOG_STATUS "Restoring ${SDL_gfx_ARCHIVE}."
		${MV} -f ${SDL_gfx_ARCHIVE}.save ${SDL_gfx_ARCHIVE}
		exit 10
	fi

	${MV} -f ${SDL_gfx_ARCHIVE}.save ${SDL_gfx_ARCHIVE}
	${RM} -f "SDL_gfx-${SDL_gfx_VERSION}.tar"

	printOK

}


generateSDL_gfx()
{

	LOG_STATUS "Generating SDL_gfx..."

	cd "SDL_gfx-${SDL_gfx_VERSION}"

	printItem "configuring"

	# Not running autogen.sh by ourselves, as the built-in configure is now
	# perfectly usable, and unless adding -I ${SDL_PREFIX}/share/aclocal to
	# the aclocal call, the right sdl.m4 would not be found anyway.

	#{
	#	setBuildEnv ./autogen.sh
	#} 1>>"$LOG_OUTPUT" 2>&1

	#if [ $? != 0 ] ; then
	#	echo
	#	ERROR "Unable to configure SDL_gfx: autogen failed."
	#	exit 11
	#fi

	if [ -f "config.cache" ] ; then
		${RM} -f config.cache
	fi

	if [ -n "$prefix" ] ; then
	{

		SDL_gfx_PREFIX=${prefix}/SDL_gfx-${SDL_gfx_VERSION}

		# SDL_gfx uses wrongly SDL includes: asks for SDL/SDL.h instead of
		# SDL.h.
		#
		# Ugly hack:
		# (copy could be used instead, to avoid needing symbolic links for
		# filesystems such as vfat)
		${LN} -s ${SDL_PREFIX}/include/SDL ${SDL_PREFIX}/include/SDL/SDL

		OLD_CPP_FLAGS=$CPP_FLAGS
		CPP_FLAGS="-I${SDL_PREFIX}/include $CPP_FLAGS"
		export CPP_FLAGS

		OLD_LD_FLAGS=$LD_FLAGS

		LDFLAGS="-L${SDL_PREFIX}/lib $LDFLAGS"
		export LDFLAGS

		LIBS=$LDFLAGS
		export LIBS

		# --disable-sdltest added since configure tries to compile a test
		# without letting the system libraries locations to be redefined.
		#
		# Therefore a wrong libstdc++.so could be chosen, leading to errors
		# such as:
		# "undefined reference to `_Unwind_Resume_or_Rethrow@GCC_3.3'"

		setBuildEnv ./configure --prefix=${SDL_gfx_PREFIX} --exec-prefix=${SDL_gfx_PREFIX} --with-sdl-prefix=${SDL_PREFIX} --disable-sdltest

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ./configure
	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure SDL_gfx."
		exit 11
	fi

	if [ -n "$prefix" ] ; then

		CPP_FLAGS=$OLD_CPP_FLAGS
		export CPP_FLAGS

		LD_FLAGS=$OLD_LD_FLAGS
		export LD_FLAGS

	fi

	# SDL_gfx will not be compiled with debug machinery:
	{
		setBuildEnv ./nodebug.sh
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "SDL_gfx post-configure script failed (nodebug.sh)."
		exit 12
	fi

	printOK

	printItem "building"

	{
		setBuildEnv ${MAKE} LDFLAGS="-lgcc_s"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build SDL_gfx."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{

		echo "# SDL_gfx section." >> ${OSDL_ENV_FILE}

		echo "SDL_gfx_PREFIX=${SDL_gfx_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export SDL_gfx_PREFIX" >> ${OSDL_ENV_FILE}
		echo "LD_LIBRARY_PATH=\$SDL_gfx_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		LD_LIBRARY_PATH=${SDL_gfx_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		if [ $is_windows -eq 0 ] ; then

			PATH=${SDL_gfx_PREFIX}/lib:${PATH}
			export PATH

			echo "PATH=\$SDL_gfx_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}

		setBuildEnv ${MAKE} install prefix=${SDL_gfx_PREFIX}

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE} install

	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install SDL_gfx (application)."
		exit 13
	fi

	FIXED_FONT_REPOSITORY="${OSDL_DATA_FULL_DIR_NAME}/fonts/fixed"
	${MKDIR} -p "${FIXED_FONT_REPOSITORY}" && ${CP} -f Fonts/*.fnt "${FIXED_FONT_REPOSITORY}"
	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install SDL_gfx fonts."
		exit 14
	fi

	printOK

	{
		setBuildEnv ${MAKE} distclean
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		WARNING "SDL_gfx post-install cleaning failed, non-fatal error."
	fi

	printEndList

	LOG_STATUS "SDL_gfx successfully installed."

	cd "$initial_dir"

}


cleanSDL_gfx()
{
	LOG_STATUS "Cleaning SDL gfx library build tree..."
	${RM} -rf "SDL_gfx-${SDL_gfx_VERSION}"
}



################################################################################
# SDL_gfx build thanks to Visual Express:
################################################################################

getSDL_gfx_win()
{
	LOG_STATUS "Getting SDL_gfx_win..."
	launchFileRetrieval SDL_gfx_win
	return $?
}


prepareSDL_gfx_win()
{

	LOG_STATUS "Preparing SDL_gfx_win..."
	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunzipped."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "SDL_gfx    "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${SDL_gfx_win_ARCHIVE} ${SDL_gfx_win_ARCHIVE}.save && ${GUNZIP} -f ${SDL_gfx_win_ARCHIVE} && ${TAR} -xvf "SDL_gfx-${SDL_gfx_win_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${SDL_gfx_win_ARCHIVE}."
		LOG_STATUS "Restoring ${SDL_gfx_win_ARCHIVE}."
		${MV} -f ${SDL_gfx_win_ARCHIVE}.save ${SDL_gfx_win_ARCHIVE}
		exit 10
	fi

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/SDL_gfx-from-LOANI" "SDL_gfx-${SDL_gfx_win_VERSION}"

	${MV} -f ${SDL_gfx_win_ARCHIVE}.save ${SDL_gfx_win_ARCHIVE}
	${RM} -f "SDL_gfx-${SDL_gfx_win_VERSION}.tar"

	printOK

}


generateSDL_gfx_win()
{

	LOG_STATUS "Generating SDL_gfx_win..."

	cd "SDL_gfx-${SDL_gfx_win_VERSION}"

	printItem "configuring"
	printOK

	sdl_gfx_solution=`pwd`"/SDL_gfx-from-LOANI/SDL_gfx-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress SDL_gfx ${sdl_gfx_solution}
	printOK

	printItem "installing"

	sdl_gfx_install_dir="${prefix}/SDL_gfx-${SDL_gfx_win_VERSION}"
	sdl_gfx_include_install_dir="${sdl_gfx_install_dir}/include"
	${MKDIR} -p "${sdl_gfx_include_install_dir}"
	${CP} -f *.h "${sdl_gfx_include_install_dir}"

	printOK

	printEndList

	LOG_STATUS "SDL_gfx successfully installed."

	cd "$initial_dir"

}


cleanSDL_gfx_win()
{
	LOG_STATUS "Cleaning SDL gfx library build tree..."
	${RM} -rf "SDL_gfx-${SDL_gfx_win_VERSION}"
}



################################################################################
################################################################################
# Freetype
################################################################################
################################################################################


#TRACE "[loani-requiredTools] freetype"


getfreetype()
{
	LOG_STATUS "Getting freetype..."
	launchFileRetrieval freetype
	return $?
}


preparefreetype()
{

	LOG_STATUS "Preparing freetype..."

	if findTool bunzip2 ; then
		BUNZIP2=$returnedString
	else
		ERROR "No bunzip2 tool found, whereas some files have to be bunzip2-ed."
		exit 14
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 15
	fi

	printBeginList "freetype   "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of bunzip2.
	{
		${CP} -f ${freetype_ARCHIVE} ${freetype_ARCHIVE}.save && ${BUNZIP2} -f ${freetype_ARCHIVE} && ${TAR} -xvf "freetype-${freetype_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${freetype_ARCHIVE}."
		LOG_STATUS "Restoring ${freetype_ARCHIVE}."
		${MV} -f ${freetype_ARCHIVE}.save ${freetype_ARCHIVE}
		exit 10
	fi

	${MV} -f ${freetype_ARCHIVE}.save ${freetype_ARCHIVE}
	${RM} -f "freetype-${freetype_VERSION}.tar"

	printOK

}


generatefreetype()
{

	LOG_STATUS "Generating freetype..."

	cd "freetype-${freetype_VERSION}"

	printItem "configuring"

	if [ -n "$prefix" ] ; then
	{

		freetype_PREFIX=${prefix}/freetype-${freetype_VERSION}

		setBuildEnv ./configure --prefix=${freetype_PREFIX} --exec-prefix=${freetype_PREFIX}
	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ./configure
	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure freetype."
		exit 11
	fi

	printOK

	printItem "building"

	{
		# LDFLAGS="-lgcc_s" not added
		setBuildEnv ${MAKE}
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build freetype."
		exit 12
	fi

	printOK


	printItem "installing"

	# PATH is managed as well, as Agar will try to run the freetype-config
	# script afterwards:

	if [ -n "$prefix" ] ; then
	{
		echo "# freetype section." >> ${OSDL_ENV_FILE}

		echo "freetype_PREFIX=${freetype_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export freetype_PREFIX" >> ${OSDL_ENV_FILE}

		echo "PATH=\$freetype_PREFIX/bin:\${PATH}" >> ${OSDL_ENV_FILE}
		echo "LD_LIBRARY_PATH=\$freetype_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		if [ $is_windows -eq 0 ] ; then

			PATH=${freetype_PREFIX}/lib:${freetype_PREFIX}/bin:${PATH}
			export PATH

			echo "PATH=\$freetype_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}

		PATH=${freetype_PREFIX}/bin:${PATH}
		export PATH

		LD_LIBRARY_PATH=${freetype_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		setBuildEnv ${MAKE} install

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
			setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install freetype."
		exit 13
	fi

	printOK


	printEndList

	LOG_STATUS "freetype successfully installed."

	cd "$initial_dir"

}


cleanfreetype()
{
	LOG_STATUS "Cleaning Freetype library build tree..."
	${RM} -rf "freetype-${freetype_VERSION}"
}



################################################################################
# Freetype build thanks to Visual Express:
################################################################################


getfreetype_win()
{
	LOG_STATUS "Getting freetype_win..."
	launchFileRetrieval freetype_win
	return $?
}


preparefreetype_win()
{

	LOG_STATUS "Preparing freetype_win..."

	if findTool bunzip2 ; then
		BUNZIP2=$returnedString
	else
		ERROR "No bunzip2 tool found, whereas some files have to be bunzip2-ed."
		exit 14
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 15
	fi

	printBeginList "freetype   "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of bunzip2.
	{
		${CP} -f ${freetype_win_ARCHIVE} ${freetype_win_ARCHIVE}.save && ${BUNZIP2} -f ${freetype_win_ARCHIVE} && ${TAR} -xvf "freetype-${freetype_win_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${freetype_win_ARCHIVE}."
		LOG_STATUS "Restoring ${freetype_win_ARCHIVE}."
		${MV} -f ${freetype_win_ARCHIVE}.save ${freetype_win_ARCHIVE}
		exit 10
	fi

	${MV} -f ${freetype_win_ARCHIVE}.save ${freetype_win_ARCHIVE}
	${RM} -f "freetype-${freetype_win_VERSION}.tar"

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/Freetype-from-LOANI" "freetype-${freetype_win_VERSION}"
	${CP} -f "${WINDOWS_SOLUTIONS_ROOT}/Freetype-from-LOANI/ftoption.h" "freetype-${freetype_win_VERSION}/include/freetype/config"

	printOK

}


generatefreetype_win()
{

	LOG_STATUS "Generating freetype_win..."

	cd "freetype-${freetype_win_VERSION}"

	printItem "configuring"
	printOK

	freetype_solution=`pwd`"/Freetype-from-LOANI/Freetype-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress freetype ${freetype_solution}
	printOK

	printItem "installing"

	freetype_install_dir="${prefix}/Freetype-${freetype_win_VERSION}"
	freetype_include_install_dir="${freetype_install_dir}/include"
	${MKDIR} -p "${freetype_include_install_dir}"
	${CP} -rf include/* "${freetype_include_install_dir}"

	printOK

	printEndList

	LOG_STATUS "freetype_win successfully installed."

	cd "$initial_dir"

}


cleanfreetype_win()
{
	LOG_STATUS "Cleaning Freetype library build tree..."
	${RM} -rf "freetype-${freetype_win_VERSION}"
}



################################################################################
################################################################################
# SDL_ttf
################################################################################
################################################################################

################################################################################
# SDL_ttf for non-Windows platforms
################################################################################

#TRACE "[loani-requiredTools] SDL_ttf"


getSDL_ttf()
{
	LOG_STATUS "Getting SDL_ttf..."
	launchFileRetrieval SDL_ttf
	return $?
}


prepareSDL_ttf()
{

	LOG_STATUS "Preparing SDL_ttf..."
	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunzipped."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "SDL_ttf    "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${SDL_ttf_ARCHIVE} ${SDL_ttf_ARCHIVE}.save && ${GUNZIP} -f ${SDL_ttf_ARCHIVE} && ${TAR} -xvf "SDL_ttf-${SDL_ttf_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${SDL_ttf_ARCHIVE}."
		LOG_STATUS "Restoring ${SDL_ttf_ARCHIVE}."
		${MV} -f ${SDL_ttf_ARCHIVE}.save ${SDL_ttf_ARCHIVE}
		exit 10
	fi

	${MV} -f ${SDL_ttf_ARCHIVE}.save ${SDL_ttf_ARCHIVE}
	${RM} -f "SDL_ttf-${SDL_ttf_VERSION}.tar"

	printOK

}


generateSDL_ttf()
{

	LOG_STATUS "Generating SDL_ttf..."

	cd "SDL_ttf-${SDL_ttf_VERSION}"

	printItem "configuring"

	{
		setBuildEnv ./autogen.sh
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure SDL_ttf: autogen failed."
		exit 11
	fi

	if [ -n "$prefix" ] ; then
	{

		SDL_ttf_PREFIX=${prefix}/SDL_ttf-${SDL_ttf_VERSION}

		# --disable-sdltest added since configure tries to compile
		# a test without letting the system libraries locations to
		# be redefined. Therefore a wrong libstdc++.so
		# could be chosen, leading to errors such as:
		# "undefined reference to `_Unwind_Resume_or_Rethrow@GCC_3.3'"

		# SDL_ttf.c needs freetype/internal/ftobjs.h, which is in the
		# freetype sources only (not installed), hence the CPPFLAGS:

		setBuildEnv ./configure --prefix=${SDL_ttf_PREFIX} --exec-prefix=${SDL_ttf_PREFIX} --with-freetype-prefix=${freetype_PREFIX} --with-freetype-exec-prefix=${freetype_PREFIX} --with-sdl-prefix=${SDL_PREFIX} --with-sdl-exec-prefix=${SDL_PREFIX} --disable-sdltest CPPFLAGS="-I${repository}/freetype-${freetype_VERSION}/include"
	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ./configure
	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure SDL_ttf."
		exit 11
	fi

	printOK

	printItem "building"

	# SDL_ttf will not compile if not patched:
	# Ugly tr-based hack to prevent the numerous versions of sed to
	# panic when having a newline in replacement string:
	# This modification used to work with previous SDL_ttf releases:
	#${CAT} SDL_ttf.c | ${SED} 's|#include <freetype/freetype.h>|#include <ft2build.h>?#include FT_FREETYPE_H??#include <freetype/freetype.h>|1' | ${TR} "?" "\n" > SDL_ttf-patched.c && ${RM} -f SDL_ttf.c && ${MV} -f SDL_ttf-patched.c SDL_ttf.c

	# This one works, at least for SDL_ttf-2.0.8:
	# See also:
	#    - http://www.freetype.org/freetype2/freetype-2.2.0.html
	#    - http://www.freetype.org/freetype2/patches/rogue-patches.html
	${CAT} SDL_ttf.c | ${SED} 's|#include <freetype/internal/ftobjs.h>||g' | sed 's|library->memory|NULL|g' > SDL_ttf-patched.c && ${RM} -f SDL_ttf.c && ${MV} -f SDL_ttf-patched.c SDL_ttf.c
	if [ $? != 0 ] ; then
		echo
		WARNING "SDL_ttf include patch failed, continuing anyway."
	fi

	{
		setBuildEnv ${MAKE} LDFLAGS="-L${prefix}/LOANI-installations/SDL-${SDL_VERSION}/lib -lgcc_s"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build SDL_ttf."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{


		echo "# SDL_ttf section." >> ${OSDL_ENV_FILE}

		echo "SDL_ttf_PREFIX=${SDL_ttf_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export SDL_ttf_PREFIX" >> ${OSDL_ENV_FILE}
		echo "LD_LIBRARY_PATH=\$SDL_ttf_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		if [ $is_windows -eq 0 ] ; then

			PATH=${SDL_ttf_PREFIX}/lib:${PATH}
			export PATH

			echo "PATH=\$SDL_ttf_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}

		LD_LIBRARY_PATH=${SDL_ttf_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		setBuildEnv ${MAKE} install prefix=${SDL_ttf_PREFIX}

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install SDL_ttf."
		exit 13
	fi

	printOK

	printEndList

	LOG_STATUS "SDL_ttf successfully installed."

	cd "$initial_dir"

}


cleanSDL_ttf()
{
	LOG_STATUS "Cleaning SDL ttf library build tree..."
	${RM} -rf "SDL_ttf-${SDL_ttf_VERSION}"
}



################################################################################
# SDL_ttf build thanks to Visual Express.
################################################################################


getSDL_ttf_win()
{
	LOG_STATUS "Getting SDL_ttf for windows..."
	launchFileRetrieval SDL_ttf_win
	return $?
}


prepareSDL_ttf_win()
{

	LOG_STATUS "Preparing SDL_ttf for windows.."
	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "SDL_ttf    "

	printItem "extracting"

	cd $repository

	{
		${UNZIP} -o ${SDL_ttf_win_ARCHIVE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${SDL_ttf_win_ARCHIVE}."
		exit 10
	fi

	cd "SDL_ttf-${SDL_ttf_win_VERSION}"

	sdl_ttf_install_dir="${prefix}/SDL_ttf-${SDL_ttf_win_VERSION}"

	${MKDIR} -p ${sdl_ttf_install_dir}
	cd $repository

	${CP} -r -f "${WINDOWS_SOLUTIONS_ROOT}/SDL_ttf-from-LOANI" "SDL_ttf-${SDL_ttf_win_VERSION}" && ${CP} -f "${WINDOWS_SOLUTIONS_ROOT}/SDL_ttf-from-LOANI/SDL_ttf.c" "SDL_ttf-${SDL_ttf_win_VERSION}"
	if [ $? != 0 ] ; then
		ERROR "Unable to copy SDL_ttf solution and fixes in build tree."
		exit 11
	fi

	printOK

}


generateSDL_ttf_win()
{

	LOG_STATUS "Generating SDL_ttf for windows..."

	cd "SDL_ttf-${SDL_ttf_win_VERSION}"

	printItem "configuring"
	printOK

	sdl_ttf_solution=`pwd`"/SDL_ttf-from-LOANI/SDL_ttf-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress SDL_ttf ${sdl_ttf_solution}
	printOK

	printItem "installing"

	sdl_ttf_include_install_dir="${sdl_ttf_install_dir}/include"
	${MKDIR} -p ${sdl_ttf_include_install_dir}
	${CP} -f SDL_ttf.h ${sdl_ttf_include_install_dir}

	printOK

	printEndList

	LOG_STATUS "SDL_ttf successfully installed."

	cd "$initial_dir"

}


cleanSDL_ttf_win()
{
	LOG_STATUS "Cleaning SDL_ttf build tree..."
	${RM} -rf "SDL_ttf-${SDL_ttf_win_VERSION}"
}



################################################################################
# GNU Libtool
#
# Note that this will be a version of libtool specifically patched, so that it
# stops always linking libraries with a rpath set.
#
# Otherwise this would prevent distributing prebuilt OSDL libraries.
# See http://wiki.debian.org/RpathIssue for more informations.
################################################################################


#TRACE "[loani-requiredTools] Libtool"

#TRACE "[loani-requiredTools] Libtool: getlibtool"

getlibtool()
{
	LOG_STATUS "Getting libtool..."
	launchFileRetrieval libtool
}


#TRACE "[loani-requiredTools] Libtool: preparelibtool"

preparelibtool()
{

	LOG_STATUS "Preparing libtool..."

	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunzipped."
		exit 8
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 9
	fi

	printBeginList "libtool    "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${libtool_ARCHIVE} ${libtool_ARCHIVE}.save && ${GUNZIP} -f ${libtool_ARCHIVE} && ${TAR} -xvf "libtool-${libtool_FULL_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${libtool_ARCHIVE}."
		LOG_STATUS "Restoring ${libtool_ARCHIVE}."
		${MV} -f ${libtool_ARCHIVE}.save ${libtool_ARCHIVE}
		exit 10
	fi

	${MV} -f ${libtool_ARCHIVE}.save ${libtool_ARCHIVE}
	${RM} -f "libtool-${libtool_VERSION}.tar"

	printOK

}


#TRACE "[loani-requiredTools] Libtool: generatelibtool"

generatelibtool()
{

	LOG_STATUS "Generating libtool..."

	cd "libtool-${libtool_VERSION}"

	printItem "configuring"

	if [ -n "$prefix" ] ; then
	{

		# Warns that this is a patched version:
		libtool_PREFIX=${prefix}/libtool-${libtool_VERSION}-no-rpath

		# configure calls libltdl configure and loose env.
		old_path=$PATH
		old_ld_path=$LD_LIBRARY_PATH
		old_cc=$CC
		old_cxx=$CXX

		if [ $is_mingw -eq 0 ] ; then

			PATH=${MINGW_PATH}:$PATH
			export PATH

			LD_LIBRARY_PATH=${MINGW_LD_LIBRARY_PATH}:$LD_LIBRARY_PATH
			export LD_LIBRARY_PATH

			CC=${MINGW_GCC}
			export CC

			CXX=${MINGW_GPP}
			export CXX

		fi

		setBuildEnv ./configure --prefix=${libtool_PREFIX} --exec-prefix=${libtool_PREFIX} --enable-ltdl-install

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		if [ $is_mingw -eq 0 ] ; then

			PATH=${MINGW_PATH}:$PATH
			export PATH

			LD_LIBRARY_PATH=${MINGW_LD_LIBRARY_PATH}:$LD_LIBRARY_PATH
			export LD_LIBRARY_PATH

			CC=${MINGW_GCC}
			export CC

			CXX=${MINGW_GPP}
			export CXX

		fi

		setBuildEnv ./configure

	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure libtool."
		exit 11
	fi

	if [ ${is_mingw} -eq 0 ] ; then

		PATH=$old_path
		export PATH

		LD_LIBRARY_PATH=$old_ld_library_path
		export LD_LIBRARY_PATH

		CC=$old_cc
		export CC

		CXX=$old_cxx
		export CXX

	fi

	printOK

	printItem "building"

	{
		setBuildEnv ${MAKE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build libtool."
		exit 12
	fi

	printOK


	printItem "installing"

	if [ -n "$prefix" ] ; then
	{
		${MKDIR} -p ${libtool_PREFIX}

		echo "# libtool section." >> ${OSDL_ENV_FILE}

		echo "libtool_PREFIX=${libtool_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export libtool_PREFIX" >> ${OSDL_ENV_FILE}
		echo "PATH=\$libtool_PREFIX/bin:\${PATH}" >> ${OSDL_ENV_FILE}
		echo "LD_LIBRARY_PATH=\$libtool_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		if [ $is_windows -eq 0 ] ; then

			PATH=${libtool_PREFIX}/lib:${PATH}
			export PATH

			echo "PATH=\$libtool_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}

		fi

		echo "" >> ${OSDL_ENV_FILE}

		PATH=${libtool_PREFIX}/bin:${PATH}
		export PATH

		LD_LIBRARY_PATH=${libtool_PREFIX}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		setBuildEnv ${MAKE} install prefix=${libtool_PREFIX}


	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install libtool."
		exit 13
	fi

	# Patching only if a prefix is used (not installing a modified
	# libtool in system tree):

	if [ -n "$prefix" ] ; then
	{

		target_script="${libtool_PREFIX}/bin/libtool"

		if [ ! -f "${target_script}" ] ; then
			echo
			ERROR "Unable to patch libtool."
			exit 14
		fi

		# Largely inspired from http://wiki.debian.org/RpathIssue:
		${CAT} "${target_script}" | sed 's|^hardcode_libdir_flag_spec=.*$|hardcode_libdir_flag_spec="-DLOANI_PATCHED_LIBTOOL_FOR_RPATH"|1' > libtool-patched
		${MV} -f libtool-patched "${target_script}"
		${CHMOD} 755 "${target_script}"


	} 1>>"$LOG_OUTPUT" 2>&1
	fi


	printOK

	printEndList

	LOG_STATUS "libtool successfully installed."

	cd "$initial_dir"

}


#TRACE "[loani-requiredTools] Libtool: cleanlibtool"

cleanlibtool()
{
	LOG_STATUS "Cleaning libtool build tree..."
	${RM} -rf "libtool-${libtool_VERSION}"
}




################################################################################
# Windows pthreads for POSIX compliance.
################################################################################


#TRACE "[loani-requiredTools] Windows pthreads"


# Using precompiled binaries.

getwin_pthread()
{

	LOG_STATUS "Getting pthread-win32..."

	PTHREADS_WIN32_DIR="pthreads-win32"

	if [ -s "${repository}/${PTHREADS_WIN32_DIR}/lib/${win_pthread_DLL}" -a -s "${repository}/${PTHREADS_WIN32_DIR}/include/${win_pthread_HEADER_MAIN}" -a -s "${repository}/${PTHREADS_WIN32_DIR}/include/${win_pthread_HEADER_SCHED}" -a -s "${repository}/${PTHREADS_WIN32_DIR}/include/${win_pthread_HEADER_SEM}" ] ; then
		DEBUG "Assuming win_pthread archive is already available (library and include files found)."
		return 0
	else
		DEBUG "Not all parts of win_pthread available, downloading them."

		# The following is commented out, it would have to be updated
		# before use:

		# Needs to download recursively the FTP root:
		#OLD_WGET_OPT="${WGET_OPT}"
		#WGET_OPT="-r ${WGET_OPT}"

		# Retrieves "at first" the whole ftp subdirectory, which we won't be able to md5 check.
		#launchFileRetrieval win_pthread_ftp_dir} ${win_pthread_DOWNLOAD_LOCATION} ${win_pthread_DLL_MD5}

		#WGET_OPT="${OLD_WGET_OPT}"

		# Get header files and the library, the latter being the one file that is checked to decide whether transfer is over.
		#launchFileRetrieval ${win_pthread_HEADER_MAIN}  ${win_pthread_DOWNLOAD_LOCATION}/include ${win_pthread_HEADER_MAIN_MD5}
		#launchFileRetrieval ${win_pthread_HEADER_SCHED} ${win_pthread_DOWNLOAD_LOCATION}/include ${win_pthread_HEADER_SCHED_MD5}
		#launchFileRetrieval ${win_pthread_HEADER_SEM}   ${win_pthread_DOWNLOAD_LOCATION}/include ${win_pthread_HEADER_SEM_MD5}

		#launchFileRetrieval ${win_pthread_ARCHIVE}      ${win_pthread_DOWNLOAD_LOCATION}/lib     ${win_pthread_DLL_MD5}

	fi

}


preparewin_pthread()
{

	LOG_STATUS "Preparing pthread-win32..."

	printBeginList "win pthread"


	printItem "extracting"

	# Nothing to do!

	printOK

}


generatewin_pthread()
{

	LOG_STATUS "Generating pthread-win32..."

	printItem "configuring"

	# Nothing to do!

	printOK

	printItem "building"

	# Nothing to do!


	printItem "installing"

	real_download_target=${win_pthread_wget_dir}/pub/${PTHREADS_WIN32_DIR}

	# If win_pthread repository is not sane, rebuild it:

	if [ -s "${repository}/${PTHREADS_WIN32_DIR}/lib/${win_pthread_DLL}" -a -s "${repository}/${PTHREADS_WIN32_DIR}/include/${win_pthread_HEADER_MAIN}" -a -s "${repository}/${PTHREADS_WIN32_DIR}/include/${win_pthread_HEADER_SCHED}" -a -s "${repository}/${PTHREADS_WIN32_DIR}/include/${win_pthread_HEADER_SEM}" ] ; then
		DEBUG "win_pthread installation in repository is ok."
	else

		# Clean-up wget mess:
		 {
			sleep 5
			${MKDIR} -p $repository/${PTHREADS_WIN32_DIR}

			if [ -s "${repository}/${win_pthread_DLL}" -a -s "${repository}/${win_pthread_HEADER_MAIN}" -a -s "${repository}/${win_pthread_HEADER_SCHED}" -a -s "${repository}/${win_pthread_HEADER_SEM}" ] ; then
				${MKDIR} -p ${repository}/${PTHREADS_WIN32_DIR}/include
				${CP} -f ${repository}/${win_pthread_HEADER_MAIN} ${repository}/${win_pthread_HEADER_SCHED} ${repository}/${win_pthread_HEADER_SEM} ${repository}/${PTHREADS_WIN32_DIR}/include

				${MKDIR} -p ${repository}/${PTHREADS_WIN32_DIR}/lib
				 ${CP} -f ${repository}/${win_pthread_DLL} ${repository}/${PTHREADS_WIN32_DIR}/lib
			else
				ERROR "Not all win_pthread parts are available while pre installing."
				exit 16
			fi

			# The include and/or lib directory might not be already retrieved:
			if ls ${real_download_target}/${win_pthread_ftp_dir}/include 1>/dev/null 2>&1 ; then
				${CP} -f ${real_download_target}/${win_pthread_ftp_dir}/* $repository/${PTHREADS_WIN32_DIR}
			else
				DEBUG "No win_pthread FTP directory to retrieve (no ${real_download_target}/${win_pthread_ftp_dir} directory)."
						fi
		} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		WARNING "Pre installation phase for win pthread had errors."
		${RM} -rf ${win_pthread_wget_dir}
	fi


	if [ -n "$prefix" ] ; then
	{
		echo "# pthreads win32 section." >> ${OSDL_ENV_FILE}

		echo "pthreads_win32_PREFIX=${prefix}/${PTHREADS_WIN32_DIR}" >> ${OSDL_ENV_FILE}
		echo "export pthreads_win32_PREFIX" >> ${OSDL_ENV_FILE}
		echo "LD_LIBRARY_PATH=\$pthreads_win32_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

		if [ $is_windows -eq 0 ] ; then

			PATH=${prefix}/${PTHREADS_WIN32_DIR}/lib:${PATH}
			export PATH

			echo "PATH=\$pthreads_win32_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}

		fi

		echo "" >> ${OSDL_ENV_FILE}

		LD_LIBRARY_PATH=${prefix}/${PTHREADS_WIN32_DIR}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		${CP} -rf ${repository}/${PTHREADS_WIN32_DIR} ${prefix}

	} 1>>"$LOG_OUTPUT" 2>&1
	else
	{
		WARNING "Win pthread installation without prefix not tested."

		echo "# pthreads win32 section." >> ${OSDL_ENV_FILE}
		echo "PATH=/usr/local/lib:\${PATH}" >> ${OSDL_ENV_FILE}
		echo "export PATH" >> ${OSDL_ENV_FILE}
		echo "" >> ${OSDL_ENV_FILE}

		LD_LIBRARY_PATH=/usr:local/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		${MKDIR} -p /usr/local/include /usr/local/lib && ${CP} -f ${repository}/${PTHREADS_WIN32_DIR}/include/* /usr/local/include && ${CP} -f ${repository}/${PTHREADS_WIN32_DIR}/lib/* /usr/local/lib

	} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install win pthread."
		exit 15
	fi

	printOK

	printEndList

	LOG_STATUS "pthread-win32 successfully installed."

	cd "$initial_dir"

}


cleanwin_pthread()
{
	LOG_STATUS "Cleaning Win pthread library build tree..."

	# Should have been copied to ${prefix}:
	pthread_build_dir=${repository}/${PTHREADS_WIN32_DIR}

	if [ -d "${pthread_build_dir}" ]; then
		${RM} -rf "${pthread_build_dir}"
	fi
}




################################################################################
# Nintendo DS section
################################################################################


################################################################################
################################################################################
# dlditool
################################################################################
################################################################################


#TRACE "[loani-requiredTools] dlditool"

getdlditool()
{
	LOG_STATUS "Getting dlditool and DLDI patches..."

	launchFileRetrieval dlditool
	launchFileRetrieval dldi_patch_sc
	launchFileRetrieval dldi_patch_r4
	launchFileRetrieval dldi_patch_fcsr

	return $?
}


preparedlditool()
{

	LOG_STATUS "Preparing dlditool..."

	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "dlditool   "

	printItem "extracting"

	cd $repository

	if [ -n "$prefix" ] ; then
		dlditool_PREFIX=${ds_prefix}/dldi
	else
		dlditool_PREFIX=dldi
	fi

	${MKDIR} -p ${dlditool_PREFIX}

	# Extract prebuilt executable in installation repository:
	{
		${UNZIP} -o ${dlditool_ARCHIVE} -d ${dlditool_PREFIX}
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${dlditool_ARCHIVE}."
		exit 11
	fi

	printOK

}


generatedlditool()
{

	LOG_STATUS "Generating dlditool..."


	printItem "configuring"

	${CP} -f ${dldi_patch_sc_ARCHIVE}   ${dlditool_PREFIX}
	${CP} -f ${dldi_patch_r4_ARCHIVE}   ${dlditool_PREFIX}
	${CP} -f ${dldi_patch_fcsr_ARCHIVE} ${dlditool_PREFIX}

	cd ${dlditool_PREFIX}

	# Extract fcsr archive:
	{
		${UNZIP} -o ${dldi_patch_fcsr_ARCHIVE} && ${CP} -f fcsr/fcsr.dldi .
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure dlditool."
		exit 11
	fi

	printOK


	printItem "building"

	printOK


	printItem "installing"

	cd $repository


	if [ -n "$prefix" ] ; then

		echo "# dlditool section." >> ${OSDL_ENV_FILE}

		echo "dlditool_PREFIX=${dlditool_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "export dlditool_PREFIX" >> ${OSDL_ENV_FILE}
		echo "PATH=\$dlditool_PREFIX:\${PATH}" >> ${OSDL_ENV_FILE}

		echo "" >> ${OSDL_ENV_FILE}

		PATH=${dlditool_PREFIX}:${PATH}
		export PATH

	fi

	${CHMOD} +x ${dlditool_PREFIX}/dlditool

	printOK

	printEndList

	LOG_STATUS "dlditool successfully installed."

	cd "$initial_dir"

}


cleandlditool()
{
	LOG_STATUS "Cleaning dlditool build tree..."
}







################################################################################
################################################################################
# Ceylan
################################################################################
################################################################################



################################################################################
# Ceylan on non-Windows platforms:
################################################################################


#TRACE "[loani-requiredTools] Ceylan"



getCeylan()
{


	DEBUG "Getting Ceylan..."

	# Ceylan can be obtained by downloading a release archive or by using VCS.

	if [ ${use_current_vcs} -eq 1 ]; then

		# Use archive, instead of VCS:
		launchFileRetrieval Ceylan
		return $?

	else

		declareRetrievalBegin "Ceylan (from ${current_vcs})"

	fi

	# Here we are to use VCS:

	# To avoid a misleading message when the retrieval is finished:
	Ceylan_ARCHIVE="from ${current_vcs}"

	cd ${repository}

	# Manage back-up directory if necessary:

	if [ -d "${repository}/ceylan" ] ; then

		if [ -d "${repository}/ceylan.save" ] ; then

			if [ $be_strict -eq 0 ] ; then
				ERROR "There already exists a back-up directory for Ceylan, it is on the way, please remove it first (${repository}/ceylan.save)"
				exit 5

			else

				WARNING "Deleting already existing back-up directory for ceylan (removing ${repository}/ceylan.save)"
				${RM} -rf "${repository}/ceylan.save" 2>/dev/null
				# Sometimes rm fails apparently (long names or other reasons):
				${MV} -f ${repository}/ceylan.save ${repository}/ceylan.save-`date '+%Hh-%Mm-%Ss'` 2>/dev/null

			fi

		fi

		${MV} -f ${repository}/ceylan ${repository}/ceylan.save 2>/dev/null
		WARNING "There already existed a directory for Ceylan (${repository}/ceylan), it has been moved to ${repository}/ceylan.save."
	fi

	LOG_STATUS "Getting Ceylan in its source directory ${repository}..."

	# Note: VCS from Cygwin is really really slow due to the filesystem access.

	if [ $developer_access -eq 0 ] ; then

		#DISPLAY "Retrieving Ceylan from developer VCS with user name ${developer_name}."

		CHECKOUT_LOCATION="$repository/ceylan/Ceylan"
		${MKDIR} -p ${CHECKOUT_LOCATION}

		VCS_SERVER_PATH="git.code.sf.net/p/ceylan/code"

		if [ $no_vcs -eq 1 ] ; then

			DISPLAY "      ----> getting Ceylan from ${current_vcs} with user name ${developer_name} (check-out)"

			vcsAttemptNumber=1
			success=1

			VCS_URL="git.code.sf.net/p/ceylan/code"

			while [ "$vcsAttemptNumber" -le "$MAX_VCS_RETRY" ]; do

				LOG_STATUS "Attempt #${vcsAttemptNumber} to retrieve Ceylan."

				{
					DEBUG "VCS command: ${GIT} clone ssh://${developer_name}@${VCS_SERVER_PATH} ${CHECKOUT_LOCATION}"

					${GIT} clone ssh://${developer_name}@${VCS_SERVER_PATH} ${CHECKOUT_LOCATION}

				} 1>>"$LOG_OUTPUT" 2>&1

				if [ $? -eq 0 ] ; then

					vcsAttemptNumber=$(($MAX_VCS_RETRY+1))
					success=0

				else

					vcsAttemptNumber=$(($vcsAttemptNumber+1))
					LOG_STATUS "VCS command failed."
					#${SLEEP} 3

					# Warning:
					# cygwin uses a quite small MAX_PATH, which limits the
					# maximum length of paths.
					#It may cause, among others, a SVN error
					# ("svn: Can't open file 'XXX': File name too long).
					# A work-around is to request the user to update herself her
					# repository with TortoiseSVN (this tool is not affected by
					# the MAX_PATH issue), as soon as an error occured.

					# Now ask the user to trigger the full update by herself,
					# with TortoiseSVN:
					DISPLAY "Ceylan VCS checkout failed, maybe because of too long pathnames."
					DISPLAY "Please use a tool like Tortoise to update manually the Ceylan repository."
					DISPLAY "To do so, right-click on ${repository}/${CHECKOUT_LOCATION}, and select 'Update'"
					waitForKey "< Press enter when the repository is up-to-date, use CTRL-C if the operation could not be performed >"
					# Suppose success:
					vcsAttemptNumber=$(($MAX_VCS_RETRY+1))
					success=0
				fi

			done


			if [ $success -ne 0 ] ; then

				ERROR "Unable to retrieve Ceylan from VCS after $MAX_VCS_RETRY attempts (did you accept permanently the Sourceforge certificate, if asked?)."

				exit 20

			fi

		else
			DISPLAY "      ----> VCS retrieval disabled for Ceylan."
		fi

		if [ $? -ne 0 ] ; then
			ERROR "Unable to retrieve Ceylan from developer VCS."
			exit 20
		fi

	else

		# Not a developer access, anonymous:

		if [ $no_vcs -eq 1 ] ; then

			DISPLAY "      ----> getting Ceylan from anonymous VCS (export)"

			vcsAttemptNumber=1
			success=1

			while [ "$vcsAttemptNumber" -le "$MAX_VCS_RETRY" ]; do

				LOG_STATUS "Attempt #${vcsAttemptNumber} to retrieve Ceylan."

				{

					# No https, no credential required:
					DEBUG "${SVN} export http://${Ceylan_SVN_SERVER}:${SVN_URL} ${SVN_OPT}"
					${GIT} clone git://${VCS_SERVER_PATH} ${CHECKOUT_LOCATION}

				} 1>>"$LOG_OUTPUT" 2>&1

				if [ $? -eq 0 ] ; then

					vcsAttemptNumber=$(($MAX_VCS_RETRY+1))
					success=0

				else

					vcsAttemptNumber=$(($vcsAttemptNumber+1))
					LOG_STATUS "SVN command failed."
					${SLEEP} 3

				fi

			done

			if [ $success -ne 0 ] ; then

				ERROR "Unable to retrieve Ceylan from VCS after $MAX_VCS_RETRY attempts."
				exit 21

			fi

		else
			DISPLAY "      ----> VCS retrieval disabled for Ceylan."
		fi

		if [ $? -ne 0 ] ; then
			ERROR "Unable to retrieve Ceylan from anonymous SVN."
			exit 21
		fi
	fi

	return 0

}



prepareCeylan()
{

	LOG_STATUS "Preparing Ceylan..."
	printBeginList "Ceylan     "

	printItem "extracting"

	if [ $no_vcs -eq 0 ] ; then
		echo
		WARNING "As the --noVCS option was used, build process stops here."
		exit 0
	fi


	if [ ${use_vcs} -eq 1 ]; then

		# Here we use source archives:

		if findTool bunzip2 ; then
			BUNZIP2=$returnedString
		else
			ERROR "No bunzip2 tool found, whereas some files have to be bunzip2-ed."
			exit 14
		fi

		if findTool tar ; then
			TAR=$returnedString
		else
			ERROR "No tar tool found, whereas some files have to be detarred."
			exit 15
		fi

		cd $repository

		# Prevent archive from disappearing because of bunzip2.
		{
			${CP} -f ${Ceylan_ARCHIVE} ${Ceylan_ARCHIVE}.save && ${BUNZIP2} -f ${Ceylan_ARCHIVE} && ${TAR} -xvf "ceylan-${Ceylan_VERSION}.tar"
		} 1>>"$LOG_OUTPUT" 2>&1


		if [ $? != 0 ] ; then
			ERROR "Unable to extract ${Ceylan_ARCHIVE}."
			DEBUG "Restoring ${Ceylan_ARCHIVE}."
			${MV} -f ${Ceylan_ARCHIVE}.save ${Ceylan_ARCHIVE}
			exit 10
		fi

	else

		# Using VCS here.

		if [ $use_current_vcs -eq 0 ] ; then

			DEBUG "No stable tag wanted, retrieving directly latest main-line head version (main branch only) from VCS."
			
			VCS_BRANCH="master"
			
		else
			
				# Should be quite uncommon for Ceylan developers:
			DEBUG "Using latest stable VCS tag (${latest_stable_ceylan})."
			
			VCS_BRANCH="${latest_stable_ceylan}"
				
		fi

		cd ${CHECKOUT_LOCATION}

		{

			${GIT} co ${VCS_BRANCH}

		} 1>>"$LOG_OUTPUT" 2>&1


		if [ $? != 0 ] ; then
			ERROR "Unable to switch to Ceylan branch ${VCS_BRANCH}."
			DEBUG "Restoring ${Ceylan_ARCHIVE}."
			${MV} -f ${Ceylan_ARCHIVE}.save ${Ceylan_ARCHIVE}
			exit 10
		fi

	fi

	printOK

}



generateCeylan()
{

	LOG_STATUS "Generating Ceylan..."

	if [ ${use_vcs} -eq 0 ]; then

		# Here, fin this source tree, we need to generate the build system:
		cd $repository/ceylan/Ceylan/trunk/src/conf/build
		{
			setBuildEnv ./autogen.sh --no-build
		} 1>>"$LOG_OUTPUT" 2>&1

		if [ $? != 0 ] ; then
			echo
			ERROR "Unable to generate build system for Ceylan (with autogen.sh)."
			exit 10
		fi

		# Going to the root of the source to continue the normal build process:
		cd $repository/ceylan/Ceylan

	else

		cd $repository/ceylan-${Ceylan_VERSION}
	fi


	# Rest of the build is common to autogen-based and release-based trees:


	printItem "configuring"

	if [ -n "$prefix" ] ; then
		{

			Ceylan_PREFIX="${prefix}/Ceylan-${Ceylan_VERSION}"

			setBuildEnv --exportEnv --appendEnv ./configure --prefix=${Ceylan_PREFIX}
		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			setBuildEnv --exportEnv --appendEnv ./configure
		} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure Ceylan."
		exit 11
	fi

	printOK

	printItem "building"

	{
		setBuildEnv ${MAKE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build Ceylan."
		exit 12
	fi

	printOK

	printItem "installing"

	if [ -n "$prefix" ] ; then
		{

			echo "# Ceylan section." >> ${OSDL_ENV_FILE}

			echo "Ceylan_PREFIX=${Ceylan_PREFIX}" >> ${OSDL_ENV_FILE}
			echo "export Ceylan_PREFIX" >> ${OSDL_ENV_FILE}


			if [ $is_windows -eq 0 ] ; then

				echo "PATH=\$Ceylan_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}

				PATH=${Ceylan_PREFIX}/lib:${PATH}
				export PATH

				echo "" >> ${OSDL_ENV_FILE}

			else

				echo "PATH=\$Ceylan_PREFIX/bin:\${PATH}" >> ${OSDL_ENV_FILE}

				echo "LD_LIBRARY_PATH=\$Ceylan_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

				echo "" >> ${OSDL_ENV_FILE}

				PATH=${Ceylan_PREFIX}/bin:${PATH}
				export PATH

				LD_LIBRARY_PATH=${Ceylan_PREFIX}/lib:${LD_LIBRARY_PATH}
				export LD_LIBRARY_PATH

			fi

			setBuildEnv ${MAKE} install prefix=${Ceylan_PREFIX}

		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			setBuildEnv ${MAKE} install
		} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to install Ceylan."
		exit 13
	fi


	LOG_STATUS "Making tests for Ceylan."

	cd test

	if [ ${use_svn} -eq 0 ]; then

		# Here we are in the SVN tree, needing to generate the
		# build system for tests:
		{
			setBuildEnv ./autogen.sh --no-build --ceylan-install-prefix ${Ceylan_PREFIX}
		} 1>>"$LOG_OUTPUT" 2>&1

		if [ $? != 0 ] ; then
			echo
			ERROR "Unable to generate build system for Ceylan tests (with autogen.sh)."
			exit 10
		fi

	fi

	# Rest of the build is common to autogen-based and release-based trees:

	if [ -n "$prefix" ] ; then
		{
			setBuildEnv --exportEnv --appendEnv ./configure --prefix=${Ceylan_PREFIX} --with-ceylan-prefix=${Ceylan_PREFIX}
		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			setBuildEnv --exportEnv --appendEnv ./configure
		} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure Ceylan tests."
		exit 14
	fi

	{
		setBuildEnv ${MAKE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build Ceylan tests."
		exit 15
	fi

	{
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install Ceylan tests."
		exit 16
	fi

	{
		setBuildEnv ./playTests.sh
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to pass Ceylan tests."
		exit 17
	fi


	printOK

	printEndList

	LOG_STATUS "Ceylan successfully installed."

	cd "$initial_dir"

}



cleanCeylan()
{
	LOG_STATUS "Cleaning Ceylan build tree..."
	# Nothing to do: we want to be able to go on with the Ceylan build.
}




################################################################################
# Ceylan build thanks to Visual Express, with SVN.
################################################################################


getCeylan_win()
{
	LOG_STATUS "Getting Ceylan for windows..."

	Ceylan_win_ARCHIVE="from SVN"
	# Cygwin let us do the same:
	getCeylan
	return $?
}



prepareCeylan_win()
{

	LOG_STATUS "Preparing Ceylan for windows.."

	ceylan_install_dir="${alternate_prefix}/Ceylan-${Ceylan_win_VERSION}"
	${MKDIR} -p ${ceylan_install_dir}

	# LOANI's version preferred to SVN's one for ease of debugging:
ceylan_solution_dir="$repository/ceylan/Ceylan/trunk/src/conf/build/visual-express"

	${CP} -f "${WINDOWS_SOLUTIONS_ROOT}"/Ceylan-from-LOANI/* ${ceylan_solution_dir}
	if [ $? != 0 ] ; then
		ERROR "Unable to copy Ceylan solution in build tree."
		exit 11
	fi

	printOK

}



generateCeylan_win()
{

	LOG_STATUS "Generating Ceylan for windows..."

	if [ ${use_svn} -eq 0 ]; then

		# Here we are in the SVN tree:
		cd $repository/ceylan/Ceylan/trunk

	else

		cd $repository/ceylan-${Ceylan_win_VERSION}
	fi


	printItem "configuring"
	printOK

	ceylan_solution="${ceylan_solution_dir}/Ceylan-from-LOANI.sln"

	printItem "building"
	GenerateWithVisualExpress Ceylan ${ceylan_solution}
	printOK

	printItem "installing"

	ceylan_install_include_dir=${ceylan_install_dir}/include
	${MKDIR} -p ${ceylan_install_include_dir}
	${FIND} . -name 'Ceylan*.h' -exec ${CP} -f '{}' ${ceylan_install_include_dir} ';'

	printOK

	printEndList

	LOG_STATUS "Ceylan successfully installed."

	cd "$repository"

}


cleanCeylan_win()
{
	LOG_STATUS "Cleaning Ceylan build tree..."
	${RM} -rf "$repository/ceylan*"
}




################################################################################
################################################################################
# OSDL
################################################################################
################################################################################



################################################################################
# OSDL on non-Windows platforms:
################################################################################


#TRACE "[loani-requiredTools] OSDL"


getOSDL()
{

	DEBUG "Getting OSDL..."

	# OSDL can be obtained by downloading a release archive or by using SVN.

	if [ ${use_svn} -eq 1 ]; then
		# Use archive instead of SVN:
		launchFileRetrieval OSDL
		return $?
	else
		declareRetrievalBegin "OSDL (from SVN)"
	fi

	# Here we are to use SVN:

	# To avoid a misleading message when the retrieval is finished:
	OSDL_ARCHIVE="from SVN"

	cd ${repository}

	# Manage back-up directory if necessary:

	if [ -d "${repository}/osdl" ] ; then
		if [ -d "${repository}/osdl.save" ] ; then
			if [ $be_strict -eq 0 ] ; then
				ERROR "There already exists a back-up directory for OSDL, it is on the way, please remove it first (${repository}/osdl.save)"
				exit 5
			else
				WARNING "Deleting already existing back-up directory for osdl (removing ${repository}/osdl.save)"
				${RM} -rf "${repository}/osdl.save" 2>/dev/null
				# Sometimes rm fails apparently (long names or other reasons):
				${MV} -f ${repository}/osdl.save ${repository}/osdl.save-`date '+%Hh-%Mm-%Ss'` 2>/dev/null

			fi
		fi
		${MV} -f ${repository}/osdl ${repository}/osdl.save 2>/dev/null
		WARNING "There already existed a directory for OSDL (${repository}/osdl), it has been moved to ${repository}/osdl.save."
	fi

	LOG_STATUS "Getting OSDL in its source directory ${repository}..."

	# Note: SVN from Cygwin is really really slow due to the filesystem access.

	if [ $developer_access -eq 0 ] ; then

		#DISPLAY "Retrieving OSDL from developer SVN with user name ${developer_name} (check-out)."

		if [ $no_vcs -eq 1 ] ; then

			DISPLAY "      ----> getting OSDL from SVN with user name ${developer_name} (check-out)"

			vcsAttemptNumber=1
			success=1

			if [ $use_current_vcs -eq 0 ] ; then
				DEBUG "No stable tag wanted, retrieving directly latest main-line version (trunk only) from SVN."

				# Only selecting trunk, as otherwise could be way too long:
				CHECKOUT_LOCATION=osdl/OSDL/trunk
				${MKDIR} -p ${CHECKOUT_LOCATION}
				SVN_URL="/svnroot/${CHECKOUT_LOCATION}"
			else

				# Should be quite uncommon for OSDL developers:
				DEBUG "Using latest stable SVN tag (${latest_stable_osdl})."

				CHECKOUT_LOCATION=osdl
				${MKDIR} -p ${CHECKOUT_LOCATION}
				SVN_URL="/svnroot/osdl/OSDL/tags/${latest_stable_osdl}"
			fi

			while [ "$vcsAttemptNumber" -le "$MAX_VCS_RETRY" ]; do

				LOG_STATUS "Attempt #${vcsAttemptNumber} to retrieve OSDL."

				# Made to force certificate checking before next non-interactive svn command:
				${SVN} info https://${OSDL_SVN_SERVER}:${SVN_URL} --username=${developer_name} 1>/dev/null

				{
					DEBUG "SVN command: ${SVN} co https://${OSDL_SVN_SERVER}:${SVN_URL} ${CHECKOUT_LOCATION} --username=${developer_name} ${SVN_OPT}"
					${SVN} co https://${OSDL_SVN_SERVER}:${SVN_URL} ${CHECKOUT_LOCATION} --username=${developer_name} ${SVN_OPT}

				} 1>>"$LOG_OUTPUT" 2>&1

				if [ $? -eq 0 ] ; then
					vcsAttemptNumber=$(($MAX_VCS_RETRY+1))
					success=0
				else
					vcsAttemptNumber=$(($vcsAttemptNumber+1))
					LOG_STATUS "SVN command failed."
					#${SLEEP} 3

					# Warning:
					# cygwin uses a quite small MAX_PATH, which limits the
					# maximum length of paths.
					# It may cause, among others, a SVN error
					# ("svn: Can't open file 'XXX': File name too long).
					# A work-around is to request the user to update herself her
					# repository with TortoiseSVN (this tool is not affected by
					# the MAX_PATH issue), as soon as an error occured.

					# Now ask the user to trigger the full update by herself,
					# with TortoiseSVN:
					DISPLAY "OSDL SVN checkout failed, maybe because of too long pathnames."
					DISPLAY "Please use TortoiseSVN to update manually the OSDL repository."
					DISPLAY "To do so, right-click on ${repository}/${CHECKOUT_LOCATION}, and select 'SVN Update'"
					waitForKey "< Press enter when the repository is up-to-date, use CTRL-C if the operation could not be performed >"
					# Suppose success:
					vcsAttemptNumber=$(($MAX_VCS_RETRY+1))
					success=0
				fi

			done


			if [ $success -ne 0 ] ; then
				ERROR "Unable to retrieve OSDL from SVN after $MAX_VCS_RETRY attempts (did you accept permanently the Sourceforge certificate, if asked ?)."
				exit 20
			fi

		else
			DISPLAY "      ----> SVN retrieval disabled for OSDL."
		fi

		if [ $? -ne 0 ] ; then
			ERROR "Unable to retrieve OSDL from developer SVN."
			exit 20
		fi

	else

		if [ $no_vcs -eq 1 ] ; then

			DISPLAY "      ----> getting OSDL from anonymous SVN (export)"

			vcsAttemptNumber=1
			success=1

			if [ $use_current_vcs -eq 0 ] ; then
				DEBUG "No stable tag wanted, retrieving directly latest version from SVN."
				SVN_URL="/svnroot/osdl"
			else
				DEBUG "Using latest stable SVN tag (${latest_stable_osdl})."
				SVN_URL="/svnroot/osdl/OSDL/tags/${latest_stable_osdl}"
			fi

			while [ "$vcsAttemptNumber" -le "$MAX_VCS_RETRY" ]; do

				LOG_STATUS "Attempt #${vcsAttemptNumber} to retrieve OSDL."

				{

					# Remove any symbolic link coming from a previous attempt:
					${RM} -f ${latest_stable_osdl} 2>/dev/null

					# No https, no credential required:
					DEBUG "${SVN} export http://${OSDL_SVN_SERVER}:${SVN_URL} ${SVN_OPT}"
					${SVN} export http://${OSDL_SVN_SERVER}:${SVN_URL} ${SVN_OPT}


				} 1>>"$LOG_OUTPUT" 2>&1

				if [ $? -eq 0 ] ; then
					vcsAttemptNumber=$(($MAX_VCS_RETRY+1))
					success=0
				else
					vcsAttemptNumber=$(($vcsAttemptNumber+1))
					LOG_STATUS "SVN command failed."
					${SLEEP} 3
				fi
			done

			if [ $success -ne 0 ] ; then
				ERROR "Unable to retrieve OSDL from SVN after $MAX_VCS_RETRY attempts."
				exit 21
			else
				EXPORT_TARGET_DIR="osdl/OSDL"
				${MKDIR} -p ${EXPORT_TARGET_DIR}
				${MV} -f ${latest_stable_osdl} ${EXPORT_TARGET_DIR}/trunk
				${LN} -s ${EXPORT_TARGET_DIR}/trunk ${latest_stable_osdl}
				WARNING "Exported OSDL sources have been placed in faked trunk, in ${EXPORT_TARGET_DIR}/trunk."
			fi

		else
			DISPLAY "      ----> SVN retrieval disabled for OSDL."
		fi

		if [ $? -ne 0 ] ; then
			ERROR "Unable to retrieve OSDL from anonymous SVN."
			exit 21
		fi
	fi

	return 0
}


prepareOSDL()
{

	LOG_STATUS "Preparing OSDL..."
	printBeginList "OSDL       "

	printItem "extracting"

	if [ $no_vcs -eq 0 ] ; then
		echo
		WARNING "As the --noSVN option was used, build process stops here."
		exit 0
	fi


	if [ ${use_svn} -eq 1 ]; then

		# Here we use source archives:

		if findTool bunzip2 ; then
			BUNZIP2=$returnedString
		else
			ERROR "No bunzip2 tool found, whereas some files have to be bunzip2-ed."
			exit 14
		fi

		if findTool tar ; then
			TAR=$returnedString
		else
			ERROR "No tar tool found, whereas some files have to be detarred."
			exit 15
		fi

		cd $repository

		# Prevent archive from disappearing because of bunzip2.
		{
			${CP} -f ${OSDL_ARCHIVE} ${OSDL_ARCHIVE}.save && ${BUNZIP2} -f ${OSDL_ARCHIVE} && ${TAR} -xvf "osdl-${OSDL_VERSION}.tar"
		} 1>>"$LOG_OUTPUT" 2>&1


		if [ $? != 0 ] ; then
			ERROR "Unable to extract ${OSDL_ARCHIVE}."
			DEBUG "Restoring ${OSDL_ARCHIVE}."
			${MV} -f ${OSDL_ARCHIVE}.save ${OSDL_ARCHIVE}
			exit 10
		fi

	fi

	printOK

}


generateOSDL()
{

	LOG_STATUS "Generating OSDL..."


	# Premature generation of OSDL-environment.sh so that it is available
	# for autogen.sh:

	if [ -n "$prefix" ] ; then
		{

			OSDL_PREFIX="${prefix}/OSDL-${OSDL_VERSION}"

			echo "# OSDL section." >> ${OSDL_ENV_FILE}

			echo "OSDL_PREFIX=${OSDL_PREFIX}" >> ${OSDL_ENV_FILE}
			echo "export OSDL_PREFIX" >> ${OSDL_ENV_FILE}

			if [ $is_windows -eq 0 ] ; then

				echo "PATH=\$OSDL_PREFIX/lib:\${PATH}" >> ${OSDL_ENV_FILE}

				PATH=${OSDL_PREFIX}/lib:${PATH}
				export PATH

				echo "" >> ${OSDL_ENV_FILE}

			else

				echo "PATH=\$OSDL_PREFIX/bin:\${PATH}" >> ${OSDL_ENV_FILE}

				echo "LD_LIBRARY_PATH=\$OSDL_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

				echo "" >> ${OSDL_ENV_FILE}

				PATH=${OSDL_PREFIX}/bin:${PATH}
				export PATH

				LD_LIBRARY_PATH=${OSDL_PREFIX}/lib:${LD_LIBRARY_PATH}
				export LD_LIBRARY_PATH

			fi

			# OSDL_ENV_FILE finishes with the needed exports:
			echo "" >> ${OSDL_ENV_FILE}
			echo "export PATH" >> ${OSDL_ENV_FILE}
			echo "export LD_LIBRARY_PATH" >> ${OSDL_ENV_FILE}

			echo "" >> ${OSDL_ENV_FILE}
			echo "LOANI_REPOSITORY=${repository}" >> ${OSDL_ENV_FILE}
			echo "export LOANI_REPOSITORY" >> ${OSDL_ENV_FILE}

			echo "" >> ${OSDL_ENV_FILE}
			echo "LOANI_INSTALLATIONS=${prefix}" >> ${OSDL_ENV_FILE}
			echo "export LOANI_INSTALLATIONS" >> ${OSDL_ENV_FILE}

			echo "" >> ${OSDL_ENV_FILE}
			echo "# End of ${OSDL_ENV_FILE}." >> ${OSDL_ENV_FILE}


		} 1>>"$LOG_OUTPUT" 2>&1

	fi


	if [ ${use_svn} -eq 0 ]; then

		# Here we are in the SVN tree, needing to generate the build system:
		cd $repository/osdl/OSDL/trunk/src/conf/build

		if [ -n "$Ceylan_PREFIX" ] ; then
			{
				setBuildEnv ./autogen.sh --no-build --ceylan-install-prefix $Ceylan_PREFIX

			} 1>>"$LOG_OUTPUT" 2>&1
		else
			{
				setBuildEnv ./autogen.sh --no-build
			} 1>>"$LOG_OUTPUT" 2>&1
		fi

		if [ $? != 0 ] ; then
			echo
			ERROR "Unable to generate build system for OSDL (with autogen.sh)."
			exit 11
		fi

		# Going to the root of the source to continue the normal build process:
		cd $repository/osdl/OSDL/trunk

	else

		cd $repository/osdl-${OSDL_VERSION}
	fi


	# Rest of the build is common to autogen-based and release-based trees:


	printItem "configuring"

	if [ -n "$prefix" ] ; then
		{

			setBuildEnv --exportEnv --appendEnv ./configure --prefix=${OSDL_PREFIX} --with-ceylan-prefix=${Ceylan_PREFIX}
		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			setBuildEnv --exportEnv ./configure
		} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure OSDL."
		exit 11
	fi

	printOK

	printItem "building"

	{
		setBuildEnv ${MAKE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build OSDL."
		exit 12
	fi

	printOK

	printItem "installing"

	if [ -n "$prefix" ] ; then
		{

			# OSDL_ENV_FILE has already been updated here.

			setBuildEnv ${MAKE} install prefix=${OSDL_PREFIX}


		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			setBuildEnv ${MAKE} install
		} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to install OSDL."
		exit 13
	fi


	LOG_STATUS "Making tests for OSDL."


	cd test

	if [ ${use_svn} -eq 0 ]; then

		# Here we are in the SVN tree, needing to generate the build system for
		# tests:
		{
			setBuildEnv ./autogen.sh --no-build --with-osdl-env-file ${OSDL_ENV_FILE}
		} 1>>"$LOG_OUTPUT" 2>&1

		if [ $? != 0 ] ; then
			echo
			ERROR "Unable to generate build system for OSDL tests (with autogen.sh)."
			exit 10
		fi

	fi

	# Rest of the build is common to autogen-based and release-based trees:


	if [ -n "$prefix" ] ; then
		{

			# We suppose here that if we have a prefix, all tools use prefixes:

			tools_prefixes="--with-osdl-prefix=$OSDL_PREFIX --with-ceylan-prefix=$Ceylan_PREFIX --with-sdl-prefix=$SDL_PREFIX --with-libjpeg-prefix=$libjpeg_PREFIX --with-zlib-prefix=$zlib_PREFIX --with-libpng-prefix=$libpng_PREFIX --with-sdl_image-prefix=$SDL_image_PREFIX --with-sdl_gfx-prefix=$SDL_gfx_PREFIX --with-freetype-prefix=$freetype_PREFIX --with-sdl_ttf-prefix=$SDL_ttf_PREFIX --with-ogg=$libogg_PREFIX --with-vorbis=$libvorbis_PREFIX --with-sdl_mixer-prefix=$SDL_mixer_PREFIX --with-pcre-prefix=$PCRE_PREFIX --with-freeimage-prefix=$FreeImage_PREFIX --with-cegui-prefix=$CEGUI_PREFIX --with-physicsfs-prefix=$PhysicsFS_PREFIX"

			setBuildEnv --exportEnv --appendEnv ./configure --prefix=${OSDL_PREFIX} ${tools_prefixes}

		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			setBuildEnv --exportEnv --appendEnv ./configure
		} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure OSDL tests."
		exit 14
	fi

	{
		setBuildEnv ${MAKE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build OSDL tests."
		exit 15
	fi

	{
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install OSDL tests."
		exit 16
	fi

	{
		setBuildEnv ./playTests.sh
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to pass OSDL tests."
		exit 17
	fi

	cd ..


	LOG_STATUS "Making tools for OSDL."


	cd tools

	if [ ${use_svn} -eq 0 ]; then

		# Here we are in the SVN tree, needing to generate the build system for
		# tools:
		{
			setBuildEnv ./autogen.sh --no-build --with-osdl-env-file ${OSDL_ENV_FILE}
		} 1>>"$LOG_OUTPUT" 2>&1

		if [ $? != 0 ] ; then
			echo
			ERROR "Unable to generate build system for OSDL tools (with autogen.sh)."
			exit 10
		fi

	fi

	# Rest of the build is common to autogen-based and release-based trees:


	if [ -n "$prefix" ] ; then
		{

			# We suppose here that if we have a prefix, all tools use
			# prefixes:

			tools_prefixes="--with-osdl-prefix=$OSDL_PREFIX --with-ceylan-prefix=$Ceylan_PREFIX --with-sdl-prefix=$SDL_PREFIX --with-libjpeg-prefix=$libjpeg_PREFIX --with-zlib-prefix=$zlib_PREFIX --with-libpng-prefix=$libpng_PREFIX --with-sdl_image-prefix=$SDL_image_PREFIX --with-sdl_gfx-prefix=$SDL_gfx_PREFIX --with-freetype-prefix=$freetype_PREFIX --with-sdl_ttf-prefix=$SDL_ttf_PREFIX --with-ogg=$libogg_PREFIX --with-vorbis=$libvorbis_PREFIX --with-sdl_mixer-prefix=$SDL_mixer_PREFIX --with-libagar-prefix=$Agar_PREFIX --with-physicsfs-prefix=$PhysicsFS_PREFIX"

			setBuildEnv --exportEnv --appendEnv ./configure --prefix=${OSDL_PREFIX} ${tools_prefixes}

		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			setBuildEnv --exportEnv --appendEnv ./configure
		} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure OSDL tools."
		exit 14
	fi

	{
		setBuildEnv ${MAKE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build OSDL tools."
		exit 15
	fi

	{
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install OSDL tools."
		exit 16
	fi


	printOK

	printEndList

	LOG_STATUS "OSDL successfully installed."

	cd "$initial_dir"

}


cleanOSDL()
{
	LOG_STATUS "Cleaning OSDL build tree..."
	# Nothing to do: we want to be able to go on with the OSDL build.
}



################################################################################
# OSDL build thanks to Visual Express, with SVN.
################################################################################


getOSDL_win()
{
	LOG_STATUS "Getting OSDL for windows..."

	OSDL_win_ARCHIVE="from SVN"
	# Cygwin let us do the same:
	getOSDL
	return $?
}


prepareOSDL_win()
{

	LOG_STATUS "Preparing OSDL for windows.."

	osdl_install_dir="${alternate_prefix}/OSDL-${OSDL_win_VERSION}"
	${MKDIR} -p ${osdl_install_dir}

osdl_solution_dir="$repository/osdl/OSDL/trunk/src/conf/build/visual-express"

	# LOANI's version preferred to SVN's LOANI one for ease of debugging,
	# copies OSDL* (sln/vcproj/vsprops etc.) from LOANI to their place in SVN:
	${CP} -f "${WINDOWS_SOLUTIONS_ROOT}"/OSDL* ${osdl_solution_dir}
	if [ $? != 0 ] ; then
		ERROR "Unable to copy OSDL solution in build tree."
		exit 21
	fi

	printOK

}


generateOSDL_win()
{

	LOG_STATUS "Generating OSDL for windows..."

	if [ ${use_svn} -eq 0 ]; then

		# Here we are in the SVN tree:
		cd $repository/osdl/OSDL/trunk

	else

		cd $repository/OSDL-${OSDL_win_VERSION}
	fi


	printItem "configuring"
	printOK

	# LOANI's version preferred to SVN's one for ease of debugging:
	osdl_solution="${osdl_solution_dir}/OSDL-${OSDL_win_VERSION}.sln"

	printItem "building"
	GenerateWithVisualExpress OSDL ${osdl_solution}
	printOK

	printItem "installing"

	osdl_install_include_dir=${osdl_install_dir}/include
	${MKDIR} -p ${osdl_install_include_dir}
	${FIND} . -name 'OSDL*.h' -exec ${CP} -f '{}' ${osdl_install_include_dir} ';'

	printOK

	printEndList

	LOG_STATUS "OSDL successfully installed."

	cd "$repository"

}


cleanOSDL_win()
{
	LOG_STATUS "Cleaning OSDL build tree..."
	${RM} -rf "$repository/osdl*"
}







#TRACE "[loani-requiredTools] End"


################################################################################
# End of loani-requiredTools.sh
################################################################################
