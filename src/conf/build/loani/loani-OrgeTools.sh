# This script is made to be sourced by LOANI when retrieving Orge tools.
# These tools are required only for server-side Orge.

# Creation date : Monday, July 7, 2008.
# Author : Olivier Boudeville (olivier.boudeville@online.fr)


# Orge tools section.

#ORGE_TOOLS="Erlang egeoip Geolite Orge"
ORGE_TOOLS="Erlang egeoip Geolite"


# Updating retrieve list :
# (new tools are put ahead of those already selected, so that CVS retrievals do not delay them)
if [ $is_windows -eq 0 ] ; then
  WARNING "on Windows, no Orge tool managed."
else
  # Put at end rather than begin so that any newly installed build tool 
  #(ex: gcc) can be used nevertheless:
  target_list="$target_list $ORGE_TOOLS"
fi

DEBUG "Scheduling retrieval of Orge tools ($ORGE_TOOLS)."



################################################################################
# Erlang
################################################################################


getErlang()
{
	DEBUG "Getting Erlang..."
	launchFileRetrieval Erlang
}


prepareErlang()
{
	DEBUG "Preparing Erlang..."
	
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
	
	printBeginList "Erlang     "
	
	printItem "extracting"
	
	cd $repository
	
	# Prevent archive from disappearing because of gunzip.
	{
		${CP} -f ${Erlang_ARCHIVE} ${Erlang_ARCHIVE}.save && ${GUNZIP} -f ${Erlang_ARCHIVE} && tar -xvf "otp_src_${Erlang_VERSION}.tar" 
	} 1>>"$LOG_OUTPUT" 2>&1
	
		
	if [ $? != 0 ] ; then
		ERROR "Unable to extract ${Erlang_ARCHIVE}."
		DEBUG "Restoring ${Erlang_ARCHIVE}."
		${MV} -f ${Erlang_ARCHIVE}.save ${Erlang_ARCHIVE} 
		exit 10
	fi
	
	${MV} -f ${Erlang_ARCHIVE}.save ${Erlang_ARCHIVE} 
	${RM} -f "Erlang-${Erlang_VERSION}.tar"
			
	printOK
	
}


generateErlang()
{
	DEBUG "Generating Erlang..."

	cd $repository/otp_src_${Erlang_VERSION}
	
	printItem "configuring"	

	ERLANG_CONFIGURE_OPT="--enable-threads --enable-smp-support --enable-kernel-poll --enable-hipe"
	
	ERLANG_PREFIX=$prefix/Erlang-${Erlang_VERSION}
	
	if [ -n "$prefix" ] ; then	
		{
			${MKDIR} -p ${ERLANG_PREFIX}
			setBuildEnv --exportEnv ./configure ${ERLANG_CONFIGURE_OPT} --prefix=${ERLANG_PREFIX}
		} 1>>"$LOG_OUTPUT" 2>&1		
	else
		{
			setBuildEnv --exportEnv ./configure	${ERLANG_CONFIGURE_OPT}
		} 1>>"$LOG_OUTPUT" 2>&1		
	fi
		

	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to configure Erlang."
		exit 11
	fi	
	
	printOK	
	
	printItem "building"
	
	{
		setBuildEnv ${MAKE} 
	} 1>>"$LOG_OUTPUT" 2>&1	 
	
	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build Erlang."
		exit 12
	fi
	
	
	printOK

	printItem "installing"

	if [ -n "$prefix" ] ; then	
		{		
			
			# First OSDL config file:	
			echo "# Erlang section." >> ${OSDL_ENV_FILE}
			echo "Erlang_PREFIX=${ERLANG_PREFIX}" >> ${OSDL_ENV_FILE}
			echo "export Erlang_PREFIX" >> ${OSDL_ENV_FILE}
			echo "PATH=\$Erlang_PREFIX/bin:\${PATH}" >> ${OSDL_ENV_FILE}		
			echo "LD_LIBRARY_PATH=\$Erlang_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${OSDL_ENV_FILE}
			echo "" >> ${OSDL_ENV_FILE}
			
			
			# Then Orge config file:	
			echo "# Erlang section." >> ${ORGE_ENV_FILE}
			echo "Erlang_PREFIX=${ERLANG_PREFIX}" >> ${ORGE_ENV_FILE}
			echo "export Erlang_PREFIX" >> ${ORGE_ENV_FILE}
			echo "PATH=\$Erlang_PREFIX/bin:\${PATH}" >> ${ORGE_ENV_FILE}		
			echo "LD_LIBRARY_PATH=\$Erlang_PREFIX/lib:\${LD_LIBRARY_PATH}" >> ${ORGE_ENV_FILE}
			echo "" >> ${ORGE_ENV_FILE}
			
			
			${MAKE} install prefix=${ERLANG_PREFIX} 

		} 1>>"$LOG_OUTPUT" 2>&1		
	else
		{		
			${MAKE} install 
		} 1>>"$LOG_OUTPUT" 2>&1			
	fi
			
	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install Erlang."
		exit 13
	fi	
	
	printOK

	printEndList
	
	DEBUG "Erlang successfully installed."
	
	cd "$initial_dir"
	
}


cleanErlang()
{
	LOG_STATUS "Cleaning Erlang build tree..."
	${RM} -rf "$repository/${ERLANG_PREFIX}"
}



################################################################################
# egeoip: Erlang tool for the geolocation of IP addresses.
################################################################################


getegeoip()
{
	DEBUG "Getting egeoip..."

	declareRetrievalBegin "egeoip (from SVN)"

	# To avoid a misleading message when the retrieval is finished:
	egeoip_ARCHIVE="from SVN"

	cd $repository
	
	{
		${SVN} co http://${egeoip_SVN_SERVER}/svn/trunk/ egeoip-read-only ${SVN_OPT}
	} 1>>"$LOG_OUTPUT" 2>&1
	
	if [ $? != 0 ] ; then
		ERROR "Unable to check-out egeoip."
		exit 10
	fi
	
}


prepareegeoip()
{

	DEBUG "Preparing egeoip..."
		
	printBeginList "egeoip     "
	
	printItem "extracting"
				
	printOK
	
}


generateegeoip()
{
	DEBUG "Generating egeoip..."

	egeoip_root=$repository/egeoip-read-only/egeoip
	
	cd $egeoip_root
	
	printItem "configuring"	

	{
		setBuildEnv ${MAKE} clean
	} 1>>"$LOG_OUTPUT" 2>&1	 
	
	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to clean egeoip initially."
		exit 11
	fi
		
	printOK	
	
	printItem "building"
		
	{
		setBuildEnv ${MAKE} 
	} 1>>"$LOG_OUTPUT" 2>&1	 
	
	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to build egeoip."
		exit 12
	fi
		
	printOK

	printItem "installing"

	{	
	
		EGEOIP_PREFIX=${prefix}/egeoip
		
		echo "# egeoip section." >> ${OSDL_ENV_FILE}
			
		echo "egeoip_PREFIX=${EGEOIP_PREFIX}" >> ${OSDL_ENV_FILE}
		echo "# See ${egeoip_PREFIX}/ebin and ${egeoip_PREFIX}/include" >> ${OSDL_ENV_FILE}
			
		echo "" >> ${OSDL_ENV_FILE}

		${MKDIR} ${EGEOIP_PREFIX}
		${CP} -rf ${egeoip_root}/* ${EGEOIP_PREFIX}
		

	} 1>>"$LOG_OUTPUT" 2>&1		
			
	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install egeoip."
		exit 13
	fi	
	
	printOK

	printEndList
	
	DEBUG "egeoip successfully installed."
	
	cd "$initial_dir"
	
}


cleanegeoip()
{
	LOG_STATUS "Cleaning egeoip..."
	${RM} -rf "$repository/egeoip-read-only"
}






################################################################################
# Geolite: free database for IP to location conversions, offered by
# MaxMind (see http://www.maxmind.com/app/geolitecity)
################################################################################


getGeolite()
{
	DEBUG "Getting Geolite database..."
	launchFileRetrieval Geolite
}


prepareGeolite()
{
	DEBUG "Preparing Geolite..."
	
	
	printBeginList "Geolite    "
	
	printItem "extracting"
	
			
	printOK
	
}


generateGeolite()
{
	DEBUG "Generating Geolite..."

	printItem "configuring"	
	
	printOK	
	
	printItem "building"
			
	printOK

	printItem "installing"

	cd $repository
	
	{
		${CP} ${Geolite_ARCHIVE} ${EGEOIP_PREFIX}/priv/
	} 1>>"$LOG_OUTPUT" 2>&1	 
				
	if [ $? != 0 ] ; then
		echo
		ERROR "Unable to install Geolite."
		exit 13
	fi	
	
	printOK

	printEndList
	
	DEBUG "Geolite successfully installed."
	
	cd "$initial_dir"
	
}


cleanGeolite()
{
	LOG_STATUS "Cleaning Geolite database..."
	${RM} -rf "$egeoip_root/priv/${Geolite_ARCHIVE}"
}



################################################################################
# End of loani-OrgeTools.sh
################################################################################
