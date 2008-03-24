#!/bin/sh

# (bash needed for arithmetics and al)

USAGE=`basename $0`" <tileset archive> <identifier directory>: processes a bitmap archive (ex: 'T_swordstan_shield.zip') from the splendid animated tilesets coming from Reiner 'Tiles' Prokein (http://reinerstileset.4players.de) to convert them for OSDL use. The identifier directory (ex: "


do_preserve_content=1
do_uncompress=0
do_correct_name=0
do_transform_to_png=0
do_sort_by_name=0

do_convert_to_oam=1

# Conversion to OSDL Animation Format:
do_convert_to_oam=1
do_zip_result=1


RM=/bin/rm
MV=/bin/mv
MKDIR="/bin/mkdir -p"

ANIMATED_OBJECT=`basename $PWD`
#echo "ANIMATED_OBJECT = ${ANIMATED_OBJECT}"

# The zoom out factor, in percent, to reduce original size to in-game targeted
# one (subsequent per-animated object ration may be applied too):
DOWNSCALE_RATIO="50"



rename_resource_file()
# Returns for specified file ($1) (ex: 'tipping-over-se0008.bmp') a canonical
# name (ex: '1-4-7-6-8.png', for
# 'Stan-ArmedWithAnAxeAndAShield-TipOver-SouthEast-FrameNumber').
# Uses $2 as local tileset directory. 
{

	SOURCE_FILE=$1
	echo "SOURCE_FILE = ${SOURCE_FILE}"
	
	LOCAL_TILESET_DIR=$2
	
	# Remove leadind './' and extension:
	PREFIX=`echo ${SOURCE_FILE}|sed 's|^\./||1'|sed 's|.bmp$||1'`

	
	# 1. guessing the frame:

	FRAME=`echo ${PREFIX}|tail -c 5` 
	FRAME_ID=`expr ${FRAME} + 0`
	echo "FRAME=${FRAME}, FRAME_ID=${FRAME_ID}"
	
	REMAINDER=`echo ${PREFIX}|sed "s|${FRAME}\$||1"`
	#echo "REMAINDER = ${REMAINDER}"


	# 2. guessing the direction:

	# Note: awk -F- '{print $2}' would not work as there might be '-' in
	# original attitude (ex: tipping-over).
	DIRECTION=`echo ${REMAINDER}|sed 's|^.*-||1'`
	
	# Order is the one of identifiers (in directions.id): 
	case "${DIRECTION}" in
	
		  "e"  ) DIRECTION_ID=${East};;
		  "n"  ) DIRECTION_ID=${North};;
		  "ne" ) DIRECTION_ID=${NorthEast};;
		  "nw" ) DIRECTION_ID=${NorthWest};;
		  "s"  ) DIRECTION_ID=${South};;
		  "se" ) DIRECTION_ID=${SouthEast};;
		  "sw" ) DIRECTION_ID=${SouthWest};;
		  "w"  ) DIRECTION_ID=${West};;
		  *    ) 
		  	echo "Error, unexpected direction (${DIRECTION})"
			exit 10;;
			
	esac      
	
	echo "DIRECTION=${DIRECTION}, DIRECTION_ID=${DIRECTION_ID}"


	# 3. guessing the attitude:

	ATTITUDE=`echo ${REMAINDER}|sed "s|-${DIRECTION}\$||1"`
	
	
	# Order is the one of identifiers (in attitudes.id): 
	case "${ATTITUDE}" in
	
		  "attack" ) 
		  	ATTITUDE_ID=${BasicAttack};;

		  "stopped" | "steht" ) 
		  	ATTITUDE_ID=${BeStill};;
		  
		  "been-hit" | "treffer" ) 
		  	ATTITUDE_ID=${BeenHit};;

		  "bückt-sich" )
		  	ATTITUDE_ID=${Bend};;

		  "labert" ) 
		  	ATTITUDE_ID=${Enjoy};;

		  "hand-attack" )
		  	ATTITUDE_ID=${HandAttack};;

		  "hikick" )
		  	ATTITUDE_ID=${HiKick};;

		  "knit" | "strickt" ) 
		  	ATTITUDE_ID=${Knit};;

		  "looking" ) 
		  	ATTITUDE_ID=${Look};;
		  		  
		  "lowkick" ) 
		  	ATTITUDE_ID=${LowKick};;
		  		 
		  "magic-attack" ) 
		  	ATTITUDE_ID=${MagicAttack};;

		  "pickup" ) 
		  	ATTITUDE_ID=${PickFIXME};;

		  "ringing" ) 
		  	ATTITUDE_ID=${Ring};;

		  "running" | "rennt" ) 
		  	ATTITUDE_ID=${Run};;
		  
		  "talking" | "spricht" ) 
		  	ATTITUDE_ID=${Talk};;
			
		  "greeting" ) 
		  	ATTITUDE_ID=${Salute};;
		  
		  "tipping-over" | "kippt-um" ) 
		  	ATTITUDE_ID=${TipOver};;
		  
		  "paused" ) 
		  	ATTITUDE_ID=${Wait};;

		  "walking" | "läuft" ) 
		  	ATTITUDE_ID=${Walk};;
		 		  	
		  * ) 
		  	echo "Error, unexpected attitude (${ATTITUDE})"
			exit 11;;
			
	esac      

	echo "ATTITUDE = ${ATTITUDE}, ATTITUDE_ID = ${ATTITUDE_ID}"


	# 4. guessing the outside look:
	# (try to respect the outside-looks.id order)
	LOOK=${LOCAL_TILESET_DIR}
	case "${LOOK}" in
	
		  "anna"                 ) LOOK_ID=${Unarmed};;
		  "arno"                 ) LOOK_ID=${Unarmed};;
		  "billy"                ) LOOK_ID=${Unarmed};;

		  "bjorn_wearingweapons" ) LOOK_ID=${WearingWeapons};;
		  "swordbjorn_shield"    ) LOOK_ID=${ArmedWithASwordAndAShield};;

		  "burra"                ) LOOK_ID=${ArmedWithASword};;
		  "elsa"                 ) LOOK_ID=${Unarmed};;

		  "freya_axe"            ) LOOK_ID=${ArmedWithAnAxe};;
		  "freya_wearingweapons" ) LOOK_ID=${WearingWeapons};;
		  "freya_noarms"         ) LOOK_ID=${Unarmed};;

		  "horst"                ) LOOK_ID=${ArmedWithACanneFIXME};;
		  "john_doe_santa"       ) LOOK_ID=${ArmedWithABell};;
		  "katie"                ) LOOK_ID=${Unarmed};;
		  "lilly"                ) LOOK_ID=${Unarmed};;

		  "bowstan"              ) LOOK_ID=${ArmedWithABow};;
		  "clubstan"             ) LOOK_ID=${ArmedWithAClub};;
		  "axestan"              ) LOOK_ID=${ArmedWithAnAxe};;
		  "axestan_shield"       ) LOOK_ID=${ArmedWithAnAxeAndAShield};;
		  "staffstan"            ) LOOK_ID=${ArmedWithAStaff};;
		  "swordstan"            ) LOOK_ID=${ArmedWithASword};;
		  "swordstan_shield"     ) LOOK_ID=${ArmedWithASwordAndAShield};;
		  "stanunarmed"          ) LOOK_ID=${Unarmed};;
		  *                      ) 
		  	echo "Error, unexpected outside look (${LOOK})"
			exit 12;;
			
	esac      

	
canonical_name_result="${ANIMATED_OBJECT_ID}-${LOOK_ID}-${ATTITUDE_ID}-${DIRECTION_ID}-${FRAME_ID}.png"

}


test_rename()
{

	TEST=$1
	rename_resource_file ${TEST} a_tileset_dir
	echo "Renaming of ${TEST} results in ${canonical_name_result}"
	exit

}

#test_rename "./rennt-e0000.bmp"



rename_if_exist()
# Renames $1 to $2 if file $1 exists.
{
	
	[ -f "$1" ] && ${MV} $1 $2

}


correct_names()
# Correct names in specified directory.
{

	LOCAL_TILESET_DIR=$1

	CURRENT_DIR=`pwd`
	
	echo " + correcting entry names in ${LOCAL_TILESET_DIR}"

	cd ${LOCAL_TILESET_DIR}
	
	# General corrections:
	# (for example, 'battlefield 96x bitmaps')
	find . -type d -exec ${CORRECT_SCRIPT} '{}' ';' 2>/dev/null
	find . -type f -exec ${CORRECT_SCRIPT} '{}' ';'
	
	cd ${CURRENT_DIR}

}


transform_to_png_in()
# Transforms all BMP files to PNG in specified directory.
{
	
	LOCAL_TILESET_DIR=$1

	CURRENT_DIR=`pwd`
	
	echo " + transforming BMP to PNG files in ${LOCAL_TILESET_DIR}"

	cd ${LOCAL_TILESET_DIR}

	# First, fix incorrect initial naming (special cases):

	# Sometimes the name of the animated object is prefixed, let's remove it:
	# First prefix ex: 'axestan-kippt-um-n0004.bmp' -> 'kippt-um-n0004.bmp')
	for f in `/bin/ls ${LOCAL_TILESET_DIR}-* 2>/dev/null`; do
		echo "Renaming $f to "`echo $f|sed "s|^${LOCAL_TILESET_DIR}-||1"`
		${MV} -f $f `echo $f|sed "s|^${LOCAL_TILESET_DIR}-||1"`		
	done

	# (ex: 'stan-strickt-s0001.bmp' -> 'strickt-s0001.bmp')
	for f in `/bin/ls stan-* 2>/dev/null`; do
		echo "Renaming $f to "`echo $f|sed "s|^stan-||1"`
		${MV} -f $f `echo $f|sed "s|^stan-||1"`		
	done
	
	# Premature translation for special fixes:
	# Ex: steht0001.bmp -> stopped0001.bmp
	for f in `/bin/ls steht* 2>/dev/null` ; do
		TARGET_FILE=`echo $f|sed 's|^steht|stopped|1'`
		echo "Renaming $f to ${TARGET_FILE}"
		${MV} -f $f ${TARGET_FILE}
	done
	
	# Some special cases to handle:
	case "${LOCAL_TILESET_DIR}" in
	
		"bowstan" )
			# Arrows are stored in original bowstan archive, whereas for 
			# us they are a separate object, created here:
			ARROW_DIR="../../../../../Objects/Weapons/Arrows/SimpleArrow"
			echo "Moving arrows to ${ARROW_DIR} from "`pwd`
			${MKDIR} ${ARROW_DIR};;
		
		"stanunarmed" )
			for f in `/bin/ls noarmstan-* 2>/dev/null` ; do
				TARGET_FILE=`echo $f|sed 's|^noarmstan-||1'`
				echo "Renaming $f to ${TARGET_FILE}"
				${MV} -f $f ${TARGET_FILE}
			done
	
			for f in `/bin/ls naormstan-* 2>/dev/null` ; do
				TARGET_FILE=`echo $f|sed 's|^naormstan-||1'`
				echo "Renaming $f to ${TARGET_FILE}"
				${MV} -f $f ${TARGET_FILE}
			done ;;					
				
	esac 
	
	# Here, 'stopped' attitude lacks direction:
	rename_if_exist stopped0000.bmp stopped-s0000.bmp
	rename_if_exist stopped0001.bmp stopped-sw0001.bmp
	rename_if_exist stopped0002.bmp stopped-w0002.bmp
	rename_if_exist stopped0003.bmp stopped-nw0003.bmp
	rename_if_exist stopped0004.bmp stopped-n0004.bmp
	rename_if_exist stopped0005.bmp stopped-ne0005.bmp
	rename_if_exist stopped0006.bmp stopped-e0006.bmp
	rename_if_exist stopped0007.bmp stopped-se0007.bmp
	
	# Scaled objects:
	case "${LOCAL_TILESET_DIR}" in

		"anna"  			    ) THIS_OBJECT_RATIO=100 ;;
		"arno"  			    ) THIS_OBJECT_RATIO=85  ;;
		"billy"  			    ) THIS_OBJECT_RATIO=100 ;;
		 
		"bjorn_wearingweapons"  ) THIS_OBJECT_RATIO=80  ;; 
		"swordbjorn_shield"     ) THIS_OBJECT_RATIO=80  ;;
		
		"burra"                 ) THIS_OBJECT_RATIO=95  ;;
		"elsa"                  ) THIS_OBJECT_RATIO=90  ;;

		"freya_axe"             ) THIS_OBJECT_RATIO=80  ;;
		"freya_wearingweapons"  ) THIS_OBJECT_RATIO=80  ;;
		"freya_noarms"          ) THIS_OBJECT_RATIO=80  ;;

		"horst"                 ) THIS_OBJECT_RATIO=75  ;;
		"john_doe_santa"        ) THIS_OBJECT_RATIO=70  ;;
		"katie"                 ) THIS_OBJECT_RATIO=80  ;;
		"lilly"                 ) THIS_OBJECT_RATIO=70  ;;

		"bowstan"               ) THIS_OBJECT_RATIO=90  ;;
		"clubstan"              ) THIS_OBJECT_RATIO=90  ;;
		"axestan"               ) THIS_OBJECT_RATIO=90  ;;
		"axestan_shield"        ) THIS_OBJECT_RATIO=90  ;;
		"staffstan"             ) THIS_OBJECT_RATIO=90  ;;
		"swordstan"             ) THIS_OBJECT_RATIO=90  ;;
		"swordstan_shield"      ) THIS_OBJECT_RATIO=90  ;;
		"stanunarmed"           ) THIS_OBJECT_RATIO=90  ;;

		
		*                       ) 
		  	echo "Error, no scale factor specified for object ${LOCAL_TILESET_DIR})"
			exit 13;;
				
	esac 
	
	ACTUAL_RATIO=$(( ${DOWNSCALE_RATIO} * ${THIS_OBJECT_RATIO} / 100 ))
	 	
	CONVERT_OPT="-strip -quality 100 -sharpen 1x.5 -filter Lanczos -resize ${ACTUAL_RATIO}%"
	
	#CONVERT_OPT="-antialias"
	
	for f in `find . -name '*.bmp'`; do
		
		rename_resource_file $f $LOCAL_TILESET_DIR
		echo
		echo "+ transforming $f and replacing it by $canonical_name_result"
		${CONVERT_TOOL} ${CONVERT_OPT} $f $canonical_name_result
		rm -f $f
		
	done
	
	cd ${CURRENT_DIR}

}




# Anticipated checkings:


TILESET_ARCHIVE="$1"

if [ -z "${TILESET_ARCHIVE}" ]; then
	echo "
	Error, no tileset archive specified. 
	Usage: $USAGE." 1>&2
	exit 1
fi


if [ ! -f "${TILESET_ARCHIVE}" ]; then
	echo "
	Error, specified tileset archive (${TILESET_ARCHIVE}) is not an existing file. 
	Usage: $USAGE." 1>&2
	exit 2
fi

# Canonifies directory names:
# (ex: 'T_axestan_shield.zip' -> 'axestan_shield')
TILESET_DIR=`echo "${TILESET_ARCHIVE}" | sed 's|^T_||1' | sed 's|.zip$||1'`
#echo "TILESET_DIR = ${TILESET_DIR}"


STARTING_DIR=`pwd`

# Will guess it this script is run from its location in OSDL build tree
# (trunk/src/code/scripts/shell):
GUESSED_CEYLAN_ROOT=`echo ${STARTING_DIR} | sed 's|LOANI-repository.*$|LOANI-repository/ceylan/Ceylan/trunk/src|1'`

#echo "GUESSED_CEYLAN_ROOT = ${GUESSED_CEYLAN_ROOT}"

if [ ! `basename ${GUESSED_CEYLAN_ROOT}` = "src" ]; then

	echo "Ceylan root could not be deduced from current directory, trying CEYLAN_SRC environment variable."
	
	if [ -z "${CEYLAN_SRC}" ]; then
		
		echo "
		Error, unable to determine Ceylan root from currect directory or from the CEYLAN_SRC environment variable (which is not set)."
		exit 18
		
	else
	
		if [ -d "${CEYLAN_SRC}" ]; then
			GUESSED_CEYLAN_ROOT=${CEYLAN_SRC}
		else
			echo "
		Error, unable to determine Ceylan root from currect directory or from the CEYLAN_SRC environment variable (which is set to a non-directory: ${CEYLAN_SRC})."
			exit 19
			
		fi
	fi

fi


if [ ! -d "${GUESSED_CEYLAN_ROOT}" ]; then
	echo "
	Error, Ceylan root (${GUESSED_CEYLAN_ROOT}) is not an existing directory. 
	Usage: $USAGE." 1>&2
	exit 20
fi


# Checking correcter script:
CORRECT_SCRIPT="${GUESSED_CEYLAN_ROOT}/code/scripts/shell/correctFilename.sh"
#echo "CORRECT_SCRIPT = ${CORRECT_SCRIPT}"

if [ ! -x "${CORRECT_SCRIPT}" ]; then
	echo "
	Error, unable to find prerequesite executable script (${CORRECT_SCRIPT})." 1>&2
	exit 21
fi


# Checking conversion tool:
CONVERT_TOOL=`which convert 2>/dev/null`
#echo "CONVERT_TOOL = ${CONVERT_TOOL}"

if [ ! -x ${CONVERT_TOOL} ]; then
	echo "
	Error, unable to find 'convert' tool from ImageMagick package.
	Install it for example with 'apt-get install imagemagick'.
	" 1>&2
	exit 21
fi


OSDL_TRUNK=`dirname $0`/../../../..

ASSET_REPOSITORY=${OSDL_TRUNK}/tools/media/video/asset-repository

if ! ls ${ASSET_REPOSITORY}/*.id 1>/dev/null 2>&1; then
	echo "
	Error, unable to find identifier lists (*.id) in asset repository (${ASSET_REPOSITORY})." 1>&2
	exit 22
fi

# Source these id files to be able to translate identifier names into values:
for f in ${ASSET_REPOSITORY}/*.id; do
	. $f
done	


eval ANIMATED_OBJECT_ID=\$$ANIMATED_OBJECT
echo "    Identifier for this animated object ${ANIMATED_OBJECT} is ${ANIMATED_OBJECT_ID}."



echo "Processing Reiner sprite tileset in ${TILESET_ARCHIVE}"

# If given .../OSDL-data/T_swordstan_shield.zip:

# ARCHIVE_NAME is T_swordstan_shield.zip:
ARCHIVE_NAME=`basename ${TILESET_ARCHIVE}`


if [ $do_preserve_content -eq 1 ]; then

	case "${TILESET_DIR}" in
	
		"bjorn" )
			if [ -d "bjorn_wearingweapons" ]; then
				${RM} -rf bjorn_wearingweapons 
			fi
		
			if [ -d "swordbjorn_shield" ]; then
				${RM} -rf swordbjorn_shield
			fi ;;
	
		"freya_axe" )
			if [ -d "freya_wearingweapons" ]; then
				${RM} -rf freya_wearingweapons 
			fi
		
			if [ -d "freya_axe" ]; then
				${RM} -rf freya_axe
			fi
	
			if [ -d "freya_noarms" ]; then
				${RM} -rf freya_noarms
			fi ;;
	
		
		* )
			if [ -d "${TILESET_DIR}" ]; then
				${RM} -rf ${TILESET_DIR}
			fi ;;
		
	esac

fi


# Perform the work:

# Move all BMP files to tileset dir (ex: axestan_shield).
if [ $do_uncompress -eq 0 ]; then

	echo " + unzipping ${TILESET_ARCHIVE}"

	${MKDIR} tmp-decompress
	cd tmp-decompress
	
	# -o: overwrite existing files without prompting
	unzip -o ../${TILESET_ARCHIVE}

	# Some archives contain multiple outside looks:
	case "${TILESET_DIR}" in
	
	
		"bjorn" )
			# In bjorn archive there are actually two outside looks:
			${MKDIR} ../bjorn_wearingweapons ../swordbjorn_shield
		
			find 'town 96x bitmaps' -type f -a -name '*.bmp' -exec ${MV} -f '{}' ../bjorn_wearingweapons ';'
		
			# The rest is in 'battlefield*':
			find . -type f -a -name '*.bmp' -exec ${MV} -f '{}' ../swordbjorn_shield ';'
			;;
			
			
		"freya_axe" )
			# In this archive there are actually two attitudes:
			${MKDIR} ../freya_wearingweapons ../freya_axe
		
			find 'freya axe bitmaps/town' -type f -a -name '*.bmp' -exec ${MV} -f '{}' ../freya_wearingweapons ';'
		
			# The rest is in 'battlefield':
			find . -type f -a -name '*.bmp' -exec ${MV} -f '{}' ../freya_axe ';'
			;;
				
				
		* )
			${MKDIR} ../${TILESET_DIR}
			find . -type f -a -name '*.bmp' -exec ${MV} -f '{}' ../${TILESET_DIR} ';'
	
	
	esac	
	
	cd ..
	
	${RM} -rf tmp-decompress
	
fi


if [ $do_correct_name -eq 0 ] ; then

	case "${TILESET_DIR}" in
	
		"bjorn" )
			correct_names bjorn_wearingweapons 
			correct_names swordbjorn_shield ;;
		
		"freya_axe" )
			correct_names freya_axe 
			correct_names freya_wearingweapons ;;
		
		* )	
			correct_names ${TILESET_DIR} ;;
			
	esac
	
fi


if [ $do_transform_to_png -eq 0 ]; then

	case "${TILESET_DIR}" in
	
		"bjorn" )
			transform_to_png_in bjorn_wearingweapons		
			transform_to_png_in swordbjorn_shield ;;
	
		"freya_axe" )
			transform_to_png_in freya_axe		
			transform_to_png_in freya_wearingweapons ;;
			
		* )
			transform_to_png_in ${TILESET_DIR}
	esac
		
fi



if [ $do_convert_to_oam -eq 0 ]; then

	echo " + converting source PNG to OSDL Animation Format"

fi


if [ $do_zip_result -eq 0 ]; then
	TARGET_ARCHIVE_NAME=`echo $ARCHIVE_NAME | sed 's|^T_|OAF_|1'`
	echo " + zipping ${TILESET_ARCHIVE} to ${TARGET_ARCHIVE_NAME}"
	
	cd ..
	NEW_TILESET_DIR=`echo ${TILESET_DIR} | sed 's|^T_|OAF_|1'`
	
	if [ -d "${NEW_TILESET_DIR}" ]; then
		rm -rf ${NEW_TILESET_DIR}
		${MKDIR} ${NEW_TILESET_DIR}
	fi
	
	mv -f ${TILESET_DIR} ${NEW_TILESET_DIR}
	zip -9 -r ${TARGET_ARCHIVE_NAME} ${TILESET_DIR} ${NEW_TILESET_DIR}
	rm -rf ${NEW_TILESET_DIR}
	
fi


echo "End of processing for the ${TILESET_ARCHIVE} archive."

