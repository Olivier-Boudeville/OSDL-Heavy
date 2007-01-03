
.PHONY: blank-MD5 prepare-release release test info-local stat\
	clean-local realclean 


# The template for the LOANI README :
LOANI_README_TEMPLATE = LOANI-README-template.txt

# The actual README to produce :
LOANI_README_TARGET   = LOANI-README



CEYLAN_TOOLS_SETTINGS = \
	@CEYLAN_INSTALL_ROOT@/share/Ceylan/CeylanToolsSettings.inc
	
OSDL_TOOLS_SETTINGS   = $(srcdir)/../OSDLToolsSettings.inc

LOANI_TOOLS_SETTINGS  = $(srcdir)/LOANIToolsSettings.inc

LOANI_INSTALLATIONS_DIR = $(srcdir)/../../../../../../..


# Where the LOANI download cache lies :
LOANI_REPOSITORY_DIR = @LOANI_REPOSITORY_DIR@

# Where LOANI will install packages :
LOANI_INSTALL_DIR    = @LOANI-LOANI_INSTALL_DIR@



# Where the Ceylan source archive is, so that its MD5 sum can be computed :
CEYLAN_RELEASE = $(LOANI_REPOSITORY_DIR)/ceylan/Ceylan-@OSDL_OLDEST_SUPPORTED_VERSION@/@CEYLAN_CURRENT_SOURCE_ARCHIVE@

# Where the OSDL source archive is, so that its MD5 sum can be computed :
OSDL_RELEASE = $(LOANI_REPOSITORY_DIR)/osdl/OSDL-@OSDL_VERSION@/@OSDL_CURRENT_SOURCE_ARCHIVE@


# The files that will be in the LOANI archive :
LOANI_FELLOWS_FILES = \
	loani.sh                    \
	loani-requiredTools.sh      \
	loani-commonBuildTools.sh   \
	loani-optionalTools.sh      \
	$(LOANI_TOOLS_SETTINGS)     \
	defaultLocations.sh         \
	termUtils.sh                \
	platformDetection.sh        \
	generateBugReport.sh        \
	makeRepositoryLinks.sh      \
	$(LOANI_README_TARGET)


# The version of LOANI matches the one of OSDL actually :
LOANI_VERSION = @OSDL_VERSION@

# The base name of LOANI archives :
LOANI_ARCHIVE_NAME = LOANI-$(LOANI_VERSION)
 
LOANI_ZIP = $(LOANI_ARCHIVE_NAME).zip
LOANI_TGZ = $(LOANI_ARCHIVE_NAME).tar.gz

# The directory used to prepare the LOANI archive :
LOANI_TMP_ARCHIVE_DIR = $(LOANI_ARCHIVE_NAME)

LOANI_TESTING_DIR = $(HOME)/tmp/LOANI-testing



EXTRA_DIST = \
	$(LOANI_FELLOWS_FILES)
	
noinst_SCRIPTS = \
	$(LOANI_FELLOWS_FILES)



REPLACE_SCRIPT = \
	@CEYLAN_INSTALL_ROOT@/share/Ceylan/scripts/shell/replaceLineStartingBy.sh


$(LOANI_README_TARGET): 
	@cp $(top_srcdir)/$(LOANI_README_TARGET) $(srcdir) 
	@cat $(LOANI_README_TARGET) | sed 's|ST_LOANI_VERSION|$(LOANI_VERSION)|g' | sed "s|ST_LOANI_DATE|`LANG= && date '+%A, %B %-e, %Y (%H:%M)'`|g" > $@


$(LOANI_TOOLS_SETTINGS): $(CEYLAN_TOOLS_SETTINGS) $(OSDL_TOOLS_SETTINGS) \
		$(CEYLAN_RELEASE) $(OSDL_RELEASE)
	@echo "@OSDL_MAKE_STYLE@    Merging $@ @OSDL_DEFAULT_STYLE@"
	@echo "# This file tells LOANI and the build system about \
	the tools that they must know." > $@
	@echo "# It corresponds to the gathering of \
	Ceylan and OSDL related tools." >> $@
	@echo "# " >> $@
	@echo "# This file is generated, hence should *not* be modified :" >> $@
	@echo "# modify instead" `basename $(CEYLAN_TOOLS_SETTINGS)`\
	"and" `basename $(OSDL_TOOLS_SETTINGS)`"." >> $@
	@echo >> $@
	@echo "# From $(CEYLAN_TOOLS_SETTINGS) :"       >> $@
	@cat $(CEYLAN_TOOLS_SETTINGS)                   >> $@
	@echo "# From $(OSDL_TOOLS_SETTINGS) :"         >> $@
	@cat $(OSDL_TOOLS_SETTINGS)                     >> $@
	@$(srcdir)/updateLOANI-MD5.sh $(LOANI_TOOLS_SETTINGS) $(CEYLAN_RELEASE) \
		$(OSDL_RELEASE)	$(REPLACE_SCRIPT)


blank-MD5:
	@echo "@OSDL_INSTALL_STYLE@    Blanking MD5 sum of OSDL release \
	to avoid MD5 recursive dependency on archive@OSDL_DEFAULT_STYLE@"
	@$(REPLACE_SCRIPT) OSDL_MD5 'OSDL_MD5="(blanked)"' $(OSDL_TOOLS_SETTINGS)


$(LOANI_TGZ): $(LOANI_FELLOWS_FILES) $(LOANI_README_TEMPLATE)
	@echo "@OSDL_MAKE_STYLE@    Building LOANI release file\
	$@ @OSDL_DEFAULT_STYLE@"
	@tar chf - $(LOANI_TMP_ARCHIVE_DIR) | gzip > $@


$(LOANI_ZIP): $(LOANI_FELLOWS_FILES) $(LOANI_README_TEMPLATE)
	@echo "@OSDL_MAKE_STYLE@    Building LOANI release file\
	$@ @OSDL_DEFAULT_STYLE@"
	@zip $@ $(LOANI_TMP_ARCHIVE_DIR)/* 1>/dev/null 2>&1
	
			
prepare-release:
	-@rm -rf $(srcdir)/$(LOANI_TMP_ARCHIVE_DIR)
	@mkdir $(srcdir)/$(LOANI_TMP_ARCHIVE_DIR)
	@cd $(srcdir) && \
	cp -f $(LOANI_FELLOWS_FILES) $(srcdir)/$(LOANI_TMP_ARCHIVE_DIR)
	@chmod +x $(srcdir)/$(LOANI_TMP_ARCHIVE_DIR)/loani.sh


release: prepare-release $(LOANI_TGZ) $(LOANI_ZIP) 
	@echo "@OSDL_MAKE_STYLE@    Release archives for LOANI\
	are available.@OSDL_DEFAULT_STYLE@"

	
test: release
	@mkdir -p $(LOANI_TESTING_DIR)
	@cp -f $(LOANI_ZIP) $(LOANI_TGZ) $(LOANI_TESTING_DIR)
	@cd $(LOANI_TESTING_DIR); unzip -qq $(LOANI_ZIP)
	@echo "You can type now : cd $(LOANI_TESTING_DIR); loani.sh -d"


info-local:
	@echo "LOANI_VERSION       = $(LOANI_VERSION)"
	@echo "LOANI_ZIP           = $(LOANI_ZIP)" 
	@echo "LOANI_TGZ           = $(LOANI_TGZ)"
	@echo "LOANI_FELLOWS_FILES = $(LOANI_FELLOWS_FILES)"
	@echo "Ceylan release      = $(CEYLAN_RELEASE)"
	@echo "OSDL release        = $(OSDL_RELEASE)"
	

stat:
	@echo "Newline, word, and byte counts for LOANI :"
	@cat $(LOANI_FELLOWS_FILES) | wc
		
		
clean-local:
	@echo "@OSDL_CLEAN_STYLE@    Cleaning all in $(srcdir)@OSDL_DEFAULT_STYLE@"
	-@cd $(srcdir) && rm -f wget-log* LOANI.log $(LOANI_ZIP) $(LOANI_TGZ)\
	$(LOANI_README_TARGET) $(LOANI_TOOLS_SETTINGS)
	-@cd $(srcdir) && rm -rf LOANI-$(LOANI_VERSION)
	
	
realclean : clean-local
	@echo "@OSDL_CLEAN_STYLE@    Real cleaning all in \
	$(srcdir)@OSDL_DEFAULT_STYLE@"
	-@rm -rf $(LOANI_REPOSITORY_DIR) # Too risky : $(LOANI_INSTALL_DIR)		
