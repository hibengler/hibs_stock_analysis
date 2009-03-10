# Makefile -- makefile for documentation
# $Id$

all : structure.ps network.ps

structure.ps : structure.fig
	fig2dev -L ps $^ $@

network.ps : network.fig
	fig2dev -L ps $^ $@

mathematics.ps : mathematics.dvi
	dvips -o $@ $^

mathematics.dvi : mathematics.tex
	latex $^

neuron.tex : neuron.fig
	fig2dev -L eepic $^ $@

version :
	cp lwneuralnet.3 lwneuralnet.bak
	(DATE=`date +"%e %B %Y" | sed "s/^ //"` ; \
	VERSION=`cat ../VERSION` ; \
	sed "s/Version [0-9.]*/Version $$VERSION/;s/[0-9]* [A-Z][a-z]* 20[0-9]*/$$DATE/" lwneuralnet.bak > lwneuralnet.3)

clean :
	-rm -f *.aux *.log *.dvi
	-rm -f structure.ps network.ps
	-rm -f network.tex

clean-backup :
	-rm -f *.bak *~

clean-all : clean clean-backup

source-dist : structure.ps network.ps
	-rm -f *.aux *.log *.dvi
	-rm -f mathematics.*
	-rm -f .cvsignore
