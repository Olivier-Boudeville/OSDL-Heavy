ARCHIVE_DIR  = @OSDL_ARCHIVE_DIR@/bin
ARCHIVE_FILE = "$(ARCHIVE_DIR)/`date '+%Y%m%d'`-Full-archive-OSDL-bin.tar.bz2" 


SHELL_SCRIPTS_SET = \
	CVS-to-aranor-webserver.crontab    \
	export-CVS-to-aranor-webserver.sh

        
EXTRA_DIST = $(SHELL_SCRIPTS_SET)		 
                  
all: 

tar:
	@mkdir -p $(ARCHIVE_DIR)
	@tar -cvf - * 2>/dev/null | bzip2 > $(ARCHIVE_FILE)
	@echo "All bin scripts have been saved in $(ARCHIVE_FILE)"

