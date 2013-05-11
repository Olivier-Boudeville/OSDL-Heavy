#!/bin/sh

USAGE="

Usage: "`basename $0`" [ -h | --help ] [ -g | --guess ] [ --nds ] [ --with-osdl-env-file <filename> || --ceylan-install-prefix <path> ] [ -n | --no-build ] [ -c | --chain-test ] [ -f | --full ] [ -o | --only-prepare-dist ] [ --configure-options [option 1] [option 2] [...] ]: (re)generates all the autotools-based build system.

	--guess: attempts to guess where the OSDL environment file can be found
	--nds: cross-compile the OSDL library so that it can be run on the Nintendo DS (LOANI installation of all prerequesites and of the DS cross-build chain is assumed)
	--with-osdl-env-file <filename>: specify where the OSDL environment file (OSDL-environment.sh) should be read
	--ceylan-install-prefix: specify where the Ceylan installation can be found
	Note: if neither --with-osdl-env-file nor --ceylan-install-prefix are specified, the location of OSDL environment file will be guessed, if possible.
	--no-build: stop just after having generated the configure script
	--chain-test: build and install the library, build the test suite and run it against the installation
	--full: build and install the library, perform all available tests, including 'make distcheck', the full test suite and all OSDL tools
	--only-prepare-dist: configure all but do not build anything
	--configure-options: all following options will be directly passed whenever configure is run (they will override default ones)"

# This script must be used:
#
# - by loani-requiredTools.sh, only to produce the configure script (only Ceylan
# installation location is needed, so that substitute.sh is found)
#
# - by the OSDL developer, who does not have to specify all tool prefixes,
# thanks to OSDL-environment.sh
#
# Hence this script does not have to recognize specific options to manage the
# prefix of each specific tool.

# Main settings section.

osdl_features_disable_opt=""

# No configure_user_opt here, as this variable is meant to override, not being
# added to, configure options.
configure_opt=""

osdl_features_opt=""

# At least one of the two will have to be set:
osdl_environment_file=""
ceylan_location=""


# To check the user can override them:
#test_overriden_options="CPPFLAGS=\"-DTEST_CPPFLAGS\" LDFLAGS=\"-LTEST_LDFLAGS\""


# 0 means true, 1 means false:
do_remove_generated=0
do_clean_prefix=0
do_guess_osdl_env_location=1
do_stop_after_configure_generation=1
do_clean=0
do_build=0
do_check=0
do_install=0
do_installcheck=0
do_distcheck=1
do_chain_tests=1
do_generate_tools=1
do_only_prepare_dist=1
do_target_nds=1


while [ $# -gt 0 ] ; do

	token_eaten=1

	if [ "$1" = "--nds" ] ; then
		# Cross-compilation for the Nintendo DS requested:
		do_target_nds=0
		token_eaten=0
	fi

	if [ "$1" = "-g" -o "$1" = "--guess" ] ; then
		do_guess_osdl_env_location=0
		token_eaten=0
	fi

	if [ "$1" = "-v" -o "$1" = "--verbose" ] ; then
		be_verbose=0
		token_eaten=0
	fi

	if [ "$1" = "-q" -o "$1" = "--quiet" ] ; then
		be_quiet=0
		token_eaten=0
	fi

	if [ "$1" = "--with-osdl-env-file" ] ; then
		shift
		osdl_environment_file="$1"
		if [ ! -f "${osdl_environment_file}" ] ; then
			echo "Error, specified OSDL environment file ($osdl_environment_file) not found.
$USAGE" 1>&2
			exit 4
		fi
		. "${osdl_environment_file}"
		token_eaten=0
	fi

	if [ "$1" = "-d" -o "$1" = "--disable-all-features" ] ; then
		osdl_features_opt="$osdl_features_disable_opt"
		token_eaten=0
	fi

	if [ "$1" = "-n" -o "$1" = "--no-build" ] ; then
		do_stop_after_configure_generation=0
		token_eaten=0
	fi

	if [ "$1" = "-c" -o "$1" = "--chain-test" ] ; then
		do_chain_tests=0
		token_eaten=0
	fi

	if [ "$1" = "-f" -o "$1" = "--full" ] ; then
		do_chain_tests=0
		do_generate_tools=0
		# No do_distcheck=0, as cannot succeed because of test sub-package.
		token_eaten=0
	fi

	if [ "$1" = "-o" -o "$1" = "--only-prepare-dist" ] ; then
		do_build=1
		do_check=1
		do_install=1
		do_installcheck=1
		do_distcheck=1
		do_chain_tests=1
		do_only_prepare_dist=0
		token_eaten=0
	fi

	if [ "$1" = "--ceylan-install-prefix" ] ; then
		shift
		ceylan_location="$1"
		if [ ! -d "${ceylan_location}" ] ; then
			echo "Error, specified Ceylan installation directory does not exist ($ceylan_location).
$USAGE" 1>&2
			exit 7
		fi
		token_eaten=0
	fi

	if [ "$1" = "--configure-options" ] ; then
		shift
		configure_user_opt="$*"
		while [ "$#" -gt "0" ] ; do
			shift
		done
		token_eaten=0
	fi

	if [ "$1" = "-h" -o "$1" = "--help" ] ; then
		echo "$USAGE"
		exit
		token_eaten=0
	fi

	if [ $token_eaten -eq 1 ] ; then
		echo "Error, unknown argument ($1).
$USAGE" 1>&2
		exit 5
	fi
	shift
done



# debug mode activated iff equal to true (0):
debug_mode=1

debug()
{
	if [ $debug_mode -eq 0 ] ; then
		echo "debug: $*"
	fi
}


warning()
{
	echo "warning: $*" 1>&2
}



# Wait-after-execution mode activated iff equal to true (0):
wait_activated=1

wait()
{

	if [ $wait_activated -eq 0 ] ; then
		echo "  <press enter key to continue>"
		read
	fi

}


RM="/bin/rm -f"

COMMAND=$0

LAUNCH_DIR=`pwd`

# Always start from 'src/conf/build' directory:
cd `dirname $COMMAND`

RUNNING_DIR=`pwd`
#echo "RUNNING_DIR = $RUNNING_DIR"

# How to go from RUNNING_DIR to base directory
# (the one containing src and test):
SOURCE_OFFSET="../../.."


# Default value guessed from current path:
loani_repository=`pwd|sed 's|/osdl/OSDL/src/conf/build||1'`
#echo "loani_repository = $loani_repository"

loani_installations=`dirname $loani_repository`/LOANI-installations
#echo "loani_installations = $loani_installations"


if [ -z "$osdl_environment_file" ] ; then

	if [ -z "$ceylan_location" ] ; then

		if [ $do_guess_osdl_env_location -eq 1 ] ; then

			echo "Error: no Ceylan location nor path to the OSDL environment file specified, and no guessing was required, thus stopping. $USAGE" 1>&2
			exit 5

		fi


		# Nothing specified, trying to guess where a OSDL-environment.sh file is
		# to be found:


		osdl_environment_file=${loani_installations}/OSDL-environment.sh

		if [ ! -f "${osdl_environment_file}" ] ; then
			echo "Error, guessed OSDL environment file not found (${osdl_environment_file}), please specify it.
$USAGE" 1>&2
			exit 6
		fi

		warning "Guessed OSDL environment file is ${osdl_environment_file}."
		. ${osdl_environment_file}

		ceylan_location="$Ceylan_PREFIX"

		if [ ! -d "$ceylan_location" ] ; then

			echo "Error, Ceylan prefix retrieved thanks to guessed OSDL environment file ($osdl_environment_file) does not exist ($ceylan_location)" 1>&2
			exit 7

		fi

	else

		# ceylan_location specified and exists, ok.

		if [ $do_stop_after_configure_generation -eq 1 ] ; then
			echo "Error, no OSDL environment file and full build requested nevertheless." 1>&2
			exit 9
		fi

	fi

else

	# osdl_environment_file specified, exists and has been sourced.
	ceylan_location="$Ceylan_PREFIX"

	if [ ! -d "$ceylan_location" ] ; then

		echo "Error, Ceylan prefix retrieved thanks to specified OSDL environment file ($osdl_environment_file) does not exist ($ceylan_location)" 1>&2
		exit 10

	fi
fi


# Nintendo DS special case:
if [ $do_target_nds -eq 0 ] ; then


	# First attempt was relying on the autotools, but it was a nightmare.
	# Hence basic specific Makefiles (Makefile.cross) are used and it works
	# great.

	echo "Cross-compiling for the Nintendo DS."

	# Go back in trunk directory:
	cd ${SOURCE_OFFSET}

	# Quite convenient:
	alias mn='make -f Makefile.cross CROSS_TARGET=nintendo-ds'

	# Clean everything:
	make -f Makefile.cross CROSS_TARGET=nintendo-ds clean

	# Build everything:
	make -f Makefile.cross CROSS_TARGET=nintendo-ds
	result=$?

	if [ $result -eq 0 ] ; then
		echo "Successful cross-compiling for the Nintendo DS."
	else
		echo "Cross-compiling for the Nintendo DS failed."
	fi

	exit ${result}

fi



# Retrieving Ceylan substitute.sh script, needed by MakeConfigure:
CEYLAN_SHARED_DIR=${ceylan_location}/share/Ceylan
if [ ! -d "${CEYLAN_SHARED_DIR}" ] ; then
	echo "Error, no Ceylan share directory found ($CEYLAN_SHARED_DIR)" 1>&2
	exit 11
fi

substitute_script="${CEYLAN_SHARED_DIR}/scripts/shell/substitute.sh"
if [ ! -x "${substitute_script}" ] ; then
	echo "Error, no executable substitute script found ($substitute_script)" 1>&2
	exit 12
fi


if [ -n "${ceylan_location}" ] ; then
	# Update accordingly the configure prefix for Ceylan macro CEYLAN_PATH:
	configure_opt="${configure_opt} --with-ceylan-prefix=$ceylan_location"
else
	# The default install location when no prefix is used:
	ceylan_location="/usr/local"
fi


# If OSDL-environment.sh is used, we have to add the tools prefix so that with
# this autogen.sh script we link to the tools specified by LOANI in this OSDL
# environment file:
if [ -n "$osdl_environment_file" ] ; then

	if [ -n "${gcc_PREFIX}" ] ; then
		configure_opt="${configure_opt} CXX=${gcc_PREFIX}/bin/g++ "
	fi

	if [ -n "${SDL_PREFIX}" ] ; then
		configure_opt="${configure_opt} --with-sdl-prefix=$SDL_PREFIX"
	fi

fi


# Prefix section for the OSDL installation.

# At first, OSDL is built thanks to LOANI, which specifies the relevant --prefix
# option to the configure script.
#
# It results in the prefix being stored in OSDL-environment.sh.
#
# For next builds from scratch (i.e. directly with this autogen.sh), the
# OSDL-environment.sh will be read, and OSDL_PREFIX will be read and used.
#
# Note that if no OSDL_PREFIX is found, then no --prefix will be specified, and
# build will fail if any prerequisite is not in a standard location, since the
# configure script relies on this prefix to determine where is the
# LOANI-installation directory, then every package directory.
if [ -n "${OSDL_PREFIX}" ] ; then
	mkdir -p ${OSDL_PREFIX}
	PREFIX_OPT="--prefix=$OSDL_PREFIX"
else

	# LOANI will use this script just to generate the configure script, then
	# will run it by its own means, thus the prefix here would not matter:
	if [ $do_stop_after_configure_generation -eq 1 ] ; then

		echo "
	#### Warning: no OSDL_PREFIX variable was set, thus no prefix will be transmitted to the configure script, prerequisite paths may be incorrect. Did you####" 1>&2

	fi

	PREFIX_OPT=""

fi


if [ -n "${configure_user_opt}" ] ; then

	# Overrides all default configure options:
	configure_opt="$configure_user_opt"

else

	configure_opt="$configure_opt $osdl_features_opt --enable-strict-ansi --enable-debug $PREFIX_OPT $test_overriden_options"
fi


# Log-on-file mode activated iff equal to true (0):
log_on_file=1

log_filename="$RUNNING_DIR/autogen.log"

debug "log_filename = $log_filename"
if [ -f "$log_filename" ]; then
	$RM "$log_filename"
fi

debug "COMMAND = $COMMAND"
debug "RUNNING_DIR = $RUNNING_DIR"




# Overall autotools settings:

# Be verbose for debug purpose:
#verbose="--verbose"
verbose=""

# Copy files instead of using symbolic link:
copy="--copy"
#copy=""

# Replace existing files:
force="--force"
#force=""

# Warning selection:
warnings="--warnings=all"
#warnings=""



echo
echo "Bootstrapping now build system thanks to the autotools"
echo "      (this may take a while...)"
echo


execute()
{

	echo "    Executing $*"

	if [ $log_on_file -eq 0 ] ; then
		echo "    Executing $* from "`pwd` >>"$log_filename"
		eval $* >>"$log_filename" 2>&1
		RES=$?
		echo "----------------------------------------" >>"$log_filename"
		echo >>"$log_filename"
	else
		eval $*
		RES=$?
	fi

	if [ $RES -ne 0 ] ; then
		echo 1>&2
		if [ $log_on_file -eq 0 ] ; then
			echo "Error while executing '$*', see $log_filename" 1>&2
		else
			echo "Error while executing '$*'" 1>&2

			AUTOMAKE_HINT="
To upgrade automake and aclocal from Debian-based distributions, do the following as root: 'apt-get install automake1.9' which updates aclocal too. One has nonetheless to update the symbolic links /etc/alternatives/aclocal so that it points to /usr/bin/aclocal-1.9, and /etc/alternatives/automake so that it points to /usr/bin/automake-1.9"

			if [ "$1" = "aclocal" ]; then
				echo "
Note: if aclocal is failing since AM_CXXFLAGS (used in configure.ac) 'cannot be found in library', then check that your aclocal version is indeed 1.9 or newer. For example, with Debian-based distributions, /usr/bin/aclocal is a symbolic link to /etc/alternatives/aclocal, which itself is a symbolic link which may or may not point to the expected aclocal version. Your version of $1 is:
	" `$1 --version` ", " `/bin/ls -l --color $(which $1)` "${AUTOMAKE_HINT}"
			elif [ "$1" = "automake" ]; then
				echo "
Note: check that your automake version is indeed 1.9 or newer. For example, with Debian-based distributions, /usr/bin/automake is a symbolic link to /etc/alternatives/automake, which itself is a symbolic link which may or may not point to the expected automake version. Your version of $1 is:
	" `$1 --version` ", " `/bin/ls -l --color $(which $1)`". See also the update-alternatives command. ${AUTOMAKE_HINT}"

			elif [ "$1" = "libtoolize" ]; then
				echo "
Note: check that your libtoolize version is indeed 2.2.4 or newer, otherwise the --install could not be available. Your version of $1 is:
	" `$1 --version` "
	The corresponding executable is:
	" `/bin/ls -l --color $(which $1)`

			elif [ "$1" = "./configure" ]; then
				echo "
Note: check the following log:" `pwd`/config.log
  			fi

		fi

		exit $RES
	fi

	wait

}


generateCustom()
# Old-fashioned way of regenerating the build system from scratch:
# (this approach is still the one used, as more reliable)
{

	echo "--- generating build system"

	if [ $do_remove_generated -eq 0 ] ; then
		echo
		echo " - removing all generated files"
		./cleanGeneratedConfigFiles.sh
	fi


	if [ $do_clean_prefix -eq 0 ] ; then
		echo
		if [ -z "$osdl_prefix" -o "$osdl_prefix" = "$HOME" ] ; then
			echo "(no OSDL prefix removed)"
		else
			returnedChar="y"
			if [ "$osdl_prefix" != "$osdl_prefix_default" ] ; then
				read -p "Do you really want to erase the whole OSDL tree in $osdl_prefix? (y/n) [n] " returnedChar
			fi

			if [ "$returnedChar" = "y" ] ; then
				echo " - cleaning OSDL prefix ($osdl_prefix)"
				${RM} -rf $osdl_prefix/include/OSDL $osdl_prefix/lib/libOSDL* $osdl_prefix/share/OSDL*
				echo "(prefix cleaned)"
			fi
		fi
	fi


	# Update timestamps since SVN may mess them up:
	CONFIG_SOURCE=configure-template.ac
	touch $CONFIG_SOURCE

	CONFIG_TARGET=configure.ac

	# Config files are to lie in 'src/conf/build' directory:
	CONFIG_DIR=$RUNNING_DIR

	SETTINGS_FILE="OSDLSettings.inc"

	echo
	echo " - generating $CONFIG_TARGET, by filling $CONFIG_SOURCE with $SETTINGS_FILE"

	# Generates 'configure.ac' with an already cooked dedicated Makefile:
	execute make -f MakeConfigure clean config-files SUBSTITUTE=$substitute_script

	# Prepare to run everything from the root directory (containing 'src' and
	# 'test').
	#
	# This is because automake and al *must* be run from that directory and that
	# configure.ac has a hardcoded AC_CONFIG_AUX_DIR


	# Go to the top directory of the sources:
	cd $SOURCE_OFFSET

	echo
	echo " - preparing libtool and ltdl, by executing libtoolize"


	(libtool --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: You must have 'libtool' installed."
		echo "You can get it from: ftp://ftp.gnu.org/pub/gnu/"
		exit 20
   	}

	(libtoolize --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: You must have 'libtoolize' installed."
		echo "You can get it from: ftp://ftp.gnu.org/pub/gnu/"
		exit 21
   	}

	if test -z "$verbose"; then
		libtoolize_verbose=""
	else
		libtoolize_verbose="--debug"
	fi

	execute libtoolize --install --ltdl --automake $copy $force $libtoolize_verbose

	echo
	echo " - generating aclocal.m4, by scanning configure.ac"

	(aclocal --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: Missing 'aclocal'.  The version of 'automake'"
		echo "installed does not appear recent enough."
		echo "You can get automake from ftp://ftp.gnu.org/pub/gnu/"
		exit 22
	}

	# Contains some *.m4 prerequesites, including ceylan.m4, sdl.m4, etc.:
	M4_DIR=${CONFIG_DIR}/m4

	ACLOCAL_OUTPUT=src/conf/build/m4/aclocal.m4

	# With newer libtool (ex: 2.2.4), we need to include a whole bunch of *.m4
	# files, otherwise 'warning: LTOPTIONS_VERSION is m4_require'd but not
	# m4_defun'd' ... ', same thing for LTSUGAR_VERSION, LTVERSION_VERSION, etc.
	GUESSED_LIBTOOL_BASE=`which libtool|sed 's|/bin/libtool$||1'`

	# Do not use '--acdir=.' since it prevents aclocal from writing its file:
	execute aclocal -I ${M4_DIR} -I ${GUESSED_LIBTOOL_BASE}/share/aclocal --output=$ACLOCAL_OUTPUT $force $verbose

	# automake wants absolutely to find aclocal.m4 in the top-level directory:
	ln -sf src/conf/build/m4/aclocal.m4

	echo
	echo " - generating a '#define'-based template file for 'configure'"

	(autoheader --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: You must have 'autoheader' installed."
		echo "You can get it from: ftp://ftp.gnu.org/pub/gnu/"
		exit 23
	}

	execute autoheader $warnings $force $verbose


	echo
	echo " - generating '.in' files from '.am' files with automake"

	(automake --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: You must have 'automake' installed."
		echo "You can get it from: ftp://ftp.gnu.org/pub/gnu/"
		exit 24
	}

	automake_strictness="--foreign"
	#automake_strictness="--gnu"

	execute automake --add-missing --include-deps $automake_strictness $warnings $copy $verbose


	(autoconf --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: You must have 'autoconf' installed."
		echo "Download the appropriate package for your distribution,"
		echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
		exit 25
	}

	echo
	echo " - generating 'configure' script"
 	execute autoconf $warnings $force $verbose

	# Add GNU gettext (autopoint)?

	if [ $do_stop_after_configure_generation -eq 0 ] ; then
		echo
		echo "Now you are ready to run $RUNNING_DIR/$SOURCE_OFFSET/configure"
		return
	fi

	echo
	echo " - executing 'configure' script with following options: ' $configure_opt'."


	(./configure --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: the 'configure' cannot be properly used"
		exit 26
	}


 	execute ./configure $configure_opt


	if [ $do_clean -eq 0 ] ; then
		echo
		echo " - cleaning all"
	 	execute make clean
	fi


	if [ $do_build -eq 0 ] ; then
		echo
		echo " - building all"
	 	execute make
	fi


	if [ $do_check -eq 0 ] ; then
		echo
		echo " - checking all"
	 	execute make check
	fi


	if [ $do_install -eq 0 ] ; then
		echo
		echo " - installing"
	 	execute make install
	fi


	if [ $do_installcheck -eq 0 ] ; then
		echo
		echo " - checking install"
	 	execute make installcheck
	fi


	if [ $do_chain_tests -eq 0 ] ; then
		echo
		echo " - building and running OSDL test suite"
		cd test
	 	execute ./autogen.sh --with-osdl-env-file $osdl_environment_file
		cd ..
	elif [ $do_only_prepare_dist -eq 0 ] ; then
		echo
		echo " - generating configure for test suite"
		cd test
	 	execute ./autogen.sh --only-prepare-dist --with-osdl-env-file $osdl_environment_file
		cd ..
		echo " - making distribution package"
		execute make dist-bzip2
	fi


	if [ $do_distcheck -eq 0 ] ; then
		echo
		echo " - making distcheck"
	 	execute make distcheck
	fi


	if [ $do_generate_tools -eq 0 ] ; then
		echo
		echo " - building and running OSDL tools"
		cd tools
	 	execute ./autogen.sh --with-osdl-env-file $osdl_environment_file
		cd ..
	fi


}


regenerateWithAutoreconf()
# The supposedly standard way of having a build system up and running, since
# autoreconf has been fixed now. However it seems to be less reliable than the
# full build from scratch, thus we still prefer the latter (custom) to the
# former.
{

	echo "--- updating build system using autoreconf"

	autoreconf_opt="--force --install"
	autoreconf_warnings="--warnings=all"

	autoreconf $autoreconf_opt $autoreconf_warnings

}


generateCustom
#regenerateWithAutoreconf
