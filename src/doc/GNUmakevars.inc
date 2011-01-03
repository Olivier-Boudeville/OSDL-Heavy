# Generic rules for documentation generation.


RST_SOURCES     = $(wildcard *.rst)
GENERATED_HTML  = $(patsubst %.rst,%.html,$(RST_SOURCES))
GENERATED_PDF   = $(patsubst %.rst,%.pdf,$(RST_SOURCES))

PLOT_SOURCES    = $(wildcard *.dat)
GENERATED_PNG   = $(patsubst %.dat,%.png,$(PLOT_SOURCES))


%.png: %.plot
	@echo "    Generating $@ from $< with gnuplot"
	@gnuplot $<


%-negated.png: %.png
	@echo "    Generating $@ from $< with convert"
	@convert $< -negate $@


clean: clean-local

clean-local:
	@echo "    Cleaning generated documentation"
	-@rm -f *.tex *.out *.toc *.aux *.log $(GENERATED_HTML) $(GENERATED_PDF) $(GENERATED_PNG)


info:
	@echo "RST_SOURCES    = $(RST_SOURCES)"
	@echo "GENERATED_HTML = $(GENERATED_HTML)"
	@echo "GENERATED_PDF  = $(GENERATED_PDF)"
	@echo "PLOT_SOURCES   = $(PLOT_SOURCES)"
	@echo "GENERATED_PNG  = $(GENERATED_PNG)"
