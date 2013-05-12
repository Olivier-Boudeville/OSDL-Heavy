# This script is made to be sourced by LOANI when retrieving basic common build
# tools.
#
# Most of them is expected to be already available in user'system.

# Creation date: 2004, February 22.
# Author: Olivier Boudeville (olivier.boudeville@esperide.com)


# Basic common build tools section.


# Automake, autoconf, aclocal, make, etc. are deemed most basic build tools and
# are supposed to be available if needed.


# Updating retrieve list:

# Note: if this list is to be updated, update as well
# osdl/OSDL/src/conf/build/loani/update-LOANI-mirror.sh

if [ $is_windows -eq 0 ] ; then

  WARNING "on Windows, no common build tool managed."
  COMMON_BUILD_TOOLS=""

else

	# Not on Windows. Aiming at the DS?

	if [ $target_nds -eq 1 ] ; then
		COMMON_BUILD_TOOLS="gcc binutils gdb"
	else

		# For the Nintendo DS we use the devkitPro-based toolchain:
		# (libnds listed here again: the PALib-provided version is ancient)
		# (libfat not listed here anymore as now is provided by libnds)
		# (libnds_examples not listed here)

		# Note: PAlib comes often with deprecated versions of its prerequisites
		# (notably: libnds), hence installing them directly can be interesting,
		# if not using PAlib.
		COMMON_BUILD_TOOLS="devkitARM libnds grit PAlib dswifi DeSmuME NoCashGBA"

	fi

fi

target_list="$COMMON_BUILD_TOOLS $target_list"

DEBUG "Scheduling retrieval of common build tools ($COMMON_BUILD_TOOLS)."





################################################################################
# GNU gcc (compiler suite)
################################################################################


# Apparently building GCC even on Debian is areal mess, and cannot be done
# safely and simply (ex: gdb build will fail), see for example
# http://stackoverflow.com/questions/7284009/cannot-find-lc-and-lm-in-g-linux.
#
# Let's stick with distro-default tools then.

getgcc()
{
	DEBUG "Getting gcc..."
	launchFileRetrieval gcc
	return $?
}



preparegcc()
{

	DEBUG "Preparing gcc..."

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

	printBeginList "gcc        "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of bunzip2.
	{
		${CP} -f ${gcc_ARCHIVE} ${gcc_ARCHIVE}.save && ${BUNZIP2} -f ${gcc_ARCHIVE} && tar -xvf "gcc-${gcc_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ ! $? -eq 0 ] ; then
		ERROR "Unable to extract ${gcc_ARCHIVE}."
		DEBUG "Restoring ${gcc_ARCHIVE}."
		${MV} -f ${gcc_ARCHIVE}.save ${gcc_ARCHIVE}
		exit 10
	fi

	${MV} -f ${gcc_ARCHIVE}.save ${gcc_ARCHIVE}
	${RM} -f "gcc-${gcc_VERSION}.tar"

	# Extracted directory might collide with the prefixed one.
	${MV} -f  "gcc-${gcc_VERSION}" ${gcc_SOURCE_TREE}

	printOK

}



generategcc()
{

	DEBUG "Generating gcc..."

	# To compile gcc, source and object directories should differ.
	${MKDIR} -p $repository/${gcc_BUILD_TREE}

	cd $repository/${gcc_BUILD_TREE}

	printItem "configuring"

	# Restricted to C++ since java would too often not bootstrap, or be slow to
	# bootstrap.

	if [ -n "$prefix" ] ; then
		{
			${MAKE_C} ../${gcc_SOURCE_TREE}/configure --prefix=${prefix}/gcc-${gcc_VERSION} --enable-languages=c++
		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			${MAKE_C} ../${gcc_SOURCE_TREE}/configure --enable-languages=c++
		} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ ! $? -eq 0 ] ; then
		echo
		ERROR "Unable to configure gcc."
		exit 11
	fi

	printOK

	printItem "building"


	{
		${MAKE} bootstrap-lean ${MAKE_C}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ ! $? -eq 0 ] ; then
		echo
		ERROR "Unable to bootstrap gcc."
		exit 12
	fi

	printOK

	printItem "installing"

	if [ -n "$prefix" ] ; then
		{
			echo "# gcc section." >> ${OSDL_ENV_FILE}

			echo "gcc_PREFIX=${prefix}/gcc-${gcc_VERSION}" >> ${OSDL_ENV_FILE}
			echo "export gcc_PREFIX" >> ${OSDL_ENV_FILE}
			echo "PATH=\$gcc_PREFIX/bin:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "LD_LIBRARY_PATH=\$gcc_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
			echo "" >> ${OSDL_ENV_FILE}
			${MAKE} install prefix=${prefix}/gcc-${gcc_VERSION} ${MAKE_C}

		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			${MAKE} install ${MAKE_C}
		} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ ! $? -eq 0 ] ; then
		echo
		ERROR "Unable to install gcc."
		exit 13
	fi

	printOK

	# Now updating all build-related environment variables:

	COMPILER_FAMILY=gcc
	export COMPILER_FAMILY

	if [ -n "$prefix" ] ; then
		GCC_ROOT=${prefix}/gcc-${gcc_VERSION}
		export GCC_ROOT
	else
		GCC_ROOT=/usr/local
		export GCC_ROOT
	fi

	C_COMPILER=${GCC_ROOT}/bin/gcc
	export C_COMPILER

	CPP_COMPILER=${GCC_ROOT}/bin/g++
	export CPP_COMPILER

	if [ ! -x "$C_COMPILER" ] ; then

		WARNING "Unable to find gcc after having installed it (not found in $C_COMPILER), trying counterparts."
		findTool gcc
		C_COMPILER=$returnedString
		DEBUG "C_COMPILER set to $C_COMPILER."

	fi


	if [ ! -x "$CPP_COMPILER" ] ; then

		WARNING "Unable to find g++ after having installed it (not found in $CPP_COMPILER), trying counterparts."
		findTool g++
		CPP_COMPILER=$returnedString
		DEBUG "CPP_COMPILER set to $CPP_COMPILER."

	fi

	MAKE_C="COMPILER_FAMILY=${COMPILER_FAMILY} GCC_ROOT=${GCC_ROOT} CC=${C_COMPILER}"
	export MAKE_C

	MAKE_CPP="COMPILER_FAMILY=${COMPILER_FAMILY} GCC_ROOT=${GCC_ROOT} CC=${CPP_COMPILER}"
	export MAKE_CPP

	# Updates build variables accordingly:
	findBuildTools


	DEBUG "gcc successfully installed."

	printEndList

	cd "$initial_dir"

}



cleangcc()
{
	LOG_STATUS "Cleaning gcc build tree..."
	${RM} -rf "$repository/${gcc_SOURCE_TREE}"
	${RM} -rf "$repository/${gcc_BUILD_TREE}"
}





################################################################################
# GNU binutils (binary utilities linked with gcc)
################################################################################



# If binutils is not compiled with the compiler-in-use, expect link errors
# (ex: DWARF unhandled form error).



getbinutils()
{
	DEBUG "Getting binutils..."
	launchFileRetrieval binutils
	return $?
}


preparebinutils()
{

	DEBUG "Preparing binutils..."

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

	printBeginList "binutils   "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${binutils_ARCHIVE} ${binutils_ARCHIVE}.save && ${BUNZIP2} -f ${binutils_ARCHIVE} && tar -xvf "binutils-${binutils_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ ! $? -eq 0 ] ; then
		ERROR "Unable to extract ${binutils_ARCHIVE}."
		DEBUG "Restoring ${binutils_ARCHIVE}."
		${MV} -f ${binutils_ARCHIVE}.save ${binutils_ARCHIVE}
		exit 10
	fi

	${MV} -f ${binutils_ARCHIVE}.save ${binutils_ARCHIVE}
	${RM} -f "binutils-${binutils_VERSION}.tar"

	printOK

}


generatebinutils()
{

	DEBUG "Generating binutils..."

	cd $repository/binutils-${binutils_VERSION}

	printItem "configuring"


	if [ -n "$prefix" ] ; then
		{
			setBuildEnv --exportEnv ./configure --prefix=${prefix}/binutils-${binutils_VERSION}
		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			setBuildEnv --exportEnv ./configure
		} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ ! $? -eq 0 ] ; then
		echo
		ERROR "Unable to configure binutils."
		exit 11
	fi

	printOK

	# Configure has guessed that 'CFLAGS = -g -O2' which is wrong, since some ld
	# (ex 2.11.90.0.8) are too old to use with gcc-3.3 and newer.  (ld doesn't
	# support debug info emitted by gcc-3.3).
	#
	# The fix is to compile binutils without -g, i.e. to use make CFLAGS=-O2.
	# Another more complex solution would have been to manage to pass
	# 'CFLAGS=-O2 configure'

	printItem "building"

	{
		setBuildEnv ${MAKE} CFLAGS=-O2 ${MAKE_C}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ ! $? -eq 0 ] ; then
		echo
		ERROR "Unable to build binutils."
		exit 12
	fi

	printOK

	printItem "installing"

	if [ -n "$prefix" ] ; then
		{
			echo "# binutils section." >> ${OSDL_ENV_FILE}

			echo "binutils_PREFIX=${prefix}/binutils-${binutils_VERSION}" >> ${OSDL_ENV_FILE}
			echo "export binutils_PREFIX" >> ${OSDL_ENV_FILE}
			echo "PATH=\$binutils_PREFIX/bin:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "LD_LIBRARY_PATH=\$binutils_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

			echo "" >> ${OSDL_ENV_FILE}

			PATH=${prefix}/binutils-${binutils_VERSION}/bin:${PATH}
			export PATH

			LD_LIBRARY_PATH=${prefix}/binutils-${binutils_VERSION}/lib:${LD_LIBRARY_PATH}
			export LD_LIBRARY_PATH

			setBuildEnv ${MAKE} install prefix=${prefix}/binutils-${binutils_VERSION} ${MAKE_C}

		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			setBuildEnv ${MAKE} install ${MAKE_C}
		} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ ! $? -eq 0 ] ; then
		echo
		ERROR "Unable to install binutils."
		exit 13
	fi

	printOK

	printEndList

	DEBUG "binutils successfully installed."

	cd "$initial_dir"

}


cleanbinutils()
{
	LOG_STATUS "Cleaning binutils build tree..."
	${RM} -rf "$repository/binutils-${binutils_VERSION}"
}





################################################################################
# GNU gdb: the GNU debugger
################################################################################


getgdb()
{
	DEBUG "Getting gdb..."
	launchFileRetrieval gdb
	return $?
}


preparegdb()
{

	DEBUG "Preparing gdb..."

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

	printBeginList "gdb        "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of bunzip2.
	{
		${CP} -f ${gdb_ARCHIVE} ${gdb_ARCHIVE}.save && ${BUNZIP2} -f ${gdb_ARCHIVE} && tar -xvf "gdb-${gdb_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ ! $? -eq 0 ] ; then
		ERROR "Unable to extract ${gdb_ARCHIVE}."
		DEBUG "Restoring ${gdb_ARCHIVE}."
		${MV} -f ${gdb_ARCHIVE}.save ${gdb_ARCHIVE}
		exit 10
	fi

	${MV} -f ${gdb_ARCHIVE}.save ${gdb_ARCHIVE}
	${RM} -f "gdb-${gdb_VERSION}.tar"

	printOK

}


generategdb()
{

	DEBUG "Generating gdb..."

	cd $repository/gdb-${gdb_VERSION}

	printItem "configuring"

	if [ -n "$prefix" ] ; then
		{
			setBuildEnv ./configure --prefix=${prefix}/gdb-${gdb_VERSION}
		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			setBuildEnv ./configure
		} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ ! $? -eq 0 ] ; then
		echo
		ERROR "Unable to configure gdb."
		exit 11
	fi

	printOK

	printItem "building"

	{
		setBuildEnv ${MAKE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ ! $? -eq 0 ] ; then
		echo
		# Make may fail because libncurses if lacking. Debian users can try:
		# 'apt-get build-dep gdb' before re-running LOANI.
		ERROR "Unable to build gdb (hint: is libncurses5-dev available?)."
		exit 12
	fi

	printOK

	printItem "installing"

	if [ -n "$prefix" ] ; then
		{
			echo "# gdb section." >> ${OSDL_ENV_FILE}

			echo "gdb_PREFIX=${prefix}/gdb-${gdb_VERSION}" >> ${OSDL_ENV_FILE}
			echo "export gdb_PREFIX" >> ${OSDL_ENV_FILE}
			echo "PATH=\$gdb_PREFIX/bin:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "LD_LIBRARY_PATH=\$gdb_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

			echo "" >> ${OSDL_ENV_FILE}
			setBuildEnv ${MAKE} install prefix=${prefix}/gdb-${gdb_VERSION} ${MAKE_C}

		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			setBuildEnv ${MAKE} install
		} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ ! $? -eq 0 ] ; then
		echo
		ERROR "Unable to install gdb."
		exit 13
	fi

	printOK

	DEBUG "gdb successfully installed."

	printEndList

	cd "$initial_dir"

}


cleangdb()
{
	LOG_STATUS "Cleaning gdb build tree..."
	${RM} -rf "$repository/gdb-${gdb_VERSION}"
}




################################################################################
# devkitARM: the ARM toolchain for Nintendo DS
################################################################################


getdevkitARM()
{
	DEBUG "Getting devkitARM..."
	launchFileRetrieval devkitARM
	return $?
}


preparedevkitARM()
{

	DEBUG "Preparing devkitARM..."

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

	printBeginList "devkitARM  "

	printItem "extracting"

	cd $repository

	if [ -n "$prefix" ] ; then
		devkitPRO_PREFIX=${ds_prefix}/devkitPro
	else
		devkitPRO_PREFIX=devkitPro
	fi

	${MKDIR} -p ${devkitPRO_PREFIX}


	TAR_FILE="devkitARM_r${devkitARM_VERSION}-linux.tar"

	# Prevent archive from disappearing because of bunzip2.
	{

		${CP} -f ${devkitARM_ARCHIVE} ${devkitARM_ARCHIVE}.save && ${BUNZIP2} -f ${devkitARM_ARCHIVE} && cd ${devkitPRO_PREFIX} && tar -xvf "$repository/${TAR_FILE}"

	} 1>>"$LOG_OUTPUT" 2>&1

	if [ ! $? -eq 0 ] ; then
		ERROR "Unable to extract ${devkitARM_ARCHIVE}."
		DEBUG "Restoring ${devkitARM_ARCHIVE}."
		cd "$repository"
		${MV} -f ${devkitARM_ARCHIVE}.save ${devkitARM_ARCHIVE}
		exit 10
	fi

	cd "$repository"
	${MV} -f ${devkitARM_ARCHIVE}.save ${devkitARM_ARCHIVE}
	${RM} -f "${TAR_FILE}"

	printOK

}


generatedevkitARM()
{

	DEBUG "Generating devkitARM..."


	printItem "configuring"
	printOK

	printItem "building"
	printOK

	printItem "installing"

	devkitARM_PREFIX=${devkitPRO_PREFIX}/devkitARM

	# Not depending on a prefix being set, needed in all cases:

	echo "# devkitARM section." >> ${OSDL_ENV_FILE}

	echo "devkitARM_PREFIX=${devkitARM_PREFIX}" >> ${OSDL_ENV_FILE}
	echo "export devkitARM_PREFIX" >> ${OSDL_ENV_FILE}
	echo "PATH=\$devkitARM_PREFIX:\${PATH}" >> ${OSDL_ENV_FILE}
	echo "" >> ${OSDL_ENV_FILE}

	echo "# Needed for PAlib:" >> ${OSDL_ENV_FILE}

	echo "DEVKITPRO=${devkitPRO_PREFIX}" >> ${OSDL_ENV_FILE}
	echo "export DEVKITPRO" >> ${OSDL_ENV_FILE}
	echo "" >> ${OSDL_ENV_FILE}

	echo "DEVKITARM=${devkitARM_PREFIX}" >> ${OSDL_ENV_FILE}
	echo "export DEVKITARM" >> ${OSDL_ENV_FILE}
	echo "" >> ${OSDL_ENV_FILE}

	PATH=${devkitARM_PREFIX}:${PATH}
	export PATH

	DEVKITPRO=${devkitPRO_PREFIX}
	export DEVKITPRO

	DEVKITARM=${devkitARM_PREFIX}
	export DEVKITARM

	printOK

	DEBUG "devkitARM successfully installed."

	printEndList

	cd "$initial_dir"

}


cleandevkitARM()
{
	LOG_STATUS "Cleaning devkitARM build tree..."
}




################################################################################
# grit: an image transformation program for the GBA/DS
################################################################################


getgrit()
{
	DEBUG "Getting grit..."
	launchFileRetrieval grit
	return $?
}


preparegrit()
{

	DEBUG "Preparing grit..."

	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "grit       "

	printItem "extracting"

	cd $repository

	grit_PREFIX=${ds_prefix}/grit-${grit_VERSION}

	${MKDIR} -p ${grit_PREFIX}

	# Extract grit sources in installation repository:
	{
		cd ${grit_PREFIX} && ${UNZIP} -o "$repository/${grit_ARCHIVE}"
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ ! $? -eq 0 ] ; then
		ERROR "Unable to extract ${grit_ARCHIVE}."
		exit 10
	fi

	cd $repository

	printOK

}


generategrit()
{

	DEBUG "Generating grit..."

	cd ${grit_PREFIX}/grit_src

	printItem "configuring"

	# Needing freeimage, use for example: 'apt-get install libfreeimage3'
	# Library link apparently not created on install (at least on Ubuntu Gutsy):
	# 'cd /usr/lib && ln -s libfreeimage.so.3 libfreeimage.so'

	# In grit_src/Makefile change 'LDFLAGS += -s -static' into
	# 'LDFLAGS += -s' as we are using a shared library for freeimage


	printOK

	printItem "building"

	${MAKE}
	printOK


	printItem "installing"

	echo "# grit section." >> ${OSDL_DS_ENV_FILE}

	echo "grit_PREFIX=${grit_PREFIX}" >> ${OSDL_DS_ENV_FILE}
	echo "export grit_PREFIX" >> ${OSDL_DS_ENV_FILE}
	echo "PATH=\$grit_PREFIX/bin:\${PATH}" >> ${OSDL_DS_ENV_FILE}

	PATH=${grit_PREFIX}/bin:${PATH}
	export PATH

	echo "" >> ${OSDL_DS_ENV_FILE}

	${MKDIR} -p ${grit_PREFIX}/bin
	${CP} -f grit ${grit_PREFIX}/bin


	printOK

	DEBUG "grit successfully installed."

	printEndList

	cd "$initial_dir"

}


cleangrit()
{
	LOG_STATUS "Cleaning grit build tree..."
	cd ${grit_PREFIX}/grit_src
	${MAKE} clean
}



################################################################################
# libnds: the low level library for Nintendo DS
################################################################################


getlibnds()
{
	DEBUG "Getting libnds..."
	launchFileRetrieval libnds
	return $?
}


preparelibnds()
{

	DEBUG "Preparing libnds..."

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

	printBeginList "libnds     "

	printItem "extracting"

	cd $repository

	libnds_PREFIX=${devkitPRO_PREFIX}/libnds

	${MKDIR} -p ${libnds_PREFIX}

	TAR_FILE="libnds-${libnds_VERSION}.tar"

	# Prevent archive from disappearing because of bunzip2.
	{

		${CP} -f ${libnds_ARCHIVE} ${libnds_ARCHIVE}.save && ${BUNZIP2} -f ${libnds_ARCHIVE} && cd ${libnds_PREFIX} && tar -xvf "$repository/${TAR_FILE}"

	} 1>>"$LOG_OUTPUT" 2>&1

	if [ ! $? -eq 0 ] ; then
		ERROR "Unable to extract ${libnds_ARCHIVE}."
		DEBUG "Restoring ${libnds_ARCHIVE}."
		cd $repository
		${MV} -f ${libnds_ARCHIVE}.save ${libnds_ARCHIVE}
		exit 10
	fi

	cd $repository

	${MV} -f ${libnds_ARCHIVE}.save ${libnds_ARCHIVE}
	${RM} -f "${TAR_FILE}"

	printOK

}


generatelibnds()
{

	DEBUG "Generating libnds..."


	printItem "configuring"
	printOK

	printItem "building"
	printOK

	printItem "installing"
	printOK

	DEBUG "libnds successfully installed."

	printEndList

	cd "$initial_dir"

}


cleanlibnds()
{
	LOG_STATUS "Cleaning libnds build tree..."
}




################################################################################
# PAlib: a higher level library for Nintendo DS
################################################################################


getPAlib()
{
	DEBUG "Getting PAlib..."
	launchFileRetrieval PAlib
	return $?
}


preparePAlib()
{

	DEBUG "Preparing PAlib..."

	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "PAlib      "

	printItem "extracting"

	cd $repository


	# Extract prebuilt executable in installation repository:
	{
		cd ${devkitPRO_PREFIX} && ${UNZIP} -o "$repository/${PAlib_ARCHIVE}"
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ ! $? -eq 0 ] ; then
		ERROR "Unable to extract ${PAlib_ARCHIVE}."
		exit 10
	fi

	cd $repository

	printOK

}


generatePAlib()
{

	DEBUG "Generating PAlib..."


	printItem "configuring"
	printOK

	printItem "building"
	printOK

	printItem "installing"

	PAlib_PREFIX=${devkitPRO_PREFIX}/PAlib

	libnds_PREFIX=${devkitPRO_PREFIX}/libnds


	# Not depending on a prefix being set, needed in all cases:

	LD_LIBRARY_PATH=$libnds_PREFIX/lib:${LD_LIBRARY_PATH}
	export LD_LIBRARY_PATH

	echo "# PAlib & prerequisites section." >> ${OSDL_ENV_FILE}

	echo "libnds_PREFIX=${libnds_PREFIX}" >> ${OSDL_ENV_FILE}
	echo "export libnds_PREFIX" >> ${OSDL_ENV_FILE}
	echo "LD_LIBRARY_PATH=\$libnds_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
	echo "" >> ${OSDL_ENV_FILE}

	echo "PAlib_PREFIX=${PAlib_PREFIX}" >> ${OSDL_ENV_FILE}
	echo "export PAlib_PREFIX" >> ${OSDL_ENV_FILE}
	echo "LD_LIBRARY_PATH=\$PAlib_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
	echo "" >> ${OSDL_ENV_FILE}

	LD_LIBRARY_PATH=$PAlib_PREFIX/lib:${LD_LIBRARY_PATH}
	export LD_LIBRARY_PATH


	echo "# Needed for PAlib internal use:" >> ${OSDL_ENV_FILE}
	echo "PAPATH=${PAlib_PREFIX}" >> ${OSDL_ENV_FILE}
	echo "export PAPATH" >> ${OSDL_ENV_FILE}
	echo "" >> ${OSDL_ENV_FILE}

	PAPATH=${PAlib_PREFIX}
	export PAPATH

	printOK

	DEBUG "PAlib successfully installed."

	printEndList

	cd "$initial_dir"

}


cleanPAlib()
{
	LOG_STATUS "Cleaning PAlib build tree..."
}



################################################################################
# dswifi: a library for Wifi management on the Nintendo DS
################################################################################


getdswifi()
{
	DEBUG "Getting dswifi..."
	launchFileRetrieval dswifi
	return $?
}


preparedswifi()
{

	DEBUG "Preparing dswifi..."

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

	printBeginList "dswifi     "

	printItem "extracting"

	cd $repository

	# Do not mess too much with LOANI repository:
	dswifi_repository="$repository/dswifi-${dswifi_VERSION}"

	${MKDIR} -p ${dswifi_repository}

	TAR_FILE="dswifi-${dswifi_VERSION}.tar"

	# Prevent archive from disappearing because of bunzip2.
	{
		${CP} -f ${dswifi_ARCHIVE} ${dswifi_ARCHIVE}.save && ${BUNZIP2} -f ${dswifi_ARCHIVE} && ${CP} -f ${TAR_FILE} ${dswifi_repository} && cd ${dswifi_repository} && tar -xvf ${TAR_FILE}
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ ! $? -eq 0 ] ; then
		ERROR "Unable to extract ${dswifi_ARCHIVE}."
		DEBUG "Restoring ${dswifi_ARCHIVE}."
		cd $repository
		${RM} -rf ${dswifi_repository}
		${MV} -f ${dswifi_ARCHIVE}.save ${dswifi_ARCHIVE}
		exit 10
	fi

	cd $repository
	${MV} -f ${dswifi_ARCHIVE}.save ${dswifi_ARCHIVE}
	${RM} -f "dswifi-${dswifi_VERSION}.tar"

	printOK

}


generatedswifi()
{

	DEBUG "Generating dswifi..."

	printItem "configuring"
	printOK

	printItem "building"
	printOK

	printItem "installing"

	# No real dswifi_PREFIX: will be integrated in PAlib directories.

	${CP} -rf ${dswifi_repository}/lib/* ${PAlib_PREFIX}/lib
	${CP} -rf ${dswifi_repository}/include/* ${PAlib_PREFIX}/include/nds

	printOK

	DEBUG "dswifi successfully installed."

	printEndList

	cd "$initial_dir"

}


cleandswifi()
{
	LOG_STATUS "Cleaning dswifi build tree..."
	${RM} -rf ${dswifi_repository}
}



################################################################################
# DeSmuME: a Nintendo DS emulator
################################################################################


getDeSmuME()
{
	DEBUG "Getting DeSmuME..."
	launchFileRetrieval DeSmuME
	return $?
}


prepareDeSmuME()
{

	DEBUG "Preparing DeSmuME..."

	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "DeSmuME    "

	printItem "extracting"

	DeSmuME_PREFIX="$ds_prefix/DeSmuME-${DeSmuME_VERSION}"
	${MKDIR} -p ${DeSmuME_PREFIX}

	# Extract prebuilt executable in installation repository:
	{
		cd ${DeSmuME_PREFIX} && ${UNZIP} -o "$repository/${DeSmuME_ARCHIVE}"
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ ! $? -eq 0 ] ; then
		ERROR "Unable to extract ${DeSmuME_ARCHIVE}."
		exit 10
	fi

	cd $repository

	printOK

}


generateDeSmuME()
{

	DEBUG "Generating DeSmuME..."

	printItem "configuring"
	printOK

	printItem "building"
	printOK

	printItem "installing"

	echo "# DeSmuME section." >> ${OSDL_ENV_FILE}

	echo "DeSmuME_PREFIX=${DeSmuME_PREFIX}" >> ${OSDL_ENV_FILE}
	echo "export DeSmuME_PREFIX" >> ${OSDL_ENV_FILE}
	echo "PATH=\$DeSmuME_PREFIX:\${PATH}" >> ${OSDL_ENV_FILE}
	echo "alias desmume=\"wine $DeSmuME_PREFIX/NDeSmuME.exe\"" >> ${OSDL_ENV_FILE}
	echo "" >> ${OSDL_ENV_FILE}

	PATH=${DeSmuME_PREFIX}:${PATH}
	export PATH

	alias desmume="wine $DeSmuME_PREFIX/NDeSmuME.exe"
	printOK

	DEBUG "DeSmuME successfully installed."

	printEndList

	cd "$initial_dir"

}


cleanDeSmuME()
{
	LOG_STATUS "Cleaning DeSmuME build tree..."
}



################################################################################
# NoCashGBA: another Nintendo DS emulator
################################################################################


getNoCashGBA()
{
	DEBUG "Getting NoCashGBA..."
	launchFileRetrieval NoCashGBA
	return $?
}


prepareNoCashGBA()
{

	DEBUG "Preparing NoCashGBA..."

	if findTool unzip ; then
		UNZIP=$returnedString
	else
		ERROR "No unzip tool found, whereas some files have to be unzipped."
		exit 8
	fi

	printBeginList "NoCashGBA  "

	printItem "extracting"


	NoCashGBA_PREFIX="$ds_prefix/NoCashGBA-${NoCashGBA_VERSION}"
	${MKDIR} -p ${NoCashGBA_PREFIX}

	# Extract prebuilt executable in installation repository:
	{
		cd ${NoCashGBA_PREFIX} && ${UNZIP} -o "$repository/${NoCashGBA_ARCHIVE}"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ ! $? -eq 0 ] ; then
		ERROR "Unable to extract ${NoCashGBA_ARCHIVE}."
		exit 10
	fi

	printOK

	cd $repository

}


generateNoCashGBA()
{

	DEBUG "Generating NoCashGBA..."

	printItem "configuring"
	printOK

	printItem "building"
	printOK

	printItem "installing"


	${MV} -f ${NoCashGBA_PREFIX}/'NO$GBA.EXE' ${NoCashGBA_PREFIX}/NoCashGBA.EXE

	# Not depending on a prefix being set, needed in all cases:

	echo "# NoCashGBA section." >> ${OSDL_ENV_FILE}

	echo "NoCashGBA_PREFIX=${NoCashGBA_PREFIX}" >> ${OSDL_ENV_FILE}
	echo "export NoCashGBA_PREFIX" >> ${OSDL_ENV_FILE}
	echo "PATH=\$NoCashGBA_PREFIX:\${PATH}" >> ${OSDL_ENV_FILE}
	echo "alias nocashgba=\"wine $NoCashGBA_PREFIX/NoCashGBA.EXE\"" >> ${OSDL_ENV_FILE}
	echo "" >> ${OSDL_ENV_FILE}

	PATH=${NoCashGBA_PREFIX}:${PATH}
	export PATH

	alias nocashgba="wine $NoCashGBA_PREFIX/NoCashGBA.EXE"

	printOK

	DEBUG "NoCashGBA successfully installed."

	printEndList

	cd "$initial_dir"

}


cleanNoCashGBA()
{
	LOG_STATUS "Cleaning NoCashGBA build tree..."
}


################################################################################
# End of loani-commonBuildTools.sh
################################################################################
