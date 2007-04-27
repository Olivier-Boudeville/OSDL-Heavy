DOT     = `which dot 2>/dev/null`
DOT_OPT = "-Tpng"


.PHONY: images clean


%.png: %.graph
	@echo "Generating with dot $@ from $<"
	@$(DOT) $(DOT_OPT) -o $@ $<
	 

images: LOANI-dependencies.png


clean:
	-@rm -f LOANI-dependencies.png
	
	
