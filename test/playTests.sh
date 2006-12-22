#!/bin/bash

TESTLOGFILE=`pwd`"/testsOutcome.txt"

USAGE="`basename $0` [--batch] : executes all tests for OSDL in a row.\n\tIf the --batch option is used, tests won't be interactive, and will be silently executed. Only their final result will be output, their messages will be stored in file ${TESTLOGFILE}."


# Please remember, when debugging it, to execute the playTest.sh from *installed* version 
# but to modify the playTest.sh from *source* code, and to copy back the latter to the former.
 

# Not in batch mode by default.
is_batch=1


if [ "$#" -ge "2" ] ; then
	echo -e "Usage : $USAGE" 1>&2
	exit 1
fi	

if [ "$#" == "1" ] ; then
	if [ "$1" != "--batch" ] ; then
		echo "$1 : unknown option." 1>&2
		echo -e "Usage : $USAGE" 1>&2
		exit 2
	else
		is_batch=0	
		WARNING_INTERNAL "Even in batch mode, some tests remain interactive since it is the purpose of the test."
	fi
fi

# Debug mode is deactivated by default (1).
debug_mode=1


DEBUG_INTERNAL()
# Prints debug informations if debug mode is on.
{
	[ "$debug_mode" == 1 ] || echo "Debug : $*"
}


WARNING_INTERNAL()
# Prints warning informations to error output.
{
	echo "Warning : $*" 1>&2
}


ERROR_INTERNAL()
# Prints error informations to error output.
{
	echo "Error : $*" 1>&2
}


DEBUG_INTERNAL "Debug mode activated"


# Guessing where OSDL sources might be (ex : OSDL=<..>/OSDL/OSDL-x.y/src)

if [ -n "${OSDL}" ] ; then
	if [ ! -d "${OSDL}" ] ; then
		WARNING_INTERNAL "OSDL environment variable was set to <${OSDL}> which is not a directory, ignoring this setting."
		OSDL=""
	fi	
fi

if [ -z "${OSDL}" ] ; then

	# If absolute path given :
	first_char=`echo $0 | head -c 1`
	if [ "$first_char" == "/" ] ; then
		root_dir=`dirname $0`
		OSDL=`dirname $root_dir`/src
		DEBUG_INTERNAL "Absolute path given, deducing OSDL : ${OSDL}"
	else
		# If launched from $OSDL_ROOT/bin
		OSDL=$PWD/`dirname $0`/../src
		DEBUG_INTERNAL "Testing OSDL=${OSDL}"

		if [ ! -d ${OSDL} ] ; then
			# If launched from $OSDL_ROOT/src/code
			OSDL=$PWD/`dirname $0`/..
			DEBUG_INTERNAL "Testing OSDL=${OSDL}"
			if [ ! -d ${OSDL} ] ; then
				echo "Unable to guess OSDL sources location, please set it (bash example : export OSDL=\$HOME/Projects/OSDL/OSDL-x.y/src)" 1>&2
				exit 1	
			else
				DEBUG_INTERNAL "OSDL candidate ${OSDL} acknowledged, script must have been launched from $OSDL_ROOT/src/code"	
			fi
		else
			DEBUG_INTERNAL "OSDL candidate ${OSDL} acknowledged, script must have been launched from $OSDL_ROOT/bin"			
		fi
	fi
fi

if [ ! -d ${OSDL} ] ; then
	ERROR_INTERNAL "No available OSDL directory found."
	exit 2
fi

DEBUG_INTERNAL "OSDL path is $OSDL"

VERSIONS_FILE="${OSDL}/conf/loani-versions.sh"

if [ ! -f ${VERSIONS_FILE} ] ; then
	echo "No version file found (${VERSIONS_FILE})" 1>&2
	exit 5
fi
 
source ${VERSIONS_FILE}

# Creates a test directory under OSDL/OSDL-${OSDL_VERSION}, 
# allows to avoid polluting other directories :
TEST_DIR="tests-results"
mkdir -p ${OSDL}/../${TEST_DIR}

cd ${OSDL}/code

OSDL_ENV_FILE="../../../../OSDL-environment.sh"
if [ ! -f "${OSDL_ENV_FILE}" ] ; then
	echo "Unable to find OSDL environment file (expected `pwd`/${OSDL_ENV_FILE})." 1>&2
	exit 6
fi
source ${OSDL_ENV_FILE}

SHELLS_LOCATION="../../../../${Ceylan_ROOT}/src/code/scripts/shell"

if [ ! -d ${SHELLS_LOCATION} ] ; then
	echo "Unable to find shell tools path (expected `pwd`/${SHELLS_LOCATION})." 1>&2
	exit 7
fi

DEFAULT_LOCATIONS="${SHELLS_LOCATION}/defaultLocations.sh"


if [ ! -f ${DEFAULT_LOCATIONS} ] ; then
	echo "No bash helper script found (expected ${DEFAULT_LOCATIONS}), are you sure you are running "`basename $0`" from \$OSDL_ROOT/src/code ?" 1>&2
	exit 8
fi

source ${DEFAULT_LOCATIONS}



# Corresponds to real LOANI default prefix settings :
LOANI_INSTALLATIONS_FIRST=`pwd`/../../../../../LOANI-installations

LOANI_INSTALLATIONS_SECOND=`pwd`/../../src/conf/LOANI-installations

LOANI_INSTALLATIONS=${LOANI_INSTALLATIONS_FIRST}


DEBUG_INTERNAL "Searching LOANI_INSTALLATIONS in ${LOANI_INSTALLATIONS}"
if [ ! -d ${LOANI_INSTALLATIONS} ] ; then
	DEBUG_INTERNAL "Not found in ${LOANI_INSTALLATIONS}."
	LOANI_INSTALLATIONS=${LOANI_INSTALLATIONS_SECOND}
	DEBUG_INTERNAL "Searching LOANI_INSTALLATIONS in ${LOANI_INSTALLATIONS}"
	if [ ! -d ${LOANI_INSTALLATIONS} ] ; then
		DEBUG_INTERNAL "Not found in ${LOANI_INSTALLATIONS}."
		
		ERROR_INTERNAL "Unable to find LOANI-installations, neither in ${LOANI_INSTALLATIONS_FIRST} nor in ${LOANI_INSTALLATIONS_SECOND}"
		exit 8
	fi	
fi


#SDL_image_ROOT="${LOANI_INSTALLATIONS}/SDL_image-${SDL_image_VERSION}"


#if [ -d ${SDL_image_ROOT}/lib ] ; then
	# Prefix can be unused, in this case we suppose SDL_image is to be found 
	# in default LD_LIBRARY_PATH. 
	#LD_LIBRARY_PATH=${SDL_image_ROOT}/lib:/usr/local/lib:/usr/lib:/lib:${LD_LIBRARY_PATH}
#fi
   

echo -e "\nEnforcing prerequesite, using OSDL = ${OSDL} for root directory :"

echo -e "\t1. make sure that the OSDL library to test is up to date"
echo -e "\t2. then compile and link tests to that OSDL library\n"

DEBUG_INTERNAL "Regenerating all build to be sure everything is up-to-date"

# Tries to used required gcc if detected :
unset GCC_PATH
if [ -n "$GCC_ROOT" ] ; then
	GCC_PATH="GCC_ROOT=$GCC_ROOT"
fi	

# Do not update in debug mode to avoid too much waiting :
if [ "$debug_mode" == "1" ] ; then
	echo -e "\n\tUpdating (might be a bit long)...."
	( cd ..; make -s all $GCC_PATH 1>/dev/null 2>&1 )
fi


# Ensure we are back in ${OSDL}/../${TEST_DIR} = <..>/OSDL/OSDL-x.y/${TEST_DIR} directory
cd ${OSDL}/../${TEST_DIR}

DEBUG_INTERNAL "We are in ${OSDL}/../${TEST_DIR} = "`pwd` " directory."


if [ "$is_batch" == "0" ] ; then
	echo -e "\n\tRunning in batch mode, tests will be silent, only results are to be output."
fi

echo -e "\t(be warned that some tests might take a long time, and that some of them have no special output except a test result)"

echo -e "\nInteractive tests will only need the enter key to be pressed one or more times."

# This script will automatically run each test of each selected OSDL module.
#TESTED_MODULES="basic events video video/twoDimensional audio"
TESTED_MODULES=`cd ../bin; find . -type d -a ! -name .`

for m in ${TESTED_MODULES} ; do
	
	printColor "\n\n${term_offset}${term_primary_marker}Playing all tests of module '"`echo $m | sed 's|./||'`"' : " $magenta_text $black_back
	
	if [ "$is_batch" == "1" ] ; then
	
		for t in ../bin/$m/test* ; do		
			if [ -x "$t" -a -f "$t" ] ; then
				printColor "${term_offset}${term_offset}+ $t" $cyan_text $black_back
			fi
		done

		echo -e "\n   <Press enter to start testing module '"`echo $m | sed 's|./||'`"'>"
		read $dummy
		clear
	fi

	# Used to test in their *build* location : for t in $m/tests/test*.exe ; do
	# Now, test them in place (in *install* directory)
	for t in ../bin/$m/test* ; do
		if [ -x "$t" -a -f "$t" ] ; then
			printColor "${term_primary_marker}Launching $t" $cyan_text $blue_back
			# The --interactive parameter is used to tell the test it is run in interactive mode,
			# so that those which are long (ex : stress tests) are shorten.
			if [ "$is_batch" == "0" ] ; then
				./$t --batch 1>${TESTLOGFILE} 2>&1
			else
				./$t --interactive
			fi
		
			if [ "$?" == 0 ] ; then
				if [ "$is_batch" == "1" ] ; then
					echo
				fi	
				printColor "${term_offset}Seems to be successful     " $green_text $black_back
			else
				if [ "$is_batch" == "1" ] ; then
					echo
				fi	
				printColor "${term_offset}Seems to be failed (exit status $?)     " $white_text $red_back
			fi
		
			if [ "$is_batch" == "1" ] ; then
				printColor "${term_primary_marker}End of $t, press enter to continue" $cyan_text $blue_back
				read 
				clear
			fi	
		fi
			
	done

done 


# Summary is only needed if full ouput was asked :

if [ "$is_batch" == "1" ] ; then
	clear
	printColor "${term_offset}${term_primary_marker}Following tests have been played : " $magenta_text $black_back

	for m in ${TESTED_MODULES} ; do
	
		printColor "${term_offset}${term_offset}${term_secondary_marker}For module '"`echo $m | sed 's|./||'`"' :" $magenta_text $black_back
		for t in ../bin/$m/test* ; do
			if [ -x "$t" -a -f "$t" ] ; then	
				printColor "${term_offset}${term_offset}${term_offset}${term_offset}+ $t" $cyan_text $black_back
			fi
		done		
	done
fi
