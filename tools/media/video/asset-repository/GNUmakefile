TOP = ../../../..

.PHONY: identifiers info clean

CONVERT_SCRIPT = $(TOP)/src/code/scripts/shell/generate-identifier-list.sh


identifiers: animated-objects.id directions.id attitudes.id outside-looks.id


%s.id: %-identifiers.txt
	@$(CONVERT_SCRIPT) $< $@
	
	

info:
	@echo "CONVERT_SCRIPT = ${CONVERT_SCRIPT}"


clean:
	@rm -f *.id
	
	
