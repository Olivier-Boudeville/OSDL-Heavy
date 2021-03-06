# This script is made to be sourced by LOANI when retrieving Orge tools.
# These tools are required only for server-side Orge.

# Creation date: Monday, July 7, 2008.
# Author: Olivier Boudeville (olivier.boudeville@esperide.com)


# Orge tools section.

# Erlang not listed, as is dynamically added:
ORGE_TOOLS="egeoip Geolite Ceylan_Erlang Orge"
#ORGE_TOOLS="egeoip Geolite"
#ORGE_TOOLS="egeoip Geolite Ceylan_Erlang Orge"
#ORGE_TOOLS="Orge"


# Updating retrieve list:
#
# (new tools are put ahead of those already selected, so that VCS retrievals do
# not delay them)


# Any user-installed version is expected to be already in the PATH:
target_erlang=`which erl`
#target_erlang="/usr/local/bin/erl"


if [ -x "$target_erlang" ] ; then

	found_erlang_version=`$target_erlang -noinput -eval 'io:format( "~s", [erlang:system_info(otp_release)]),halt()'`

	if [ "$found_erlang_version" = "$Erlang_VERSION" ] ; then

		DISPLAY "Erlang found in $target_erlang in the correct version ($found_erlang_version), thus not installing it (expecting it to have been built with the proper options)."

		install_erlang=1

	else

		WARNING "Erlang found in $target_erlang but not in the correct version (found $found_erlang_version, wanted $Erlang_VERSION), thus installing it nevertheless."

		ORGE_TOOLS="Erlang $ORGE_TOOLS"

	fi


else

	ORGE_TOOLS="Erlang $ORGE_TOOLS"

fi


if [ $is_windows -eq 0 ] ; then
	
	WARNING "on Windows, no Orge tool managed."

else
	
  # Put at end rather than begin so that any newly installed build tool
  # (ex: gcc) can be used nevertheless:
	target_list="$target_list $ORGE_TOOLS"

fi


# Removes that misleading directory coming from the LOANI archive:
if [ ! -d "${WINDOWS_SOLUTIONS_ROOT}" ] ; then

	${RM} -rf "${WINDOWS_SOLUTIONS_ROOT}"

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

	printBeginList "Erlang        "

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

			# First OSDL config file:
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

			# Use that version of Erlang to compile next LOANI-based packages:
			export PATH="${ERLANG_PREFIX}/bin:$PATH"


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

	declareRetrievalBegin "egeoip (from git)"

	# To avoid a misleading message when the retrieval is finished:
	egeoip_ARCHIVE="from git"

	cd $repository

	LOG_STATUS "Getting egeoip in its source directory ${repository}..."

	if [ -d "egeoip" ] ; then

		# Otherwise git will refuse to clone again:
		/bin/rm -rf egeoip

	fi

	{
		# Previously was using a SVN server:
		# ${SVN} co http://${egeoip_SVN_SERVER}/svn/trunk/ egeoip-read-only
		# ${SVN_OPT}

		# Now using git:
		${GIT} clone http://${egeoip_GIT_SERVER}/mochi/egeoip.git

	} 1>>"$LOG_OUTPUT" 2>&1

	if [ $? != 0 ] ; then
		ERROR "Unable to clone egeoip."
		exit 10
	fi

}



prepareegeoip()
{

	DEBUG "Preparing egeoip..."

	printBeginList "egeoip        "

	printItem "extracting"

	# Nothing to do, as retrieved from git.

	printOK

}



generateegeoip()
{
	DEBUG "Generating egeoip..."

	egeoip_root=$repository/egeoip

	cd $egeoip_root

	printItem "configuring"

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
		echo "# See \${egeoip_PREFIX}/ebin" >> ${OSDL_ENV_FILE}

		echo "" >> ${OSDL_ENV_FILE}

		${MKDIR} ${EGEOIP_PREFIX}

		for d in ebin support; do
			${CP} -rf ${egeoip_root}/* ${EGEOIP_PREFIX}
		done

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
# MaxMind (see http://www.maxmind.com/app/geolitecity)
################################################################################



getGeolite()
{
	DEBUG "Getting Geolite database..."
	launchFileRetrieval Geolite
}



prepareGeolite()
{
	DEBUG "Preparing Geolite..."


	printBeginList "Geolite       "

	printItem "extracting"

	# Nothing to do, as egeoip will directly read the .tar.gz file.

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

	# If a prefix is used, the database is installed in the egeoip installation
	# directory (not in repository), otherwise it is installed in
	# /usr/local/share/GeoIP/ and a link is made to it from egeoip:
	cd $repository



	if [ -z "${prefix}" ] ; then

		# No prefix, installing directly in system tree:
		{

			geolite_dir="/usr/local/share/GeoIP"

			${MKDIR} -p ${geolite_dir} && ${CP} ${Geolite_ARCHIVE} ${geolite_dir} && cd ${EGEOIP_PREFIX}/priv/ && ${LN} -s ${geolite_dir}/${Geolite_ARCHIVE}

		} 1>>"$LOG_OUTPUT" 2>&1

	else

		{

			geolite_dir="${EGEOIP_PREFIX}/priv/"

			# A symlink could be made:
			${MKDIR} -p ${geolite_dir} && ${CP} ${Geolite_ARCHIVE} ${EGEOIP_PREFIX}/priv/

		} 1>>"$LOG_OUTPUT" 2>&1


		loani_final_hint="$loani_final_hint
In order to share a unique Geolite database, we recommend that you run as root:
${MKDIR} -p ${geolite_dir} && ${MV} -f ${EGEOIP_PREFIX}/priv/${Geolite_ARCHIVE} ${geolite_dir} && cd ${EGEOIP_PREFIX}/priv/ && ${LN} -s ${geolite_dir}/${Geolite_ARCHIVE}"

	fi

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
# Ceylan_Erlang: The Erlang base developments gathered by Ceylan.
################################################################################


# Manage back-up directory if necessary.
# (helper)
manage_package_backup()
{

	package=$1

	if [ -d "${repository}/$package" ] ; then

		if [ -d "${repository}/$package.save" ] ; then

			if [ $be_strict -eq 0 ] ; then

				ERROR "A back-up directory for $package is already existing, please remove it first (${repository}/$package.save)"
				exit 5

			else

				WARNING "Deleting already existing back-up directory for $package (removing ${repository}/$package.save)"

				${RM} -rf "${repository}/$package.save" 2>/dev/null

				# Sometimes rm fails apparently (long names or other reasons):
				${MV} -f ${repository}/$package.save ${repository}/$package.save-`date '+%Hh-%Mm-%Ss'` 2>/dev/null

			fi
		fi

		${MV} -f ${repository}/$package ${repository}/$package.save 2>/dev/null

		WARNING "A back-up directory for $package was already existing (${repository}/$package), it has been moved to ${repository}/$package.save."

	fi

}



getCeylan_Erlang()
{

	DEBUG "Getting Ceylan-Erlang..."

	# We prefer using VCS here:
	# (simplified version of getCeylan in loani-requiredTools.sh)

	declareRetrievalBegin "Ceylan-Erlang (from ${current_vcs})"

	# To avoid a misleading message when the retrieval is finished:
	Ceylan_Erlang_ARCHIVE="from ${current_vcs}"

	cd ${repository}

	${MKDIR} -p Ceylan-Erlang

	cd Ceylan-Erlang

	# We need to download the following separate Ceylan repositories for that:
	ceylan_erlang_packages="common wooper traces tools"

	base_url="git.code.sf.net/p/ceylan/"

	if [ $developer_access -eq 0 ] ; then

		DISPLAY "      ----> getting Ceylan-Erlang packages from ${current_vcs} with user name ${developer_name}"
		protocol="ssh"
		base_url="${developer_name}@${base_url}"
		
	else

		DISPLAY "      ----> getting Ceylan-Erlang packages from ${current_vcs} (read-only)"
		protocol="git"

	fi

	LOG_STATUS "Getting Ceylan-Erlang packages in the source directory ${repository}..."


	# Manage back-up directories if necessary:
	for p in $ceylan_erlang_packages; do

		manage_package_backup "Ceylan-Erlang/$p"

		{

			${GIT} clone ${protocol}://${base_url}/$p $p
			
		} 1>>"$LOG_OUTPUT" 2>&1

		if [ ! $? -eq 0 ] ; then

			ERROR "Unable to retrieve Ceylan-Erlang package $p from VCS (using $protocol)."
			exit 20

		fi

	done


	return 0

}



prepareCeylan_Erlang()
{

	DEBUG "Preparing Ceylan-Erlang..."

	printBeginList "Ceylan-Erlang "

	printItem "extracting"

	# Nothing to do, as sources were already retrieved from VCS.

	printOK

}



generateCeylan_Erlang()
{

	DEBUG "Generating Ceylan-Erlang..."

	printItem "configuring"

	printOK

	printItem "building"

	cd ${repository}/Ceylan-Erlang

	for p in $ceylan_erlang_packages ; do

		{

			cd $p && ${MAKE} && cd ..

		} 1>>"$LOG_OUTPUT" 2>&1

		if [ ! $? -eq 0 ] ; then
			echo
			ERROR "Unable to build Ceylan-Erlang package $p."
			exit 30
		fi

	done

	printOK

	printItem "installing"

	for p in $ceylan_erlang_packages; do

		{

			cd $p && ${MAKE} install INSTALLATION_PREFIX="${prefix}/Ceylan-Erlang" && cd ..

		} 1>>"$LOG_OUTPUT" 2>&1

		if [ ! $? -eq 0 ] ; then
			echo
			ERROR "Unable to install Ceylan-Erlang package $p."
			exit 31
		fi

	done

	printOK

	printEndList

	DEBUG "Ceylan-Erlang successfully installed."

	if [ ! -f "$HOME/.bashrc.minimal" ] ; then

		cd $HOME

		# Apparently no simple way of using GIT to download just one file:
		{

			${WGET} http://sourceforge.net/p/ceylan/code/ci/master/tree/src/conf/environment/.bashrc.minimal?format=raw --output-document=.bashrc.minimal 1>/dev/null

		} 1>>"$LOG_OUTPUT" 2>&1

		loani_final_hint="$loani_final_hint

Set default ~/.bashrc.minimal file."


		if [ ! -f "$HOME/.bashrc" ] ; then

			${LN} -s $HOME/.bashrc.minimal $HOME/.bashrc

			loani_final_hint="$loani_final_hint
Linked ~/.bashrc (not existing previously) to ~/.bashrc.minimal."

		fi

		cd "$initial_dir"

	fi

	}



cleanCeylan_Erlang()
{

	LOG_STATUS "Cleaning Ceylan-Erlang install..."

}





################################################################################
# Orge: OSDL RPG Game Engine
################################################################################



getOrge()
{

	DEBUG "Getting Orge..."

	# We prefer using VCS here:
	# (simplified version of getCeylan in loani-requiredTools.sh)

	declareRetrievalBegin "Orge (from ${current_vcs})"

	# To avoid a misleading message when the retrieval is finished:
	Orge_ARCHIVE="from ${current_vcs}"

	# Orge corresponds somewhat to OSDL-Erlang.

	cd ${repository}

	CHECKOUT_LOCATION="OSDL-Erlang/Orge"

	${MKDIR} -p ${CHECKOUT_LOCATION}

	cd OSDL-Erlang

	base_url="git.code.sf.net/p/osdl"

	if [ $developer_access -eq 0 ] ; then

		DISPLAY "      ----> getting Orge from ${current_vcs} with user name ${developer_name}"
		protocol="ssh"
		base_url="${developer_name}@${base_url}"
		
	else

		DISPLAY "      ----> getting Orge from ${current_vcs} (read-only)"
		protocol="git"

	fi

	LOG_STATUS "Getting Orge in the source directory ${repository}..."


	# Manage back-up if necessary:
	manage_package_backup orge

	{

		${GIT} clone ${protocol}://${base_url}/orge orge
		
	} 1>>"$LOG_OUTPUT" 2>&1
	
	if [ ! $? -eq 0 ] ; then
		
		ERROR "Unable to retrieve Orge from VCS (using $protocol)."
		exit 40

	fi

	return 0

}



prepareOrge()
{

	DEBUG "Preparing Orge..."

	printBeginList "Orge          "

	printItem "extracting"

	# Nothing to do, as sources were already retrieved from VCS.

	printOK

}



generateOrge()
{

	DEBUG "Generating Orge..."

	printItem "configuring"

	printOK

	printItem "building"


	cd ${repository}/OSDL-Erlang/orge/src/code

	{

		${MAKE}

	} 1>>"$LOG_OUTPUT" 2>&1

	if [ ! $? -eq 0 ] ; then
		echo
		ERROR "Unable to build Orge."
		exit 40
	fi

	printOK

	printItem "installing"

	{

		# Not wanting install-doc:
		cd ${repository}/${CHECKOUT_LOCATION} && ${MAKE} install-prod INSTALLATION_PREFIX="${prefix}/Orge" && cd ..

	} 1>>"$LOG_OUTPUT" 2>&1

	if [ ! $? -eq 0 ] ; then
		echo
		ERROR "Unable to install Orge package."
		exit 31
	fi

	printOK

	printEndList

	DEBUG "Orge successfully installed."


	cookie_file="$HOME/.erlang.cookie"

	if [ -f "$cookie_file" ] ; then

		${CHMOD} 400 "$cookie_file"

		cookie_content=`cat "$cookie_file"`

		loani_final_hint="$loani_final_hint

Using already-existing Erlang cookie file ($cookie_file) whose content is:
$cookie_content"

	else

		uuidgen_exec=`which uuidgen 2>/dev/null`

		if [ -x "$uuidgen_exec" ] ; then

			$uuidgen_exec > $cookie_file

			${CHMOD} 400 "$cookie_file"

			cookie_content=`cat "$cookie_file"`

			loani_final_hint="$loani_final_hint

As none was pre-existing, created with uuidgen an Erlang cookie file in $cookie_file, whose content is:
$cookie_content"

		else

			loani_final_hint="$loani_final_hint

No uuidgen tool available (on Debian-based distributions, install the 'uuid-runtime' package), thus no Erlang cookie ($cookie_file) created."

		fi


	fi


	rc_file="$HOME/.orge.rc"

	if [ -f "$rc_file" ] ; then

		loani_final_hint="$loani_final_hint

An Orge setting file ($rc_file) was already existing, nothing done."

	else

		echo "% Orge Setting File, created by LOANI on "`date`"." > "$rc_file"
		echo "{orge_server_hostname,\""`hostname -f`"\"}." >> "$rc_file"
		echo "{orge_server_nodename,\"orge_tcp_server_exec\"}." >> "$rc_file"
		echo "{orge_server_cookie,'"`cat "$cookie_file"`"'}." >> "$rc_file"

		${CHMOD} 400 "$rc_file"

		loani_final_hint="$loani_final_hint

Created Orge setting file in $rc_file, you can copy it on your monitoring nodes."

	fi

	cd "$initial_dir"

}



cleanOrge()
{

	LOG_STATUS "Cleaning Orge install in ${prefix}/Orge..."

}




################################################################################
# End of loani-OrgeTools.sh
################################################################################
