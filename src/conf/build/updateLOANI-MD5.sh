#!/bin/sh

ceylan_release=$1
osdl_release=$2
replace_script=$3

loani_tools_settings="ToolsSettings.inc"

do_debug=1

[ "$do_debug" -eq 1 ] || echo "ceylan_release = $ceylan_release"
[ "$do_debug" -eq 1 ] || echo "osdl_release   = $osdl_release"
[ "$do_debug" -eq 1 ] || echo "replace_script = $replace_script"


target_loani="LOANIToolsSettings.inc"

if [ ! -e "${ceylan_release}" ] ; then
	echo "Error, no Ceylan source release found (${ceylan_release})." 1>&2
	exit 1
fi


if [ ! -e "${osdl_release}" ] ; then
	echo "Error, no OSDL source release found (${osdl_release})." 1>&2
	exit 2
fi


if [ ! -e "${target_loani}" ] ; then
	echo "Error, no target LOANI version file found (${target_loani})." 1>&2
	exit 3
fi

if [ ! -x "${replace_script}" ] ; then
	echo "Error, no executable replacing script found (${replace_script})." 1>&2
	exit 4
fi


MD5=`which md5sum 2>/dev/null`

if [ ! -x "${MD5}" ]; then

	MD5=`which cksum 2>/dev/null`
	if [ ! -x "${MD5}" ]; then
		echo "No MD5 sum tool found." 1>&2
		exit 10
	else
		MD5="${MD5} -5"	
	fi
		
fi

[ "$do_debug" -eq 1 ] || echo "Using MD5 = $MD5"

ceylan_md5=`$MD5 ${ceylan_release} | awk '{print $1}'`
[ "$do_debug" -eq 1 ] || echo "ceylan_md5 = $ceylan_md5"
${replace_script} Ceylan_MD5 "Ceylan_MD5=\"${ceylan_md5}\"" $(loani_tools_settings)

osdl_md5=`$MD5 ${osdl_release} | awk '{print $1}'`
[ "$do_debug" -eq 1 ] || echo "osdl_md5 = $osdl_md5"
${replace_script} OSDL_MD5 "OSDL_MD5=\"${osdl_md5}\"" $(loani_tools_settings)

[ "$do_debug" -eq 1 ] || echo "Replacement done."
