#!/bin/sh

REPORT_FILE=`date '+%Y%m%d'`-`hostname`-OSDL-bug-report.txt
BUG_ML_ADDRESS="osdl-bugs@lists.sourceforge.net"
MESSAGE_PREFIX="###########################"
OFFSET="  + "

add()
{
	echo "${OFFSET}$*" >> ${REPORT_FILE}
}

addDir()
{
	if [ -d "$1" ] ; then
		add "$1 found, content is "
		add "<---"
		ls $1 2>&1 1>>${REPORT_FILE}
		add "end of directory listing for $1 --->"
	else
		add "No $1 found."
	fi
}


echo 
echo "        Welcome to LOANI's bug report generator."

echo
echo "This script has collected for you informations about your host configuration and your OSDL installation, in order to ease the troubleshooting. The result of the inquiry has been stored in file <${REPORT_FILE}>, please feel free to peer into it before sending it."

# Blanks too any previous report file :
echo "${MESSAGE_PREFIX} Beginning of bug report" > ${REPORT_FILE}
echo >> ${REPORT_FILE}

add "Bug report generated for ${USER}@`hostname`, on `date '+%A %d %B, %Y at %H:%M:%S'`, from `pwd`" >> ${REPORT_FILE}

  
add "Host platform : `uname -a`"
add "Available disk size : " `df -k .`
 
addDir LOANI-repository
addDir LOANI-installations

ENV_FILE="LOANI-installations/OSDL-environment.sh"

if [ -f "$ENV_FILE" ] ; then
	add "Sourcing found $ENV_FILE."
	. $ENV_FILE
else
	add "No $ENV_FILE found, not sourced."
fi

add "gcc : `gcc -v 2>&1`"
add "ld : `ld -v 2>&1`"
add "PATH : $PATH"
add "LD_LIBRARY_PATH : $LD_LIBRARY_PATH "

if [ -f "LOANI.log" ] ; then
	add "Appending found LOANI.log :"
	cat LOANI.log >> ${REPORT_FILE}
else
	add "No LOANI.log found, not appending anything."
fi

echo >> ${REPORT_FILE} 
echo >> ${REPORT_FILE} 
echo "${MESSAGE_PREFIX} End of bug report" >> ${REPORT_FILE} 

echo
echo "You should send this report (${REPORT_FILE}) as attachment to our mail account dedicated to troubleshooting, ${BUG_ML_ADDRESS} (no registering needed), we will do our best to help you !" 


