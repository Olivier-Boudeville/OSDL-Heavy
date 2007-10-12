#!/bin/sh


USAGE="
Usage : "`basename $0`" [ --with-osdl-environment <path> ] [ -g | --guess-osdl-environment ] [ -n | --no-build ] [ -o | --only-prepare-dist] : (re)generates all the autotools-based build system for OSDL tests.

	--with-osdl-environment : specify which OSDL environment file shall be used (OSDL-environment.sh full path)
	--guess-osdl-environment : try to guess where the OSDL environment file lies. If one is found, then it is used, otherwise stops on failure
	--no-build : stop just after having generated the configure script
	--only-prepare-dist : perform only necessary operations so that the test directory can be distributed afterwards"

# These tests must rely on a related Ceylan source directory, since they :
#	- need to know which Ceylan version is to be tested
#	- use some Ceylan facilities (ex : Ceylan substitute script)


# Main settings section.

# 0 means true, 1 means false :
do_remove_generated=0
do_stop_after_configure=1
do_clean=0
do_build=0
do_install=0
do_test=0


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

# Always start from 'src/conf/build' directory :
cd `dirname $COMMAND`


# Default value guessed from current path :
loani_repository=`pwd|sed 's|/osdl/OSDL/trunk/test||1'`
#echo "loani_repository = $loani_repository"

loani_installations=`dirname $loani_repository`/LOANI-installations
#echo "loani_installations = $loani_installations"



osdl_environment_file=""

while [ $# -gt 0 ] ; do
	token_eaten=1
		
		
	if [ "$1" = "-g" -o "$1" = "--guess-osdl-environment" ] ; then
	
		osdl_environment_file=${loani_installations}/OSDL-environment.sh
		
		if [ ! -f "${osdl_environment_file}" ] ; then
			echo "Error, guessed OSDL environment file not found (${osdl_environment_file}), please specify it.
$USAGE" 1>&2		
			exit 6
		fi
			
		warning "Guessed OSDL environment file is ${osdl_environment_file}." 
		. ${osdl_environment_file}
		token_eaten=0
		
	fi
	

	if [ "$1" = "-n" -o "$1" = "--no-build" ] ; then
		do_stop_after_configure=0
		token_eaten=0
	fi

	if [ "$1" = "-o" -o "$1" = "--only-prepare-dist" ] ; then
		do_stop_after_configure=0
		token_eaten=0
	fi
	
	if [ "$1" = "--with-osdl-environment" ] ; then
		shift
		osdl_environment_file="$1"
		if [ ! -f "${osdl_environment_file}" ] ; then
			echo "Error, specified OSDL environment file does not exist ($osdl_environment_file).
$USAGE" 1>&2	
			exit 10
		fi
		. ${osdl_environment_file}
		token_eaten=0
	fi
	
	if [ "$1" = "-h" -o "$1" = "--help" ] ; then
		echo "$USAGE"
		exit
		token_eaten=0
	fi

	if [ $token_eaten -eq 1 ] ; then
		echo "Error, unknown argument ($1).$USAGE" 1>&2
		exit 4
	fi	
	shift
done





# Where the libraries should be found :
if [ -n "$osdl_environment_file" ] ; then

	# If these lines are changed, change accordingly loani-requiredTools.sh,
	# search for "Making tests for OSDL".
	
	osdl_install_prefix_opt="--with-osdl-prefix=$OSDL_PREFIX"	
	ceylan_install_prefix_opt="--with-ceylan-prefix=$Ceylan_PREFIX"	
	sdl_install_prefix_opt="--with-sdl-prefix=$SDL_PREFIX"
	libjpeg_install_prefix_opt="--with-libjpeg-prefix=$libjpeg_PREFIX"
	zlib_install_prefix_opt="--with-zlib-prefix=$zlib_PREFIX"
	libpng_install_prefix_opt="--with-libpng-prefix=$libpng_PREFIX"
	sdl_image_install_prefix_opt="--with-sdl_image-prefix=$SDL_image_PREFIX"
	sdl_gfx_install_prefix_opt="--with-sdl_gfx-prefix=$SDL_gfx_PREFIX"
	freetype_install_prefix_opt="--with-freetype-prefix=$freetype_PREFIX"
	sdl_ttf_install_prefix_opt="--with-sdl_ttf-prefix=$SDL_ttf_PREFIX"
	libogg_install_prefix_opt="--with-ogg=$libogg_PREFIX"
	libvorbis_install_prefix_opt="--with-vorbis=$libvorbis_PREFIX"
	sdl_mixer_install_prefix_opt="--with-sdl_mixer-prefix=$SDL_mixer_PREFIX"
	
	prerequesites_prefix_opt="$osdl_install_prefix_opt $ceylan_install_prefix_opt $sdl_install_prefix_opt $libjpeg_install_prefix_opt $zlib_install_prefix_opt $libpng_install_prefix_opt $sdl_image_install_prefix_opt $sdl_gfx_install_prefix_opt $freetype_install_prefix_opt $sdl_ttf_install_prefix_opt $libogg_install_prefix_opt $libvorbis_install_prefix_opt $sdl_mixer_install_prefix_opt"
	
fi


# Where these tests should be installed :
test_install_location="$OSDL_PREFIX"

if [ -n "$test_install_location" ] ; then
	test_install_location_opt="--prefix=$test_install_location"
else
	test_install_location_opt=""
fi


# To check the user can override them :
#test_overriden_options="CPPFLAGS=\"-DTEST_CPPFLAGS\" LDFLAGS=\"-LTEST_LDFLAGS\""
test_overriden_options=""

configure_opt="-enable-strict-ansi --enable-debug $prerequesites_prefix_opt $test_install_location_opt $test_overriden_options"


if [ -n "$Ceylan_PREFIX" ] ; then
	ceylan_location="$Ceylan_PREFIX"
else
	# Default prefix :
	ceylan_location="/usr/local"
fi
		
if [ ! -d "$ceylan_location" ] ; then
	
	echo "Error, Ceylan prefix retrieved thanks to guessed OSDL environment file ($osdl_environment_file) does not exist ($ceylan_location)" 1>&2
	exit 7
		
fi	


if [ -n "$OSDL_PREFIX" ] ; then
	osdl_location="$OSDL_PREFIX"
else
	# Default prefix :
	osdl_location="/usr/local"
fi

	
if [ ! -d "$osdl_location" ] ; then
	
	echo "Error, OSDL prefix retrieved thanks to guessed OSDL environment file ($osdl_environment_file) does not exist ($osdl_location)" 1>&2
	exit 8
		
fi	


# Searches for the Ceylan substitute script :
CEYLAN_SUBSTITUTE_SCRIPT="$ceylan_location/share/Ceylan/scripts/shell/substitute.sh"

if [ ! -x "${CEYLAN_SUBSTITUTE_SCRIPT}" ] ; then
	echo "Error, no executable Ceylan substitute script found (searched for ${CEYLAN_SUBSTITUTE_SCRIPT})." 1>&2
	if [ "$ceylan_location" = "/usr/local" ] ; then
		echo "Forgot to specify an OSDL-environment file, or to request to guess it ?"  1>&2
	fi
	exit 1
fi


# Searches for the OSDL settings file :
OSDL_SETTINGS_FILE="../src/conf/OSDLSettings.inc"
if [ ! -f "${OSDL_SETTINGS_FILE}" ] ; then
	echo "Error, no OSDL settings file found (${OSDL_SETTINGS_FILE})." 1>&2
	exit 2
fi


# Log-on-file mode activated iff equal to true (0) :
log_on_file=1

log_filename="autogen.log"

debug "log_filename = $log_filename"
if [ -f "$log_filename" ]; then
	$RM "$log_filename"
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
echo "Bootstrapping now build for test system thanks to the autotools"
echo "      (this may take a while ...)"
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

	if [ ! $RES -eq 0 ] ; then
		echo 1>&2
		if [ $log_on_file -eq 0 ] ; then
			echo "Error while executing '$*', see $log_filename" 1>&2
		else
			echo "Error while executing '$*'" 1>&2
			if [ "$1" = "./configure" ]; then
				echo "
Note : check the following log :" test/config.log	
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
	
	# Update timestamps since SVN may mess them up :
	CONFIG_SOURCE=configure-template.ac
	CONFIG_TARGET=configure.ac
	
	touch $CONFIG_SOURCE
	
		
	echo
	echo " - generating $CONFIG_TARGET, by filling $CONFIG_SOURCE with ${OSDL_SETTINGS_FILE}"

	# Generates 'configure.ac' with an already cooked dedicated Makefile :
	make -f MakeConfigure clean config-files SETTINGS_FILE=${OSDL_SETTINGS_FILE} SUBSTITUTE=${CEYLAN_SUBSTITUTE_SCRIPT}

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
		echo "installed does not appear recent enough."
		echo "You can get automake from ftp://ftp.gnu.org/pub/gnu/"
		exit 22
	}

	# Where ceylan.m4, pkg.m4, etc. should be found : 
	CEYLAN_M4_DIR=$ceylan_location/share/Ceylan
	
	# Where osdl.m4, sdl.m4, etc. should be found : 
	OSDL_M4_DIR=$osdl_location/share/OSDL
	
	ACLOCAL_OUTPUT=aclocal.m4
	
	# Do not use '--acdir=.' since it prevents aclocal from writing its file :
	execute aclocal -I $CEYLAN_M4_DIR -I $OSDL_M4_DIR --output=$ACLOCAL_OUTPUT $force $verbose
	
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
	
	if [ "$do_stop_after_configure" -eq 0 ] ; then
		echo
		echo "Now you are ready to run configure"
		return
	fi
		
	echo
	echo " - executing 'configure' script"
	
	(./configure --version) < /dev/null > /dev/null 2>&1 || {
		echo
		echo "**Error**: the 'configure' cannot be properly used"
		exit 26
	}
	

	if [ -n "$ceylan_install_prefix_opt" ] ; then
		echo "(updating, for this script only, library search path with ${ceylan_install_prefix}/lib)"
		LD_LIBRARY_PATH=$ceylan_install_prefix/lib:$LD_LIBRARY_PATH
	fi
	
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
	
	
	if [ $do_install -eq 0 ] ; then
		echo
		echo " - installing"
	 	execute make install
	else
	
		if [ -n "$osdl_install_prefix_opt" ] ; then
			echo 1>&2
			echo "Warning : not installing tests and using $osdl_install_prefix_opt implies updating library search paths to select the correct library, for example one may enter : " 1>&2
			echo "export LD_LIBRARY_PATH=$osdl_install_prefix/lib:\$LD_LIBRARY_PATH" 1>&2
		fi
		
	fi

	if [ $do_test -eq 0 ] ; then
		export LD_LIBRARY_PATH=$ceylan_install_prefix/lib:$LD_LIBRARY_PATH
		echo
		echo " - running unit tests"
	 	execute make check
	fi
	
	
	if [ -n "$osdl_environment_file" ] ; then
		echo "Note that before running tests from the command-line, the runtime environment must be set (ex : OSDL-environment.sh must have been sourced beforehand)."
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



