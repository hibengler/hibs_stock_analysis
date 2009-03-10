all :
	$(MAKE) -C source
	$(MAKE) -C tests
	$(MAKE) -C util
	$(MAKE) -C ocr/ocrlib
	$(MAKE) -C ocr
	$(MAKE) -C documentation

clean :
	$(MAKE) -C source clean
	$(MAKE) -C tests clean
	$(MAKE) -C ocr/ocrlib clean
	$(MAKE) -C ocr clean
	$(MAKE) -C documentation clean

clean-all :
	$(MAKE) -C source clean-all
	$(MAKE) -C tests clean-all
	$(MAKE) -C ocr/ocrlib clean-all
	$(MAKE) -C ocr clean-all
	$(MAKE) -C documentation clean-all

version :
	$(MAKE) -C source version
	$(MAKE) -C documentation version

source-dist :
	$(MAKE) -C ocr network1.net network2.net
	$(MAKE) -C source source-dist
	$(MAKE) -C tests source-dist
	$(MAKE) -C ocr/ocrlib source-dist
	$(MAKE) -C ocr source-dist
	$(MAKE) -C documentation source-dist
	(VERSION=`cat VERSION` ; \
	tar --create --directory .. --exclude CVS --exclude .cvsignore \
	    --exclude ocrdata --exclude website --exclude private \
	    -f /tmp/lwneuralnet-$${VERSION}.tar.gz \
	    --gzip --verbose lwneuralnet-$${VERSION} ; \
	mv /tmp/lwneuralnet-$${VERSION}.tar.gz .)



