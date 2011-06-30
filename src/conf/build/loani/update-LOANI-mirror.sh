#!/bin/sh

# Expected to be run from its directory (osdl/OSDL/trunk/src/conf/build/loani).

mirror_host="ftp.esperide.com"


base_dir="LOANI-archive-repository"


cp="/bin/cp -f"
md5sum="/usr/bin/md5sum"
rm="/bin/rm -f"


if [ -z "${LOANI_REPOSITORY}" ] ; then

	echo "Error, environment variable LOANI_REPOSITORY is not set." 1>&2
	exit 5

fi


if [ ! -d "${LOANI_REPOSITORY}" ] ; then

	echo "Error, the directory specified by the LOANI_REPOSITORY environment variable (${LOANI_REPOSITORY}) does not exist." 1>&2
	exit 6

fi


if [ -z "${CEYLAN_SRC}" ] ; then

	echo "Error, environment variable CEYLAN_SRC is not set." 1>&2
	exit 7

fi


if [ ! -d "${CEYLAN_SRC}" ] ; then

	echo "Error, the directory specified by the CEYLAN_SRC environment variable (${CEYLAN_SRC}) does not exist." 1>&2
	exit 8

fi



ceylan_settings_file="${CEYLAN_SRC}/conf/build/CeylanToolsSettings.inc"

if [ ! -f "${ceylan_settings_file}" ] ; then

	echo "Error, Ceylan setting file (${ceylan_settings_file}) could not be found." 1>&2
	exit 10

fi

. "${ceylan_settings_file}"



osdl_settings_file="../OSDLToolsSettings.inc"

if [ ! -f "${osdl_settings_file}" ] ; then

	echo "Error, OSDL setting file (${osdl_settings_file}) could not be found." 1>&2
	exit 11

fi

. "${osdl_settings_file}"



# Taken from loani-commonBuildTools.sh (for DS or for PC):

#common_package_name_list="gcc binutils gdb devkitARM libnds grit PAlib dswifi DeSmuME NoCashGBA"

# Deactivated as not updated anymore:
common_package_name_list=""


# Taken from loani-optionalTools.sh:

#optional_package_name_list="dot doxygen"

# Deactivated as not updated anymore:
optional_package_name_list=""


# Taken from loani-OrgeTools.sh:
# (egeoip was removed from that mirror script, as it is available from SVN only)
orge_package_name_list="Erlang Geolite"


# Taken from loani-requiredTools.sh:
required_package_name_list_win="SDL_win zlib_win libjpeg_win libpng_win SDL_image_win SDL_gfx_win freetype_win SDL_ttf_win libogg_win libvorbis_win SDL_mixer_win PCRE_win FreeImage_win CEGUI_win PhysicsFS_win Ceylan_win OSDL_win"

required_package_name_list_non_win="libtool SDL libjpeg zlib libpng SDL_image SDL_gfx freetype SDL_ttf libogg libvorbis SDL_mixer PCRE FreeImage CEGUI PhysicsFS Ceylan OSDL"

required_package_name_list="${required_package_name_list_non_win} ${required_package_name_list_win}"


full_package_name_list="${common_package_name_list} ${optional_package_name_list} ${orge_package_name_list} ${required_package_name_list}"

echo " Updating mirror ${mirror_host} with latest LOANI archives"


tmp_dir="tmp-update-LOANI-mirror"

if [ -d "${tmp_dir}" ] ; then

	echo "(a previous ${tmp_dir} directory was still existing, it has been removed)"

	${rm} -r ${tmp_dir}

fi




# Archives will be stored under ${base_dir}/${package_name}, without the _win
# suffix (ex: libvorbis_win will be in LOANI-archive-repository/libvorbis,
# i.e. with libvorbis)
mkdir -p ${tmp_dir}/${base_dir}

cd ${tmp_dir}/${base_dir}

for package_name in ${full_package_name_list}; do

	archive_name="${package_name}_ARCHIVE"
	eval archive_file="\$$archive_name"
	#echo "archive_file = $archive_file"

	archive_mirror_dir=`echo "${package_name}" | sed 's|_win$||1'`
	mkdir -p ${archive_mirror_dir}

	if [ ! -f "${LOANI_REPOSITORY}/${archive_file}" ] ; then

		# Rely on first official server:
		location_name="${package_name}_DOWNLOAD_LOCATION"
		eval download_url="\$$location_name"

		# Creating the path in two steps removes some problems with no$gba:
		download_url="${download_url}/${archive_file}"

		echo "    + for package ${package_name}, archive file is ${archive_file}, not found in LOANI repository, downloading it from ${download_url}"
		wget $PROXY_CONF --passive-ftp --output-document="${LOANI_REPOSITORY}/${archive_file}" ${download_url}

		if [ ! $? -eq 0 ] ; then

			echo "Error, downloading of ${archive_file} from ${download_url} failed." 1>&2
			exit 20

		fi

		target_md5_var="${package_name}_MD5"
		eval expected_md5="\$$target_md5_var"

		md5_res=`${md5sum} "${LOANI_REPOSITORY}/${archive_file}"`

		computed_md5=`echo ${md5_res}| awk '{printf $1}'`

		if [ "${computed_md5}" = "${expected_md5}" ] ; then
			echo "MD5 sums match."
		else
			echo "Error, MD5 sums not matching: expected ${expected_md5}, computed ${computed_md5}." 1>&2
			exit 25
		fi


	else

		echo "    + for package ${package_name}, archive file is ${archive_file}, found in LOANI repository, just copied in ${archive_mirror_dir} directory"

		md5_res=`${md5sum} "${LOANI_REPOSITORY}/${archive_file}"`

		computed_md5=`echo ${md5_res}| awk '{printf $1}'`

	fi

	echo "Latest recommended archive for package ${package_name} is ${archive_file}, whose MD5 code is ${computed_md5}." >> "${archive_mirror_dir}/MD5SUMS.txt"

	${cp} ${LOANI_REPOSITORY}/${archive_file} ${archive_mirror_dir}

done

cd ..
tree ${base_dir}

rsync_target="wondersye@aranor:/home/ftp"

echo
echo "Sending now updates to the mirror (${rsync_target})..."

rsync -r ${base_dir} ${rsync_target}

if [ ! $? -eq 0 ] ; then

	echo "Error, rsync to ${rsync_target} failed." 1>&2
	exit 30

fi

echo "... done."

echo "Finally, a 'chown -R ftp:wondersye /home/ftp' should be issued on the mirror as root."

echo "Then you can check the result with this link: ftp://${mirror_host}."

${rm} -r ${tmp_dir}
