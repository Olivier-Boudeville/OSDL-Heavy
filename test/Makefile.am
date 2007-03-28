SUBDIRS = audio basic engine events video

# All programs using the OSDL library should use in their 
# configure.ac the OSDL_PATH macro defined in osdl.m4 :
EXTRA_DIST = osdl.m4 playTests.sh

# Disabled since this script is not to be run from an installed tree :
#bin_SCRIPTS = playTests.sh


check:
	@$(srcdir)/playTests.sh


install-data-local:
	@echo; echo "@OSDL_INSTALL_STYLE@    Installing playTests.sh @OSDL_DEFAULT_STYLE@"
	@mkdir -p $(DESTDIR)@OSDL_TEST_DIR@
	@cp -f $(srcdir)/playTests.sh $(DESTDIR)@OSDL_TEST_DIR@


clean-local:
	@echo " Cleaning all test logs"	
	-@/bin/rm -f  `find . -name 'testsOutcome.txt'`
	-@/bin/rm -f  `find . -name 'test*.log'`
	-@/bin/rm -rf `find . -name 'testOSDL*-logs'`
