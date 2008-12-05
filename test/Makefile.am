SUBDIRS = audio basic engine events video

# All programs using the OSDL library should use in their 
# configure.ac the OSDL_PATH macro defined in osdl.m4:
EXTRA_DIST = osdl.m4 playTests.sh

# Disabled since this script is not to be run from an installed tree:
#bin_SCRIPTS = playTests.sh


check:
	@echo; echo " Checking OSDL installation with $(srcdir)/playTests.sh"
	@$(srcdir)/playTests.sh


install-data-local:
	@echo; echo " Installing playTests.sh"
	@mkdir -p $(DESTDIR)@OSDL_TEST_DIR@
	@cp -f $(srcdir)/playTests.sh $(DESTDIR)@OSDL_TEST_DIR@


clean-local:
	@echo " Cleaning all test logs"	
	-@/bin/rm -f  `find . -name 'testsOutcome.txt'`
	-@/bin/rm -f  `find . -name 'test*.log'`
	-@/bin/rm -rf `find . -name 'testOSDL*-logs'`
	-@/bin/rm -f granularity.dat
	@( cd cross-tests && $(MAKE) clean )

