#!/bin/sh


USAGE="Usage: "`basename $0`": cleans the OSDL test repository by removing all files generated for the build system. Useful while debugging the autotools."

FIND=`which find 2>/dev/null`
#echo "FIND = $FIND"

RM="/bin/rm"

# First clean tests:								 
./cleanTestGeneratedFiles.sh

# Then perform autotools-specific cleaning:

# Directories:
$FIND . \( -name 'autom4te.cache' -o -name '.deps' -o -name '.libs' -o -name 'testOSDL*-logs' \) -exec $RM -rf '{}' ';' 2>/dev/null

# Files:
$FIND . \( -name ltmain.sh -o -name aclocal.m4 -o -name install-sh -o -name missing -o -name depcomp -o -name stamp-h1 -o -name configure -o -name 'auto*.log' -o -name 'auto*.err' -o -name config.guess -o -name configure.ac -o -name config.log -o -name config.status -o -name config.sub -o -name libtool -o -name Makefile -o -name Makefile.in  -o -name '*.o' -o -name '*.loT' -o -name '*.lo' -o -name 'testOSDL*.log' -o -name 'testOSDL*.xml' -o -name testsOutcome.txt \) -exec $RM -f '{}' ';' 2>/dev/null


echo "    Cleaning done."

