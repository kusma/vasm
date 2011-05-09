# Unix

TARGET =
TARGETEXTENSION = 

CC = gcc
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
	$(INSTALL) -D $(VASMEXE) $(prefix)/bin/$(VASMEXE)
	$(INSTALL) -D $(VOBJDMPEXE) $(prefix)/bin/$(VOBJDMPEXE)

install-doc: doc/vasm.pdf
	$(INSTALL) -D doc/vasm.pdf $(prefix)/share/doc/vbcc/vasm.pdf
