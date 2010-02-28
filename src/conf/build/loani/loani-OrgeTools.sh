# This script is made to be sourced by LOANI when retrieving Orge tools.
# These tools are required only for server-side Orge.

# Creation date: Monday, July 7, 2008.
# Author: Olivier Boudeville (olivier.boudeville@online.fr)


# Orge tools section.

# Erlang not listed, as is dynamically added:
ORGE_TOOLS="egeoip Geolite Ceylan_Erlang Orge"
#ORGE_TOOLS="egeoip Geolite"
#ORGE_TOOLS="egeoip Geolite Ceylan_Erlang Orge"
#ORGE_TOOLS="Orge"


# Updating retrieve list:
# (new tools are put ahead of those already selected, so that CVS retrievals
# do not delay them)


target_erlang="/usr/local/bin/erl"
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
  #(ex: gcc) can be used nevertheless:
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

	declareRetrievalBegin "egeoip (from SVN)"

	# To avoid a misleading message when the retrieval is finished:
	egeoip_ARCHIVE="from SVN"

	cd $repository

	LOG_STATUS "Getting egeoip in its source directory ${repository}..."

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

	printBeginList "egeoip        "

	printItem "extracting"

	# Nothing to do, as retrived from SVN.

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
		echo "# See \${egeoip_PREFIX}/ebin and \${egeoip_PREFIX}/include" >> ${OSDL_ENV_FILE}

		echo "" >> ${OSDL_ENV_FILE}

		${MKDIR} ${EGEOIP_PREFIX}

		for d in ebin include priv; do
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

	# If no prefix is used, the database is installed in the egeoip
	# installation (not in repository), otherwise it is installed in
	# /usr/local/share/GeoIP/ and a link is made to it from egeoip:
	cd $repository

	geolite_dir="/usr/local/share/GeoIP"

	if [ -z "${prefix}" ] ; then

		# No prefix, installing in system tree:
		{
			${MKDIR} -p ${geolite_dir} && ${CP} ${Geolite_ARCHIVE} ${geolite_dir} && cd ${EGEOIP_PREFIX}/priv/ && ${LN} -s ${geolite_dir}/${Geolite_ARCHIVE}

		} 1>>"$LOG_OUTPUT" 2>&1

	else

		{
			${CP} ${Geolite_ARCHIVE} ${EGEOIP_PREFIX}/priv/
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
				ERROR "There already exist a back-up directory for $package, it is on the way, please remove it first (${repository}/$package.save)"
				exit 5
			else
				WARNING "Deleting already existing back-up directory for $package (removing ${repository}/$package.save)"
				${RM} -rf "${repository}/$package.save" 2>/dev/null
				# Sometimes rm fails apparently (long names or other reasons):
				${MV} -f ${repository}/$package.save ${repository}/$package.save-`date '+%Hh-%Mm-%Ss'` 2>/dev/null
			fi
		fi
		${MV} -f ${repository}/$package ${repository}/$package.save 2>/dev/null
		WARNING "There already existed a directory for $package (${repository}/$package), it has been moved to ${repository}/$package.save."
	fi

}



getCeylan_Erlang()
{

	DEBUG "Getting Ceylan-Erlang..."

	# We prefer using SVN here:
	# (simplified version of getCeylan in loani-requiredTools.sh)

	declareRetrievalBegin "Ceylan-Erlang (from SVN)"

	# To avoid a misleading message when the retrieval is finished:
	Ceylan_Erlang_ARCHIVE="from SVN"

	cd ${repository}

	${MKDIR} -p Ceylan-Erlang

	cd Ceylan-Erlang

	SVN_URL="svnroot/ceylan/Ceylan/trunk/src/code/scripts/erlang"

	base_svn_url="http://${Ceylan_SVN_SERVER}/${SVN_URL}"

	if [ $developer_access -eq 0 ] ; then

		user_opt="--username=${developer_name}"

		DISPLAY "      ----> getting Ceylan-Erlang packages from SVN with user name ${developer_name} (check-out)"

		svn_command="co"

	else

		# Not really supported, http should be used, not https...

		user_opt=""

		DISPLAY "      ----> getting Ceylan-Erlang packages from anonymous SVN (export)"

		svn_command="export"

	fi

	LOG_STATUS "Getting Ceylan-Erlang packages in the source directory ${repository}..."

	ceylan_erlang_packages="common wooper traces"

	# Manage back-up directories if necessary:
	for p in $ceylan_erlang_packages; do

		manage_package_backup $p

		{

			${SVN} ${svn_command} ${base_svn_url}/$p ${user_opt}

		} 1>>"$LOG_OUTPUT" 2>&1

		if [ ! $? -eq 0 ] ; then

			ERROR "Unable to retrieve Ceylan-Erlang package $p from SVN."
			exit 20

		fi

	done

	# Dead symbolic links, as the rest of the Ceylan tree has not been
	# retrieved:
	rules_dead_link="common/doc/GNUmakerules-docutils.inc"

	if [ -h "${rules_dead_link}" ] ; then

		${RM} "${rules_dead_link}"

	fi

	# Replaced by an empty file here:
	touch "${rules_dead_link}"


	script_dead_link="common/src/scripts/generate-docutils.sh"

	if [ -h "${script_dead_link}" ] ; then

		${RM} "${script_dead_link}"

	fi

	# Replaced by an empty file here:
	touch "${script_dead_link}"

	return 0

}



prepareCeylan_Erlang()
{

	DEBUG "Preparing Ceylan-Erlang..."

	printBeginList "Ceylan-Erlang "

	printItem "extracting"

	# Nothing to do, as sources were already retrieved from SVN.

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

		{

			${SVN} export http://${Ceylan_SVN_SERVER}/svnroot/ceylan/Ceylan/trunk/src/conf/environment/.bashrc.minimal 1>/dev/null

		} 1>>"$LOG_OUTPUT" 2>&1

		loani_final_hint="$loani_final_hint

Set default ~/.bashrc.minimal file."


		if [ ! -f "$HOME/.bashrc" ] ; then

			${LN} -s $HOME/.bashrc.minimal $HOME/.bashrc

			loani_final_hint="$loani_final_hint
Linked ~/.bashrc to ~/.bashrc.minimal."

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

	declareRetrievalBegin "Orge (from SVN)"

	# To avoid a misleading message when the retrieval is finished:
	Orge_ARCHIVE="from SVN"

	cd ${repository}

	${MKDIR} -p OSDL-Erlang/Orge

	cd OSDL-Erlang/Orge

	SVN_URL="svnroot/osdl/Orge/trunk"

	base_svn_url="http://${OSDL_SVN_SERVER}/${SVN_URL}"

	if [ $developer_access -eq 0 ] ; then

		user_opt="--username=${developer_name}"

		DISPLAY "      ----> getting Orge from SVN with user name ${developer_name} (check-out)"

		svn_command="co"

	else

		# Not really supported, http should be used, not https...

		user_opt=""

		DISPLAY "      ----> getting Orge from anonymous SVN (export)"

		svn_command="export"

	fi

	LOG_STATUS "Getting Orge in the source directory ${repository}..."

	# Not in osdl any more, in OSDL-Erlang: manage_package_backup osdl

	{

		${SVN} ${svn_command} ${base_svn_url} ${user_opt}

	} 1>>"$LOG_OUTPUT" 2>&1

	if [ ! $? -eq 0 ] ; then

		ERROR "Unable to retrieve Orge from SVN."
		exit 20

	fi

	return 0

}



prepareOrge()
{

	DEBUG "Preparing Orge..."

	printBeginList "Orge          "

	printItem "extracting"

	# Nothing to do, as sources were already retrieved from SVN.

	printOK

}



generateOrge()
{

	DEBUG "Generating Orge..."

	printItem "configuring"

	printOK

	printItem "building"


	cd ${repository}/OSDL-Erlang/Orge/trunk

	# Dead symbolic link, as the rest of the Ceylan tree has not been
	# retrieved:
	rules_dead_link="doc/GNUmakerules-docutils.inc"

	if [ -h "${rules_dead_link}" ] ; then

		${RM} "${rules_dead_link}"

	fi

	touch "${rules_dead_link}"


	cd ${repository}/OSDL-Erlang/Orge/trunk/src/code

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
		cd ${repository}/OSDL-Erlang/Orge/trunk && ${MAKE} install-prod INSTALLATION_PREFIX="${prefix}/Orge" && cd ..

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
