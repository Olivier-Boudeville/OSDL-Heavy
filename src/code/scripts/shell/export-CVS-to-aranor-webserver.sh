#!/bin/bash

# Created by Olivier Boudeville (olivier.boudeville@online.fr)
# 2004, January 14.

OSDL_VERSION=0.3
CEYLAN_VERSION=0.2


# This script will export Ceylan and OSDL's website stored in CVS into the OSDL's webserver.
# This script is intended to be executed by crontab (see CVS-to-webserver.crontab).

CVS_SERVER=cvs.sourceforge.net
CVS_ROOT=/cvsroot/osdl
WEB_ROOT=/var/www

# The file in which timestamps and possible errors should be reported :
LOG_FILE=~/last-exported-CVS.txt

# Touched to know the last time things went right :
SUCCESS_FILE=~/last-success

# Touched to know the last time things went wrong :
FAILURE_FILE=~/last-failure


cd $WEB_ROOT

# First, check we have a chance of having a working export before erasing everything,
# thanks to an export attempt for one file that is always available.

# Remove test file to avoid CVS message "file is in the way"
CVS_TESTER="OSDL/OSDL-${OSDL_VERSION}/src/doc/web/CVS-tester.txt"

rm -f $CVS_TESTER
rm -f $LOG_FILE

date '+%d/%m/%Y - %H:%M:%S' > $LOG_FILE


# Try to have it back, if CVS is working
cvs -d:pserver:anonymous@$CVS_SERVER:$CVS_ROOT export -Dtomorrow $CVS_TESTER 1>>$LOG_FILE 2>&1

if [ "$?" -eq "0" ] ; then 

	# Successs : go on with the exports	
	
	# Sadly enough, we've got to erase previously exported files, 
	# otherwise CVS would complain that these already-existing files are "in the way"
	# (nevertheless api documentation should be preserved) :
	rm -rf Ceylan/Ceylan-${CEYLAN_VERSION}/src OSDL/OSDL-${OSDL_VERSION}/src index.html

	# First, export OSDL :
	cvs -d:pserver:anonymous@$CVS_SERVER:$CVS_ROOT export -Dtomorrow OSDL/OSDL-${OSDL_VERSION}/src/doc/web 1>>$LOG_FILE 2>&1

	# Then, do the same thing for Ceylan :
	cvs -d:pserver:anonymous@$CVS_SERVER:$CVS_ROOT export -Dtomorrow  Ceylan/Ceylan-${CEYLAN_VERSION}/src/doc/web 1>>$LOG_FILE 2>&1

	# Updates the timestamp at the bottom of OSDL's portal main page :
	mv OSDL/OSDL-${OSDL_VERSION}/src/doc/web/main/MainOSDL.html OSDL/OSDL-${OSDL_VERSION}/src/doc/web/main/MainOSDL-temp.html
	cat OSDL/OSDL-${OSDL_VERSION}/src/doc/web/main/MainOSDL-temp.html | sed "s|PG_DATE|`date '+%Y, %A %B %e, at %T (%Z)'`|1" > OSDL/OSDL-${OSDL_VERSION}/src/doc/web/main/MainOSDL.html
	
	# Finally, put the right index.html at the website root :
	cp -f OSDL/OSDL-${OSDL_VERSION}/src/doc/web/index-SF.html index.html

	touch $SUCCESS_FILE
	
else

 	# Failure : do not erase anything, nothing would replace it.
	touch $FAILURE_FILE
	echo `date '+%d/%m/%Y - %H:%M:%S'` "Failure to access CVS server, aborting" 1>&2
	
fi


