# Unix

TARGET =
TARGETEXTENSION = 

CCOUT = -o 
COPTS = -c -O2 

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
	$(INSTALL) -D doc/vasm.pdf $(DESTDIR)/share/doc/vbcc/vasm.pdf
