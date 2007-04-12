#!/bin/sh

TESTLOGFILE=`pwd`"/testsOutcome.txt"

USAGE="`basename $0` [--interactive] : executes all tests for OSDL in a row.
	
	If the --interactive option is used, tests will not be run in batch mode, and will prompt the user for various inputs. Otherwise only their final result will be output. In all cases their messages will be stored in file ${TESTLOGFILE}. The return code of this script will be the number of failed tests (beware to overflow of the return code)"


# Please remember, when debugging it, to execute the playTests.sh from
# *installed* version, but to modify the playTests.sh from *source* code, 
# and to copy back the latter to the former.
 

# In batch (non-interactive) mode by default (0) :
is_batch=0



if [ "$#" -ge "2" ] ; then
	echo "
	Usage : $USAGE" 1>&2
	exit 1
fi	

if [ "$#" = "1" ] ; then
	if [ "$1" != "--interactive" ] ; then
		echo "$1 : unknown option." 1>&2
		echo "
		Usage : $USAGE" 1>&2
		exit 2
	else
		is_batch=1	
	fi
fi



# Debug mode is deactivated by default (1).
debug_mode=1


DEBUG_INTERNAL()
# Prints debug informations if debug mode is on.
{
	[ "$debug_mode" = 1 ] || echo "Debug : $*"
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



# Some useful test functions :

display_launching()
# Usage : display_launching <name of the test>
{

	test_name="$1"
	
	if [ $is_batch -eq 1 ] ; then
		printColor "${term_primary_marker}Launching $test_name" $cyan_text $blue_back
	else
		printf "[${cyan_text}m%-${space_for_test_name}s" `echo $test_name|sed 's|^./||'`
	fi
	
}


display_test_result()
# Usage : display_test_result <name of the test> <test path> <returned code>
{

	test_name="$1"
	t="$2"
	return_code="$3"
	
	if [ "$return_code" = 0 ] ; then
		# Test succeeded :
		if [ $is_batch -eq 1 ] ; then
			echo
			printColor "${term_offset}$test_name seems to be successful     " $green_text $black_back
		else
			printf  "[${white_text}m[[${green_text}mOK[${white_text}m]\n"
		fi	
				
	else
			
		# Test failed :
		error_count=`expr $error_count + 1`
		if [ $is_batch -eq 1 ] ; then
			echo
			printColor "${term_offset}$t seems to be failed (exit status $return_code)     " $white_text $red_back
		else
		
			if [ "$check_dependency" -eq "0" ] ; then
			
				if [ "$on_cygwin" -eq "0" ] ; then
					# See also : http://www.dependencywalker.com/
					PATH="/cygdrive/c/Program Files/Microsoft Platform SDK for Windows Server 2003 R2/bin:$PATH"
					depend_tool="Depends.exe"
					if which $depend_tool 1>/dev/null 2>&1; then
						depends_exe=`which $depend_tool`
						"$depends_exe" $t >> ${TESTLOGFILE}
					else
						echo "No $depend_tool available, no dependency displayed."  >> ${TESTLOGFILE}
					fi
				else
					echo "$t failed, whose shared library dependencies are : " >> ${TESTLOGFILE}
					ldd $t >>${TESTLOGFILE}
				fi
			fi
			
			printf "[${white_text}m[[${red_text}mKO[${white_text}m]\n"
		fi	
	fi
}


run_test()
# Usage : run_test <name of the test> <test path> 
{

	test_name="$1"
	t="$2"
	
	# The --interactive parameter is used to tell the test it is 
	# run in interactive mode, so that those which are long 
	# (ex : stress tests) are shorten.
	if [ $is_batch -eq 0 ] ; then
		echo "
		
		########### Running now $t" >>${TESTLOGFILE}
		echo "Library dependenies : " >>${TESTLOGFILE}
		ldd $t >>${TESTLOGFILE}
		echo "Command line : $t --batch ${network_option} ${log_plug_option}" >>${TESTLOGFILE}
		$t --batch ${network_option} ${log_plug_option} 1>>${TESTLOGFILE} 2>&1
		
		#FIXME
		#if [ "$test_name" = "testOSDLScheduler" ] ; then
		#	echo STOP
		#	exit
		#fi
	else
		$t --interactive ${network_option} ${log_plug_option}
	fi			
		
	return_code="$?"
	
	display_test_result "$test_name" "$t" "$return_code"
	
		
	if [ $is_batch -eq 1 ] ; then
		printColor "${term_primary_marker}End of $test_name, press enter to continue" $cyan_text $blue_back
		read 
		clear
	fi		
			
}


display_final_stats()
{
	echo 

	if [ "$error_count" -eq "0" ] ; then
		echo "   Test result : [${green_text}m all $test_count tests succeeded[${white_text}m"
	else
		echo "   Test result : [${red_text}m $error_count out of $test_count tests failed[${white_text}m"
		echo "   (see ${TESTLOGFILE} for more details)"
	fi
}


get_logical_test_name()
# Converts executables names on Cygwin to the usual names
# 'system-testOSDLX.exe' should become 'testOSDLX'
{
	
	if [ "$on_cygwin" -eq "0" ] ; then
		returned_string=`basename $t |sed 's|^.*-test|test|1' |sed 's|.exe$||1'`
	else
		returned_string=`basename $t |sed 's|.exe$||1'`
	fi
		
}


# Try to find automagically the relevant OSDL-environment.sh file :

# If absolute path given :
first_char=`echo $0 | head -c 1`
if [ "$first_char" = "/" ] ; then
	starting_dir=`dirname $0`
	
else
	starting_dir=$PWD`dirname $0`/
fi

#echo "starting_dir = $starting_dir"

# Suppose we are in the build tree : 
loani_installations=`echo $starting_dir | sed 's|osdl/OSDL/trunk/test||1'`"../LOANI-installations"
	
#echo "loani_installations = $loani_installations"

if [ ! -d "$loani_installations" ] ; then

	# No, maybe we are on an installed tree ?
	loani_installations=`echo $starting_dir | sed 's|share/OSDL-test||1'`"../../LOANI-installations"

	if [ ! -d "$loani_installations" ] ; then
	
		ERROR_INTERNAL "unable to guess the LOANI installation repository (tried finally $loani_installations)"
		exit 1
	fi
	
fi

osdl_environment_file="$loani_installations/OSDL-environment.sh"

if [ ! -f "$osdl_environment_file" ] ; then

	ERROR_INTERNAL "unable to find OSDL environment file (tried $osdl_environment_file)"
	exit 2

fi

. $osdl_environment_file

	
# Try to find term utilities :

TEST_ROOT=`dirname $0`

SHELLS_LOCATION="$Ceylan_PREFIX/share/Ceylan/scripts/shell"

# Triggers also termUtils.sh and platformDetection.sh :
DEFAULT_LOCATIONS_PATH="$SHELLS_LOCATION/defaultLocations.sh"

if [ -f "$DEFAULT_LOCATIONS_PATH" ] ; then
	. $DEFAULT_LOCATIONS_PATH
else
	ERROR_INTERNAL "default location script not found (tried $DEFAULT_LOCATIONS_PATH)"
	exit 3
fi	

# For ping :
findSupplementaryShellTools

# For tests that need to search relative paths :
# (do not know why shell fails when doing a 'cd test' when run from trunk)
cd ${TEST_ROOT}

# Creates a test directory to avoid polluting other directories :
TEST_DIR="tests-results-"`date '+%Y%m%d'`


# Specifies the log plug the tests should be run with. 
# Note : avoid using the classical plug because it may cause scheduling 
# failures (ex : with testOSDLScheduler) because of its default synchronous 
# file I/O). 
log_plug_option="--HTMLPlug"


if [ $is_batch -eq 0 ] ; then
	echo "
	Running in batch mode, tests will be short and silent, only results are to be output."
else	
	echo "
	Interactive tests will need an input device (joystick, mouse, keyboard) to be used. Be warned though that some tests might take a long time, and that some of them have no special output except a test result."
fi

# Test whether we are online (needed for DNS queries) :
if ${PING} ${PING_OPT} 2 google.com 1>/dev/null 2>&1; then
	is_online=0
	network_option="--online"
	echo "
	Running in online mode, in-depth network testing enabled."
else	
	is_online=1
	network_option=""
	echo "
	No Internet connection detected, some network tests will be disabled."
fi


test_count=0
error_count=0

# Not using Cygwin by default to chain the tests :
on_cygwin=1

# Tells whether link dependencies should be checked in case a test fails :
check_dependency=1

# Special case for tests generated on Windows :
if [ `uname -s | cut -b1-6` = "CYGWIN" ] ; then
	
	on_cygwin=0
	DEBUG_INTERNAL "Running tests in the Windows (Cygwin) context."
	
	# Updated PATH needed to find the OSDL DLL :
	export PATH="../src/Debug:$PATH"
	
fi	

# This script will automatically run each test of each selected OSDL module.
TESTED_ROOT_MODULES=`cd ${TEST_ROOT}; find . -type d | grep -v tmp | grep -v Debug | grep -v autom4te.cache | grep -v .svn | grep -v '.deps' | grep -v '.libs' | grep -v 'testOSDL'| grep -v '.exe-logs' | grep -v '^\.$'`

# For debug purpose :
#TESTED_ROOT_MODULES="generic logs interfaces modules system maths network middleware"

DEBUG_INTERNAL "Tested modules are : ${TESTED_ROOT_MODULES}"


# "[OK] " is 5 character wide and must be aligned along the right edge :
if [ -z "${COLUMNS}" ] ; then
	DEBUG_INTERNAL "Retrieving columns thanks to tput"	
	COLUMNS=`tput cols`
	if [ -z "${COLUMNS}" ] ; then
		DEBUG_INTERNAL "Retrieving columns thanks to stty"	
		COLUMNS=`stty size |awk '{print $2}'`
	fi	
fi
DEBUG_INTERNAL "Columns = ${COLUMNS}"	

space_for_test_name=`expr ${COLUMNS} - 5`
DEBUG_INTERNAL "Space for test names = ${space_for_test_name}"	


if [ $is_batch -eq 0 ] ; then
	echo "
		Test results established at "`date '+%A, %B %-e, %Y'`"\n\n" > ${TESTLOGFILE}
fi

if [ "$on_cygwin" -eq "0" ] ; then
	echo "
	
	Library search path is : PATH='$PATH'" >> ${TESTLOGFILE}
else
	echo "
	
	Library search path is : LD_LIBRARY_PATH='$LD_LIBRARY_PATH'" >> ${TESTLOGFILE}
fi

# So that test plugin can be found :
saved_LTDL_LIBRARY_PATH="$LTDL_LIBRARY_PATH"


for m in ${TESTED_ROOT_MODULES} ; do

	DEBUG_INTERNAL "Testing module ${m}"
	
	LTDL_LIBRARY_PATH="$saved_LTDL_LIBRARY_PATH:${TEST_ROOT}/$m"
	export LTDL_LIBRARY_PATH
	
	# Some local scripts are needed in some cases, for example when a test
	# client requires a test server to be launched before.
	
	PLAYTEST_LOCAL="${TEST_ROOT}/$m/${PLAYTEST_LOCAL_FILE}"
	
	printColor "
	${term_offset}${term_primary_marker}Playing all tests of module '"`echo $m | sed 's|^./||1'`"' : " $magenta_text $black_back
	
	if [ -f "${PLAYTEST_LOCAL}" ] ; then
		EXCLUDED_TESTS=""
		DEBUG_INTERNAL "Sourcing ${PLAYTEST_LOCAL}"
		. ${PLAYTEST_LOCAL}
	fi
		
	if [ "$on_cygwin" -eq "0" ] ; then
		TESTS=`ls ${TEST_ROOT}/$m/*-testOSDL*.exe 2>/dev/null`
	else	
		TESTS=`ls ${TEST_ROOT}/$m/testOSDL*.exe 2>/dev/null`	
	fi
	
	DEBUG_INTERNAL "Tests in module ${m} are : '${TESTS}'"
	
	if [ $is_batch -eq 1 ] ; then
	
		# Lists all tests that are to be run :
		for t in $TESTS ; do
			if [ -x "$t" -a -f "$t" ] ; then
				printColor "${term_offset}${term_offset}+ `basename $t`" $cyan_text $black_back
			fi
		done

		echo "
		<Press enter to start testing module '"`echo $m | sed 's|./||'`"'>"
		read $dummy
		clear
	fi

	for t in $TESTS ; do
	
		if [ -x "$t" -a -f "$t" ] ; then

			get_logical_test_name $t
			logical_test_name=$returned_string
			
			to_be_excluded=1
			for excluded in ${EXCLUDED_TESTS} ; do
				if [ "$logical_test_name" = "$excluded" ] ; then
					to_be_excluded=0
				fi	
			done	
		
			if [ "$to_be_excluded" = "0" ] ; then
				# Skip this test, as PLAYTEST_LOCAL_FILE took care of it :
				DEBUG_INTERNAL "Skipping test $t"
				continue
			fi
			
			test_count=`expr $test_count + 1`
			
			get_logical_test_name $t
			test_name="$returned_string"
			
			display_launching $test_name
			
			run_test $test_name "$t"
			
		fi
			
	done
		
done 

display_final_stats


echo "
End of tests"

LTDL_LIBRARY_PATH="$saved_LTDL_LIBRARY_PATH"

exit $error_count

