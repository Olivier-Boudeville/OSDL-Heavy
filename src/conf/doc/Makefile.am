
.PHONY: all show-found-tools clean-local real-clean


EXTRA_DIST = \
	astyle.conf     \
	doxygen.conf.in \
	tidy.conf



all: 
	
	
show-found-tools:
	@echo; echo "@OSDL_INSTALL_STYLE@    Listing found tools\
	@OSDL_DEFAULT_STYLE@"; echo
	@if [ -x "$(DOXYGEN)" ]; then echo "  + doxygen found, using <$(DOXYGEN)>\
	with configuration file <$(DOXYGEN_CONF)>"; fi
	@if [ -x "$(DOT)" ]; then  echo "  + dot found, using <$(DOT)>"; fi
	@if [ -x "$(ASTYLE)" ]; then echo "  + astyle found, using <$(ASTYLE)>\
	with configuration file <$(ASTYLE_CONF)>"; fi
	@if [ -x "$(TIDY)" ]; then echo "  + tidy found, using <$(TIDY)>"; fi
	

clean-local:
	@echo "@OSDL_CLEAN_STYLE@    Cleaning configuration doc repository\
	@OSDL_DEFAULT_STYLE@"


real-clean:
	@echo "@OSDL_CLEAN_STYLE@    Real cleaning configuration doc repository\
	@OSDL_DEFAULT_STYLE@"
	-@rm -f doxygen.conf Makefile.in
	
