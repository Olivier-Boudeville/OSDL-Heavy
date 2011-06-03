# This script is made to be sourced by LOANI when retrieving optional tools.
# These tools are not required, but are deemed useful.

# Creation date: 2004, February 22.
# Author: Olivier Boudeville (olivier.boudeville@online.fr)


# Optional tools section.

# tidy deactivated since seldom used and causes problems
# (configure step fails with libtool, md5sum not constant since no
# version number provided)

# Note: if this list is to be updated, update as well:
# osdl/OSDL/trunk/src/conf/build/loani/update-LOANI-mirror.sh
OPTIONAL_TOOLS="dot doxygen"


# Updating retrieve list:
# (new tools are put ahead of those already selected, so that CVS retrievals do not delay them)
if [ $is_windows -eq 0 ] ; then
  WARNING "on Windows, no optional tool managed."
else
  target_list="$OPTIONAL_TOOLS $target_list"
fi

DEBUG "Scheduling retrieval of optional tools ($OPTIONAL_TOOLS)."




################################################################################
# Dot
################################################################################


getdot()
{
	DEBUG "Getting dot..."
	launchFileRetrieval dot
}


preparedot()
{
	DEBUG "Preparing dot..."

	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunziped."
		exit 14
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 15
	fi

	printBeginList "dot        "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${dot_ARCHIVE} ${dot_ARCHIVE}.save && ${GUNZIP} -f ${dot_ARCHIVE} && tar -xvf "graphviz-${dot_VERSION}.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${dot_ARCHIVE}."
		DEBUG "Restoring ${dot_ARCHIVE}."
		${MV} -f ${dot_ARCHIVE}.save ${dot_ARCHIVE}
		exit 10
	fi

	${MV} -f ${dot_ARCHIVE}.save ${dot_ARCHIVE}
	${RM} -f "dot-${dot_VERSION}.tar"

	printOK

}


generatedot()
{
	DEBUG "Generating dot..."

	cd $repository/graphviz-${dot_VERSION}

	printItem "configuring"

	if [ -n "$prefix" ] ; then
		{
			${MKDIR} -p $prefix/dot-graphviz-${dot_VERSION}
			setBuildEnv --exportEnv ./configure --prefix=$prefix/dot-graphviz-${dot_VERSION}
		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			setBuildEnv --exportEnv ./configure
		} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure dot."
		exit 11
	fi

	printOK

	printItem "building"

	# tk should be installed, for the build to be successful.

	{
		setBuildEnv ${MAKE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build dot."
		exit 12
	fi

	# Disabled since graphviz-2.4 version: check fails while in tclpkg/gv
	# for target test_perl: no 'perl' directory.
	#
	#{
	#	${MAKE} check
	#} 1>>"$LOG_OUTPUT" 2>&1
	#
	#if [ $? != 0 ] ; then
	#	echo
	#	ERROR "Unable to build dot: auto-check failed."
	#	exit 12
	#fi

	printOK

	printItem "installing"

	if [ -n "$prefix" ] ; then
		{
			echo "# dot section." >> ${OSDL_ENV_FILE}

			echo "dot_PREFIX=${prefix}/dot-graphviz-${dot_VERSION}" >> ${OSDL_ENV_FILE}
			echo "export dot_PREFIX" >> ${OSDL_ENV_FILE}
			echo "PATH=\$dot_PREFIX/bin:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "LD_LIBRARY_PATH=\$dot_PREFIX/lib/graphviz:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

			echo "" >> ${OSDL_ENV_FILE}
			${MAKE} install prefix=${prefix}/dot-graphviz-${dot_VERSION}

		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			${MAKE} install
		} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install dot."
		exit 13
	fi

	printOK

	printEndList

	DEBUG "dot successfully installed."

	cd "$initial_dir"

}


cleandot()
{
	LOG_STATUS "Cleaning graphwiz (dot) build tree..."
	${RM} -rf "$repository/graphviz-${dot_VERSION}"
}




################################################################################
# Doxygen
################################################################################

getdoxygen()
{
	DEBUG "Getting doxygen..."
	launchFileRetrieval doxygen
}


preparedoxygen()
{
	DEBUG "Preparing doxygen..."

	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunziped."
		exit 14
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 15
	fi

	printBeginList "doxygen    "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${doxygen_ARCHIVE} ${doxygen_ARCHIVE}.save && ${GUNZIP} -f ${doxygen_ARCHIVE} && tar -xvf "doxygen-${doxygen_VERSION}.src.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${doxygen_ARCHIVE}."
		DEBUG "Restoring ${doxygen_ARCHIVE}."
		${MV} -f ${doxygen_ARCHIVE}.save ${doxygen_ARCHIVE}
		exit 10
	fi

	${MV} -f ${doxygen_ARCHIVE}.save ${doxygen_ARCHIVE}
	${RM} -f "doxygen-${doxygen_VERSION}.tar"

	printOK

}


generatedoxygen()
{
	DEBUG "Generating doxygen..."

	cd $repository/doxygen-${doxygen_VERSION}

	printItem "configuring"

	if [ -n "$prefix" ] ; then
		{
			${MKDIR} -p $prefix/doxygen-${doxygen_VERSION}
			setBuildEnv --exportEnv ./configure --prefix $prefix/doxygen-${doxygen_VERSION}
		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			setBuildEnv --exportEnv ./configure
		} 1>>"$LOG_OUTPUT" 2>&1
	fi


	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure doxygen."
		exit 11
	fi

	printOK

	printItem "building"

	# The doxygen build seems to search in some cases some include files
	# with no luck:
	# "/usr/include/string.h:33:20: stddef.h: No such file or directory"
	# It should have been searched through the include files for the
	# LOANI-installed gcc.

	{
		setBuildEnv ${MAKE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build doxygen."
		exit 12
	fi

	# Disabled since relies on too many prerequisites.
	#{
	#	${MAKE} docs
	#} 1>>"$LOG_OUTPUT" 2>&1

	#if [ $? != 0 ] ; then
	#	echo
	#	ERROR "Unable to build doxygen doc."
	#	exit 12
	#fi
	printOK

	printItem "installing"

	if [ -n "$prefix" ] ; then
		{
			echo "# doxygen section." >> ${OSDL_ENV_FILE}

			echo "doxygen_PREFIX=$prefix/doxygen-${doxygen_VERSION}" >> ${OSDL_ENV_FILE}
			echo "export doxygen_PREFIX" >> ${OSDL_ENV_FILE}
			echo "PATH=\$doxygen_PREFIX/bin:\${PATH}" >> ${OSDL_ENV_FILE}						echo "LD_LIBRARY_PATH=\$doxygen_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

			echo "" >> ${OSDL_ENV_FILE}

			${MAKE} install prefix=${prefix}/doxygen-${doxygen_VERSION}

		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			${MAKE} install
		} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install doxygen."
		exit 13
	fi

	printOK

	printEndList

	DEBUG "doxygen successfully installed."

	cd "$initial_dir"

}


cleandoxygen()
{
	LOG_STATUS "Cleaning doxygen build tree..."
	${RM} -rf "$repository/doxygen-${doxygen_VERSION}"
}





################################################################################
# tidy
################################################################################



gettidy()
{
	DEBUG "Getting tidy..."
	launchFileRetrieval tidy
}


preparetidy()
{

	DEBUG "Preparing tidy..."

	if findTool gunzip ; then
		GUNZIP=$returnedString
	else
		ERROR "No gunzip tool found, whereas some files have to be gunziped."
		exit 14
	fi

	if findTool tar ; then
		TAR=$returnedString
	else
		ERROR "No tar tool found, whereas some files have to be detarred."
		exit 15
	fi

	printBeginList "tidy       "

	printItem "extracting"

	cd $repository

	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${tidy_ARCHIVE} ${tidy_ARCHIVE}.save && ${GUNZIP} -f ${tidy_ARCHIVE} && tar -xvf "tidy_src.tar"
	} 1>>"$LOG_OUTPUT" 2>&1


	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${tidy_ARCHIVE}."
		DEBUG "Restoring ${tidy_ARCHIVE}."
		${MV} -f ${tidy_ARCHIVE}.save ${tidy_ARCHIVE}
		exit 10
	fi

	${MV} -f ${tidy_ARCHIVE}.save ${tidy_ARCHIVE}
	${RM} -f "tidy-${tidy_VERSION}.tar"

	printOK

}


generatetidy()
{
	DEBUG "Generating tidy..."

	cd $repository/tidy

	printItem "configuring"

	/bin/sh build/gnuauto/setup.sh 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to run tidy setup."
		exit 11
	fi

	if [ -n "$prefix" ] ; then
		{
			${MKDIR} -p $prefix/tidy
			setBuildEnv --exportEnv ./configure --prefix=$prefix/tidy
		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			setBuildEnv --exportEnv ./configure
		} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure tidy."
		exit 11
	fi

	printOK

	printItem "building"


	{
		setBuildEnv ${MAKE}
	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build tidy."
		exit 12
	fi

	printOK

	printItem "installing"

	if [ -n "$prefix" ] ; then
		{
			echo "# tidy section." >> ${OSDL_ENV_FILE}

			echo "tidy_PREFIX=$prefix/tidy" >> ${OSDL_ENV_FILE}
			echo "export tidy_PREFIX" >> ${OSDL_ENV_FILE}
			echo "PATH=\$tidy_PREFIX/bin:\${PATH}" >> ${OSDL_ENV_FILE}
			echo "LD_LIBRARY_PATH=\$tidy_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}

			echo "" >> ${OSDL_ENV_FILE}
			${MAKE} install prefix=${prefix}/tidy

		} 1>>"$LOG_OUTPUT" 2>&1
	else
		{
			${MAKE} install
		} 1>>"$LOG_OUTPUT" 2>&1
	fi

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install tidy."
		exit 13
	fi

	printOK

	printEndList

	DEBUG "tidy successfully installed."

	cd "$initial_dir"

}


cleantidy()
{
	LOG_STATUS "Cleaning tidy build tree..."
	${RM} -rf "$repository/tidy"
}


################################################################################
# End of loani-optionalTools.sh
################################################################################
