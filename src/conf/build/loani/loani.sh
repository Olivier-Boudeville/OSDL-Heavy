#!/bin/sh

# LOANI
# Lazy OSDL Automatic Net Installer.

# Creation date: 2003, April 14
# Author: Olivier Boudeville (olivier.boudeville@online.fr)


USAGE="Usage: "`basename $0`" [ -d | --debug ] [ -s | --strict ] [ -q | --quiet ] [ -w | --wizard ] [ -u | --useSVN ] [ -c | --currentSVN ] [ --sourceforge <user name> ] [ --buildTools ] [ --optionalTools ] [ --OrgeTools ] [ --onlyOrgeTools ] [ --allTools ] [ --nds ] [ --setEnv ] [ --fetchonly ] [ --all ] [ --prefix <a path> ] [ --repository <a path> ] [ --noLog ] [ --noClean ] [ -h | --help ]"

EXAMPLE="    Recommended examples (long but safe):
	for a end-user  (export of last stable)       : ./"`basename $0`"
	for a developer (check-out of current sources): ./"`basename $0`" --allTools --sourceforge <your developer login> --currentSVN
	"

# For testing purposes:
# ./loani.sh --debug --strict --currentSVN --sourceforge wondersye --allTools

HELP="This is LOANI, the famous Lazy OSDL Automatic Net Installer.

	$USAGE

Options:
	-d or --debug		 : display debug informations to screen
	-s or --strict		 : be strict, stop on wrong md5 checksums or on unexpected tool locations
	-q or --quiet		 : avoid being too verbose
	-w or --wizard		 : enter wizard interactive mode, so that questions are asked to configure
	-u or --useSVN           : retrieve Ceylan and OSDL from SVN, instead of downloading prepackaged source archives
	-c or --currentSVN       : retrieve current SVN for Ceylan and OSDL, instead of latest SVN tagged stable release (implies --useSVN)
	--sourceforge <user name>: uses SourceForge developer access to retrieve projects from SVN  (implies --currentSVN)
	--buildTools		 : retrieve and install common build tools too (ex: gcc, binutils, gdb)
	--optionalTools		 : retrieve and install optional tools too (ex: doxygen, dot, tidy)
	--OrgeTools              : retrieve and install all tools for Orge (ex: Erlang)
	--onlyOrgeTools          : retrieve and install all tools for Orge, and no other tool
	--allTools		 : retrieve all tools (required, build, optional, Orge tools)
	--nds			 : set mode for cross-compilation from GNU/Linux to Nintendo DS homebrew
	--setEnv		 : set full developer environment (ex: bash, nedit configuration)
	--fetchonly		 : only retrieve (download in cache) prerequisite, do not install them
	--all			 : install all and set all, including developer environment
	--prefix <a path>	 : install everything under <a path>
	--repository <a path>	 : specify an alternate cache repository for downloads
	--noLog			 : do not log installation results
	--noClean                : do not remove build trees after a successful installation
	-h or --help		 : display this message

$EXAMPLE
	"

starting_time=`date '+%H:%M:%S'`


# Undocumented (since seldom used) options:
#   --onlyBuildTools: only build tools will be installed.
#   --onlyOptionalTools: only optional tools will be installed.
#   --onlyOrgeTools: only Orge tools will be installed.
#   --onlyThirdPartyTools: only third party tools will be installed (no Ceylan,
# OSDL or Orge installed). Used to test LOANI or during Sourceforge outages.
#   --noSVN: do not retrieve anything from SVN, merely used for LOANI
# debugging (variable: no_svn)

if [ "$1" = "-h" -o "$1" = "--help" ] ; then
	echo "$HELP"
	exit 0
fi


declareRetrievalBegin()
# Declares to the user that a package is enqueued in download spool.
# $1: name of the archive file.
{

 if [ $be_quiet -eq 1 ] ; then
   DISPLAY "      ----> enqueuing $1 in download spool"
 fi

}



launchFileRetrieval()
# Retrieves specified package by looking at the cache (repository first).
# If not available, will try to download it.
# From the package name, the archive, location and MD5 will be deduced.
#
# Usage: launchFileRetrieval <package name>
# Example: launchFileRetrieval SDL
{

	#TRACE "launchFileRetrieval called"

	package_name="$1"
	archive_name="${package_name}_ARCHIVE"
	eval archive_file="\$$archive_name"
	full_archive_path="${repository}/${archive_file}"

	md5_name="${package_name}_MD5"
	eval md5=\$$md5_name

	# Is file already present in cache?

	if [ -f "${full_archive_path}" ] ; then

		computed_md5=`${MD5SUM} "${full_archive_path}" | ${AWK} '{printf $1}'`
		#DEBUG "launchFileRetrieval: ${full_archive_path}: $computed_md5"

		if [ "${computed_md5}" = "$md5" ] ; then
			DEBUG "${archive_file} found in cache, its integrity has been checked, retrieved."
			return 0
		else
			DEBUG "${archive_file} found in cache, bad md5sum, still being retrieved from network."
			return 1
		fi
	else

		# File not here, having to guess the download location to be used:

		# Will set 'current_download_location'
		# (iterate through mirrors if needed):
		getDownloadLocation ${package_name}

		download_url="${current_download_location}/${archive_file}"

		DEBUG "${archive_file} not found in cache, retrieving it from network (${download_url})."

		declareRetrievalBegin ${archive_file}

		if [ $do_simulate -eq 1 ] ; then
			DEBUG ${WGET} ${WGET_OPT} --output-document=${full_archive_path} ${download_url}
			${WGET} ${WGET_OPT} --output-document=${full_archive_path} ${download_url} 1>>"$LOG_OUTPUT" 2>&1
			return 2
		else
			DISPLAY "Simulating only, no network retrieval for $1."
			return 0
		fi

	fi

}



getDownloadLocation()
{
# Returns the URL where the archive for the specified package can be downloaded,
# in variable named 'current_download_location'
#
# If all mirrors have been tried unsuccessfully, issues an error message and
# exit.
#
# Basically, one main download site should be tried, then one alternate location
# and, on failure, our own private last-chance mirror.
#
# A location is tried only once: its variable is blanked afterwards, so that
# further attempts can use next mirror, if any.
#
# Usage :
#   getDownloadLocation <package name>;
#   echo ${current_download_location}
# Example: getDownloadLocation SDL

	# ex: $1 = SDL:
	package_name="$1"
	location_name="${package_name}_DOWNLOAD_LOCATION"
	eval actual_location_name="\$$location_name"
	location_value="${actual_location_name}"

	# Trying first the main site:
	if [ -z "${location_value}" ] ; then

		# Not available? Try the mirror (if any):
		location_name="${package_name}_ALTERNATE_DOWNLOAD_LOCATION"
		eval actual_location_name="\$$location_name"
		location_value="${actual_location_name}"

		if [ -z "${location_value}" ] ; then
			# Last hope, our little private mirror:

			private_mirror_used="${package_name}_USED_MIRROR"
			eval actual_private_mirror_used="\$$private_mirror_used"
			if [ "${actual_private_mirror_used}" = "0" ] ; then
				# Set to zero means already tried:
				ERROR "${package_name} archive not available through main server or known mirrors."
				exit 12
			else
				current_download_location=${private_archive_mirror}/${package_name}
				DEBUG "Selected download location (last-chance mirror): ${current_download_location}"

				# Remember the attempt:
				eval ${private_mirror_used}="0"
				return
			fi
		else
			saved="${location_value}"
			eval ${location_name}=""
			current_download_location=${saved}
			DEBUG "Selected download location (mirror): ${current_download_location}"
			return

		fi

	else
		saved=${location_value}
		eval ${location_name}=""
		current_download_location=${saved}
		DEBUG "Selected download location (main site): ${current_download_location}"
		return
	fi

}



# Used to test 'getDownloadLocation':
testGetDownloadLocation()
{
  target=doxygen

  getDownloadLocation $target
  getDownloadLocation $target
  getDownloadLocation $target

  exit 0
}



getFileAvailability()
# Returns 0 if specified file is in cache and its md5sum is correct or
# not specified,
#         1 if specified file is in cache but its md5sum is wrong,
#         2 if specified file is not at all in cache or is empty
# (in this case, it is deleted).
# Usage : getFileAvailability <file name> <md5 sum of file>
# Example: getFileAvailability dummy.tgz "886924ab144af672af9596115088ff20"
{

	filename=${1}
	full_file_path=${repository}/${filename}
	md5=${2}

	# Accept files or directories entries:
	if [ ! -e "${full_file_path}" ] ; then
		DEBUG "File <${filename}> not in cache."
		return 2
	else
		if [ `${DU} -L ${full_file_path} | ${AWK} '{printf $1}'` -eq 0 ] ; then
			DEBUG "File <${filename}> present in cache but empty, removing it."
			${RM} -f "${full_file_path}"
			return 2
		fi

		if [ -z "${md5}" ] ; then
			WARNING "<${filename}> found in cache, no md5sum given, no checking performed."
			return 0
		else

			computed_md5=`${MD5SUM} "${full_file_path}" | ${AWK} '{printf $1}'`
			DEBUG "getFileAvailability: ${full_file_path}: computed $computed_md5, expected ${md5}"

			if [ "${computed_md5}" = "${md5}" ] ; then
				DEBUG "<${filename}> found in cache and its md5sum is correct."
				return 0
			else
				DEBUG "<${filename}> found in cache, but its md5sum does not match the recorded one (<${computed_md5}> versus <${md5}>)."
				return 1
			fi
		fi
	fi

}



launchwizard()
# Allows the user to interactively choose his settings.
{

	echo
	printColor "      This is LOANI's wizard!" $cyan_text

	echo
	echo

	DISPLAY "Entering wizard-assisted configuration."
	echo

	OFFSET="    + "

	if askDefaultYes "${OFFSET}Activate log mode?" ; then
		DISPLAY "Log mode activated."
		do_log=0
	else
		DISPLAY "Log mode deactivated."
		do_log=1
	fi


	if askDefaultNo "${OFFSET}Activate strict mode?" ; then
		DISPLAY "Strict mode activated."
		be_strict=0
		do_strict_md5=0
		must_find_tool=0
		be_strict_on_location=0
	else
		DISPLAY "Strict mode deactivated."
		be_strict=1
	fi


	if askDefaultNo "${OFFSET}Activate quiet mode?" ; then
		DISPLAY "Quiet mode activated."
		be_quiet=0
	else
		DISPLAY "Quiet mode deactivated."
		be_quiet=1
	fi


	if askDefaultNo "${OFFSET}Activate verbose debug on screen?" ; then
		DISPLAY "Verbose debug on screen mode activated."
		do_debug=0
	else
		DISPLAY "Verbose debug on screen mode deactivated."
		do_debug=1
	fi


	if askDefaultNo "${OFFSET}Use SVN to retrieve sources instead of downloading source archives?" ; then
		DISPLAY "SVN mode activated."
		use_svn=0

		if askDefaultNo "${OFFSET}Use current SVN, not last stable version, for Ceylan and OSDL? [not recommended]" ; then

			DISPLAY "Current SVN will be used (let's hope the build is not currently broken)."
			use_current_svn=0

			if askDefaultNo "${OFFSET}Use developer access for Sourceforge's SVN?" ; then
				DISPLAY "SVN developer mode activated."
				developer_access=0
				askNonVoidString "${OFFSET}Please enter your Sourceforge's user name:"
				developer_name="$returnedString"
				DISPLAY "Developer name will be $developer_name"
			else
				DISPLAY "SVN developer mode deactivated."
				developer_access=1
			fi

		else
			DISPLAY "Latest stable SVN tag will be used for Ceylan and OSDL."
			use_current_svn=1
		fi

	else
		DISPLAY "SVN mode deactivated."
		use_svn=1
	fi


	if askDefaultNo "${OFFSET}Install all and set all, including environment?" ; then
		DISPLAY "Everything will be installed and set."
		manage_build_tools=0
		manage_optional_tools=0
		set_env=0
	else
		if askDefaultNo "${OFFSET}Install all tools? (required tools, common build tools, optional tools, Orge tools)" ; then
			DISPLAY "All tools will be installed."
			manage_build_tools=0
			manage_optional_tools=0
			manage_orge_tools=0
		else
			if askDefaultNo "${OFFSET}Install build tools? (ex: gcc, binutils) [this is the recommended setting]" ; then
				DISPLAY "Build tools will be installed."
				manage_build_tools=0
			else
				DISPLAY "Build tools will not be installed."
				manage_build_tools=1
			fi


			if askDefaultNo "${OFFSET}Install optional tools? (ex: doxygen, dot, tidy)" ; then
				DISPLAY "Optional tools will be installed."
				manage_optional_tools=0
			else
				DISPLAY "Optional tools will not be installed."
				manage_optional_tools=1
			fi

			if askDefaultNo "${OFFSET}Install Orge tools? (ex: Erlang)" ; then
				DISPLAY "Orge tools will be installed."
				manage_orge_tools=0
			else
				DISPLAY "Orge tools will not be installed."
				manage_orge_tools=1
			fi
		fi

		if askDefaultNo "${OFFSET}Fetch only? (tools will only be downloaded)" ; then
			DISPLAY "Fetch only mode activated."
			fetch_only=0
		else
			DISPLAY "Fetch only mode deactivated."
			fetch_only=1
		fi

		if askDefaultNo "${OFFSET}Set full developer environment? (ex: bash, nedit configuration)" ; then
			DISPLAY "Full developer environment will be set."
			set_env=0
		else
			DISPLAY "Developer environment will not be set."
			set_env=1
		fi

		if askDefaultYes "${OFFSET}Clean build trees after a successful installation?" ; then
			DISPLAY "Build trees will be cleaned if possible."
			clean_on_success=0
		else
			DISPLAY "No build tree removal."
			clean_on_success=1
		fi

	fi

	if askDefaultNo "${OFFSET}Perform a cross-compilation build to the Nintendo DS?" ; then
		DISPLAY "Using DevKitPro-based cross-compilation toolchain for the Nintendo DS."
		target_nds=0
	else
		target_nds=1
	fi

	if askDefaultYes "${OFFSET}Use an installation prefix? [recommended]" ; then
		askString "${OFFSET}Please enter prefix directory where installations should be done (leave blank to let LOANI find automatically an appropriate prefix):"
		prefix="$returnedString"
		if [ -z "${prefix}" ] ; then
			DISPLAY "Prefix will be set automatically."
		else
			DISPLAY "Prefix $prefix demanded."
		fi
	else
		WARNING "No prefix demanded (rather unusual choice)."
	fi

	if askDefaultNo "${OFFSET}Use an alternate cache repository?" ; then
		askNonVoidString "${OFFSET}Please enter path to the non-default cache repository:"
		repository="$returnedString"
		DISPLAY "Repository changed to $repository."
	else
		DISPLAY "Repository not changed."
	fi

	DISPLAY "Congratulations, you made your way through LOANI's wizard!"

}



# Too early, no TRACE available.


# Checking own LOANI's prerequisites.

# This script will make available all common UNIX tools that LOANI will use, as
# well as some utilities for terminals (termUtils.sh) that it relies on, for
# example for text output.
#
# Finally, as the locations of these tools are platform-dependent, a dedicated
# script is automatically used too (platformDetection.sh).
#
# Such detection is necessary for example for libpng.

#echo "Trace: Just before defaultLocations.sh"

SHELL_TOOLBOX="./defaultLocations.sh"


if [ ! -f "$SHELL_TOOLBOX" ] ; then

	echo 1>&2
	echo "     Error, helper script not found ($SHELL_TOOLBOX)." 1>&2
	exit 1

fi


. $SHELL_TOOLBOX

#TRACE "Just after defaultLocations.sh"

if [ $platform_family_detected -eq 1 ] ; then
	ERROR "the detection of the platform family did not succeed."
	exit 2

fi

if [ $precise_platform_detected -eq 1 ] ; then
	ERROR "the detection of the precise platform did not succeed (platform family: $platform_family_detected)."
	exit 3
fi


# Some platform-specific notifications:

if [ "$is_macosx" -eq 0 ] ; then
	WARNING "Mac OS X support not tested at the moment."
fi


if [ "$is_netbsd" -eq 0 ] ; then
	WARNING "Only experimental support for NetBSD at the moment."
fi


if [ "$is_freebsd" -eq 0 ] ; then
	WARNING "Only very experimental support for FreeBSD at the moment."
fi

#displayPlatformFlags

#TRACE "Beginning of LOANI."

# Pre defined set of default behaviour:


# This flag will trigger other strict flags if set [default: false (1)]:
be_strict=1

# Used to enable strict md5 sum checking [default: false (1)]:
do_strict_md5=1

# Used to check that tools are in their expected location [default: false (1)]:
be_strict_on_location=1

# Raise a fatal error if a tool is nowhere to be found [default: true (0)]:
must_find_tool=0

# Used to display more informations [default: false (1)]:
do_debug=1

# Used to simulate only (no downloading performed) [default: false (1)]:
do_simulate=1

# Used to specify if common build tools should be managed too [default: false
# (1)]:
manage_build_tools=1

# Used to specify whether optional tools should be managed too [default: false
# (1)]:
manage_optional_tools=1

# Used to specify whether Orge tools should be managed too [default: false (1)]:
manage_orge_tools=1

# Used to demand only prerequisite retrieval, not their installation
# [default: false (1)]:
fetch_only=1

# Used to specify whether wizard should be used to configure LOANI interactively
# [default: false (1)]:
wizard=1

# Disable SVN retrieval [default: false (1)]:
no_svn=1

# Tells whether SVN should be used (if 0) or if source archives should be
# downloaded (if 1) [default: false (1)]:
use_svn=1

# Used to specify whether current SVN should be used for Ceylan and OSDL
# (true) or latest stable version tagged in SVN (false) [default: false (1)]:
use_current_svn=1

# Used to specify whether developer access should be used with Sourceforge's SVN
# (developer access: SVN checkout, otherwise SVN export) [default: false (1)]:
developer_access=1

# Set developer environment [default: false (1)]:
set_env=1

# Tells whether after successfull installation the build trees are to be removed
# [default: true (0)]:
clean_on_success=0

# Used to activate quiet mode [default: false (1)]:
be_quiet=1

# Used to select whether log should be stored [default: true (0)]:
do_log=0



# Install only build tools: [default: false (1)]
only_build_tools=1

# Install only optional tools: [default: false (1)]
only_optional_tools=1

# Install only Orge tools: [default: false (1)]
only_orge_tools=1

# Install only third-party tools: [default: false (1)]
manage_only_third_party_tools=1

# Cross-compilation to Nintendo DS homebrew: [default: false (1)]
target_nds=1

# prefix is the directory where all files will be installed
prefix=""

# The directory to which OSDL data will be set.
OSDL_DATA_DIR_NAME="OSDL-data"

# Name of OSDL environment file, generated by LOANI from the tools location:
OSDL_ENV_FILE_NAME="OSDL-environment.sh"

# Name of OSDL environment file for DS, generated by LOANI from the tools
# location:
OSDL_DS_ENV_FILE_NAME="OSDL-environment-for-DS.sh"

# Name of Orge environment file, generated by LOANI from the Orge location:
ORGE_ENV_FILE_NAME="Orge-environment.sh"

# The cache directory where already available archives should be found:
repository=`pwd`"/LOANI-repository"

# Remember the starting working directory:
initial_dir=`pwd`


# Root URL of our last chance mirror:
# (see also: update-LOANI-mirror.sh)
private_archive_mirror="ftp://ftp.esperide.com/LOANI-archive-repository"


# wget should run in background, using any specified proxy directive and using
# passive FTP to solve client firewall issues:
WGET_OPT="--background $PROXY_CONF --passive-ftp"

CVS_OPT="-Q -z6"
CVS_RSH="ssh"

# We may need to be prompted to accept credentials, but normally a dummy svn
# 'list' command is issued first to check them:
SVN_OPT="--non-interactive"

# Maximum count of attempts to retrieve a module by SVN (when Sourceforge's SVN
# servers are overloaded, they throw "connection closed").
MAX_SVN_RETRY=8

# Saving the whole command line to have it stored in logs:
SAVED_CMD_LINE="$0 $*"


# Scans each and every argument, from the left to the right:


#TRACE "Default settings set."

while [ $# -gt 0 ] ; do

	DEBUG  "Evaluating argument $1."
	token_eaten=1

	if [ "$1" = "-d" -o "$1" = "--debug" ] ; then
		DEBUG "Debug to screen mode activated."
		do_debug_to_screen=0
		token_eaten=0
	fi

	if [ "$1" = "-s" -o "$1" = "--strict" ] ; then
		DEBUG "Strict mode activated."
		do_strict_md5=0
		must_find_tool=0
		be_strict_on_location=0
		be_strict=0
		token_eaten=0
	fi

	if [ "$1" = "-q" -o "$1" = "--quiet" ] ; then
		DEBUG "Quiet mode activated."
		be_quiet=0
		token_eaten=0
	fi

	if [ "$1" = "-w" -o "$1" = "--wizard" ] ; then
		DEBUG "wizard mode activated."
		wizard=0
		token_eaten=0
	fi

	if [ "$1" = "-u" -o "$1" = "--useSVN" ] ; then
		DEBUG "SVN mode activated."
		use_svn=0
		token_eaten=0
	fi

	if [ "$1" = "-c" -o "$1" = "--currentSVN" ] ; then
		DEBUG "Use current SVN mode activated."
		use_svn=0
		use_current_svn=0
		token_eaten=0
	fi

	if [ "$1" = "--sourceforge" ] ; then
		use_svn=0
		use_current_svn=0
		shift
		developer_access=0
		developer_name="$1"
		DEBUG "Developer SVN access set with user name $developer_name."
		token_eaten=0
	fi

	if [ "$1" = "--buildTools" ] ; then
		DEBUG "Common build tools will be retrieved and managed too."
		manage_build_tools=0
		token_eaten=0
	fi

	if [ "$1" = "--optionalTools" ] ; then
		DEBUG "Optional tools will be retrieved and managed too."
		manage_optional_tools=0
		token_eaten=0
	fi

	if [ "$1" = "--OrgeTools" ] ; then
		DEBUG "Orge tools will be retrieved and managed too."
		manage_orge_tools=0
		token_eaten=0
	fi

	if [ "$1" = "--fetchonly" ] ; then
		DEBUG "Fetch only mode activated."
		fetch_only=0
		token_eaten=0
	fi

	if [ "$1" = "--allTools" ] ; then
		DEBUG "All tools will be retrieved and managed."
		manage_build_tools=0
		manage_optional_tools=0
		manage_orge_tools=0
		token_eaten=0
	fi

	if [ "$1" = "--setEnv" ] ; then
		DEBUG "Developer environment will be set."
		set_env=0
		token_eaten=0
	fi

	if [ "$1" = "--all" ] ; then
		DEBUG "Everything will be set and installed."
		manage_build_tools=0
		manage_optional_tools=0
		manage_orge_tools=0
		set_env=0
		token_eaten=0
	fi

	if [ "$1" = "--nds" ] ; then
		DEBUG "Cross compilation to the Nintendo DS selected."
		target_nds=0
		token_eaten=0
	fi

	if [ "$1" = "--prefix" ] ; then
		shift
		prefix="$1"
		if [ -z "$prefix" ] ; then
			ERROR "No prefix specified after --prefix option."
			exit 1
		fi
		DEBUG "Prefix will be $prefix."
		token_eaten=0
	fi

	if [ "$1" = "--repository" ] ; then
		shift
		repository="$1"
		if [ -z "$repository" ] ; then
			ERROR "No repository specified after --repository option."
			exit 1
		fi
		DEBUG "Repository will be $repository."
		token_eaten=0
	fi

	if [ "$1" = "--noLog" ] ; then
		DEBUG "No log will be stored."
		do_log=1
		token_eaten=0
	fi

	if [ "$1" = "--noClean" ] ; then
		DEBUG "No build tree will be removed even if installation is a success."
		clean_on_success=1
		token_eaten=0
	fi


	if [ "$1" = "--noSVN" ] ; then
		DEBUG "No SVN retrieval will be performed."
		no_svn=0
		token_eaten=0
	fi

	if [ "$1" = "--onlyBuildTools" ] ; then
		DEBUG "Only build tools will be installed (keep in mind that LOANI installations should be built with these tools)."

		only_build_tools=0
		manage_build_tools=0
		token_eaten=0

	fi

	if [ "$1" = "--onlyOptionalTools" ] ; then
		DEBUG "Only optional tools will be installed."

		# Save any previously existing environment file:
		if [ -f "${OSDL_ENV_FILE}" ] ; then
			${CP} -f ${OSDL_ENV_FILE} ${OSDL_ENV_FILE_BACKUP}
		fi

		only_optional_tools=0
		manage_optional_tools=0
		token_eaten=0
	fi

	if [ "$1" = "--onlyOrgeTools" ] ; then
		DEBUG "Only Orge tools will be installed."

		# Save any previously existing environment file:
		if [ -f "${OSDL_ENV_FILE}" ] ; then
			${CP} -f ${OSDL_ENV_FILE} ${OSDL_ENV_FILE_BACKUP}
		fi

		only_orge_tools=0
		manage_orge_tools=0
		token_eaten=0
	fi

	if [ "$1" = "--onlyThirdPartyTools" ] ; then
		DEBUG "Only third-party tools will be installed."
		manage_only_third_party_tools=0
		token_eaten=0
	fi

	if [ "$1" = "-h" -o "$1" = "--help" ] ; then
		DISPLAY "\n$HELP"
		exit 0
	fi

	if [ "$token_eaten" -eq 1 ] ; then
		echo "Error, unknown argument: $1"
		echo
		echo "$USAGE"
		exit 1
	fi


	shift

done

#TRACE "Command line parsed."


# Welcome message.

if [ $be_quiet -eq 1 ] ; then

	printColor "\n\n\t< Welcome to Loani >" $cyan_text
	DISPLAY "\nThis is the Lazy OSDL Automatic Net Installer, dedicated to the lazy and the fearless."
	DISPLAY "\nIts purpose is to have OSDL and all its prerequisites installed with the minimum of time and effort. Some time is nevertheless needed, since some downloads may have to be performed, and the related build is CPU-intensive, so often a bit long. Therefore, even with a powerful computer and broadband access, some patience will be needed."

fi


# Entering wizard mode if asked.
#TRACE "Before wizard"

if [ $wizard -eq 0 ] ; then
	launchwizard
fi

# We hereby consider will always need debug informations, be it on file and/or
# on screen:
do_debug=0

if [ $do_log -eq 0 ] ; then

	# Activates termUtils's debug facility:

	LOG_OUTPUT=`pwd`"/LOANI.log"

	# Override default log file:
	debug_file="$LOG_OUTPUT"

	# Allow debug information to go to file:
	do_debug_in_file=0

	if [ -f "$LOG_OUTPUT" ] ; then
		DEBUG "A previous LOANI log existed, removing it."
		${RM} -f "$LOG_OUTPUT"
	fi

	DEBUG "Logs will be stored in $LOG_OUTPUT."

else

	LOG_OUTPUT="/dev/null"
	DEBUG "Log will be discarded."

fi


DEBUG "Specified command line was: ${SAVED_CMD_LINE}"

# Prerequisites continued.

# Check developer name is set if necessary.
if [ $developer_access -eq 0 ] ; then
	if [ -z "$developer_name" ] ; then
		ERROR "No developer name specified whereas SVN developer access demanded."
		exit 2
	else
		DEBUG "SVN developer access selected, with username ${developer_name}."
	fi
fi

# Retrieve tool versions informations.

TOOLS_META_FILE="./LOANIToolsSettings.inc"

if [ ! -f "${TOOLS_META_FILE}" ] ; then
	ERROR "Unable to find LOANI file containing metadata about prerequisite tools (${TOOLS_META_FILE})."
	exit 2
else
	. ${TOOLS_META_FILE}
fi


alternate_prefix=$HOME/LOANI-installations


# Manage prefix choice.
# alternate_prefix is required so that project tools (OSDL, Ceylan, etc.)
# will not be installed under the root of the system ('/') to avoid any harm.
if [ -z "$prefix" ] ; then
	DEBUG "No prefix assigned."
	if [ `${ID} -u` -eq 0 ] ; then
		WARNING "The user running LOANI is root."
		if askDefaultNo "For safety reason, we do not recommend running this script as root, continue nevertheless (few users choose that, errors might more easily occur)?" ; then
			DEBUG "Run as root, all tools being installed in their default locations except projet ones which will be in ${alternate_prefix}."
		else
			echo "We advise you to re-run LOANI as a non-privileged user."
			exit 0
		fi
	else
		prefix=`pwd`"/LOANI-installations"
		alternate_prefix="$prefix"
		WARNING "No prefix specified and not run as root (which is recommended indeed): adding prefix $prefix."
		${MKDIR} -p "${alternate_prefix}"
	fi

else

	${MKDIR} -p "$prefix"
	if [ $? -ne "0" ] ; then
		ERROR "Unable to create prefix directory $prefix."
		exit 2
	fi
	alternate_prefix="$prefix"

fi


if [ $target_nds -eq 0 ] ; then

	ds_prefix=${alternate_prefix}/Nintendo-DS-development

	${MKDIR} -p "$ds_prefix"
	if [ $? -ne "0" ] ; then
		ERROR "Unable to create DS prefix directory $ds_prefix."
		exit 3
	fi

fi


# Manage OSDL-data repository:
OSDL_DATA_FULL_DIR_NAME="${alternate_prefix}/${OSDL_DATA_DIR_NAME}"
#${MKDIR} -p "${OSDL_DATA_FULL_DIR_NAME}"
DEBUG "OSDL data repository will be ${OSDL_DATA_FULL_DIR_NAME}."


if [ -n "$prefix_option" ] ; then
	DEBUG "prefix option will be $prefix_option."
else
	DEBUG "No prefix option set."
fi


# In debug mode, unset SILENT make variable to have more build details with
# Ceylan and OSDL:
if [ $do_debug -eq 0 ] ; then
	BUILD_SILENT="SILENT="
else
	BUILD_SILENT=""
fi

if [ $is_windows -eq 0 ] ; then
	# On Windows with Visual Express, sources available only from SVN at the
	# moment:
	use_svn=0
fi

# If developer access is selected, use current SVN is implied:
if [ $developer_access -eq 0 ] ; then
	use_current_svn=0
fi

if [ "$manage_optional_tools" -eq 0 ] ; then
	if [ -x "$FLEX" ] ; then
		ERROR "No executable flex tool available ($FLEX), whereas needed by doxygen."
	fi
fi


USER_ID=`${ID} -u`

#TRACE "Prerequisites checked."


# Setting and initializing OSDL environment file:
OSDL_ENV_FILE=${alternate_prefix}/${OSDL_ENV_FILE_NAME}
DEBUG "OSDL environment file will be ${OSDL_ENV_FILE}."

# In case of a previously existing OSDL environment, file should be kept:
OSDL_ENV_FILE_BACKUP="${OSDL_ENV_FILE}.bak"

if [ -f "${OSDL_ENV_FILE}" ] ; then
	${MV} -f "${OSDL_ENV_FILE}" "${OSDL_ENV_FILE_BACKUP}"
fi



echo "# This is the OSDL environment file." > ${OSDL_ENV_FILE}
echo "# It has been generated by LOANI on "`LANG= ; date +'%B %Y, %d (%A)'`"." >> ${OSDL_ENV_FILE}
echo "# Source it to update your environment, so that it takes into " >> ${OSDL_ENV_FILE}
echo "# account this LOANI installation." >> ${OSDL_ENV_FILE}
echo "# PATH and LD_LIBRARY_PATH will be automatically updated accordingly." >> ${OSDL_ENV_FILE}
echo "#" >> ${OSDL_ENV_FILE}
echo "# Usage example: " >> ${OSDL_ENV_FILE}
echo "# . ${OSDL_ENV_FILE}" >> ${OSDL_ENV_FILE}
echo "" >> ${OSDL_ENV_FILE}
echo "" >> ${OSDL_ENV_FILE}


# Same thing for DS:
if [ $target_nds -eq 0 ] ; then

	OSDL_DS_ENV_FILE=${ds_prefix}/${OSDL_DS_ENV_FILE_NAME}
	DEBUG "OSDL environment file for DS will be ${OSDL_DS_ENV_FILE}."

	# In case of a previously existing OSDL environment, file should be kept:
	OSDL_DS_ENV_FILE_BACKUP="${OSDL_DS_ENV_FILE}.bak"

	echo "# This is the OSDL environment file for the Nintendo DS." > ${OSDL_DS_ENV_FILE}
	echo "# It has been generated by LOANI on "`LANG= ; date +'%B %Y, %d (%A)'`"." >> ${OSDL_DS_ENV_FILE}
	echo "# Source it to update your environment, so that it takes into " >> ${OSDL_DS_ENV_FILE}
	echo "# account this LOANI installation." >> ${OSDL_DS_ENV_FILE}
	echo "# PATH and LD_LIBRARY_PATH will be automatically updated accordingly." >> ${OSDL_DS_ENV_FILE}
	echo "#" >> ${OSDL_DS_ENV_FILE}
	echo "# Usage example: " >> ${OSDL_DS_ENV_FILE}
	echo "# . ${OSDL_DS_ENV_FILE}" >> ${OSDL_DS_ENV_FILE}
	echo "" >> ${OSDL_DS_ENV_FILE}
	echo "" >> ${OSDL_DS_ENV_FILE}

fi


# Same thing for Orge:
if [ $manage_orge_tools -eq 0 ] ; then

	ORGE_ENV_FILE=${alternate_prefix}/${ORGE_ENV_FILE_NAME}
	DEBUG "Orge environment file will be ${ORGE_ENV_FILE}."

	# In case of a previously existing OSDL environment, file should be kept:
	ORGE_ENV_FILE_BACKUP="${ORGE_ENV_FILE}.bak"

	echo "# This is the Orge environment file." > ${ORGE_ENV_FILE}
	echo "# It has been generated by LOANI on "`LANG= ; date +'%B %Y, %d (%A)'`"." >> ${ORGE_ENV_FILE}
	echo "# Source it to update your environment, so that it takes into " >> ${ORGE_ENV_FILE}
	echo "# account this LOANI installation." >> ${ORGE_ENV_FILE}
	echo "# PATH and LD_LIBRARY_PATH will be automatically updated accordingly." >> ${ORGE_ENV_FILE}
	echo "#" >> ${ORGE_ENV_FILE}
	echo "# Usage example: " >> ${ORGE_ENV_FILE}
	echo "# . ${ORGE_ENV_FILE}" >> ${ORGE_ENV_FILE}
	echo "# This script can be also appended to a shell configuration file." >> ${ORGE_ENV_FILE}
	echo "# Ex: 'cat ${ORGE_ENV_FILE} >> ~/.bashrc'." >> ${ORGE_ENV_FILE}
	echo "" >> ${ORGE_ENV_FILE}
	echo "echo '--- Orge Settings File sourced ---'" >> ${ORGE_ENV_FILE}
	echo "" >> ${ORGE_ENV_FILE}

fi


# Anticipated checkings:

lacking_tool_message=""

if [ $is_linux -eq 0 ] ; then

	# Trying to guess the distribution:
	distro=`cat /etc/lsb-release 2>/dev/null | grep DISTRIB_ID | sed 's|^DISTRIB_ID=||' `

	# Some distros do not define /etc/lsb-release.

	intro="
To ensure most needed tools are installed, one may run:
  "

	# Only to be done if dealing with Orge:
	if [ $only_orge_tools -eq 0 ] ; then

		# more and ping must be built-in:
		lacking_tool_message="${intro} sudo apt-get update && apt-get install coreutils gawk tar gzip bzip2 wget make gcc subversion"

	else

		# Probably not exactly the correct string to match:
		if [ "$distro" = "openSUSE" ] ; then

			lacking_tool_message="${intro} sudo yast --update && yast -i coreutils gawk tar gzip bzip2 wget make cmake gcc gcc-c++ flex subversion autoconf automake xorg-x11-proto-devel libjpeg-devel Mesa Mesa-devel"

		#else if [ "$distro" = "Ubuntu" ] ; then
		else

			# Too many problems with libpng, thus added here:
			lacking_tool_message="${intro} sudo apt-get update && apt-get install coreutils gawk tar gzip bzip2 xz-utils wget make cmake gcc g++ flex subversion autoconf automake x11proto-xext-dev libjpeg-dev mesa-common-dev libglu1-mesa-dev libpulse-dev libpng12-0 libpng12-dev p7zip"

		fi

	fi

fi



# findBasicShellTools already automatically run when sourced.

if [ $only_orge_tools -eq 0 ] ; then

	findTool awk $1 "${lacking_tool_message}"
	AWK=$returnedString

	findTool tar $1 "${lacking_tool_message}"
	TAR=$returnedString

	findTool gunzip $1 "${lacking_tool_message}"
	GUNZIP=$returnedString

	findTool bunzip2 $1 "${lacking_tool_message}"
	BUNZIP2=$returnedString

	findTool xz $1 "${lacking_tool_message}"
	XZ=$returnedString

	findTool ping $1 "${lacking_tool_message}"
	PING=$returnedString

	findTool sleep $1 "${lacking_tool_message}"
	SLEEP=$returnedString

	findTool make $1 "${lacking_tool_message}"
	MAKE=$returnedString

	findTool gcc $1 "${lacking_tool_message}"
	GCC=$returnedString

	findTool more $1 "${lacking_tool_message}"
	MORE=$returnedString

	findTool svn $1 "${lacking_tool_message}"
	SVN=$returnedString

	findTool git $1 "${lacking_tool_message}"
	GIT=$returnedString

else

	findBuildTools "${lacking_tool_message}"
	findAutoTools "${lacking_tool_message}"
	findSupplementaryShellTools "${lacking_tool_message}"

	if [ ! -f "/usr/lib/libpulse-simple.so" ] ; then

		# Not all distro rely on PulseAudio though:
		WARNING "No PulseAudio development support found, the generated SDL library may not be able to output sound. ${lacking_tool_message}"

	fi

fi



if [ ! -d "$repository" ] ; then
	DEBUG "Creating non already existing repository ($repository)."
	${MKDIR} -p $repository
fi



DISPLAY "Retrieving all prerequisites, pipe-lining when possible."


#TRACE "Sourcing toolsets."


# First, select wanted tools.

# Register all LOANI strict prerequisites for download.
. ./loani-requiredTools.sh

# Put the optional tools before the build tools so that their are taken into
# account in following order: build / optional / required / Orge (best both for
# download and build).
if [ $manage_optional_tools -eq 0 ] ; then
	# Empty list only if optional tools are wanted:
	if [ $only_optional_tools -eq 0 ] ; then
		target_list=""
	fi
	. ./loani-optionalTools.sh
fi


if [ $manage_build_tools -eq 0 ] ; then
	# Empty list only if build tools are wanted:
	if [ $only_build_tools -eq 0 ] ; then
		target_list=""
	fi
	. ./loani-commonBuildTools.sh
fi

if [ $manage_orge_tools -eq 0 ] ; then
	# Empty list only if build tools are wanted:
	if [ $only_orge_tools -eq 0 ] ; then
		target_list=""
	fi
	# SVN needed by egeoip:
	use_svn=0
	. ./loani-OrgeTools.sh
fi


# Check for SVN tool if needed:
if [ $no_svn -eq 1 ] ; then
	if [ $use_svn -eq 0 ] ; then
		if findTool svn ; then
			SVN=$returnedString
		else
			ERROR "No svn tool found, whereas SVN retrieval was requested."
			exit 15
		fi
	fi
fi




# Anticipated checkings section.


# Only to be done if dealing with something else than Orge:
if [ $only_orge_tools -eq 1 ] ; then

	# Directory removed, as could be misleading:
	visual_dir="$repository/visual-express"

	# Could have already been removed by a prior execution:
	if [ -d "${visual_dir}" ] ; then
		${RM} -rf "${visual_dir}"
	fi

	# On GNU/Linux, early test for OpenGL headers and al:
	if [ $is_linux -eq 0 ] ; then


		if findTool cmake ; then
			CMAKE=$returnedString
		else
			ERROR "No cmake tool found, whereas needed (ex: for PhysicsFS)."
			exit 16
		fi


		if [ ! -f "/usr/include/GL/gl.h" ] ; then

				ERROR "No OpenGL headers found, users of Debian-based distributions may retrieve them thanks to: 'sudo apt-get install mesa-common-dev'."
				exit 16

		fi

		if [ ! -f "/usr/include/GL/glu.h" ] ; then

				ERROR "No GLU headers found, users of Debian-based distributions may retrieve them thanks to: 'sudo apt-get install libglu1-mesa-dev'."
				exit 16

		fi

		if [ ! -f "/usr/include/X11/extensions/XShm.h" ] ; then

			ERROR "No headers for X11 extension about shared memory found, users of Debian-based distributions may retrieve them thanks to: 'sudo apt-get install x11proto-xext-dev libxext-dev'."
			exit 17

		fi

		if [ ! -f "/usr/lib/libGL.so" ] ; then

			ERROR "No OpenGL library found, users of Debian-based distributions may retrieve it thanks to: 'sudo apt-get install libgl1-mesa-dev' (note also that hardware-accelerated drivers should be preferred)."
			exit 18

		fi

		libjpeg_header="/usr/include/jpeglib.h"

		if [ ! -f "${libjpeg_header}" ] ; then

			ERROR "Libjpeg does not seem installed on the system (no ${libjpeg_header} found). LOANI will install it, but currently Agar cannot be told to search outside the system tree for libjpeg files, and therefore needs them installed there as well. Users of Debian-based distributions may retrieve them thanks to: 'sudo apt-get install libjpeg62-dev'."
			exit 19

		fi

		gettext_executable="/usr/bin/gettext"
		if [ ! -x "${gettext_executable}" ] ; then

			ERROR "No gettext executable found (${gettext_executable}), users of Debian-based distributions may retrieve it thanks to: 'sudo apt-get install gettext'."
			exit 20

		fi

	fi

fi


#TRACE "Toolsets sourced."

DISPLAY "Target package list is <$target_list>."


# Insert here any bypass for testing


# Checks that there is enough available space on disk:

# Available size in megabytes (1048576 is 1024^2):
available_size=`${DF} -m . | ${AWK} '{print $4}' | ${TAIL} -n 1`


DEBUG "Detected available size on current disk is ${available_size} megabytes."

# The cross-compilation for Nintendo DS requires less than mainstream:
if [ $target_nds -eq 1 ] ; then
	minimum_size=400
else
	minimum_size=100
fi

if [ $manage_build_tools -eq 0 ] ; then

	if [ $target_nds -eq 1 ] ; then
		minimum_size=`expr $minimum_size + 900`
	else
		# devkitARM & co are prebuilt and quite small:
		minimum_size=`expr $minimum_size + 100`
	fi
fi

if [ $manage_optional_tools -eq 0 ] ; then
	minimum_size=`expr $minimum_size + 200`
fi


if [ $manage_orge_tools -eq 0 ] ; then
	minimum_size=`expr $minimum_size + 400`
fi


if [ $available_size -lt $minimum_size ] ; then
	WARNING "According to the selected tools which are to install, a rough estimate for peek need of available disk space is $minimum_size megabytes, whereas only $available_size megabytes are available in the current partition. Consider interrupting this script (CTRL-C) if you believe it will not suffice."
else
	DEBUG "Enough space on disk ($available_size megabytes available for an estimation of $minimum_size needed)."
fi

# Second, sort out which tools are available and which are not:

available_list=""
retrieve_list=""

for t in $target_list; do

	res=0

	DEBUG "Examining <$t>"

	if [ $use_svn -eq 0 ] ; then

		# Now, as soon as SVN is used for a package, we return res=2:
		if [ "$t" = "Ceylan" -o "$t" = "Ceylan_win" -o "$t" = "Ceylan_Erlang" -o "$t" = "OSDL" -o "$t" = "OSDL_win" ] ; then
			res=2
		fi

	fi


	# Note: some tools are always accessed thanks to SVN, they must be tested
	# inconditionally as such, otherwise as soon as LOANI is run without the
	# --sourceforge option, we have errors like: '[: 1: Illegal number:' and a
	# giant integer.

	if [ "$t" = "Orge" -o "$t" = "egeoip" ] ; then
		res=2
	fi

	if [ ! $res -eq 2 ] ; then

		target_archive=${t}_ARCHIVE
		target_md5=${t}_MD5

		eval actual_target_archive="\$$target_archive"
		eval actual_target_md5="\$$target_md5"

		#TRACE "Getting availability of ${actual_target_archive} whose expected MD5 is ${actual_target_md5}"

		getFileAvailability ${actual_target_archive} ${actual_target_md5}
		res=$?

	fi

	if [ $res -eq 0 ] ; then
		# Avoid leading space in the beginning of the list:
		DEBUG "Adding $t in available list."
		if [ -z "$available_list" ] ; then
			available_list="$t"
		else
			available_list="$available_list $t"
		fi
	else

		if [ $res -eq 1 ] ; then
			target_file=${t}_ARCHIVE
			eval actual_target_file="\$$target_file"
			real_file="$repository/${actual_target_file}"

			WARNING "$t archive found in cache (${real_file}), but its md5 checksum does not match recorded one."

			if [ $do_strict_md5 -eq 0 ] ; then
				WARNING "Erasing this faulty file and retrieving it from scratch (strict md5 checking mode is on)."
				${RM} -f "${real_file}}"
				DEBUG "Adding $t in retrieve list."
				if [ -z "$retrieve_list" ] ; then
					retrieve_list="$t"
				else
					retrieve_list="$retrieve_list $t"
				fi
			else
				WARNING "Will use $t file (${real_file}) nevertheless (strict md5 checking mode is off)."
				DEBUG "Adding $t in available list."
				if [ -z "$available_list" ] ; then
					available_list="$t"
				else
					available_list="$available_list $t"
				fi
			fi

		else
			# res is 2, not in cache at all
			DEBUG "Adding $t in retrieve list."
			if [ -z "$retrieve_list" ] ; then
				retrieve_list="$t"
			else
				retrieve_list="$retrieve_list $t"
			fi
		fi

	fi
done


if [ -z "$available_list" ] ; then
	DISPLAY "No tool already available in repository, will download them all ($retrieve_list)."
else
	 # Some are available. All of them?
	if [ -z "$retrieve_list" ] ; then
		DISPLAY "All tools already available in repository ($available_list), no download needed."
	else
		# Mixed
		DISPLAY "Some tools already available ($available_list), others will be downloaded ($retrieve_list)."
	fi
fi

# Check wget is available, that an internet connection is available and there
# exist end-to-end web access.

# Server which should almost never be down:
RELIABLE_SERVER="google.com"

if [ -n "$retrieve_list" ] ; then

	if findTool wget ; then
		WGET=$returnedString
	else
		ERROR "No wget tool found, whereas some files have to be downloaded ($retrieve_list)."
		exit 5
	fi

	# Checking whether an Internet connection is working is not performed with
	# ping anymore, as Virtualboxed OS are based on user-space NAT, thus no ICMP
	# ping can be used.
	#
	#if findTool ping ; then
	#
	#	PING=$returnedString
	#	if ! ${PING} ${PING_OPT} 5 ${RELIABLE_SERVER} 1>>"$LOG_OUTPUT" 2>&1 ; then
	#		ERROR "No available internet connection (unable to ping ${RELIABLE_SERVER})."
	#		exit 6
	#	else
	#		DEBUG "Working internet connection detected."
	#	fi

	#else
	#	WARNING "No ping tool found, disabling some checks."
	#fi

	if ! ${WGET} ${PROXY_CONF} --spider http://${RELIABLE_SERVER}/index.html 1>>"$LOG_OUTPUT" 2>&1  ; then
		ERROR "Unable to access the web (reading ${RELIABLE_SERVER}/index.html). If you are behind a proxy, please set PROXY_CONF environment variable with the relevant options to be used by wget. /bin/sh example: PROXY_CONF='--proxy-user=<my user name> --proxy-passwd=<my password>'; export PROXY_CONF"
		exit 7
	else
		DEBUG "End-to-end web access successfully checked."
	fi

	# Pre-check that no wget process is already running, since it would confuse
	# LOANI:
	if ${PS} ax | ${GREP} -v grep | ${GREP} wget 1>/dev/null 2>&1 ; then
		ERROR "An executable whose name matches wget (possibly wget itself) appears to be already running ("`${PS} ax | ${GREP} -v grep | ${GREP} wget`"). Please ensure that this executable is not running any more in parallel with LOANI before re-launching our script, since it might confuse LOANI."

		echo "  Wget match was:"
		${PS} ax | ${GREP} -v grep | ${GREP} wget

		exit 8
	fi

fi


# Third step, retrieve the lacking ones.

# Needed to force the whole loop to run one more time, to prevent following
# scenario:
#  1. launchFileRetrieval returned 2, because a file is still being downloaded,
#  2. the file is retrieved and wget stops
#  3. this loop sees launchFileRetrieval returned 2 but does not find any wget
# running, concludes wrongly the file is corrupted.
double_checked=1

while [ -n "$retrieve_list" ] ; do

	DEBUG "Iterating on following retrieve list: [$retrieve_list]"

	for t in $retrieve_list ; do
		#TRACE "Taking care of ${t}"

		# Retrieves the file, and retrieve the result of the operation:
		get${t}
		res=$?
		if [ $res -eq 0 ] ; then

			# Success, one fewer file to take care of.
			DEBUG "Removing $t of retrieve list and adding it to available list."
			if [ $be_quiet -eq 1 ] ; then
				target_file="${t}_ARCHIVE"
				eval actual_target_file="\$$target_file"
				DISPLAY "      <---- ${t} retrieved [${actual_target_file}]"
			fi
			available_list="$available_list $t"
			new_retrieve_list=""
			for i in $retrieve_list ; do
				if [ $i != $t ] ; then
					new_retrieve_list="$new_retrieve_list $i"
				fi
			done
			retrieve_list=$new_retrieve_list
		else

			DEBUG "md5sum is not the expected one, still being downloaded?"
			if ! ${PS} ax | ${GREP} wget | ${GREP} -v -i grep 1>/dev/null 2>&1 ; then
				# No!

				DEBUG "No wget running, having downloaded the file, but its md5sum seems to be wrong."

				if [ "$do_strict_md5" -eq 0 ] ; then

					if [ $double_checked -eq 1 ] ; then
						double_checked=0
					else
						ERROR "Unable to download file for $t with correct md5 checksum, and strict md5 checking mode is on."
						exit 4
					fi

				else
					target_file=${t}_ARCHIVE
					eval actual_target_file="\$$target_file"

					if [ ! -e "$repository/${actual_target_file}" ] ; then
						DEBUG "Unable to download $t (no $repository/${actual_target_file} file found), forcing mirror change."
						break
					fi

					if [ `${DU} $repository/${actual_target_file} | ${AWK} '{printf $1}'` -eq 0 ] ; then
						DEBUG "Unable to download a non-empty file for $t (${actual_target_file}), removing this file to force mirror change."
						DEBUG "File is $repository/${actual_target_file}, "`ls -l $repository/${actual_target_file}` `${MD5SUM} $repository/${actual_target_file}`
						${RM} -f $repository/${actual_target_file}
						break
					fi
					WARNING "The downloaded file for $t has not the right md5 checksum, continuing anyway (strict md5 checking mode is off)"
					if [ $be_quiet -eq 1 ] ; then
						target_file="${t}_ARCHIVE"
						eval actual_target_file="\$$target_file"
						DISPLAY "      <---- ${t} retrieved [${actual_target_file}]"
					fi

					available_list="$available_list $t"
					new_retrieve_list=""
					for i in $retrieve_list ; do
						if [ $i != $t ] ; then
							new_retrieve_list="$new_retrieve_list $i"
						fi
					done
					retrieve_list=$new_retrieve_list
				fi
			else
				DEBUG "(wget still running)"
			fi # wget not running
		fi # get result

	done

	sleep 4

	DEBUG "Available list : <$available_list>"
	DEBUG "To retrieve list: <$retrieve_list>"

done

#DEBUG "Final available list : $available_list"
#DEBUG "Final to retrieve list: $retrieve_list"

DISPLAY "All prerequisites available."

if [ "$fetch_only" -eq 0 ] ; then
	DISPLAY "Fetch only mode was on, work done."
	exit 0
fi


# Now installing each package:

if [ ! -d "$prefix" ] ; then
	DEBUG "Creating non already existing installation repository ($prefix)."
	${MKDIR} -p $prefix
fi


for t in $target_list; do
	DEBUG "Preparing $t"
	prepare$t
	DEBUG "Generating $t"
	generate$t
done


# Manages set_env too:

if [ $set_env -eq 0 ] ; then
	RETRIEVE_SCRIPT="${repository}/ceylan/Ceylan/trunk/src/conf/environment/retrieveEnvironment.sh"
	DISPLAY "Modifying environment to have it be developer-friendly."
	if [ ! -x "$RETRIEVE_SCRIPT" ] ; then
		ERROR "Unable to modify environment to have it be developer-friendly: script $RETRIEVE_SCRIPT not found or not executable."
		exit 1
	fi
	${RETRIEVE_SCRIPT} ${alternate_prefix} --link
fi


if [ $only_optional_tools -eq 0 ] || [ $only_orge_tools -eq 0 ] ; then

	# Upgrades from any previously existing environment file:
	if [ -f "${OSDL_ENV_FILE_BACKUP}" ] ; then
		${CAT} ${OSDL_ENV_FILE} >> ${OSDL_ENV_FILE_BACKUP}
		${MV} -f ${OSDL_ENV_FILE_BACKUP} ${OSDL_ENV_FILE}
	fi

fi


if [ $clean_on_success -eq 0 ] ; then
	DISPLAY "Post-install cleaning of build trees."
	for t in $target_list; do
		clean$t
	done
fi


. ${OSDL_ENV_FILE}

DISPLAY "End of LOANI, started at ${starting_time}, successfully ended at "`date '+%H:%M:%S'`"."


if [ $target_nds -eq 1 ] ; then

	DISPLAY "You can now test the whole installation by executing ${alternate_prefix}/share/OSDL/scripts/shell/playTests.sh"

fi


exit 0


# End of LOANI.
