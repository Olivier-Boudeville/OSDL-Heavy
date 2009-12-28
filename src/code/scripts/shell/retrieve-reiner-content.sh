#!/bin/sh

TARGET_URL="http://reinerstileset.4players.de"

USAGE=`basename $0`': retrieves the splendid animated tilesets coming from Reiner "Tiles" Prokein (${TARGET_URL})'

DF=df
AWK=awk
TAIL=tail

# Available size in megabytes (1048576 is 1024^2):
AVAILABLE_SIZE=`${DF} -m . | ${AWK} '{print $4}' | ${TAIL} -n 1`

ESTIMATED_SIZE=1024

if [ $AVAILABLE_SIZE -lt $ESTIMATED_SIZE ] ; then

	echo  "Warning: according to the estimated size of download ($ESTIMATED_SIZE megabytes), as you have only $AVAILABLE_SIZE megabytes in the current partition, the mirror should not fit on your directory. Consider interrupting this script (CTRL-C) if you believe it will not suffice." 1>&2
	
fi 

echo "Mirror should fit in directory:"
echo "  - estimated size of mirror: ${ESTIMATED_SIZE} megabytes."
echo "  - available size of  ${AVAILABLE_SIZE} megabytes."


echo "Launching retrieval from ${TARGET_URL}"
date

WGET=wget

# M_*.zip are MMF files, not wanted:
WGET_OPT="--limit-rate=10k --wait=5 --random-wait --retry-connrefused --recursive --level=10 --reject exe,"M_*.zip" --exclude-directories=forum,buttons,meshes,screenshots"

${WGET} ${WGET_OPT} ${TARGET_URL}
 
date

echo "End of retrieval."

