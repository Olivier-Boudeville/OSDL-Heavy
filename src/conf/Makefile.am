.PHONY: clean-local real-clean


SUBDIRS = build doc

DIST_SUBDIRS = $(SUBDIRS)

EXTRA_DIST = OSDLSettings.inc


clean-local: 
	@echo "@OSDL_CLEAN_STYLE@    Cleaning build repository\
	@OSDL_DEFAULT_STYLE@"
	

real-clean: clean-local
	@echo "@OSDL_CLEAN_STYLE@    Deep cleaning of build repository\
	@OSDL_DEFAULT_STYLE@"
	-@rm -f Makefile.in config.guess config.sub install-sh ltmain.sh \
	missing stamp-h1
