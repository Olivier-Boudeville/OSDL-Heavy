TOP = ..

include $(TOP)/GNUmakevars.inc


# This documentation generation has been a pain and a mess for months. 
# The issue is : rsync won't create non already-existing target base directories.
# The only solution : to make a perfect similar tree and to rsync it blindly.
# The root of all evil : webserver will be regularly filled with CVS, so in the 
# htdocs directory, there will be something like src/doc/web/...
# We are therefore forced to locally generate the same scheme, 
# which means having in our doc directory something like :
#    - for the doc pages : PROJECT/PROJECT-VERSION/src/doc/web/...
#    - for the api pages : PROJECT/PROJECT-VERSION/html/...
# That way it works great and should not be changed.

# Therefore, for all web roots ${ROOT}, the result of the rsync should lead to a tree like :
# ${ROOT}/OSDL/OSDL-x.y which contains the 'src' and 'html' directories.
# Furthermore, the command 'ln -s OSDL/OSDL-x.y/src/doc/web/index-SF.html index.html'
# should be issued once (one time for all) from ${ROOT} so that the relevant first page is loaded.


# Content directories.

# Used not to mix projects and versions :
WEB_PREFIX             := $(PROJECTNAME)/$(PROJECTNAME)-$(VERSION)
DOC_OFFSET             := src/doc

# Local directories.
LOCAL_DOC_DIR          := $(shell dirname $(BASEDIR))/doc
LOCAL_WEB_DIR          := $(LOCAL_DOC_DIR)/web
LOCAL_API_DIR          := $(LOCAL_WEB_DIR)/$(WEB_PREFIX)
LOCAL_DOWNLOAD_DIR     := $(LOCAL_WEB_DIR)/$(WEB_PREFIX)/$(DOC_OFFSET)/web/main/download

# Sourceforge settings.
SF_PROJECT_NAME        := osdl
SF_WEB_SERVER          := sourceforge.net
SF_SHELL_SERVER        := shell.$(SF_WEB_SERVER)
SF_WEB_DIR             := /home/groups/`echo "$(SF_PROJECT_NAME)" | head -c 1`/`echo "$(SF_PROJECT_NAME)" | head -c 2`/"$(SF_PROJECT_NAME)"/htdocs

SF_API_DIR             := $(SF_WEB_DIR)/$(WEB_PREFIX)
SF_USER                := wondersye
SF_SHELL_HOMEDIR       := /home/users/`echo "$(SF_USER)" | head -c 1`/`echo "$(SF_USER)" | head -c 2`/"$(SF_USER)"


# Mirror settings.
MIRROR_HOSTNAME        := aranor
MIRROR_WEB_URL         := http://$(PROJECTNAME).esperide.com
MIRROR_WEB_USER        := www-data
MIRROR_WEB_DIR         := /var/www/$(PROJECTNAME)


# Web content should not contain common section, since doc and api need it..
WEB_CONTENT_FILES := `find web -path 'web/common' -prune  -a -type f -o ! -path '*CVS*' -a -type f`
COMMON_WEB_FILES  := `find web/common ! -path '*CVS*' -type f -follow`
HTML_FILES        := `find web -name '*.html' -a -type f`


		
.PHONY: all install doc build inc web links site common loaniarchives api checkdot updatedoxconf  \
	tidy transfer toSF toSF-rsync toSF-scp toMirror toMirror-rsync toMirror-scp \
	checkscp remindLocations info infodirs infofiles infosf infohtml infotools  \
	clean cleanweb cleanobjs cleandeps cleaninstall
	
	
	
# This section is devoted to local documentation generation only.


# If api is wanted and can be generated, will be built :
all: install remindLocations
	@if [ "$(ALWAYS_GENERATE_DOC)" == "yes" ]; then if [ -x "$(DOXYGEN)" ];\
	then $(MAKE) -s api; else echo "$(warning_style)    Warning : No doxygen found,\
	no API documentation generated.$(default_style)"; fi; fi
	

install: site 


doc: web remindLocations
	
	
build: all


#inc:


web: site api


links:


site: common loaniarchives
	@echo; echo "$(install_style)    Copying web source tree to local web repository\
	[$(LOCAL_WEB_DIR)]$(default_style)"
	@cp --parents $(WEB_CONTENT_FILES) $(LOCAL_WEB_DIR)/$(WEB_PREFIX)/$(DOC_OFFSET)
	@cd $(LOCAL_WEB_DIR); ln -sf $(WEB_PREFIX)/$(DOC_OFFSET)/web/main/documentation/OSDL/OSDL-$(VERSION)
	@cd $(LOCAL_WEB_DIR); ln -sf OSDL-$(VERSION) OSDL-latest	
	@cd $(LOCAL_WEB_DIR)/$(WEB_PREFIX)/$(DOC_OFFSET)/web/main; ln -sf index.html index-SF-latest.html
	@cd $(LOCAL_WEB_DIR); ln -sf OSDL/OSDL-0.3/src/doc/web/index-SF.html index.html	
	@echo "$(command_style)    + you can bookmark local doc site, which is to be found in :"
	@echo "      file://$(LOCAL_WEB_DIR)/$(WEB_PREFIX)/$(DOC_OFFSET)/web/index.html"

	
common: 
	@mkdir -p $(LOCAL_WEB_DIR)/$(WEB_PREFIX)/$(DOC_OFFSET)
	@echo; echo "$(install_style)    Copying common web files to local doc repository\
	[$(LOCAL_WEB_DIR)/$(WEB_PREFIX)/$(DOC_OFFSET)]$(default_style)"
	@cp --parents $(COMMON_WEB_FILES) $(LOCAL_WEB_DIR)/$(WEB_PREFIX)/$(DOC_OFFSET)
	
	
loaniarchives:
	@# Disabled since releases only to be done on user request : cd ../conf; $(MAKE) release
	@mkdir -p $(LOCAL_DOWNLOAD_DIR)
	-@cp -f ../conf/LOANI-*.zip ../conf/LOANI-*.tar.gz $(LOCAL_DOWNLOAD_DIR) 2>/dev/null

	
api: checkdot updatedoxconf
	@[ -x "$(DOXYGEN)" ] || \
	( echo; echo "$(error_style)    Unable to create source documentation :\
	doxygen tool not found\
	[doxygen homepage : http://www.doxygen.org].$(default_style)"; echo; false )
	@echo; echo "$(command_style)    Generating API (doxygen configuration taken from\
	$(DOXYGEN_CONF))$(default_style)"
	@echo
	@mkdir -p $(LOCAL_API_DIR)
	@# to prevent doxygen from overwriting my master \
	doxygen-$(PROJECTNAME).css through the link !
	-@PATH=`pwd`/$(DOT_ROOT)/bin:$$PATH LD_LIBRARY_PATH=`pwd`/$(GCC_ROOT)/lib:$$LD_LIBRARY_PATH $(DOXYGEN) $(DOXYGEN_CONF)
	@rm -f $(LOCAL_API_DIR)/html/doxygen.css
	@cp -f web/common/css/doxygen-$(PROJECTNAME).css $(LOCAL_API_DIR)/html/doxygen.css 
	@cp -f web/common/doxygen/*.png $(LOCAL_API_DIR)/html
	@echo
	@echo "$(command_style)    + you can bookmark $(PROJECTNAME)-$(VERSION) API,\
	which is located in : file://$(LOCAL_API_DIR)/html/index.html$(default_style)"
	
			
checkdot:
	@[ -x "$(DOT)" ] \
	|| ( echo; echo "$(error_style)    Unable to generate any useful documentation\
	from source code : dot tool not found.$(default_style)"; echo; false ) 
	@echo "Dot found in <$(DOT)>"
	
	
updatedoxconf:
	@cd ../conf; \
	cat doxygen-original.conf | sed "s|PG_OUTPUT_DIR|$(LOCAL_API_DIR)|1" | sed "s|PG_VERSION|$(VERSION)|1" | sed "s|PG_PROJECT|$(PROJECTNAME)|1" > $(DOXYGEN_CONF)
	@#echo "API documentation will be ouput in $(LOCAL_API_DIR)"
	
	
tidy:	
	@[ -x "$(TIDY)" ] \
	|| ( echo; echo "$(error_style)    Unable to enforce W3C compliance and formatting\
	of html documentation files according to $(PROJECTNAME)'s conventions :\
	tidy tool not found.$(default_style)"; echo; false ) 
	@echo "$(command_style)    Tidying all html files$(default_style)"
	@for f in $(HTML_FILES); do ( echo "  + tidying $$f";\
	$(TIDY) -config $(TOP)/conf/tidy.conf -m $$f ); done


	
# This section is devoted to remote documentation synchronization only.
	
	
transfer: toSF toMirror


# toSF will copy all documentation files (doc and, if available, api) to SF webserver.
# As doc is in CVS, one would generally only need to transfer api.
# Updating doc manually is notably useful to instantaneously update the website.
toSF: 
	@if [ -x "$(RSYNC)" ]; then $(MAKE) toSF-rsync; else $(MAKE) toSF-scp; fi 
	

toSF-rsync: site
	@echo
	@echo "$(install_style)    Updating $(PROJECTNAME) Sourceforge's website from local web repository [$(LOCAL_WEB_DIR)] using rsync$(default_style)"
	@echo
	@opt=$(RSYNC_OPT); $(RSYNC) $$opt $(LOCAL_WEB_DIR)/* $(SF_USER)@$(SF_SHELL_SERVER):$(SF_WEB_DIR)
	@echo "$(command_style)    + you can bookmark $(PROJECTNAME)'s website on Sourceforge at http://$(SF_PROJECT_NAME).$(SF_WEB_SERVER)/$(WEB_PREFIX)/$(DOC_OFFSET)/web/index.html$(default_style)"


toSF-scp: checkscp site
	@echo
	@echo "$(install_style)    Updating $(PROJECTNAME) Sourceforge's website from local web repository [$(LOCAL_WEB_DIR)] using scp$(default_style)"
	@echo
	@opt=$(SCP_OPT); $(SCP) $$opt $(LOCAL_WEB_DIR)/* $(SF_USER)@$(SF_SHELL_SERVER):$(SF_WEB_DIR)
	@echo "$(command_style)    + you can bookmark $(PROJECTNAME)'s website on Sourceforge at http://$(SF_PROJECT_NAME).$(SF_WEB_SERVER)/$(WEB_PREFIX)/$(DOC_OFFSET)/web/index.html$(default_style)"


toMirror:
	@if [ -x "$(RSYNC)" ]; then $(MAKE) toMirror-rsync; else $(MAKE) toMirror-scp; fi 


toMirror-rsync: site
	@echo
	@echo; echo "$(install_style)    Updating $(PROJECTNAME) $(MIRROR_HOSTNAME)'s website from local web repository [$(LOCAL_WEB_DIR)] using rsync$(default_style)"
	@echo
	@opt=$(RSYNC_OPT); $(RSYNC) $$opt $(LOCAL_WEB_DIR)/* $(MIRROR_WEB_USER)@$(MIRROR_HOSTNAME):$(MIRROR_WEB_DIR)
	@echo "$(command_style)    + you can bookmark $(PROJECTNAME)'s website on $(MIRROR_HOSTNAME) at $(MIRROR_WEB_URL)$(default_style)"


toMirror-scp: checkscp site
	@echo
	@echo; echo "$(install_style)    Updating $(PROJECTNAME) $(MIRROR_HOSTNAME)'s website from local web repository [$(LOCAL_WEB_DIR)] using scp$(default_style)"
	@echo
	@opt=$(SCP_OPT); $(SCP) $$opt $(LOCAL_WEB_DIR)/* $(MIRROR_WEB_USER)@$(MIRROR_HOSTNAME):$(MIRROR_WEB_DIR)
	@echo "$(command_style)    + you can bookmark $(PROJECTNAME)'s website on $(MIRROR_HOSTNAME) at $(MIRROR_WEB_URL)$(default_style)"


checkscp:
	@[ -x "$(SCP)" ] \
	|| ( echo; echo "$(error_style)    Unable to transfer html documentation files to remote web server : \
	rsync and its alternative scp not available.\
	Please check src/GNUmakevars.inc or update PATH environment variable.$(default_style)"; echo; false ) 
	@echo; echo "$(warning_style)    Consider installing rsync, which is far more efficient than scp at updating mirrors.$(default_style)"
		
		
remindLocations:
	@echo; echo "$(install_style)    + the following documentation repositories are available :"
	@echo "  * local doc directory : $(LOCAL_DOC_DIR)"
	@echo "  * local web site      : $(LOCAL_WEB_DIR)/main/index.html"
	@echo "  * local web API       : $(LOCAL_API_DIR)/html/index.html"
	@echo "$(default_style)"		
		
		
info: infodirs infofiles infosf
 

infodirs:
	@echo "    + prefix is  : $(PREFIX)"
	@echo "    + local base directory is : $(BASEDIR)"
	@echo "    + local documentation directory is : $(LOCAL_DOC_DIR)"
	@echo "    + local web directory is  : $(LOCAL_WEB_DIR)"
	@echo "    + local api directory is  : $(LOCAL_API_DIR)"
	@echo "    + local final root : $(LOCAL_WEB_DIR)/$(WEB_PREFIX)/$(DOC_OFFSET)"
	
	
infofiles: infohtml
	@echo; echo "    + web documentation files are   : "; echo "$(WEB_CONTENT_FILES)"
	@echo; echo "    + common web files are          : "; echo "$(COMMON_WEB_FILES)"
	@echo; echo "    + doxygen configuration file is : $(DOXYGEN_CONF)"

	
infosf:
	@echo "    + sourceforge UNIX project name : $(SF_PROJECT_NAME)"
	@echo "    + sourceforge shell server      : $(SF_SHELL_SERVER)"
	@echo "    + full sourceforge login        : ssh $(SF_USER)@$(SF_SHELL_SERVER)"
	@echo "    + sourceforge web directory     : $(SF_WEB_DIR)"
	@echo "    + sourceforge home directory    : $(SF_SHELL_HOMEDIR)"
	@echo "    + sourceforge api directory     : $(SF_API_DIR)"	
	@echo "    + SSH target                    : $(SF_USER)@$(SF_SHELL_SERVER):$(SF_WEB_DIR)"
	@echo "    + files to be copied will be    : scp myFiles $(SF_USER)@$(SF_SHELL_SERVER):$(SF_SHELL_HOMEDIR)"


infomirror:
	@echo "    + mirror web directory          : $(MIRROR_WEB_DIR)"


infohtml: 	
	@echo
	@echo "    + HTML files are    :"; echo "$(HTML_FILES)"
	@echo "    + rsync options are : $(RSYNC_OPT)"
	@echo "    + scp options are   : $(SCP_OPT)"

		
infotools:
	@echo "DOXYGEN       = <$(DOXYGEN)>"
	@echo "DOXYGEN_ROOT  = <$(DOXYGEN_ROOT)>"
	@echo "DOT           = <$(DOT)>"
	@echo "DOT_ROOT      = <$(DOT_ROOT)>"
	
	
clean: cleanweb


cleanweb:
	@echo "$(clean_style)    Cleaning local doc repository [$(LOCAL_WEB_DIR)]$(default_style)"
	@rm -rf $(LOCAL_WEB_DIR)


cleanobjs:


cleandeps:
	

cleaninstall: cleanweb
	@echo "$(clean_style)    Cleaning installed doc repository [$(LOCAL_DOC_DIR)]$(default_style)"
	@rm -rf $(LOCAL_DOC_DIR)

	
