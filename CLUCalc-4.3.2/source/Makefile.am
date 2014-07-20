bin_PROGRAMS	= CLUCalc

CLUCalc_SOURCES	= CLUCalc.cpp \
			OGLWin.cpp \
			CLUScriptEditor.cpp CLUOutput.cpp \
			FLTKEditor.cpp FLTKOutput.cpp \
			CLUScriptEditor.h FLTKEditor.h OGLWin.h \
			FLTKOutput.h CLUOutput.h StdAfx.h

CLUCalc_LDFLAGS = -L${DEVIL_HOME}/lib 
CLUCalc_LDADD	= @CLU_LIBS@ @FLTK_LIBS@ @XML_LIBS@ -lm \
		  -lnsl -lrt -ldl ${DEVIL_HOME}/lib/libIL.a \
		${DEVIL_HOME}/lib/libILU.a ${DEVIL_HOME}/lib/libILUT.a -ltiff -lmng -lpng -ljpeg

EXTRA_DIST      = \
		CLUCalc.ico CLUCalc.rc CLUCalc_v3.rtf \
		StdAfx.cpp resource.h 
