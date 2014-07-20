SUBDIRS         = source

AUTOMAKE_OPTIONS = foreign

EXTRA_DIST      = \
		BUILD SETUP license.txt CLUCalc_Install.txt


dist-hook:
	mkdir -p $(distdir)/ExampleScripts
	mkdir -p $(distdir)/ExampleScripts/Presentation1
	mkdir -p $(distdir)/ExampleScripts/Images
	mkdir -p $(distdir)/ExampleScripts/2D_Conics
	mkdir -p $(distdir)/ExampleScripts/TextureMapping
	mkdir -p $(distdir)/CLUCalcDoc
	mkdir -p $(distdir)/CLUCalcDoc/images
	mkdir -p $(distdir)/Documentation
	cp -p $(srcdir)/ExampleScripts/*.clu $(distdir)/ExampleScripts/.
	cp -p $(srcdir)/ExampleScripts/*.png $(distdir)/ExampleScripts/.
	cp -p $(srcdir)/ExampleScripts/*.bmp $(distdir)/ExampleScripts/.
	cp -p $(srcdir)/ExampleScripts/*.jpg $(distdir)/ExampleScripts/.
	cp -p $(srcdir)/ExampleScripts/*.txt $(distdir)/ExampleScripts/.
	cp -p $(srcdir)/ExampleScripts/Presentation1/*.clu $(distdir)/ExampleScripts/Presentation1/.
	cp -p $(srcdir)/ExampleScripts/Presentation1/*.png $(distdir)/ExampleScripts/Presentation1/.
	cp -p $(srcdir)/ExampleScripts/Presentation1/*.tex $(distdir)/ExampleScripts/Presentation1/.
	cp -p $(srcdir)/ExampleScripts/Presentation1/*.bmp $(distdir)/ExampleScripts/Presentation1/.
	cp -p $(srcdir)/ExampleScripts/Images/*.clu $(distdir)/ExampleScripts/Images/.
	cp -p $(srcdir)/ExampleScripts/Images/*.bmp $(distdir)/ExampleScripts/Images/.
	cp -p $(srcdir)/ExampleScripts/2D_Conics/*.clu $(distdir)/ExampleScripts/2D_Conics/.
	cp -p $(srcdir)/ExampleScripts/TextureMapping/*.clu $(distdir)/ExampleScripts/TextureMapping/.
	cp -p $(srcdir)/ExampleScripts/TextureMapping/*.png $(distdir)/ExampleScripts/TextureMapping/.
	cp -p $(srcdir)/ExampleScripts/TextureMapping/*.jpg $(distdir)/ExampleScripts/TextureMapping/.
	cp -p $(srcdir)/CLUCalcDoc/*.dox $(distdir)/CLUCalcDoc/.
	cp -p $(srcdir)/CLUCalcDoc/*.ini $(distdir)/CLUCalcDoc/.
	cp -p $(srcdir)/CLUCalcDoc/*.tex $(distdir)/CLUCalcDoc/.
	cp -p $(srcdir)/CLUCalcDoc/images/*.jpg $(distdir)/CLUCalcDoc/images/.
	cp -p $(srcdir)/Documentation/* $(distdir)/Documentation/.
