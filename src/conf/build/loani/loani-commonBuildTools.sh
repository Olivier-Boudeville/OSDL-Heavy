# This script is made to be sourced by LOANI when retrieving basic 
# common build tools.
# Most of them is expected to be already available in user'system.

# Creation date : 2004, February 22.
# Author : Olivier Boudeville (olivier.boudeville@online.fr)


# Basic common build tools section.

COMMON_BUILD_TOOLS="gcc binutils gdb"

# Automake, autconf, aclocal, make, etc. are deemed most basic build tools
# and are supposed to be available if needed.


# Updating retrieve list.
target_list="$COMMON_BUILD_TOOLS $target_list"


DEBUG "Scheduling retrieval of common build tools ($COMMON_BUILD_TOOLS)."




################################################################################
# GNU gcc (compiler suite)
################################################################################


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
	
		
	if [ $? != "0" ] ; then
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
	
	# Restricted to C++ since java would too often not bootstrap, or be slow to bootstrap.
	
	if [ -n "$prefix" ] ; then	
		{
			${MAKE_C} ../${gcc_SOURCE_TREE}/configure --prefix=${prefix}/gcc-${gcc_VERSION} --enable-languages=c++
		} 1>>"$LOG_OUTPUT" 2>&1		
	else
		{
			${MAKE_C} ../${gcc_SOURCE_TREE}/configure --enable-languages=c++
		} 1>>"$LOG_OUTPUT" 2>&1		
	fi
		
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to configure gcc."
		exit 11
	fi	
	
	printOK	
	
	printItem "building"
	
	
	{
		${MAKE} bootstrap-lean ${MAKE_C}
	} 1>>"$LOG_OUTPUT" 2>&1	 
	
	if [ $? != "0" ] ; then
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
		
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to install gcc."
		exit 13
	fi	
	
	printOK
	
	# Now updating all build-related environment variables :
	 
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
	
	# Updates build variables accordingly : 
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


# If binutils is not compiled with the compiler-in-use, expect link errors. 
# (ex : DWARF unhandled form error).


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
	
		
	if [ $? != "0" ] ; then
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
		

	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to configure binutils."
		exit 11
	fi	
	
	printOK	

	# Configure has guessed that 'CFLAGS = -g -O2' which is wrong, since 
	# some ld (ex 2.11.90.0.8) are too old to use with gcc-3.3 and newer.
	# (ld  doesn't support debug info emitted by gcc-3.3).
	# The fix is to compile binutils without -g, i.e. to use make CFLAGS=-O2.
	# Another more complex solution would have been to manage to pass 'CFLAGS=-O2 configure'
	
	printItem "building"
		
	{
		setBuildEnv ${MAKE} CFLAGS=-O2 ${MAKE_C} 
	} 1>>"$LOG_OUTPUT" 2>&1	 
	
	if [ $? != "0" ] ; then
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
				
	if [ $? != "0" ] ; then
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
# GNU gdb : the GNU debugger
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
	
		
	if [ $? != "0" ] ; then
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
	
	# Restricted to C++ since java would too often not bootstrap, or be slow to bootstrap.
	
	if [ -n "$prefix" ] ; then	
		{
			setBuildEnv ./configure --prefix=${prefix}/gdb-${gdb_VERSION} 
		} 1>>"$LOG_OUTPUT" 2>&1		
	else
		{
			setBuildEnv ./configure
		} 1>>"$LOG_OUTPUT" 2>&1		
	fi
		
	if [ $? != "0" ] ; then
		echo
		ERROR "Unable to configure gdb."
		exit 11
	fi	
	
	printOK	
	
	printItem "building"
		
	{
		setBuildEnv ${MAKE}
	} 1>>"$LOG_OUTPUT" 2>&1	 
	
	if [ $? != "0" ] ; then
		echo
		# Make may fail because libncurses if lacking :
		# Debian users can try : 'apt-get build-dep gdb'
		# before re-running LOANI.
		ERROR "Unable to build gdb (hint : is libncurses5-dev available ?)."
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
		
	if [ $? != "0" ] ; then
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


####################################################################################################
# End of loani-commonBuildTools.sh
####################################################################################################
