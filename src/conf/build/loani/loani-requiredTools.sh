# This script is made to be sourced by LOANI when retrieving required tools.
# Therefore, all tools managed here should be strict LOANI pre-requesites.

# Creation date : 2004, February 22.
# Author : Olivier Boudeville (olivier.boudeville@online.fr)


####################################################################################################
# CVS tags to select versions to retrieve (if use_current_cvs not selected) :
latest_stable_ceylan="STABLE_20051107"
latest_stable_osdl="STABLE_20051107"

####################################################################################################



# Required tools section.


if [ "$is_windows" -eq "1" ] ; then     
        # All non-windows platforms should build everything from sources :    
        REQUIRED_TOOLS="libtool SDL libjpeg zlib libpng SDL_image SDL_gfx freetype SDL_ttf libogg libvorbis SDL_mixer Ceylan OSDL"		
else
        # Windows special case :
        REQUIRED_TOOLS="libtool SDL libjpeg SDL_image_win_precompiled zlib libpng SDL_image win_pthread"
		
		# Tools whose build is still to fix for Windows platform :
		REMAINING_TOOLS="SDL_gfx freetype SDL_ttf libogg libvorbis SDL_mixer Ceylan OSDL"
fi        

# Maybe libmikmod should be added for SDL_mixer, if MOD music was to be supported.


# TIFF library was removed from the list, since its build is a nonsense and that image format 
# is not compulsory : PNG and JPEG are better choices and should be used instead.


if [ "$is_windows" -eq "0" ] ; then
        SDL_PREFIX=`cygpath -w ${prefix}/SDL-${SDL_VERSION} | ${SED} 's|\\\|/|g'`
else
        SDL_PREFIX="${prefix}/SDL-${SDL_VERSION}"
fi

DEBUG "Setting SDL_PREFIX to <$SDL_PREFIX>."

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




####################################################################################################
# SDL
####################################################################################################


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
		${CP} -f ${SDL_ARCHIVE} ${SDL_ARCHIVE}.save && ${GUNZIP} -f ${SDL_ARCHIVE} && tar -xvf "SDL-${SDL_VERSION}.tar" 
	} 1>>"$LOG_OUTPUT" 2>&1
	
		
	if [ $? != "0" ] ; then
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
		${MKDIR} -p ${prefix}/SDL-${SDL_VERSION}
		setBuildEnv ./configure --prefix=${prefix}/SDL-${SDL_VERSION} --exec-prefix=${prefix}/SDL-${SDL_VERSION}
			
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{			
		setBuildEnv ./configure			
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi
	
		
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to configure SDL."
		exit 11
	fi	
	
	printOK	
	
	printItem "building"
	
	{
	
		 setBuildEnv ${MAKE} LDFLAGS="-lgcc_s"
		 
	} 1>>"$LOG_OUTPUT" 2>&1	 
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to build SDL."
		exit 12
	fi

	printOK


	printItem "installing"
	
	if [ -n "$prefix" ] ; then	
	{				
	
		echo "# SDL section." >> ${OSDL_ENV_FILE}
		
		echo "PATH=${prefix}/SDL-${SDL_VERSION}/bin:\${PATH}" >> ${OSDL_ENV_FILE}
		echo "export PATH" >> ${OSDL_ENV_FILE}
		
		echo "LD_LIBRARY_PATH=${prefix}/SDL-${SDL_VERSION}/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
		echo "export LD_LIBRARY_PATH" >> ${OSDL_ENV_FILE}
		
		PATH=${prefix}/SDL-${SDL_VERSION}/bin:${PATH}
		export PATH
		
		LD_LIBRARY_PATH=${prefix}/SDL-${SDL_VERSION}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH
		
		if [ "$is_windows" -eq "0" ] ; then
			# Always remember that, on Windows, DLL are searched through the PATH, not the LD_LIBRARY_PATH.
			
			PATH=${prefix}/SDL-${SDL_VERSION}/lib:${PATH}	
			export PATH
				
			echo "PATH=:${prefix}/SDL-${SDL_VERSION}/lib:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "export PATH" >> ${OSDL_ENV_FILE}
		fi
		
		echo "" >> ${OSDL_ENV_FILE}
				
        LIBPATH="-L${prefix}/SDL-${SDL_VERSION}/lib"
        
		# Don't ever imagine that to avoid bad nedit syntax highlighting you could change :
		# include/*.h to "include/*.h" in next line.
		# It would fail at runtime with "include/*.h" not found...
		
		setBuildEnv ${MAKE} install && ${CP} -f include/*.h ${prefix}/SDL-${SDL_VERSION}/include/SDL

	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{		
		setBuildEnv ${MAKE} install 
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi
	
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to install SDL."
		exit 13
	fi	
	
	if [ "$is_windows" -eq "0" ] ; then
    	${MV} -f ${prefix}/SDL-${SDL_VERSION}/bin/*.dll ${prefix}/SDL-${SDL_VERSION}/lib
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


# Pre requesites of SDL_image : JPEG library, PNG library, zlib library.
# TIFF support is disabled for the moment.




####################################################################################################
# JPEG library
####################################################################################################



# This won't rightly compile on windows since this ltconfig does not support cygwin or mingw.
# To overcome this issue, prebuilt binaries will be installed instead.
# In the future, one might create its own Makefile for JPEG and maybe make use of libtool 1.5.2 
# to directly generate the DLL without the configure nightmare. 
# Maybe one could be inspired by the pure cygwin makefile.

# However, even on Windows, the building of the static library is still maintained, since 
# one of its byproducts is a generated header file which is needed by SDL_image's own 
# building.


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
		${CP} -f ${libjpeg_ARCHIVE} ${libjpeg_ARCHIVE}.save && ${GUNZIP} -f ${libjpeg_ARCHIVE} && tar -xvf "jpegsrc.v${libjpeg_VERSION}.tar" 
	} 1>>"$LOG_OUTPUT" 2>&1
	
		
	if [ $? != "0" ] ; then
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
	# used without jconfig.h which ... is only generated by the build process ! 
        
	printItem "configuring"
	
    # Non windows : standard way of building.
                
	# On some platforms, libtool is unable to guess the correct host type :
	# config.guess fails to detect anything as soon as --enable-shared is added.
	# One attempt is being made in case of failure, to test whether it cannot 
	# be the most common platform used for OSDL by far, i686-pc-linux-gnu.
	
	if ! ./ltconfig ltmain.sh 1>>"$LOG_OUTPUT" 2>&1 ; then 
	
	        LOG_STATUS "ltconfig host detection will fail in the configure step."
	
                if [ "$is_linux" -eq "0" ] ; then
					WORK_AROUND_PLATFORM=i686-pc-linux-gnu        
		        	LOG_STATUS "Linux detected, trying workaround of most common platform (${WORK_AROUND_PLATFORM})."
		        	${CAT} configure | ${SED} "s|ltmain.sh$|ltmain.sh ${WORK_AROUND_PLATFORM}|1" > configure.tmp
					${MV} -f configure.tmp configure
		        	${CHMOD} +x configure
				else
		        	echo
		        	ERROR "Unable to pre-configure libjpeg, host detection failed and there is no host work-around for your platform."
					exit 11		
				fi
	fi	
	
	if [ -n "$prefix" ] ; then	
	{		
                        
		if [ "$use_mingw" -eq "0" ] ; then
			OLD_LD=$LD  
			LD=${MINGW_PATH}/ld						 
			export LD					 
			DEBUG "Selected LD is $LD."
	   
			if [ ! -x "$LD" ] ; then
				ERROR "No executable ld found (tried $LD)."
				exit 14
			fi

		fi
                        
		setBuildEnv --exportEnv ./configure --prefix=${prefix}/jpeg-${libjpeg_VERSION} --exec-prefix=${prefix}/jpeg-${libjpeg_VERSION} --enable-shared
                 
	 } 1>>"$LOG_OUTPUT" 2>&1		
	 else
	 {		                      
		setBuildEnv ./configure --enable-shared
	 } 1>>"$LOG_OUTPUT" 2>&1			
	 fi
	
	 if [ $? != "0" ] ; then
	        echo
	        ERROR "Unable to configure libjpeg."
	        exit 12
	 fi
                
        
	printOK	

        
	printItem "building"
	
	# On Windows (Cygwin/MinGW), the configure step will pretend 
	# shared libraries cannot be generated :
	# checking for ld used by GCC... no
	# ltconfig: error: no acceptable ld found in $PATH
	# so jpeg dll won't be created. 
	# Nevertheless some other generated files (ex : jconfig.h)
	# will be needed to build libraries using JPEG, such as SDL_image : 
	# it remains useful.
        
	{
		setBuildEnv ${MAKE} LDFLAGS="-lgcc_s"
	} 1>>"$LOG_OUTPUT" 2>&1	 
	
 	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to build libjpeg."
		exit 12
	fi
        
	printOK


	printItem "installing"
	
	if [ -n "$prefix" ] ; then	
	{		
	
		LIBFLAG="-L${prefix}/jpeg-${libjpeg_VERSION}/lib ${LIBFLAG}"
		
		echo "# libjpeg section." >> ${OSDL_ENV_FILE}
		
		echo "LD_LIBRARY_PATH=${prefix}/jpeg-${libjpeg_VERSION}/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
		echo "export LD_LIBRARY_PATH" >> ${OSDL_ENV_FILE}
		
		# In order SDL_image configure does not fail :
		LD_LIBRARY_PATH=${prefix}/jpeg-${libjpeg_VERSION}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH
		
		if [ "$is_windows" -eq "0" ] ; then
			# Always remember that, on Windows, DLL are searched through the PATH, not the LD_LIBRARY_PATH.
			
			PATH=${prefix}/jpeg-${libjpeg_VERSION}/lib:${PATH}	
			export PATH
				
			echo "PATH=${prefix}/jpeg-${libjpeg_VERSION}/lib:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "export PATH"
			
		fi

		echo "" >> ${OSDL_ENV_FILE}
	
		
		${MKDIR} -p ${prefix}/jpeg-${libjpeg_VERSION}/include		
		${MKDIR} -p ${prefix}/jpeg-${libjpeg_VERSION}/bin
		${MKDIR} -p ${prefix}/jpeg-${libjpeg_VERSION}/lib
		${MKDIR} -p ${prefix}/jpeg-${libjpeg_VERSION}/man/man1
                
        setBuildEnv ${MAKE} install prefix=${prefix}/jpeg-${libjpeg_VERSION}
                   
                
	    if [ $? != "0" ] ; then
			ERROR "Unable to install libjpeg."
			exit 13
		fi	
                
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{	
			setBuildEnv ${MAKE} install 
 
	        if [ $? != "0" ] ; then
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





####################################################################################################
# SDL_image_*_precompiled targets.
####################################################################################################



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
	
 
	if [ $? != "0" ] ; then
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
	
	# Nothing to do !
	
	printOK	

	printItem "building"
	
	# Nothing to do !

	printOK


	printItem "installing"
	
	if [ -n "$prefix" ] ; then	
	{		
	
		# Let us suppose the precompiled version has for version libjpeg_VERSION.

		# The libjpeg target should already have :
		#       - created ${prefix}/jpeg-${libjpeg_VERSION}/lib
		#       - defined LIBFLAG="-L${prefix}/jpeg-${libjpeg_VERSION}/lib ${LIBFLAG}"
		#       - added the libjpeg section." in ${OSDL_ENV_FILE}
		#       - updated LD_LIBRARY_PATH *and* PATH with ${prefix}/jpeg-${libjpeg_VERSION}/lib
		
		${CP} -f lib/jpeg.dll ${prefix}/jpeg-${libjpeg_VERSION}/lib
                	
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{		
		if [ "$is_windows" -eq "1" ] ; then	
			setBuildEnv ${MAKE} install 
		else
			DEBUG "Using ${windows_dll_location} as target Windows DLL location." 
			${CP} -f lib/jpeg.dll ${windows_dll_location}
		fi
  
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi

	if [ $? != "0" ] ; then
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





####################################################################################################
# zlib library
####################################################################################################



getzlib()
{
	LOG_STATUS "Getting zlib library..."
	launchFileRetrieval zlib
	return $?
}


preparezlib()
{

	LOG_STATUS "Preparing zlib library..."

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
	
	printBeginList "libzlib    "
		
	printItem "extracting"
	
	cd $repository
	
	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${zlib_ARCHIVE} ${zlib_ARCHIVE}.save && ${GUNZIP} -f ${zlib_ARCHIVE} && tar -xvf "zlib-${zlib_VERSION}.tar" 
	} 1>>"$LOG_OUTPUT" 2>&1
	
		
	if [ $? != "0" ] ; then
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
	
	# Not all platforms have symbolic links :
	${MV} -f "zlib-${zlib_VERSION}" zlib
	cd zlib
	
	printItem "configuring"

	if [ -n "$prefix" ] ; then	
	{		
		setBuildEnv ./configure --shared --prefix=${prefix}/zlib-${zlib_VERSION} --exec_prefix=${prefix}/zlib-${zlib_VERSION}
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{		
		setBuildEnv ./configure --shared 
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi
	if [ $? != "0" ] ; then
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
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to build zlib."
		exit 12
	fi

	printOK


	printItem "installing"
	
	if [ -n "$prefix" ] ; then	
	{		
		LIBFLAG="-L${prefix}/zlib-${zlib_VERSION}/lib ${LIBFLAG}"
		
		echo "# zlib section." >> ${OSDL_ENV_FILE}
		
		echo "LD_LIBRARY_PATH=${prefix}/zlib-${zlib_VERSION}/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
		echo "export LD_LIBRARY_PATH" >> ${OSDL_ENV_FILE}

		# In order SDL_image configure does not fail :
		LD_LIBRARY_PATH=${prefix}/zlib-${zlib_VERSION}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH
		
		if [ "$is_windows" -eq "0" ] ; then
			# Always remember that, on Windows, DLL are searched through the PATH, not the LD_LIBRARY_PATH.
			
			PATH=${prefix}/zlib-${zlib_VERSION}/lib:${PATH}	
			export PATH
			
			echo "PATH=${prefix}/zlib-${zlib_VERSION}/lib:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "export PATH" >> ${OSDL_ENV_FILE}
			
		fi

		echo "" >> ${OSDL_ENV_FILE}	
	
		setBuildEnv ${MAKE} install prefix=${prefix}/zlib-${zlib_VERSION} 
		
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{		
		setBuildEnv ${MAKE} install 
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi
			
	if [ $? != "0" ] ; then
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




####################################################################################################
# PNG library
####################################################################################################


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
		${CP} -f ${libpng_ARCHIVE} ${libpng_ARCHIVE}.save && ${BUNZIP2} -f ${libpng_ARCHIVE} && tar -xvf "libpng-${libpng_VERSION}.tar" 
	} 1>>"$LOG_OUTPUT" 2>&1
	
		
	if [ $? != "0" ] ; then
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
	
	# Not all platforms support symbolic links :
	${MV} -f "libpng-${libpng_VERSION}" libpng
	cd libpng

	printItem "configuring"
		
		
	if [ "$is_linux" -eq 0 ] ; then
		${CP} -f scripts/makefile.linux makefile
	fi
	
	if [ "$is_pure_cygwin" -eq 0 ] ; then
		${CP} -f scripts/makefile.cygwin makefile
	fi

	if [ "$is_cygwin_mingw" -eq 0 ] ; then
		${CP} -f scripts/makefile.cygwin makefile
	fi

	if [ "$is_msys" -eq 0 ] ; then
		WARNING "Assuming cygwin settings for libpng for this Windows MSYS/Mingw platform."
		${CP} -f scripts/makefile.cygwin makefile	
	fi
	
	if [ "$is_macosx" -eq 0 ] ; then
		${CP} -f scripts/makefile.macosx makefile
	fi
		
	if [ ! -f "makefile" ] ; then
		ERROR "Your platform does not seem to be supported by LOANI. If you want to continue nevertheless, you must select in "`pwd`"/scripts the makefile.XXX where XXX matches your platform, and copy it in "`pwd`" : cd "`pwd`"; cp -f scripts/makefile.XXX makefile"
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
		if [ "$use_mingw" -eq "0" ] ; then
			# zLib Library version could be used.
			setBuildEnv ${MAKE} ${BUILD_LOCATIONS} prefix=${prefix}/PNG-${libpng_VERSION} MINGW_CCFLAGS=${MINGW_CFLAGS} MINGW_LDFLAGS=${MINGW_LFLAGS} SHAREDLIB=libpng${PNG_NUMBER}.dll ZLIBINC=../zlib ZLIBLIB=../zlib 
		else
			# LDFLAGS="-lgcc_s" not added.
			setBuildEnv ${MAKE} ${BUILD_LOCATIONS} prefix=${prefix}/PNG-${libpng_VERSION}
		fi
 
	} 1>>"$LOG_OUTPUT" 2>&1	 
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to build libpng."
		exit 12
	fi

	printOK


	printItem "installing"
	
	if [ -n "$prefix" ] ; then	
	{	
		LIBFLAG="-L${prefix}/PNG-${libpng_VERSION}/lib ${LIBFLAG}"	
			
		echo "# libpng section." >> ${OSDL_ENV_FILE}
		
		echo "LD_LIBRARY_PATH=${prefix}/PNG-${libpng_VERSION}/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
		echo "export LD_LIBRARY_PATH" >> ${OSDL_ENV_FILE}
	
		# In order SDL_image configure does not fail :
		LD_LIBRARY_PATH=${prefix}/PNG-${libpng_VERSION}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		if [ "$is_windows" -eq "0" ] ; then
			# Always remember that, on Windows, DLL are searched through the PATH, not the LD_LIBRARY_PATH.
			PATH=${prefix}/PNG-${libpng_VERSION}/lib:${PATH}	
			export PATH	
			
			echo "PATH=${prefix}/PNG-${libpng_VERSION}/lib:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "export PATH" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}	
				
		${MKDIR} -p ${prefix}/PNG-${libpng_VERSION}/include
		${MKDIR} -p ${prefix}/PNG-${libpng_VERSION}/lib
		${MKDIR} -p ${prefix}/PNG-${libpng_VERSION}/man
		${MKDIR} -p ${prefix}/PNG-${libpng_VERSION}/bin	
		                
		if [ "$use_mingw" -eq "0" ] ; then
			PNG_SHARED_LIB=libpng${PNG_NUMBER}.dll
			setBuildEnv ${MAKE} install prefix=${prefix}/PNG-${libpng_VERSION} SHAREDLIB=${PNG_SHARED_LIB}
			${CP} -f ${prefix}/PNG-${libpng_VERSION}/bin/${PNG_SHARED_LIB} ${prefix}/PNG-${libpng_VERSION}/lib/libpng.dll
			${MV} -f ${prefix}/PNG-${libpng_VERSION}/include/libpng${PNG_NUMBER}/* ${prefix}/PNG-${libpng_VERSION}/include
		else
			setBuildEnv ${MAKE} install prefix=${prefix}/PNG-${libpng_VERSION}
		fi
			
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{		
		setBuildEnv ${MAKE} install 
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi
	
	if [ $? != "0" ] ; then
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




####################################################################################################
# TIFF library (disabled for the moment)
####################################################################################################


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
		${CP} -f ${libtiff_ARCHIVE} ${libtiff_ARCHIVE}.save && ${GUNZIP} -f ${libtiff_ARCHIVE} && tar -xvf "tiff-v${libtiff_VERSION}-tar" 
	} 1>>"$LOG_OUTPUT" 2>&1
	
		
	if [ $? != "0" ] ; then
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
	
	#################################################################################
	# Not gone any further, since tiff support is not required and
	# the makefile generated from ./configure --noninteractive is totally unusable.
	ERROR "TIFF support not implemented yet."
	exit 1
	#################################################################################
	
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
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to build libtiff."
		exit 12
	fi

	printOK


	printItem "installing"
	
	if [ -n "$prefix" ] ; then	
	{	
		${MKDIR} -p ${prefix}/TIFF-${libtiff_VERSION}/include
		${MKDIR} -p ${prefix}/TIFF-${libtiff_VERSION}/lib
		${MKDIR} -p ${prefix}/TIFF-${libtiff_VERSION}/man
		${MKDIR} -p ${prefix}/TIFF-${libtiff_VERSION}/bin	
		${MAKE} install prefix=${prefix}/TIFF-${libtiff_VERSION}
			
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{		
			${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi
	
	if [ $? != "0" ] ; then
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




####################################################################################################
# SDL_image itself
####################################################################################################



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
		${CP} -f ${SDL_image_ARCHIVE} ${SDL_image_ARCHIVE}.save && ${GUNZIP} -f ${SDL_image_ARCHIVE} && tar -xvf "SDL_image-${SDL_image_VERSION}.tar" 
	} 1>>"$LOG_OUTPUT" 2>&1
	
		
	if [ $? != "0" ] ; then
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

	if [ "$is_windows" -eq "0" ] ; then	
		DEBUG "Correcting sdl-config for SDL_image."
		
        sdl_config=${prefix}/SDL-${SDL_VERSION}/bin/sdl-config
        
        DEBUG "Correcting ${sdl_config}"
        prefix_one=`cygpath -w ${prefix}/SDL-${SDL_VERSION} | ${SED} 's|\\\|/|g'`
        prefix_two=`cygpath -w ${prefix}/SDL-${SDL_VERSION} | ${SED} 's|\\\|/|g'`

        ${CAT} ${sdl_config} | ${SED} "s|^prefix=.*$|prefix=$prefix_one|1" > sdl-config.tmp && ${CAT} sdl-config.tmp | ${SED} "s|^exec_prefix=.*$|exec_prefix=$prefix_two|1" > sdl-config.tmp2 && ${RM} -f ${sdl_config} sdl-config.tmp && ${MV} -f sdl-config.tmp2 ${sdl_config}
        
		if [ "$?" != "0" ] ; then
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
	
		# Debug purpose (should be set from other targets) :
				
		LIBFLAG="-L${prefix}/SDL-${SDL_VERSION}/lib"
		LIBFLAG="-L${prefix}/jpeg-${libjpeg_VERSION}/lib ${LIBFLAG}"
		LIBFLAG="-L${prefix}/zlib-${zlib_VERSION}/lib ${LIBFLAG}"
		LIBFLAG="-L${prefix}/PNG-${libpng_VERSION}/lib ${LIBFLAG}"

		if [ "$is_windows" -eq "0" ] ; then

			LIBFLAG="-L`cygpath -w ${prefix}/SDL-${SDL_VERSION}/lib`"
			LIBFLAG="-L`cygpath -w ${prefix}/jpeg-${libjpeg_VERSION}/lib ${LIBFLAG}`"
			LIBFLAG="-L`cygpath -w ${prefix}/zlib-${zlib_VERSION}/lib ${LIBFLAG}`"
			LIBFLAG="-L`cygpath -w ${prefix}/PNG-${libpng_VERSION}/lib ${LIBFLAG}`"

			LIBFLAG=`echo $LIBFLAG | ${SED} 's|\\\|/|g'`
			
			# DLL are searched from PATH on Windows.
			
		    PATH=${prefix}/SDL-${SDL_VERSION}/lib:${PATH}			
			PATH=${prefix}/jpeg-${libjpeg_VERSION}/lib:${PATH}
			PATH=${prefix}/zlib-${zlib_VERSION}/lib:${PATH}			
			PATH=${prefix}/PNG-${libpng_VERSION}/lib:${PATH}
			
			export PATH
	
		fi

		PATH=${prefix}/SDL-${SDL_VERSION}/bin:${PATH}
		export PATH
		
		LD_LIBRARY_PATH=${prefix}/SDL-${SDL_VERSION}/lib:${LD_LIBRARY_PATH}
		LD_LIBRARY_PATH=${prefix}/jpeg-${libjpeg_VERSION}/lib:${LD_LIBRARY_PATH}				
		LD_LIBRARY_PATH=${prefix}/zlib-${zlib_VERSION}/lib:${LD_LIBRARY_PATH}		 
		LD_LIBRARY_PATH=${prefix}/PNG-${libpng_VERSION}/lib:${LD_LIBRARY_PATH}   
		             
		export LD_LIBRARY_PATH           

							
		LOG_STATUS "PATH for SDL_image configure is <${PATH}>."
		LOG_STATUS "LD_LIBRARY_PATH for SDL_image configure is <${LD_LIBRARY_PATH}>."
 
		if [ "$use_mingw" -eq "0" ] ; then
        
        	LOG_STATUS "Using SDL prefix $SDL_PREFIX for SDL_image."
			
			OLD_LDFLAGS=${LDFLAGS}
			
        	LDFLAGS=${LIBFLAG}
        	export LDFLAGS
			
			setBuildEnv ./configure --with-sdl-prefix=${SDL_PREFIX} 
			
			LDFLAGS=${OLD_LDFLAGS}
			export LDFLAGS
			
		else
		
			# --disable-sdltest added since configure tries to compile a test without letting
			# the system libraries locations to be redefined. Therefore a wrong libstdc++.so
			# could be chosen, leading to errors such as : 
			# "undefined reference to `_Unwind_Resume_or_Rethrow@GCC_3.3'"
			
	  		setBuildEnv ./configure --with-sdl-prefix=${prefix}/SDL-${SDL_VERSION} --disable-sdltest             
		fi
 
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{		
		setBuildEnv ./configure
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to configure SDL_image."
		exit 11
	fi	
	
	printOK	
	
	printItem "building"
		
	if [ -n "$prefix" ] ; then	
	{
            
		#setBuildEnv ${MAKE} "CFLAGS=-O2 -I${SDL_PREFIX}/include/SDL -D_REENTRANT -DLOAD_BMP -DLOAD_GIF -DLOAD_LBM -DLOAD_PCX -DLOAD_PNM -DLOAD_TGA -DLOAD_XPM -DLOAD_JPG -DLOAD_PNG" "LDFLAGS=${LIBFLAG} -ljpeg -lpng -lz" "IMG_LIBS=-ljpeg -lpng -lz"
		JPEG_INC=`cygpath -w ${prefix}/jpeg-${libjpeg_VERSION}/include | ${SED} 's|\\\|/|g'`
		PNG_INC=`cygpath -w ${prefix}/PNG-${libpng_VERSION}/include | ${SED} 's|\\\|/|g'`
		ZLIB_INC=`cygpath -w ${prefix}/zlib-${zlib_VERSION}/include | ${SED} 's|\\\|/|g'`
                
		setBuildEnv ${MAKE} LDFLAGS="-lgcc_s" INCLUDES="-I${JPEG_INC} -I${PNG_INC} -I${PNG_INC}/ -I${ZLIB_INC}" 	
                
	} 1>>"$LOG_OUTPUT" 2>&1	
	else
	{
		setBuildEnv ${MAKE} LDFLAGS="-lgcc_s"

	} 1>>"$LOG_OUTPUT" 2>&1	 
	fi
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to build SDL_image."
		exit 12
	fi

	printOK


	printItem "installing"
	
	if [ -n "$prefix" ] ; then	
	{		
		echo "# SDL_image section." >> ${OSDL_ENV_FILE}
		
		echo "LD_LIBRARY_PATH=${prefix}/SDL_image-${SDL_image_VERSION}/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
		echo "export LD_LIBRARY_PATH" >> ${OSDL_ENV_FILE}

		LD_LIBRARY_PATH=${prefix}/SDL_image-${SDL_image_VERSION}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH
		
		if [ "$is_windows" -eq "0" ] ; then
			# Always remember that, on Windows, DLL are searched through the PATH, not the LD_LIBRARY_PATH.
			PATH=${prefix}/SDL_image-${SDL_image_VERSION}/lib:${PATH}	
			export PATH
				
			echo "PATH=${prefix}/SDL_image-${SDL_image_VERSION}/lib:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "export PATH" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}	
		
		${MKDIR} -p ${prefix}/SDL_image-${SDL_image_VERSION}
			
		setBuildEnv ${MAKE} install prefix=${prefix}/SDL_image-${SDL_image_VERSION}  
		
		if [ $? != "0" ] ; then
			echo
			ERROR "Unable to install SDL_image."
			exit 13
		fi	
		
		# Rename 'libSDL_image.la', to prevent libtool from detecting it when linking OSDL and 
		# issuing very annoying messages twice, such as :
		# "libtool: link: warning: library `[...]/libSDL_image.la' was moved."
		
		${MV} -f ${prefix}/SDL_image-${SDL_image_VERSION}/lib/libSDL_image.la ${prefix}/SDL_image-${SDL_image_VERSION}/lib/libSDL_image.la-hidden-by-LOANI

		if [ $? != "0" ] ; then
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
	
	
	if [ $? != "0" ] ; then
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




####################################################################################################
# libogg (libogg)
####################################################################################################


getlibogg()
{
	LOG_STATUS "Getting libogg..."
	launchFileRetrieval libogg
	return $?
}


preparelibogg()
{

	LOG_STATUS "Preparing libogg..."
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
	
	printBeginList "libogg     "

	printItem "extracting"
	
	cd $repository
	
	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${libogg_ARCHIVE} ${libogg_ARCHIVE}.save && ${GUNZIP} -f ${libogg_ARCHIVE} && tar -xvf "libogg-${libogg_VERSION}.tar" 
	} 1>>"$LOG_OUTPUT" 2>&1
	
		
	if [ $? != "0" ] ; then
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
	
		setBuildEnv ./configure --prefix=${prefix}/libogg-${libogg_VERSION} --exec-prefix=${prefix}/libogg-${libogg_VERSION}
 
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{		
		setBuildEnv ./configure
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi
	
	if [ $? != "0" ] ; then
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
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to build libogg."
		exit 12
	fi

	printOK


	printItem "installing"
	
	if [ -n "$prefix" ] ; then	
	{		
		echo "# libogg section." >> ${OSDL_ENV_FILE}
		
		echo "LD_LIBRARY_PATH=${prefix}/libogg-${libogg_VERSION}/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
		echo "export LD_LIBRARY_PATH" >> ${OSDL_ENV_FILE}

		LD_LIBRARY_PATH=${prefix}/libogg-${libogg_VERSION}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH
		
		if [ "$is_windows" -eq "0" ] ; then
		
			# Always remember that, on Windows, DLL are searched through the PATH, not the LD_LIBRARY_PATH.
			
			PATH=${prefix}/libogg-${libogg_VERSION}/lib:${PATH}	
			export PATH
			
			echo "PATH=${prefix}/libogg-${libogg_VERSION}/lib:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "export PATH" >> ${OSDL_ENV_FILE}
			
		fi

		echo "" >> ${OSDL_ENV_FILE}	
		
		${MKDIR} -p ${prefix}/libogg-${libogg_VERSION}
			
		setBuildEnv ${MAKE} install prefix=${prefix}/libogg-${libogg_VERSION}  
		
		if [ $? != "0" ] ; then
			echo
			ERROR "Unable to install libogg."
			exit 13
		fi	
		
		# Rename 'libogg.la', to prevent libtool from detecting it when linking OSDL and 
		# issuing very annoying messages twice, such as :
		# "libtool: link: warning: library `[...]/libogg.la' was moved."
		
		# Disabled since it would prevent SDL_mixer build :
		#${MV} -f ${prefix}/libogg-${libogg_VERSION}/lib/libogg.la ${prefix}/libogg-${libogg_VERSION}/lib/libogg.la-hidden-by-LOANI
		#
		#if [ $? != "0" ] ; then
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
	
	
	if [ $? != "0" ] ; then
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





####################################################################################################
# libvorbis (libvorbis)
####################################################################################################



getlibvorbis()
{
	LOG_STATUS "Getting libvorbis..."
	launchFileRetrieval libvorbis
	return $?
}


preparelibvorbis()
{

	LOG_STATUS "Preparing libvorbis..."
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
	
	printBeginList "libvorbis  "

	printItem "extracting"
	
	cd $repository
	
	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${libvorbis_ARCHIVE} ${libvorbis_ARCHIVE}.save && ${GUNZIP} -f ${libvorbis_ARCHIVE} && tar -xvf "libvorbis-${libvorbis_VERSION}.tar" 
	} 1>>"$LOG_OUTPUT" 2>&1
	
		
	if [ $? != "0" ] ; then
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
	
		setBuildEnv ./configure --prefix=${prefix}/libvorbis-${libvorbis_VERSION} --exec-prefix=${prefix}/libvorbis-${libvorbis_VERSION} --with-libogg=${prefix}/libogg-${libogg_VERSION}
 
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{		
		setBuildEnv ./configure
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi
	
	if [ $? != "0" ] ; then
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
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to build libvorbis."
		exit 12
	fi

	printOK


	printItem "installing"
	
	if [ -n "$prefix" ] ; then	
	{		
		echo "# libvorbis section." >> ${OSDL_ENV_FILE}
		
		echo "LD_LIBRARY_PATH=${prefix}/libvorbis-${libvorbis_VERSION}/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
		echo "export LD_LIBRARY_PATH" >> ${OSDL_ENV_FILE}

		LD_LIBRARY_PATH=${prefix}/libvorbis-${libvorbis_VERSION}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH
		
		if [ "$is_windows" -eq "0" ] ; then
			# Always remember that, on Windows, DLL are searched through the PATH, not the LD_LIBRARY_PATH.
			
			PATH=${prefix}/libvorbis-${libvorbis_VERSION}/lib:${PATH}	
			export PATH
				
			echo "PATH=${prefix}/libvorbis-${libvorbis_VERSION}/lib:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "export PATH" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}	
		
		${MKDIR} -p ${prefix}/libvorbis-${libvorbis_VERSION}
			
		setBuildEnv ${MAKE} install prefix=${prefix}/libvorbis-${libvorbis_VERSION}  
		
		if [ $? != "0" ] ; then
			echo
			ERROR "Unable to install libvorbis."
			exit 13
		fi	
		
		# Rename 'libvorbis.la', to prevent libtool from detecting it when linking OSDL and 
		# issuing very annoying messages twice, such as :
		# "libtool: link: warning: library `[...]/libvorbis.la' was moved."
		
		# Disabled since would prevent SDL_mixer build :
		#${MV} -f ${prefix}/libvorbis-${libvorbis_VERSION}/lib/libvorbis.la ${prefix}/libvorbis-${libvorbis_VERSION}/lib/libvorbis.la-hidden-by-LOANI
		#
		#if [ $? != "0" ] ; then
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
	
	
	if [ $? != "0" ] ; then
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





####################################################################################################
# SDL_mixer
####################################################################################################


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
		${CP} -f ${SDL_mixer_ARCHIVE} ${SDL_mixer_ARCHIVE}.save && ${GUNZIP} -f ${SDL_mixer_ARCHIVE} && tar -xvf "SDL_mixer-${SDL_mixer_VERSION}.tar" 
	} 1>>"$LOG_OUTPUT" 2>&1
	
		
	if [ $? != "0" ] ; then
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
	
		#LDFLAGS="-L${prefix}/libogg-${libogg_VERSION}/lib -L${prefix}/libvorbis-${libvorbis_VERSION}/lib ${LDFLAGS}"
		#export LDFLAGS
		
		#CFLAGS="-I${prefix}/libogg-${libogg_VERSION}/include -I${prefix}/libvorbis-${libvorbis_VERSION}/include"
		#export CFLAGS
		
		# Following features are deactivated automatically (their companion library is not installed
		# by LOANI yet, and they are currently considered as less useful than the ones of the core
		# selection) : 
		#  - MOD support (including libmikmod), 
		#  - MIDI support (including native and timidity), 
		#  - MP3 support (including SMPEG) 
		#
		# The recommended sound encodings are : 
		#  - WAVE (for short samples)
		#  - OggVorbis (for longer ones, including music).
		#
		setBuildEnv ./configure --prefix=${prefix}/SDL_mixer-${SDL_mixer_VERSION} --exec-prefix=${prefix}/SDL_mixer-${SDL_mixer_VERSION} -with-sdl-prefix=${prefix}/SDL-${SDL_VERSION}  --disable-static --disable-music-libmikmod --disable-music-mod --disable-music-midi --disable-music-timidity-midi --disable-music-native-midi --disable-music-native-midi-gpl --disable-music-mp3 --disable-smpegtest --enable-music-wave --enable-music-libogg LDFLAGS="-L${prefix}/libogg-${libogg_VERSION}/lib -L${prefix}/libvorbis-${libvorbis_VERSION}/lib" CFLAGS="-I${prefix}/libogg-${libogg_VERSION}/include -I${prefix}/libvorbis-${libvorbis_VERSION}/include"
 
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{		
		setBuildEnv ./configure
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi
	
	if [ $? != "0" ] ; then
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
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to build SDL_mixer."
		exit 12
	fi

	printOK


	printItem "installing"
	
	if [ -n "$prefix" ] ; then	
	{		
		echo "# SDL_mixer section." >> ${OSDL_ENV_FILE}
		
		echo "LD_LIBRARY_PATH=${prefix}/SDL_mixer-${SDL_mixer_VERSION}/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
		echo "export LD_LIBRARY_PATH" >> ${OSDL_ENV_FILE}

		LD_LIBRARY_PATH=${prefix}/SDL_mixer-${SDL_mixer_VERSION}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH
		
		if [ "$is_windows" -eq "0" ] ; then
		
			# Always remember that, on Windows, DLL are searched through the PATH, not the LD_LIBRARY_PATH.
			
			PATH=${prefix}/SDL_mixer-${SDL_mixer_VERSION}/lib:${PATH}	
			export PATH
				
			echo "PATH=${prefix}/SDL_mixer-${SDL_mixer_VERSION}/lib:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "export PATH" >> ${OSDL_ENV_FILE}
		fi

		echo "" >> ${OSDL_ENV_FILE}	
		
		${MKDIR} -p ${prefix}/SDL_mixer-${SDL_mixer_VERSION}
			
		setBuildEnv ${MAKE} install prefix=${prefix}/SDL_mixer-${SDL_mixer_VERSION}  
		
		if [ $? != "0" ] ; then
			echo
			ERROR "Unable to install SDL_mixer."
			exit 13
		fi	
		
		# Rename 'libSDL_mixer.la', to prevent libtool from detecting it when linking OSDL and 
		# issuing very annoying messages twice, such as :
		# "libtool: link: warning: library `[...]/libSDL_mixer.la' was moved."
		
		${MV} -f ${prefix}/SDL_mixer-${SDL_mixer_VERSION}/lib/libSDL_mixer.la ${prefix}/SDL_mixer-${SDL_mixer_VERSION}/lib/libSDL_mixer.la-hidden-by-LOANI

		if [ $? != "0" ] ; then
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
	
	
	if [ $? != "0" ] ; then
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





####################################################################################################
# SDL_gfx
####################################################################################################



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
		${CP} -f ${SDL_gfx_ARCHIVE} ${SDL_gfx_ARCHIVE}.save && ${GUNZIP} -f ${SDL_gfx_ARCHIVE} && tar -xvf "SDL_gfx-${SDL_gfx_VERSION}.tar" 
	} 1>>"$LOG_OUTPUT" 2>&1
	
		
	if [ $? != "0" ] ; then
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
	{			
		setBuildEnv ./autogen.sh
	} 1>>"$LOG_OUTPUT" 2>&1			

	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to configure SDL_gfx : autogen failed."
		exit 11
	fi	
	
	if [ -f "config.cache" ] ; then
		${RM} -f config.cache
	fi
        
	if [ -n "$prefix" ] ; then	
	{		
		# SDL_gfx uses wrongly SDL includes : asks for SDL/SDL.h instead of SDL.h
		# Ugly hack :
		# (copy could be used instead, to avoid needing symbolic links for filesystems such as vfat)	
		${LN} -s ${SDL_PREFIX}/include/SDL ${SDL_PREFIX}/include/SDL/SDL
		
		OLD_CPP_FLAGS=$CPP_FLAGS
		CPP_FLAGS="-I${SDL_PREFIX}/include $CPP_FLAGS"
		export CPP_FLAGS
                
		OLD_LD_FLAGS=$LD_FLAGS
		
		LDFLAGS="-L${SDL_PREFIX}/lib $LDFLAGS"
		export LDFLAGS
		
		LIBS=$LDFLAGS
		export LIBS
		
		# --disable-sdltest added since configure tries to compile a test without letting
		# the system libraries locations to be redefined. Therefore a wrong libstdc++.so
		# could be chosen, leading to errors such as : 
		# "undefined reference to `_Unwind_Resume_or_Rethrow@GCC_3.3'"
		
		setBuildEnv ./configure --prefix=${prefix}/SDL_gfx-${SDL_gfx_VERSION} --exec-prefix=${prefix}/SDL_gfx-${SDL_gfx_VERSION} --with-sdl-prefix=${SDL_PREFIX} --disable-sdltest
                 
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{		
		setBuildEnv ./configure
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi
	
	if [ $? != "0" ] ; then
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
        
	# SDL_gfx will not be compiled with debug machinery :	
	{
		setBuildEnv ./nodebug.sh
	} 1>>"$LOG_OUTPUT" 2>&1	
	
	if [ $? != "0" ] ; then
		echo
		ERROR "SDL_gfx post-configure script failed (nodebug.sh)."
		exit 12
	fi
	
	printOK	
	
	printItem "building"
	
	{
		setBuildEnv ${MAKE} LDFLAGS="-lgcc_s"
	} 1>>"$LOG_OUTPUT" 2>&1	 
	
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to build SDL_gfx."
		exit 12
	fi

	printOK


	printItem "installing"
	
	if [ -n "$prefix" ] ; then	
	{		
	
		echo "# SDL_gfx section." >> ${OSDL_ENV_FILE}
		
		echo "LD_LIBRARY_PATH=${prefix}/SDL_gfx-${SDL_gfx_VERSION}/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
		echo "export LD_LIBRARY_PATH" >> ${OSDL_ENV_FILE}
			
		LD_LIBRARY_PATH=${prefix}/SDL_gfx-${SDL_gfx_VERSION}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH
			
		if [ "$is_windows" -eq "0" ] ; then
			# Always remember that, on Windows, DLL are searched through the PATH, not the LD_LIBRARY_PATH.
			
			PATH=${prefix}/SDL_gfx-${SDL_gfx_VERSION}/lib:${PATH}	
			export PATH
			
			echo "PATH=${prefix}/SDL_gfx-${SDL_gfx_VERSION}/lib:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "export PATH" >> ${OSDL_ENV_FILE}
			
		fi

		echo "" >> ${OSDL_ENV_FILE}	
		
		setBuildEnv ${MAKE} install prefix=${prefix}/SDL_gfx-${SDL_gfx_VERSION} 
			
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{		
		setBuildEnv ${MAKE} install 
			
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi
	
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to install SDL_gfx (application)."
		exit 13
	fi	
	
	FIXED_FONT_REPOSITORY="${OSDL_DATA_FULL_DIR_NAME}/fonts/fixed"
	${MKDIR} -p "${FIXED_FONT_REPOSITORY}" && ${CP} -f Fonts/*.fnt "${FIXED_FONT_REPOSITORY}"
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to install SDL_gfx fonts."
		exit 14
	fi	
	
	printOK
	
	{
		setBuildEnv ${MAKE} distclean
	} 1>>"$LOG_OUTPUT" 2>&1	
			
	if [ $? != "0" ] ; then
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




####################################################################################################
# freetype
####################################################################################################


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
		${CP} -f ${freetype_ARCHIVE} ${freetype_ARCHIVE}.save && ${BUNZIP2} -f ${freetype_ARCHIVE} && tar -xvf "freetype-${freetype_VERSION}.tar" 
	} 1>>"$LOG_OUTPUT" 2>&1
	
		
	if [ $? != "0" ] ; then
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
		setBuildEnv ./configure --prefix=${prefix}/freetype-${freetype_VERSION} --exec-prefix=${prefix}/freetype-${freetype_VERSION}
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{		
		setBuildEnv ./configure
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi
	
	if [ $? != "0" ] ; then
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
	
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to build freetype."
		exit 12
	fi

	printOK


	printItem "installing"
	
	if [ -n "$prefix" ] ; then	
	{		
		echo "# freetype section." >> ${OSDL_ENV_FILE}
		
		echo "LD_LIBRARY_PATH=${prefix}/freetype-${freetype_VERSION}/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
		echo "export LD_LIBRARY_PATH" >> ${OSDL_ENV_FILE}
	
		if [ "$is_windows" -eq "0" ] ; then
		
			# Always remember that, on Windows, DLL are searched through the PATH, not the LD_LIBRARY_PATH.
			
			PATH=${prefix}/freetype-${freetype_VERSION}/lib:${PATH}	
			export PATH
			
			echo "PATH=${prefix}/freetype-${freetype_VERSION}:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "export PATH" >> ${OSDL_ENV_FILE}
			
		fi

		echo "" >> ${OSDL_ENV_FILE}	
		
		LD_LIBRARY_PATH=${prefix}/freetype-${freetype_VERSION}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH
			
		setBuildEnv ${MAKE} install 
			
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{		
			setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi
	
	
	if [ $? != "0" ] ; then
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




####################################################################################################
# SDL_ttf
####################################################################################################



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
		${CP} -f ${SDL_ttf_ARCHIVE} ${SDL_ttf_ARCHIVE}.save && ${GUNZIP} -f ${SDL_ttf_ARCHIVE} && tar -xvf "SDL_ttf-${SDL_ttf_VERSION}.tar" 
	} 1>>"$LOG_OUTPUT" 2>&1
	
		
	if [ $? != "0" ] ; then
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

	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to configure SDL_ttf : autogen failed."
		exit 11
	fi	
	
	if [ -n "$prefix" ] ; then	
	{		

		# --disable-sdltest added since configure tries to compile a test without letting
		# the system libraries locations to be redefined. Therefore a wrong libstdc++.so
		# could be chosen, leading to errors such as : 
		# "undefined reference to `_Unwind_Resume_or_Rethrow@GCC_3.3'"
	
		setBuildEnv ./configure --prefix=${prefix}/SDL_ttf-${SDL_ttf_VERSION} --exec-prefix=${prefix}/SDL_ttf-${SDL_ttf_VERSION} --with-freetype-prefix=${prefix}/freetype-${freetype_VERSION} --with-freetype-exec-prefix=${prefix}/freetype-${freetype_VERSION} --with-sdl-prefix=${prefix}/SDL-${SDL_VERSION} --with-sdl-exec-prefix=${prefix}/SDL-${SDL_VERSION} --disable-sdltest
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{		
		setBuildEnv ./configure
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to configure SDL_ttf."
		exit 11
	fi
		
	printOK	
	
	printItem "building"
	
	# SDL_ttf won't compile if not patched :
	# Ugly tr-based hack to prevent the numerous versions of sed to panic when having a 
	# newline in replacement string :
	${CAT} SDL_ttf.c | ${SED} 's|#include <freetype/freetype.h>|#include <ft2build.h>?#include FT_FREETYPE_H??#include <freetype/freetype.h>|1' | ${TR} "?" "\n" > SDL_ttf-patched.c && ${RM} -f SDL_ttf.c && ${MV} -f SDL_ttf-patched.c SDL_ttf.c

	if [ $? != "0" ] ; then
		echo
		WARNING "SDL_ttf include patch failed, continuing anyway."
	fi	
	
	{
		setBuildEnv ${MAKE} LDFLAGS="-L${prefix}/LOANI-installations/SDL-${SDL_VERSION}/lib -lgcc_s"
	} 1>>"$LOG_OUTPUT" 2>&1	 
	
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to build SDL_ttf."
		exit 12
	fi

	printOK


	printItem "installing"
	
	if [ -n "$prefix" ] ; then	
	{		
		echo "# SDL_ttf section." >> ${OSDL_ENV_FILE}
		
		echo "LD_LIBRARY_PATH=${prefix}/SDL_ttf-${SDL_ttf_VERSION}/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
		echo "export LD_LIBRARY_PATH" >> ${OSDL_ENV_FILE}

		if [ "$is_windows" -eq "0" ] ; then
		
			# Always remember that, on Windows, DLL are searched through the PATH, not the LD_LIBRARY_PATH.
			
			PATH=${prefix}/SDL_ttf-${SDL_ttf_VERSION}/lib:${PATH}	
			export PATH
				
			echo "PATH=${prefix}/SDL_ttf-${SDL_ttf_VERSION}/lib:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "export PATH" >> ${OSDL_ENV_FILE}
			
		fi

		echo "" >> ${OSDL_ENV_FILE}	
		
		LD_LIBRARY_PATH=${prefix}/SDL_ttf-${SDL_ttf_VERSION}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		setBuildEnv ${MAKE} install prefix=${prefix}/SDL_ttf-${SDL_ttf_VERSION}
			
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{		
		setBuildEnv ${MAKE} install
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi
	
	
	if [ $? != "0" ] ; then
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




####################################################################################################
# GNU Libtool 
####################################################################################################


getlibtool()
{
	LOG_STATUS "Getting libtool..."
	launchFileRetrieval libtool
}


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
		${CP} -f ${libtool_ARCHIVE} ${libtool_ARCHIVE}.save && ${GUNZIP} -f ${libtool_ARCHIVE} && tar -xvf "libtool-${libtool_VERSION}.tar" 
	} 1>>"$LOG_OUTPUT" 2>&1
	
		
	if [ $? != "0" ] ; then
		ERROR "Unable to extract ${libtool_ARCHIVE}."
		LOG_STATUS "Restoring ${libtool_ARCHIVE}."
		${MV} -f ${libtool_ARCHIVE.save} ${libtool_ARCHIVE} 
		exit 10
	fi
	
	${MV} -f ${libtool_ARCHIVE}.save ${libtool_ARCHIVE}
	${RM} -f "libtool-${libtool_VERSION}.tar" 
	
	printOK
	
}


generatelibtool()
{

	LOG_STATUS "Generating libtool..."

	cd "libtool-${libtool_VERSION}"
	
	printItem "configuring"

	if [ -n "$prefix" ] ; then	
	{
		# configure calls libltdl configure and loose env.
		old_path=$PATH
		old_ld_path=$LD_LIBRARY_PATH
		old_cc=$CC
		old_cxx=$CXX	
		
		if [ "$use_mingw" -eq "0" ] ; then 
		
			PATH=${MINGW_PATH}:$PATH
			export PATH
			
			LD_LIBRARY_PATH=${MINGW_LD_LIBRARY_PATH}:$LD_LIBRARY_PATH
			export LD_LIBRARY_PATH
			
			CC=${MINGW_GCC}	
			export CC
			
	        CXX=${MINGW_GPP}	
	        export CXX
			
		fi


		# To avoid libltdl problems, we make use of --disable-ltdl-install.    
		# Parameters and/or environment set for libtool does not seem to be passed to libltdl.
		# (a tag is to be specified with ltdl whereas it is ok with libtool)
		# A work-around could be to recurse make in ltdl directory before performing the global 
		# libtool make.
           
		setBuildEnv ./configure --prefix=${prefix}/libtool-${libtool_VERSION} --exec-prefix=${prefix}/libtool-${libtool_VERSION} --disable-ltdl-install
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{
		if [ "$use_mingw" -eq "0" ] ; then 
		
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
	
			
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to configure libtool."
		exit 11
	fi	

	if [ "${use_mingw}" -eq "0" ] ; then	
	
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
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to build libtool."
		exit 12
	fi

	printOK


	printItem "installing"
	
	
	if [ -n "$prefix" ] ; then	
	{		
		${MKDIR} -p ${prefix}/libtool-${libtool_VERSION}
                
		echo "# libtool section." >> ${OSDL_ENV_FILE}
		
		echo "PATH=${prefix}/libtool-${libtool_VERSION}/bin:\${PATH}" >> ${OSDL_ENV_FILE}
		echo "export PATH" >> ${OSDL_ENV_FILE}
		
		echo "LD_LIBRARY_PATH=${prefix}/libtool-${libtool_VERSION}/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
		echo "export LD_LIBRARY_PATH" >> ${OSDL_ENV_FILE}
		
		if [ "$is_windows" -eq "0" ] ; then
		
			# Always remember that, on Windows, DLL are searched through the PATH, not the LD_LIBRARY_PATH.
			
			PATH=${prefix}/libtool-${libtool_VERSION}/lib:${PATH}	
			export PATH
			
			echo "PATH=${prefix}/libtool-${libtool_VERSION}/lib:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "export PATH" >> ${OSDL_ENV_FILE}
			
		fi

		echo "" >> ${OSDL_ENV_FILE}
		
		PATH=${prefix}/libtool-${libtool_VERSION}/bin:${PATH}
		export PATH
		
		LD_LIBRARY_PATH=${prefix}/libtool-${libtool_VERSION}/lib:${LD_LIBRARY_PATH}
		export LD_LIBRARY_PATH

		setBuildEnv ${MAKE} install prefix=${prefix}/libtool-${libtool_VERSION} 
		
		#if [ "$?" != "0" ] ; then 
		#	WARNING "First libtool install failed"
		#	${MAKE} install
		#fi		
		
	} 1>>"$LOG_OUTPUT" 2>&1		
	else
	{		
		setBuildEnv ${MAKE} install 
	} 1>>"$LOG_OUTPUT" 2>&1			
	fi
		
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to install libtool to libtool-${libtool_VERSION}."
		WARNING "Step on failure disabled"
		#exit 13
	fi	
	
	printOK

	printEndList
	
	LOG_STATUS "libtool successfully installed."
	
	cd "$initial_dir"
	
}


cleanlibtool()
{
	LOG_STATUS "Cleaning libtool build tree..."
	${RM} -rf "libtool-${libtool_VERSION}"
}




####################################################################################################
# Windows pthreads for POSIX compliance.
####################################################################################################


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
         
		# The following is commented out, it would have to be updated before use :
						
		# Needs to download recursively the FTP root :
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
	
	# Nothing to do !
	
	printOK
	
}


generatewin_pthread()
{

	LOG_STATUS "Generating pthread-win32..."
	
	printItem "configuring"

	# Nothing to do !
        
	printOK	
	
	printItem "building"
	
	# Nothing to do !
       

	printItem "installing"

	real_download_target=${win_pthread_wget_dir}/pub/${PTHREADS_WIN32_DIR}
		
	# If win_pthread repository is not sane, rebuild it : 
	
	if [ -s "${repository}/${PTHREADS_WIN32_DIR}/lib/${win_pthread_DLL}" -a -s "${repository}/${PTHREADS_WIN32_DIR}/include/${win_pthread_HEADER_MAIN}" -a -s "${repository}/${PTHREADS_WIN32_DIR}/include/${win_pthread_HEADER_SCHED}" -a -s "${repository}/${PTHREADS_WIN32_DIR}/include/${win_pthread_HEADER_SEM}" ] ; then
		DEBUG "win_pthread installation in repository is ok."
	else  
	      
		# Clean-up wget mess :
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
                                       
			# The include and/or lib directory might not be already retrieved :                       
			if ls ${real_download_target}/${win_pthread_ftp_dir}/include 1>/dev/null 2>&1 ; then      
				${CP} -f ${real_download_target}/${win_pthread_ftp_dir}/* $repository/${PTHREADS_WIN32_DIR} 
			else
				DEBUG "No win_pthread FTP directory to retrieve (no ${real_download_target}/${win_pthread_ftp_dir} directory)."
                        fi
		} 1>>"$LOG_OUTPUT" 2>&1	
	fi
        
	if [ $? != "0" ] ; then
		echo
		WARNING "Pre installation phase for win pthread had errors."
		${RM} -rf ${win_pthread_wget_dir}
	fi
        
        
 	if [ -n "$prefix" ] ; then	        
	{              
		echo "# pthreads win32 section." >> ${OSDL_ENV_FILE}	
			
		echo "LD_LIBRARY_PATH=${prefix}/${PTHREADS_WIN32_DIR}/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
		echo "export LD_LIBRARY_PATH" >> ${OSDL_ENV_FILE}
		
		if [ "$is_windows" -eq "0" ] ; then
		
			# Always remember that, on Windows, DLL are searched through the PATH, not the LD_LIBRARY_PATH.
			PATH=${prefix}/${PTHREADS_WIN32_DIR}/lib:${PATH}	
			export PATH	
			
			echo "PATH=${prefix}/${PTHREADS_WIN32_DIR}/lib:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "export PATH" >> ${OSDL_ENV_FILE}
			
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
		echo "LD_LIBRARY_PATH=/usr/local/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
		echo "export LD_LIBRARY_PATH" >> ${OSDL_ENV_FILE}
		echo "" >> ${OSDL_ENV_FILE}

		LD_LIBRARY_PATH=/usr:local/lib:${LD_LIBRARY_PATH}               
		export LD_LIBRARY_PATH               
                
		${MKDIR} -p /usr/local/include /usr/local/lib && ${CP} -f ${repository}/${PTHREADS_WIN32_DIR}/include/* /usr/local/include && ${CP} -f ${repository}/${PTHREADS_WIN32_DIR}/lib/* /usr/local/lib
        
	} 1>>"$LOG_OUTPUT" 2>&1	
	fi
             
	if [ $? != "0" ] ; then
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
	
	# Should have been copied to ${prefix} :
	pthread_build_dir=${repository}/${PTHREADS_WIN32_DIR}
	
	if [ -d "${pthread_build_dir}" ]; then
		${RM} -rf "${pthread_build_dir}"
	fi	
}




####################################################################################################
# Ceylan 
####################################################################################################


getCeylan()
{
	
	DEBUG "Getting Ceylan..."

	# Ceylan can be obtained by downloading a release archive or by using CVS.
	
	if [ "${use_cvs}" -eq "1" ]; then
		# Use archive instead of CVS : 
		launchFileRetrieval Ceylan
		return $?
	fi
	
	# Here we are to use CVS :

	DISPLAY "      ----> getting Ceylan from CVS"
	
	cd ${alternate_prefix}

	if [ -d "${alternate_prefix}/${Ceylan_ROOT}" ] ; then
		if [ -d "${alternate_prefix}/${Ceylan_ROOT}.save" ] ; then
			if [ "$be_strict" -eq "0" ] ; then
				ERROR "There already exist a back-up directory for Ceylan, it is on the way, please remove it first (${alternate_prefix}/${Ceylan_ROOT}.save)"
				exit 5
			else	
				WARNING "Deleting already existing back-up directory for ${Ceylan_ROOT} (removing ${alternate_prefix}/${Ceylan_ROOT}.save)"
			 	${RM} -rf "${alternate_prefix}/${Ceylan_ROOT}.save"
			fi
		fi		
		${MV} -f ${alternate_prefix}/${Ceylan_ROOT} ${alternate_prefix}/${Ceylan_ROOT}.save
		WARNING "There already existed a directory for Ceylan (${alternate_prefix}/${Ceylan_ROOT}), it has been moved to ${alternate_prefix}/${Ceylan_ROOT}.save." 
	fi	
	
	LOG_STATUS "Getting Ceylan in its final directory ${alternate_prefix}..."
	
	if [ "$developer_access" -eq "0" ] ; then
		DISPLAY "Retrieving Ceylan from developer CVS with user name ${developer_name}."
		
		if [ "$no_cvs" -eq 1 ] ; then
		
			cvsAttemptNumber=1
			success=1
			
			if [ "$use_current_cvs" -eq 0 ] ; then 
				DEBUG "No stable tag wanted, retrieving directly latest version from CVS."
				# Not wanting sticky tags : TAG_OPTION="-D tomorrow" commented
				TAG_OPTION=""
			else
				DEBUG "Using latest stable CVS tag (${latest_stable_ceylan})."
				TAG_OPTION="-r ${latest_stable_ceylan}"
			fi
			
			while [ "$cvsAttemptNumber" -le "$MAX_CVS_RETRY" ]; do
				LOG_STATUS "Attempt #${cvsAttemptNumber} to retrieve Ceylan."
				{
					DEBUG "CVS command : ${CVS} ${CVS_OPT} -d:ext:${developer_name}@${Ceylan_CVS_SERVER}:/cvsroot/osdl co ${TAG_OPTION} ${Ceylan_ROOT}"
					${CVS} ${CVS_OPT} -d:ext:${developer_name}@${Ceylan_CVS_SERVER}:/cvsroot/osdl co ${TAG_OPTION} ${Ceylan_ROOT} 
				} 1>>"$LOG_OUTPUT" 2>&1	
				
				if [ "$?" -eq "0" ] ; then
					cvsAttemptNumber=$(($MAX_CVS_RETRY+1))
					success=0	
				else
					cvsAttemptNumber=$(($cvsAttemptNumber+1))	
					LOG_STATUS "CVS command failed."
					${SLEEP} 3
				fi	
			done
			
			
			if [ "$success" != 0 ] ; then
				ERROR "Unable to retrieve Ceylan from CVS after $MAX_CVS_RETRY attempts."
				exit 20
			fi
							
		else
			LOG_STATUS "CVS retrieval disabled for Ceylan."
		fi
			
		if [ "$?" != "0" ] ; then
			ERROR "Unable to retrieve Ceylan from developer CVS."
			exit 20	
		fi

	else			
	
		LOG_STATUS "Retrieving Ceylan from anonymous CVS."
		
		if [ "$no_cvs" -eq 1 ] ; then
		{
			cvsAttemptNumber=1
			success=1

			if [ "$use_current_cvs" -eq 0 ] ; then 
				DEBUG "No stable tag wanted, retrieving directly latest version from CVS."
				# Not wanting sticky tags : TAG_OPTION="-D tomorrow" commented
				TAG_OPTION=""
			else
				DEBUG "Using latest stable CVS tag (${latest_stable_ceylan})."
				TAG_OPTION="-r ${latest_stable_ceylan}"
			fi
			
			while [ "$cvsAttemptNumber" -le "$MAX_CVS_RETRY" ]; do
				LOG_STATUS "Attempt #${cvsAttemptNumber} to retrieve Ceylan."
				
				{
					DEBUG "CVS command : ${CVS} -d:pserver:anonymous@${Ceylan_CVS_SERVER}:/cvsroot/osdl export ${TAG_OPTION} ${Ceylan_ROOT}"
					${CVS} -d:pserver:anonymous@${Ceylan_CVS_SERVER}:/cvsroot/osdl export ${TAG_OPTION} ${Ceylan_ROOT} 
				} 1>>"$LOG_OUTPUT" 2>&1
				
				if [ "$?" -eq "0" ] ; then
					cvsAttemptNumber=$(($MAX_CVS_RETRY+1))
					success=0	
				else
					cvsAttemptNumber=$(($cvsAttemptNumber+1))	
					LOG_STATUS "CVS command failed."
					${SLEEP} 3
				fi	
			done			
			
			if [ "$success" != 0 ] ; then
				ERROR "Unable to retrieve Ceylan from CVS after $MAX_CVS_RETRY attempts."
				exit 21
			fi
			
		} 1>>"$LOG_OUTPUT" 2>&1
		else
			LOG_STATUS "CVS retrieval disabled for Ceylan."
		fi
				
		if [ "$?" != "0" ] ; then
			ERROR "Unable to retrieve Ceylan from anonymous CVS."
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

	if [ "$no_cvs" -eq 0 ] ; then
		WARNING "As the option noCVS was used, build process stops here."
		exit 0 	
	fi


	if [ "${use_cvs}" -eq "1" ]; then

		# Here we use source archives :
		
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
	
		# For project archives (Ceylan and OSDL), all build and install trees are expected to
		# be in ${alternate_prefix} (usually LOANI-installations) instead of $repository.
		# Hence the copy for bzip2 is made from the repository to the installation directory :
		
		${MKDIR} -p ${alternate_prefix}/Ceylan
		
		cd ${alternate_prefix}/Ceylan
	
		# Prevent archive from disappearing because of bunzip2.
		{
			${CP} -f $repository/${Ceylan_ARCHIVE} . && ${BUNZIP2} -f ${Ceylan_ARCHIVE} && tar -xvf "Ceylan-${Ceylan_VERSION}.${Ceylan_RELEASE}.tar" 
		} 1>>"$LOG_OUTPUT" 2>&1
	
		
		if [ $? != "0" ] ; then
			ERROR "Unable to extract ${Ceylan_ARCHIVE}."
			DEBUG "Restoring ${Ceylan_ARCHIVE}."
			${RM} -rf ${alternate_prefix}/Ceylan
			exit 10
		fi
	
		${RM} -f "${alternate_prefix}/Ceylan/Ceylan-${Ceylan_VERSION}.${Ceylan_RELEASE}.tar"
	
	fi	
	
	printOK	

}


generateCeylan()
{

	LOG_STATUS "Generating Ceylan..."
	
	cd "${alternate_prefix}/${Ceylan_ROOT}/src"
	
	printItem "configuring"
	
	#{
	#	 ./configure	
	#} 1>>"$LOG_OUTPUT" 2>&1
	
	LOG_STATUS "Configure test : checking tools."
	{
       	setBuildEnv ${MAKE} checktools ${BUILD_LOCATIONS} ${BUILD_SILENT}
	} 1>>"$LOG_OUTPUT" 2>&1
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to configure Ceylan."
		exit 11
	fi	
	
	printOK	
	
	printItem "building"
		
	{
		setBuildEnv ${MAKE} deps build ${BUILD_LOCATIONS} ${BUILD_SILENT}
	} 1>>"$LOG_OUTPUT" 2>&1	 
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to build Ceylan."
		exit 12
	fi

	printOK
	
	printItem "installing"
	# We hereby install Ceylan alongside to its sources :
	# We will end up under ${Ceylan_ROOT} with src, bin, lib, etc.
		
	{		
	
		echo "# Ceylan section." >> ${OSDL_ENV_FILE}
				
		echo "LD_LIBRARY_PATH=${alternate_prefix}/${Ceylan_ROOT}/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
		echo "export LD_LIBRARY_PATH" >> ${OSDL_ENV_FILE}

	
		LD_LIBRARY_PATH=${alternate_prefix}/${Ceylan_ROOT}/lib:${LD_LIBRARY_PATH}	
		export LD_LIBRARY_PATH
		
		if [ "$is_windows" -eq "0" ] ; then
		
			# Always remember that, on Windows, DLL are searched through the PATH, not the LD_LIBRARY_PATH.
			
			PATH=${alternate_prefix}/${Ceylan_ROOT}/lib:${PATH}	
			export PATH	
			
			echo "PATH=${alternate_prefix}/${Ceylan_ROOT}/lib:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "export PATH" >> ${OSDL_ENV_FILE}
			
		fi

		echo "" >> ${OSDL_ENV_FILE}
		
		setBuildEnv ${MAKE} install PREFIX=${alternate_prefix}/${Ceylan_ROOT} ${BUILD_LOCATIONS} ${BUILD_SILENT}
		
	} 1>>"$LOG_OUTPUT" 2>&1	
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to install Ceylan to ${alternate_prefix}/${Ceylan_ROOT}."
		exit 13
	fi	
	
	LOG_STATUS "Making tests for Ceylan."
	
	{		
		setBuildEnv ${MAKE} tests PREFIX=${alternate_prefix}/${Ceylan_ROOT} ${BUILD_LOCATIONS} ${BUILD_SILENT}
	} 1>>"$LOG_OUTPUT" 2>&1	
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to build and set Ceylan tests to ${alternate_prefix}/${Ceylan_ROOT}."
		exit 14
	fi	
	
	
	LOG_STATUS "Making Ceylan post-install"
	
	{		
		setBuildEnv ${MAKE} postinstall PREFIX=${alternate_prefix}/${Ceylan_ROOT} ${BUILD_LOCATIONS} ${BUILD_SILENT}
	} 1>>"$LOG_OUTPUT" 2>&1	
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to post-install Ceylan to ${alternate_prefix}/${Ceylan_ROOT}."
		exit 15
	fi	
	
	
	# Avoid CVS pitfall and remove sticky tags (only if necessary) :
	
	if [ "$use_cvs" -eq "0" -a "$use_current_cvs" -eq "1" ] ; then  
		{		
			LOG_STATUS "Removing CVS sticky tag used for stable version"
			cd "${alternate_prefix}/Ceylan" && ${CVS} update -A
		} 1>>"$LOG_OUTPUT" 2>&1	

		if [ $? != "0" ] ; then
			echo
			WARNING "Non-fatal error while removing stable sticky CVS tag from Ceylan version."
		fi	

	fi
	
	printOK

	printEndList
	
	LOG_STATUS "Ceylan successfully installed."
	
	cd "$initial_dir"
	
}


cleanCeylan()
{
	LOG_STATUS "Cleaning Ceylan build tree..."
	# Nothing to do : directly managed in install location.
}




####################################################################################################
# OSDL
####################################################################################################


getOSDL()
{

	DEBUG "Getting OSDL..."

	# OSDL can be obtained by downloading a release archive or by using CVS.
	
	if [ "${use_cvs}" -eq "1" ]; then
		# Use archive instead of CVS : 
		launchFileRetrieval OSDL
		return $?
	fi
	
	# Here we are to use CVS :
	
	DISPLAY "      ----> getting OSDL from CVS"
	
	cd ${alternate_prefix}
	
	if [ -d "${alternate_prefix}/${OSDL_ROOT}" ] ; then
		if [ -d "${alternate_prefix}/${OSDL_ROOT}.save" ] ; then
			if [ "$be_strict" -eq "0" ] ; then
				ERROR "There already exist a back-up directory for OSDL, it is on the way, please remove it first (${alternate_prefix}/${OSDL_ROOT}.save)"
				exit 5
			else	
				WARNING "Deleting already existing back-up directory for ${OSDL_ROOT} (removing ${alternate_prefix}/${OSDL_ROOT}.save)"
			 	${RM} -rf "${alternate_prefix}/${OSDL_ROOT}.save"
			fi
		fi		
		${MV} -f ${alternate_prefix}/${OSDL_ROOT} ${alternate_prefix}/${OSDL_ROOT}.save
		WARNING "There already existed a directory for OSDL (${alternate_prefix}/${OSDL_ROOT}), it has been moved to ${alternate_prefix}/${OSDL_ROOT}.save." 
	fi	
	

	LOG_STATUS "Getting OSDL in its final directory ${alternate_prefix}..."
	
	if [ "$developer_access" -eq "0" ] ; then
	
		DISPLAY "Retrieving OSDL from developer CVS with user name ${developer_name}."
				
		if [ "$no_cvs" -eq 1 ] ; then
		
			cvsAttemptNumber=1
			success=1

			if [ "$use_current_cvs" -eq 0 ] ; then 
				DEBUG "No stable tag wanted, retrieving directly latest version from CVS."
				# Not wanting sticky tags : TAG_OPTION="-D tomorrow" commented
				TAG_OPTION=""
			else
				DEBUG "Using latest stable CVS tag (${latest_stable_osdl})."
				TAG_OPTION="-r ${latest_stable_osdl}"
			fi
			
			while [ "$cvsAttemptNumber" -le "$MAX_CVS_RETRY" ]; do
				LOG_STATUS "Attempt #${cvsAttemptNumber} to retrieve OSDL."
				{
					DEBUG "CVS command : ${CVS} ${CVS_OPT} -d:ext:${developer_name}@${OSDL_CVS_SERVER}:/cvsroot/osdl co ${TAG_OPTION} ${OSDL_ROOT}"
					${CVS} ${CVS_OPT} -d:ext:${developer_name}@${OSDL_CVS_SERVER}:/cvsroot/osdl co ${TAG_OPTION} ${OSDL_ROOT}
				} 1>>"$LOG_OUTPUT" 2>&1					
				
				if [ "$?" -eq "0" ] ; then
					cvsAttemptNumber=$(($MAX_CVS_RETRY+1))
					success=0	
				else
					cvsAttemptNumber=$(($cvsAttemptNumber+1))	
					LOG_STATUS "CVS command failed."
					${SLEEP} 3
				fi	
			done
			
			
			if [ "$success" != 0 ] ; then
				ERROR "Unable to retrieve OSDL from CVS after $MAX_CVS_RETRY attempts."
				exit 20
			fi		
		
			
		else
			LOG_STATUS "CVS retrieval disabled for OSDL."
		fi
					
		if [ "$?" != "0" ] ; then
			ERROR "Unable to retrieve OSDL from developer CVS."
			exit 22	
		fi

	else			
	
		LOG_STATUS "Retrieving OSDL from anonymous CVS."
				
		if [ "$no_cvs" -eq 1 ] ; then
		{
		
		
			cvsAttemptNumber=1
			success=1

			if [ "$use_current_cvs" -eq 0 ] ; then 
				DEBUG "No stable tag wanted, retrieving directly latest version from CVS."
				# Not wanting sticky tags : TAG_OPTION="-D tomorrow" commented
				TAG_OPTION=""
			else
				DEBUG "Using latest stable CVS tag (${latest_stable_osdl})."
				TAG_OPTION="-r ${latest_stable_osdl}"
			fi
			
			while [ "$cvsAttemptNumber" -le "$MAX_CVS_RETRY" ]; do
				LOG_STATUS "Attempt #${cvsAttemptNumber} to retrieve OSDL."
				{
					DEBUG "CVS command : ${CVS} -d:pserver:anonymous@${OSDL_CVS_SERVER}:/cvsroot/osdl export ${TAG_OPTION} ${OSDL_ROOT}"
					${CVS} -d:pserver:anonymous@${OSDL_CVS_SERVER}:/cvsroot/osdl export ${TAG_OPTION} ${OSDL_ROOT} 
				} 1>>"$LOG_OUTPUT" 2>&1
					
				if [ "$?" -eq "0" ] ; then
					cvsAttemptNumber=$(($MAX_CVS_RETRY+1))
					success=0	
				else
					cvsAttemptNumber=$(($cvsAttemptNumber+1))	
					LOG_STATUS "CVS command failed."
					${SLEEP} 3
				fi	
			done
			
			
			if [ "$success" != 0 ] ; then
				ERROR "Unable to retrieve OSDL from CVS after $MAX_CVS_RETRY attempts."
				exit 20
			fi	
						
		} 1>>"$LOG_OUTPUT" 2>&1
		else
			LOG_STATUS "CVS retrieval disabled for OSDL."
		fi
	
		if [ "$?" != "0" ] ; then
			ERROR "Unable to retrieve OSDL from anonymous CVS."
			exit 23	
		fi
	fi	
	
	return 0
	
}


prepareOSDL()
{
	
	
	LOG_STATUS "Preparing OSDL..."
	
	printBeginList "OSDL       "
	
	printItem "extracting"


	if [ "${use_cvs}" -eq "1" ]; then


		# Here we use source archives :
		
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
	
		# For project archives (Ceylan and OSDL), all build and install trees are expected to
		# be in ${alternate_prefix} (usually LOANI-installations) instead of $repository.
		# Hence the copy for bzip2 is made from the repository to the installation directory :
		
		${MKDIR} -p ${alternate_prefix}/OSDL
		
		cd ${alternate_prefix}/OSDL
	
		# Prevent archive from disappearing because of bunzip2.
		{
			${CP} -f $repository/${OSDL_ARCHIVE} . && ${BUNZIP2} -f ${OSDL_ARCHIVE} && tar -xvf "OSDL-${OSDL_VERSION}.${OSDL_RELEASE}.tar" 
		} 1>>"$LOG_OUTPUT" 2>&1
	
		
		if [ $? != "0" ] ; then
			ERROR "Unable to extract ${OSDL_ARCHIVE}."
			DEBUG "Restoring ${OSDL_ARCHIVE}."
			${RM} -rf ${alternate_prefix}/OSDL
			exit 10
		fi
	
		${RM} -f "${alternate_prefix}/OSDL/OSDL-${OSDL_VERSION}.${OSDL_RELEASE}.tar"
	
	fi
	
	printOK	
	
}


generateOSDL()
{

	LOG_STATUS "Generating OSDL..."
	
	
	cd "${alternate_prefix}/${OSDL_ROOT}/src"
	
	printItem "configuring"
	
	#{
	#	 ./configure	
	#} 1>>"$LOG_OUTPUT" 2>&1
	
	LOG_STATUS "Configure test : checking tools."
	{
       	setBuildEnv ${MAKE} checktools ${BUILD_LOCATIONS} ${BUILD_SILENT}
	} 1>>"$LOG_OUTPUT" 2>&1
		
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to configure OSDL."
		exit 11
	fi	
	
	printOK	
	
	printItem "building"
	
		
	{
		setBuildEnv ${MAKE} build ${BUILD_LOCATIONS} ${BUILD_SILENT}
	} 1>>"$LOG_OUTPUT" 2>&1	 
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to build OSDL."
		exit 12
	fi

	printOK
	
	printItem "installing"
	# We hereby install OSDL alongside to its sources :
	# We will end up under ${OSDL_ROOT} with src, bin, lib, etc.
		
	{	
	
		echo "# OSDL section." >> ${OSDL_ENV_FILE}	
			
		echo "LD_LIBRARY_PATH=${alternate_prefix}/${OSDL_ROOT}/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
		echo "export LD_LIBRARY_PATH" >> ${OSDL_ENV_FILE}

		LD_LIBRARY_PATH=${alternate_prefix}/${OSDL_ROOT}/lib:${LD_LIBRARY_PATH}	
		export LD_LIBRARY_PATH	
		
		if [ "$is_windows" -eq "0" ] ; then
		
			# Always remember that, on Windows, DLL are searched through the PATH, not the LD_LIBRARY_PATH.
			
			PATH=${alternate_prefix}/${OSDL_ROOT}/lib:${PATH}	
			export PATH
			
			echo "PATH=${alternate_prefix}/${OSDL_ROOT}/lib:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "export PATH" >> ${OSDL_ENV_FILE}
			
		fi

		echo "" >> ${OSDL_ENV_FILE}
		
		# Register OSDL-data in OSDL environment file :
		echo "# OSDL-data section." >> ${OSDL_ENV_FILE}	
			
		echo "FONT_PATH=${OSDL_DATA_FULL_DIR_NAME}/fonts/fixed:${OSDL_DATA_FULL_DIR_NAME}/fonts/truetype" >> ${OSDL_ENV_FILE}		
		echo "export FONT_PATH" >> ${OSDL_ENV_FILE}	
			
		echo "" >> ${OSDL_ENV_FILE}
		
		setBuildEnv ${MAKE} install PREFIX=${alternate_prefix}/${OSDL_ROOT} ${BUILD_LOCATIONS} ${BUILD_SILENT}
		
	} 1>>"$LOG_OUTPUT" 2>&1	
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to install OSDL to ${alternate_prefix}/${OSDL_ROOT}."
		exit 13
	fi	
	
	LOG_STATUS "Making tests for OSDL."
	
	{		
		setBuildEnv ${MAKE} tests PREFIX=${alternate_prefix}/${OSDL_ROOT} ${BUILD_LOCATIONS} ${BUILD_SILENT}
	} 1>>"$LOG_OUTPUT" 2>&1	
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to build and set OSDL tests to ${alternate_prefix}/${OSDL_ROOT}."
		exit 14
	fi	
	
	
	LOG_STATUS "Making OSDL post-install"
	
	{		
		setBuildEnv ${MAKE} postinstall PREFIX=${alternate_prefix}/${OSDL_ROOT} ${BUILD_LOCATIONS} ${BUILD_SILENT}
	} 1>>"$LOG_OUTPUT" 2>&1	
	
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to post-install OSDL to ${alternate_prefix}/${OSDL_ROOT}."
		exit 15
	fi	

	# Avoid CVS pitfall and remove sticky tags (only if necessary, i.e. ) :
	
	if [ "$use_cvs" -eq "0" -a "$use_current_cvs" -eq "1" ] ; then  
		{		
			LOG_STATUS "Removing CVS sticky tag used for stable version"
			cd "${alternate_prefix}/OSDL" && ${CVS} update -A
		} 1>>"$LOG_OUTPUT" 2>&1	

		if [ $? != "0" ] ; then
			echo
			WARNING "Non-fatal error while removing stable sticky CVS tag from OSDL version."
		fi	

	fi
		
	printOK

	printEndList
	
	LOG_STATUS "OSDL successfully installed."
	
	cd "$initial_dir"
	
}


cleanOSDL()
{
	LOG_STATUS "Cleaning OSDL build tree..."
	# Nothing to do : directly managed in install location.
}



####################################################################################################
# End of loani-requiredTools.sh
####################################################################################################
