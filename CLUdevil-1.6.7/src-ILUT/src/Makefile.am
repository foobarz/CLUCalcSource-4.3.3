lib_LIBRARIES = libILUT.a

libILUT_a_SOURCES = ilut_allegro.c     ilut_main.c \
                     ilut_sdlsurface.c  ilut_states.c \
                     ilut_internal.c    ilut_opengl.c \
                     ilut_win32.c       ilut_directx.c \
					 ilut_directx9.c

AM_CFLAGS = @LIBILUT_CFLAGS@

libILUT_a_LFLAGS = -no-undefined @VERSIONINFO@ \
                     -L$(top_srcdir)/src-IL/src \
                     -L$(top_srcdir)/src-ILU/src \
                     @LIBSDL@ @LIBALLEGRO@ @LIBILUT_WIN32LIBPATH@

# libILUT_a_LIBADD = @LIBGL@ @LIBGDI32@
 
INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/src-ILUT/include \
           @HSDL@ @HALLEGRO@ @X_CFLAGS@
