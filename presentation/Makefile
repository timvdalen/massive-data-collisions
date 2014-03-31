LATEX = pdflatex
LATEXOPTS = -interaction=nonstopmode -halt-on-error -shell-escape
DOC = main

all: once.a

once.%:
	$(LATEX) $(LATEXOPTS) $(DOC)

clean:
	rm -f $(DOC).{aux,log,nav,out,snm,toc}

remove: clean
	rm -f $(DOC).pdf

release: remove once.a once.b once.c

