.PHONY: fix-version-file blank-MD5 clean-local real-clean

SUBDIRS = build doc environment

DIST_SUBDIRS = $(SUBDIRS)

EXTRA_DIST = OSDLSettings.inc


# This part will have to be updated :

OSDL_VERSION_FILE = versions.sh
OSDL_VERSION_FILE  = ../../../../OSDL/OSDL-0.3/src/conf/loani-versions.sh


fix-version-file:
	@echo "@OSDL_INSTALL_STYLE@    Fixing OSDL version file\
	(${OSDL_VERSION_FILE}) so that it is a link to OSDL version file\
	(${OSDL_VERSION_FILE})@OSDL_DEFAULT_STYLE@"
	@-rm -f ${OSDL_VERSION_FILE}.back
	@-mv -f ${OSDL_VERSION_FILE} ${OSDL_VERSION_FILE}.back 2>/dev/null
	@ln -s ${OSDL_VERSION_FILE} ${OSDL_VERSION_FILE}
	@echo "Done"

	
blank-MD5:
	@echo "@OSDL_INSTALL_STYLE@    Blanking MD5 sum of OSDL release\
	to avoid MD5 recursive dependency of archive@OSDL_DEFAULT_STYLE@"
	@../code/scripts/shell/replaceLineStartingBy.sh Ceylan_MD5\
	'Ceylan_MD5="(blanked)"' ceylan-versions.sh 



clean-local: 
	@echo "@OSDL_CLEAN_STYLE@    Cleaning build repository\
	@OSDL_DEFAULT_STYLE@"
	

real-clean: clean-local
	@echo "@OSDL_CLEAN_STYLE@    Deep cleaning of build repository\
	@OSDL_DEFAULT_STYLE@"
	-@rm -f Makefile.in config.guess config.sub install-sh ltmain.sh \
	missing stamp-h1
