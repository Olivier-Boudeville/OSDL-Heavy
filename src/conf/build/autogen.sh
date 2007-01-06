#!/bin/bash

USAGE="Usage : "`basename $0`" [ -h | --help ] [ -n | --no-build ] [ -c | --chain-test ] [ -f | --full-test ] [ -o | --only-prepare-dist ] [ --loani-repository <path> ] [ --loani-installations <path> ] [ --ceylan-install-prefix <path> ] [ --configure-options [option 1] [option 2] [...] ] : (re)generates all the autotools-based build system.\n\t --disable-all-features : build a library with none of the optional features\n\t --no-build : stop just after having generated the configure script\n\t --chain-test : build and install the library, build the test suite and run it against the installation\n\t --full-test : build and install the library, perform all available tests, including 'make distcheck' and the full test suite\n\t --only-prepare-dist : configure all but do not build anything\n\t --loani-repository : specify where the LOANI repository can be found. If none is specified, it will be guessed from the current directory\n\t --loani-installations : specify where the LOANI installations can be found. If none is specified, it will be guessed from the current directory\n\t --ceylan-install-prefix : specify where the Ceylan installation can be found.\n\t --configure-options : all following options will be directly passed whenever configure is run"


# Main settings section.

osdl_features_disable_opt=""

osdl_features_opt=""


# To check the user can override them :
#test_overriden_options="CPPFLAGS=\"-DTEST_CPPFLAGS\" LDFLAGS=\"-LTEST_LDFLAGS\""

# Default value guessed from current path :
loani_repository=`pwd|sed 's|/osdl/OSDL/trunk/src/conf/build||1'`
#echo "loani_repository = $loani_repository"

loani_installations=`dirname $loani_repository`/LOANI-installations
#echo "loani_installations = $loani_installations"


loani_repository_specified=1
loani_installations_specified=1

ceylan_location=""


# 0 means true, 1 means false :
do_remove_generated=0
do_clean_prefix=0
do_stop_after_configure_generation=1
do_clean=0
do_build=0
do_check=0
do_install=0
do_installcheck=0
do_distcheck=1
do_chain_tests=1
do_only_prepare_dist=1


while [ "$#" -gt "0" ] ; do
	token_eaten=1
	
	if [ "$1" == "-v" ] || [ "$1" == "--verbose" ] ; then
		be_verbose=0
		token_eaten=0
	fi
	
	if [ "$1" == "-q" ] || [ "$1" == "--quiet" ] ; then
		be_quiet=0
		token_eaten=0
	fi
	
	if [ "$1" == "-d" ] || [ "$1" == "--disable-all-features" ] ; then
		osdl_features_opt="$osdl_features_disable_opt"
		token_eaten=0
	fi

	if [ "$1" == "-n" ] || [ "$1" == "--no-build" ] ; then
		do_stop_after_configure_generation=0
		token_eaten=0
	fi
	
	if [ "$1" == "-c" ] || [ "$1" == "--chain-test" ] ; then
		do_chain_tests=0		
		token_eaten=0
	fi
	
	if [ "$1" == "-f" ] || [ "$1" == "--full-test" ] ; then
		do_chain_tests=0	
		do_distcheck=1	
		token_eaten=0
	fi
	
	if [ "$1" == "-o" ] || [ "$1" == "--only-prepare-dist" ] ; then
		do_build=1
		do_check=1
		do_install=1
		do_installcheck=1
		do_distcheck=1
		do_chain_tests=1
		do_only_prepare_dist=0
		token_eaten=0
	fi
	
	if [ "$1" == "--loani-repository" ] ; then
		shift
		loani_repository_specified=0
		loani_repository="$1"
		if [ ! -d "${loani_repository}" ] ; then
			echo -e "Error, specified LOANI repository directory does not exist ($loani_repository).\n$USAGE" 1>&2	
			exit 5
		fi
		token_eaten=0
	fi
	
	if [ "$1" == "--loani-installations" ] ; then
		shift
		loani_installations_specified=0
		loani_installations="$1"
		if [ ! -d "${loani_installations}" ] ; then
			echo -e "Error, specified LOANI installations directory does not exist ($loani_installations).\n$USAGE" 1>&2	
			exit 6
		fi
		token_eaten=0
	fi
	
	if [ "$1" == "--ceylan-install-prefix" ] ; then
		shift
		ceylan_location="$1"
		if [ ! -d "${ceylan_location}" ] ; then
			echo -e "Error, specified Ceylan installation directory does not exist ($ceylan_location).\n$USAGE" 1>&2	
			exit 7
		fi
		token_eaten=0
	fi
	
	if [ "$1" == "--configure-options" ] ; then
		shift
		configure_opt="$*"
		while [ "$#" -gt "0" ] ; do
			shift
		done		
		token_eaten=0
	fi
	
	if [ "$1" == "-h" ] || [ "$1" == "--help" ] ; then
		echo -e "$USAGE"
		exit
		token_eaten=0
	fi

	if [ "$token_eaten" == "1" ] ; then
		echo -e "Error, unknown argument ($1).\n$USAGE" 1>&2
		exit 4
	fi	
	shift
done


# debug mode activated iff equal to true (0) :
debug_mode=1

debug()
{
	if [ $debug_mode -eq 0 ] ; then
		echo "debug : $*"
	fi	
}


warning()
{
	echo "warning : $*" 1>&2
}



# Wait-after-execution mode activated iff equal to true (0) :
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

# Always start from 'src/conf/build' directory :
cd `dirname $COMMAND`

RUNNING_DIR=`pwd`
#echo "RUNNING_DIR = $RUNNING_DIR"

# How to go from RUNNING_DIR to base directory 
# (the one containing src and test) :
SOURCE_OFFSET="../../.."


# Prefix section for the OSDL installation.

PREFIX_DEFAULT=$loani_installations
debug "Default prefix = ${PREFIX_DEFAULT}"

PREFIX_SECOND_DEFAULT="$HOME/tmp-OSDL-test-install"

if [ ! -d `dirname ${PREFIX_DEFAULT}` ] ; then
	echo "Base of first default install directory (${PREFIX_DEFAULT}) not existing, switching to second default directory (${PREFIX_SECOND_DEFAULT})"
	PREFIX="${PREFIX_SECOND_DEFAULT}"
else
	PREFIX="${PREFIX_DEFAULT}"
fi

#debug "Prefix = ${PREFIX}"
 
mkdir -p ${PREFIX}


if [ -n "${PREFIX}" ] ; then
	PREFIX_OPT="--prefix=$PREFIX"
else
	PREFIX_OPT=""
fi


if [ -z "${configure_opt}" ] ; then
	configure_opt="$osdl_features_opt --enable-strict-ansi --enable-debug $PREFIX_OPT $test_overriden_options"
fi



# Log-on-file mode activated iff equal to true (0) :
log_on_file=1

log_filename="$RUNNING_DIR/autogen.log"

debug "log_filename = $log_filename"
if [ -f "$log_filename" ]; then
	$RM "$log_filename"
fi

debug "COMMAND = $COMMAND"
debug "RUNNING_DIR = $RUNNING_DIR" 

debug "loani_installations = $loani_installations"

echo 

if [ $loani_repository_specified -eq 1 ] ; then
	if [ ! -d "${loani_repository}" ] ; then
		echo -e "Error, no LOANI repository directory specified, and guessed one does not exist ($loani_repository).\n$USAGE" 1>&2	
		exit 7
	fi	
		
	warning "No directory specified for LOANI repository, defaulting to $loani_repository"
	
fi


if [ $loani_installations_specified -eq 1 ] ; then
	if [ ! -d "${loani_installations}" ] ; then
		echo -e "Error, no LOANI installation directory specified, and guessed one does not exist ($loani_installations).\n$USAGE" 1>&2	
		exit 8
	fi	
		
	warning "No directory specified for LOANI installations, defaulting to $loani_installations"
	
fi


# Guessing where Ceylan should be found :
#   1. try under corresponding specified or guessed LOANI-installations 
#   2. otherwise, look into standard locations (no specific option passed)

if [ -z "${ceylan_location}" ] ; then

	# Ignore elements of Makefile syntax :
	source ../OSDLSettings.inc 2>/dev/null
		ceylan_loanized_dir="${loani_installations}/Ceylan-${CEYLAN_OLDEST_SUPPORTED_MAJOR}.${CEYLAN_OLDEST_SUPPORTED_MINOR}"
	
	if [ -d "${ceylan_loanized_dir}" ] ; then
		ceylan_location="${ceylan_loanized_dir}"
	fi
	
fi


if [ -n "${ceylan_location}" ] ; then
	# Update accordingly the configure prefix for Ceylan macro CEYLAN_PATH :
	configure_opt="${configure_opt} --with-ceylan-prefix=$ceylan_location"
else
	# The default install location when no prefix is used :
	ceylan_location="/usr/local"
fi

#echo "ceylan_location = ${ceylan_location}"


# Retrieving Ceylan substitute.sh script, needed by MakeConfigure :
CEYLAN_SHARED_DIR=${ceylan_location}/share/Ceylan
if [ ! -d "${CEYLAN_SHARED_DIR}" ] ; then
	echo -e "Error, no Ceylan share directory found ($CEYLAN_SHARED_DIR)" 1>&2	
	exit 9
fi

SUBSTITUTE_SCRIPT="${CEYLAN_SHARED_DIR}/scripts/shell/substitute.sh"
if [ ! -x "${SUBSTITUTE_SCRIPT}" ] ; then
	echo -e "Error, no executable substitute script found ($SUBSTITUTE_SCRIPT)" 1>&2	
	exit 10
fi
	

# Overall autotools settings :

# Be verbose for debug purpose :
#verbose="--verbose"
verbose=""

# Copy files instead of using symbolic link :
copy="--copy"
#copy=""

# Replace existing files :
#force="--force"
force=""

# Warning selection : 
warnings="--warnings=all"
#warnings=""



echo
echo "Bootstrapping now build system thanks to the autotools"
echo "      (this may take a while ...)"
echo


execute()
{

	echo "    Executing $*"
	
	if [ "$log_on_file" -eq 0 ] ; then
		echo "    Executing $* from "`pwd` >>"$log_filename"
		eval $* >>"$log_filename" 2>&1
		RES=$?
		echo "----------------------------------------" >>"$log_filename"
		echo >>"$log_filename"
	else
		eval $* 
		RES=$?
	fi

	if [ ! $RES -eq 0 ] ; then
		echo 1>&2
		if [ "$log_on_file" -eq 0 ] ; then
			echo "Error while executing '$*', see $log_filename" 1>&2
		else
			echo "Error while executing '$*'" 1>&2
			
			AUTOMAKE_HINT="\nTo upgrade automake and aclocal from Debian-based distributions, do the following as root : 'apt-get install automake1.9' which updates aclocal too. One has nonetheless to update the symbolic links /etc/alternatives/aclocal so that it points to /usr/bin/aclocal-1.9, and /etc/alternatives/automake so that it points to /usr/bin/automake-1.9"
			
			if [ "$1" == "aclocal" ]; then
				echo -e "\nNote : if aclocal is failing since AM_CXXFLAGS (used in configure.ac) 'cannot be found in library', then check that your aclocal version is indeed 1.9 or newer. For example, with Debian-based distributions, /usr/bin/aclocal is a symbolic link to /etc/alternatives/aclocal, which itself is a symbolic link which may or may not point to the expected aclocal version. Your version of $1 is :\n\t" `$1 --version` "\n\n\t" `/bin/ls -l --color $(type -p $1)` "${AUTOMAKE_HINT}"
			elif [ "$1" == "automake" ]; then
				echo -e "\nNote : check that your automake version is indeed 1.9 or newer. For example, with Debian-based distributions, /usr/bin/automake is a symbolic link to /etc/alternatives/automake, which itself is a symbolic link which may or may not point to the expected automake version. Your version of $1 is :\n\t" `$1 --version` "\n\n\t" `/bin/ls -l --color $(type -p $1)` "${AUTOMAKE_HINT}"
			elif [ "$1" == "./configure" ]; then
				echo -e "\nNote : check the following log :" `pwd`/config.log
  			fi
			
		fi
			
		exit $RES
	fi
	
	wait
	
}                 
    
	
	                                                 
generateCustom()
# Old-fashioned way of regenerating the build system from scratch : 
{

	echo "--- generating build system"
	
	if [ "$do_remove_generated" -eq 0 ] ; then
		echo
		echo " - removing all generated files"
		./cleanGeneratedConfigFiles.sh
	fi
	
	
	if [ "$do_clean_prefix" -eq 0 ] ; then
		echo
		if [ -z "$PREFIX" -o "$PREFIX" == "$HOME" ] ; then
			echo "(no PREFIX=$PREFIX removed)"
		else	
			returnedChar="y"
			if [ "$PREFIX" != "$PREFIX_DEFAULT" ] ; then
				read -p "Do you really want to erase the whole OSDL tree in $PREFIX ? (y/n) [n] " returnedChar 
			fi
			
			if [ "$returnedChar" == "y" ] ; then
				echo " - cleaning PREFIX = $PREFIX"
				${RM} -rf $PREFIX/include/OSDL $PREFIX/lib/libOSDL* $PREFIX/share/OSDL* 
				echo "(prefix cleaned)"
			fi
		fi	
	fi
	

	# Update timestamps since SVN may mess them up :
	CONFIG_SOURCE=configure-template.ac
	touch $CONFIG_SOURCE

	CONFIG_TARGET=configure.ac
	
	# Config files are to lie in 'src/conf/build' directory :
	CONFIG_DIR=$RUNNING_DIR
	
	SETTINGS_FILE="OSDLSettings.inc"
	
	echo
	echo " - generating $CONFIG_TARGET, by filling $CONFIG_SOURCE with $SETTINGS_FILE"

	# Generates 'configure.ac' with an already cooked dedicated Makefile :
	execute make -f MakeConfigure clean config-files SUBSTITUTE=$SUBSTITUTE_SCRIPT
	
	# Prepare to run everything from the root directory (containing 'src'
	# and 'test').
	# This is because automake and al *must* be run from that directory
	# and that configure.ac has a hardcoded AC_CONFIG_AUX_DIR
	
	
	# Go to the top directory of the sources :
	cd $SOURCE_OFFSET
		
	echo
	echo " - preparing libtool, by executing libtoolize"
	
	
	(libtool --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: You must have \`libtool' installed."
		echo "You can get it from: ftp://ftp.gnu.org/pub/gnu/" 
		exit 20
   	}
	
	(libtoolize --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: You must have \`libtoolize' installed."
		echo "You can get it from: ftp://ftp.gnu.org/pub/gnu/" 
		exit 21
   	}

	if test -z "$verbose"; then
		libtoolize_verbose=""
	else
		libtoolize_verbose="--debug"
	fi
	
	execute libtoolize --automake $copy $force $libtoolize_verbose
	
	echo
	echo " - generating aclocal.m4, by scanning configure.ac"
	
	(aclocal --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: Missing \`aclocal'.  The version of \`automake'"
		echo "installed doesn't appear recent enough."
		echo "You can get automake from ftp://ftp.gnu.org/pub/gnu/"
		exit 22
	}

	M4_DIR=${CONFIG_DIR}/m4 
	M4_CEYLAN_DIR=${CEYLAN_SHARED_DIR}
	
	ACLOCAL_OUTPUT=src/conf/build/m4/aclocal.m4
	
	# Do not use '--acdir=.' since it prevents aclocal from writing its file :
	execute aclocal -I ${M4_DIR} -I ${M4_CEYLAN_DIR} --output=$ACLOCAL_OUTPUT $force $verbose

	# automake wants absolutely to find aclocal.m4 in the top-level directory :
	ln -sf src/conf/build/m4/aclocal.m4

	echo
	echo " - generating a '#define'-based template file for 'configure'"
	
	(autoheader --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: You must have \`autoheader' installed."
		echo "You can get it from: ftp://ftp.gnu.org/pub/gnu/"
		exit 23
	}

	execute autoheader $warnings $force $verbose

	
	echo
	echo " - generating '.in' files from '.am' files with automake"

	(automake --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: You must have \`automake' installed."
		echo "You can get it from: ftp://ftp.gnu.org/pub/gnu/"
		exit 24
	}

	automake_strictness="--foreign" 
	#automake_strictness="--gnu"
	
	execute automake --add-missing --include-deps $automake_strictness $warnings $copy $verbose  

	
	(autoconf --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: You must have \`autoconf' installed."
		echo "Download the appropriate package for your distribution,"
		echo "or get the source tarball at ftp://ftp.gnu.org/pub/gnu/"
		exit 25
	}
	
	echo
	echo " - generating 'configure' script"
 	execute autoconf $warnings $force $verbose

	# Add GNU gettext (autopoint) ?
	
	if [ "$do_stop_after_configure_generation" -eq 0 ] ; then
		echo
		echo "Now you are ready to run $RUNNING_DIR/$SOURCE_OFFSET/configure"
		return
	fi
		
	echo
	echo " - executing 'configure' script with following options : ' $configure_opt'."
	

	(./configure --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: the 'configure' cannot be properly used"
		exit 26
	}
	
	
 	execute ./configure $configure_opt
	

	if [ "$do_clean" -eq 0 ] ; then
		echo
		echo " - cleaning all"
	 	execute make clean
	fi
	
	
	if [ "$do_build" -eq 0 ] ; then
		echo
		echo " - building all"
	 	execute make
	fi
	
	
	if [ "$do_check" -eq 0 ] ; then
		echo
		echo " - checking all"
	 	execute make check
	fi
	
	
	if [ "$do_install" -eq 0 ] ; then
		echo
		echo " - installing"
	 	execute make install
	fi
	
	
	if [ "$do_installcheck" -eq 0 ] ; then
		echo
		echo " - checking install"
	 	execute make installcheck
	fi
	
	
	if [ "$do_distcheck" -eq 0 ] ; then
		echo
		echo " - making distcheck"
	 	execute make distcheck
	fi
	
	
	if [ "$do_chain_tests" -eq 0 ] ; then
		echo
		echo " - building and running test suite"
		cd test
	 	execute ./autogen.sh --osdl-install-prefix $PREFIX
	elif [ "$do_only_prepare_dist" -eq 0 ] ; then
		echo
		echo " - generating configure for test suite"
		cd test
	 	execute ./autogen.sh --only-prepare-dist
		cd .. 
		echo " - making distribution package"
		execute make dist-bzip2 
	fi
		
		
}


regenerateWithAutoreconf()
# The current way of having a build system up and running, since autoreconf
# has been fixed now. However it seems to
{

	echo "--- updating build system using autoreconf"
	
	autoreconf_opt="--force --install"
	autoreconf_warnings="--warnings=all" 
	
	autoreconf $autoreconf_opt $autoreconf_warnings
	
}
	
	
generateCustom
#regenerateWithAutoreconf



