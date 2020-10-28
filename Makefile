# Unix, using gcc

CC = gcc
TARGET =
TARGETEXTENSION = 
OUTFMTS = -DOUTAOUT -DOUTBIN -DOUTELF -DOUTHUNK -DOUTSREC -DOUTTOS -DOUTVOBJ \
          -DOUTXFIL

CCOUT = -o 
COPTS = -c -std=c99 -O2 -Wpedantic -DUNIX $(OUTFMTS)

LD = $(CC)
LDOUT = $(CCOUT)
LDFLAGS = -lm
MKDIR = mkdir -p

RM = rm -f
INSTALL = install

include make.rules

install: all
	$(INSTALL) -D $(VASMEXE) $(DESTDIR)/bin/$(VASMEXE)
	$(INSTALL) -D $(VOBJDMPEXE) $(DESTDIR)/bin/$(VOBJDMPEXE)

install-doc: doc/vasm.pdf
	$(INSTALL) -D doc/vasm.pdf $(DESTDIR)/share/doc/vasm/vasm.pdf
