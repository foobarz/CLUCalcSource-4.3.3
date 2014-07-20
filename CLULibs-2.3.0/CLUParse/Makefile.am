SUBDIRS		= source 

AUTOMAKE_OPTIONS = foreign

EXTRA_DIST	= CLUParse.vcproj license.txt Readme.txt Header.txt

dist-hook:
	mkdir -p $(distdir)/CLUScriptDoc
	mkdir -p $(distdir)/CLUScriptDoc/images
	cp -p $(srcdir)/CLUScriptDoc/*.dox $(distdir)/CLUScriptDoc/.
	cp -p $(srcdir)/CLUScriptDoc/images/*.jpg $(distdir)/CLUScriptDoc/images/.
