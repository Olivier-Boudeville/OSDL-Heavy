#!/bin/sh

USAGE="Usage :"`basename $0`"[diff tool] : compares current configure.ac with the ones suggested first by autoscan, then by autoupdate."

# Debug mode activated iff equal to true (0) :
debug_mode=1

DEBUG()
{
	if [ $debug_mode -eq 0 ] ; then
		echo "Debug : $*"
	fi	
}


RM="/bin/rm -f"

COMMAND=$0
DEBUG "COMMAND = $COMMAND"

LAUNCHDIR=`pwd`

# Always start from 'test' directory :
cd `dirname $COMMAND`


RUNNINGDIR=`pwd`
DEBUG "RUNNINGDIR = $RUNNINGDIR" 


DIFF_TOOL=`which tkdiff`
if [ -n "$1" ] ; then
	DIFF_TOOL="$1"
fi

if [ ! -x "$DIFF_TOOL" ] ; then
	echo "Error, no diff tool found." 1>&2
	exit 1
fi
DEBUG "DIFF_TOOL = $DIFF_TOOL"


# Autoscan section.

AUTOSCAN=`which autoscan`
DEBUG "AUTOSCAN = $AUTOSCAN"

AUTOSCAN_OPT=""
#AUTOSCAN_OPT="-v"

AUTOSCAN_LOG="$RUNNINGDIR/autoscan.log"
if [ -e "$AUTOSCAN_LOG" ]; then
	${RM} "$AUTOSCAN_LOG"
fi

AUTOSCAN_ERR="$RUNNINGDIR/autoscan.err"
if [ -e "$AUTOSCAN_ERR" ]; then
	${RM} -f "$AUTOSCAN_ERR"
fi


# Autoupdate section.


AUTOUPDATE=`which autoupdate`
DEBUG "AUTOUPDATE = $AUTOUPDATE"

AUTOUPDATE_OPT=""
#AUTOUPDATE_OPT="--verbose"

AUTOUPDATE_LOG="$RUNNINGDIR/autoupdate.log"
if [ -e "$AUTOUPDATE_LOG" ]; then
	${RM} "$AUTOUPDATE_LOG"
fi

AUTOUPDATE_ERR="$RUNNINGDIR/autoupdate.err"
if [ -e "$AUTOUPDATE_ERR" ]; then
	${RM} -f "$AUTOUPDATE_ERR"
fi


CURRENT_CONFIGURE_FILE=configure.ac


######## autoscan ########
DEBUG "Being in "`pwd`" directory"

echo
echo "  + comparing current configure.ac with the one autoscan would suggest :"

DEBUG "Launching : $AUTOSCAN $AUTOSCAN_OPT"
$AUTOSCAN $AUTOSCAN_OPT > $AUTOSCAN_LOG 2> $AUTOSCAN_ERR
RES=$?

CONF_SCANNED=configure-from-autoscan.ac
if [ $RES -eq 0 ] ; then
	#echo "Autoscan succeeded"
	mv -f configure.scan $RUNNINGDIR/$CONF_SCANNED
	cd $RUNNINGDIR
	# Left file is current configure.ac, right one is the suggested one :
	if diff $CURRENT_CONFIGURE_FILE $CONF_SCANNED 1>/dev/null 2>&1; then
		echo "      (current configure.ac and the one suggested by autoscan are the same)"
	else
		$DIFF_TOOL $CURRENT_CONFIGURE_FILE $CONF_SCANNED
	fi	
else
	echo "Error, Autoscan exited on failure : " 1>&2
	more $AUTOSCAN_ERR
	echo
	exit 10
fi



######## autoupdate ########
DEBUG "Being in "`pwd`" directory"

echo
echo
echo "  + comparing current configure.ac with the one autoupdate would suggest :"

DEBUG "Launching : $AUTOUPDATE $AUTOUPDATE_OPT"
CONF_UPDATED=$RUNNINGDIR/configure-from-autoupdate.ac

CONF_BACKUP=configure-original.ac
cp -f $CURRENT_CONFIGURE_FILE $CONF_BACKUP

$AUTOUPDATE $AUTOUPDATE_OPT > $AUTOUPDATE_LOG 2> $AUTOUPDATE_ERR
RES=$?

mv $CURRENT_CONFIGURE_FILE $CONF_UPDATED
mv $CONF_BACKUP $CURRENT_CONFIGURE_FILE

cd $RUNNINGDIR

if [ $RES -eq 0 ] ; then
	# Autoupdate succeeded :
	# Left file is current configure.ac, right one is the suggested one :
	if diff $CURRENT_CONFIGURE_FILE $CONF_UPDATED 1>/dev/null 2>&1; then
		echo "      (current configure.ac and the one suggested by autoupdate are the same)"
	else
		$DIFF_TOOL configure.ac $CONF_UPDATED
	fi
else
	echo "Error, Autoupdate exited on failure : " 1>&2
	more $AUTOUPDATE_ERR
	echo
	exit 11
fi


${RM} auto*.log
${RM} auto*.err

${RM} configure.scan
${RM} autoscan*.log  
${RM} -r autom4te.cache

echo 
echo "End of comparison"

