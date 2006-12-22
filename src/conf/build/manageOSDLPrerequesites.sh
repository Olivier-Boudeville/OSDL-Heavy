#!/bin/sh

USAGE="Usage : `basename $0` [ -s ] < hide | unhide > \nWill hide or unhide OSDL prerequesites, in order LOANI to be safely tested. No other softwares should be needed than those installed by LOANI.\n\tThe -s option allows to simulate only (no actual hide or unhide performed)."

if [ `id -u` != "0" ] ; then
	echo "This script must be run as root."
	exit 1
fi


if [ -z "$1" ] ; then
	echo"No argument given."
	echo
	echo "$USAGE"
	exit 2
fi

do_hide=1
do_unhide=1
do_simulate=1

ACTION_MESSAGE="moving"

if [ "$1" = "-s" ] ; then
	echo "Warning : simulating only."
	ACTION_MESSAGE="would move"
	do_simulate=0
	shift
fi 


DISABLED_PREFIX="disabled-"


if [ "$1" = "hide" ] ; then
	do_hide=0
else
	if [ "$1" = "unhide" ] ; then
		do_unhide=0
	else
		echo -e "Specified argument ($1) is neither 'hide' nor 'unhide'. $USAGE"
		exit 3
	fi
fi

echo
echo "Taking care of OSDL pre requesites on `hostname`."

echo 


hide()
# hides the files correspondong to the specified wildcard.
{
	#echo "In current directory "`pwd`" doing : ls $1*"
	#ls $1*
	#if [ -n "$*" ] ; then
	if ls $1* 1>/dev/null 2>&1 ; then	
		echo "--> hiding $1"	
		for f in $1* ; do
			source_file=$f
			target_file=${DISABLED_PREFIX}$f
			echo "    + ${ACTION_MESSAGE} $source_file to $target_file"
			[ "$do_simulate" = "0" ] || mv -f $source_file $target_file
		done
	else
		echo "    (nothing to do for $1)" 
	fi
}


unhide()
# unhides the files correspondong to the specified wildcard.
{
	
	if ls ${DISABLED_PREFIX}$1* 1>/dev/null 2>&1 ; then	
		echo "--> unhiding $1"
		for f in ${DISABLED_PREFIX}$1* ; do
			source_file=$f
			target_file=`echo $f | sed "s|^${DISABLED_PREFIX}||"`
			echo "    + ${ACTION_MESSAGE} $source_file to $target_file"
			[ "$do_simulate" -eq "0" ] || mv -f $source_file $target_file
		done
	else
		echo "    (nothing to do for $1)" 
	fi

}


iterateOnTargets()
{
	if [ "$do_hide" -eq 0 ] ; then
		ACTION=hide
	else
		ACTION=unhide		
	fi	
	
	${ACTION} libSDL       
	${ACTION} libjpeg      
	${ACTION} libpng     
	# Disabled since ssh (used by CVS) needs it :   
	#${ACTION} libz         
	${ACTION} libtiff      
	${ACTION} libSDL_image 
	${ACTION} libtool      
	
}


performWorkOnDirectory()
{
	target_dir=$1
	if [ ! -d "$target_dir" ] ; then
		echo "Nothing to do for non-existing directory $target_dir."
	else
		echo
		echo "###### Moving to $target_dir"
		cd $target_dir
		iterateOnTargets
	fi
	echo "###### Done for directory $target_dir."
}

dir_list=`echo $LD_LIBRARY_PATH:/lib:/usr/lib:/usr/local/lib:$PATH:/bin:/usr/bin:/usr/local/bin | sed 's|:| |g'`
#dir_list=/usr/local/lib
echo "Operating on directories : $dir_list"

for d in ${dir_list}; do
	performWorkOnDirectory $d
done

