#!/bin/bash


USAGE="Usage: "`basename $0`": cleans the install by removing all files generated for the build system. Useful while debugging the autotools."

FIND=`which find 2>/dev/null`
#echo "FIND = $FIND"

RM="/bin/rm"

# Must act from very first root directory.
# Ex: from OSDL/trunk, not OSDL/trunk/src
BASE_DIR=`dirname $0`/../../..
#echo "BASE_DIR = $BASE_DIR"

cd $BASE_DIR
#pwd

 
 
# Only the Makefile (if any) knows which executables should be removed:
cd test
#echo "cleaning test"
make clean 1>/dev/null 2>&1
cd ..


cd tools
#echo "cleaning tools"
make clean 1>/dev/null 2>&1
cd ..


# Directories:
$FIND . \( -name 'autom4te.cache' -o -name '.deps' -o -name 'libltdl' -o -name '.libs' -o -name 'test*.exe-logs' \) -exec $RM -rf '{}' ';' 2>/dev/null

# Files:
$FIND . \( -name ltmain.sh -o -name aclocal.m4 -o -name install-sh -o -name missing -o -name depcomp -o -name stamp-h1 -o -name configure.ac -o -name configure-from-autoscan.ac -o -name configure-from-autoupdate.ac -o -name configure -o -name 'auto*.log' -o -name 'auto*.err' -o -name config.guess -o -name config.log -o -name config.status -o -name config.sub -o -name libtool -o -name Makefile -o -name Makefile.in -o -name OSDLTemporaryConfig.h -o -name OSDLConfig.h -o -name OSDLHeaderVersion.cc -o -name '*.so' -o -name '*.a' -o -name 'svn-commit*.tmp' -o -name '*.o' -o -name '*.loT' -o -name '*.lo' -o -name 'test*.pal' -o -name 'test*.png' -o -name 'test*.bmp' -o -name 'test*.rgb' -o -name 'test*.bmp' -o -name '*.la' -o -name core \) -exec $RM -f '{}' ';' 2>/dev/null

$RM -f TODO MAINTENERS AUTHORS INSTALL FAQ ChangeLog THANKS NEWS COPYING.LIB README LOANI-README src/conf/build/loani/LOANI-README-template.txt src/conf/build/loani/LOANI-README osdl-*.tar.gz osdl-*.tar.bz2 test/testsOutcome.txt
																	
$RM -f src/doc/basic/OSDL-*-template.txt src/conf/doc/doxygen.conf src/conf/build/osdl-*.pc src/conf/build/m4/osdl.m4 src/code/OSDLConfig.h.in

		
						 
echo "    Cleaning done."

