# Copyright 2022, Jefferson Science Associates, LLC.
# Subject to the terms in the LICENSE file found in the top-level directory.
#
#    Author:  Bryan Moffit
#             moffit@jlab.org                   Jefferson Lab, MS-12B3
#             Phone: (757) 269-5660             12000 Jefferson Ave.
#             Fax:   (757) 269-5800             Newport News, VA 23606
#
# Description:
#    Makefile for readout list timer library
#
#
BASENAME=rolTimer
#
#
INSTALL_INC ?= build/include
INSTALL_LIB ?= build/lib
#
# Uncomment DEBUG line, to include some debugging info ( -g and -Wall)
DEBUG	?= 1
QUIET	?= 1
#
ifeq ($(QUIET),1)
        Q = @
else
        Q =
endif

ARCH	?= $(shell uname -m)

OS			= LINUX

CC			= gcc
ifeq ($(ARCH),i686)
CC			+= -m32
endif
AR                      = ar
RANLIB                  = ranlib
CFLAGS			= -L.
INCS			= -I.

LIBS			= lib${BASENAME}.a lib${BASENAME}.so

ifeq ($(DEBUG),1)
CFLAGS			+= -Wall -Wno-unused -g
else
CFLAGS			+= -O2
endif
SRC			= ${BASENAME}Lib.c
HDRS			= $(SRC:.c=.h)
OBJ			= $(SRC:.c=.o)
DEPS			= $(SRC:.c=.d)

all: echoarch ${LIBS}

%.o: %.c
	@echo " CC     $@"
	${Q}$(CC) $(CFLAGS) $(INCS) -c -o $@ $<

%.so: $(SRC)
	@echo " CC     $@"
	${Q}$(CC) -fpic -shared $(CFLAGS) $(INCS) -o $(@:%.a=%.so) $<

%.a: $(OBJ)
	@echo " AR     $@"
	${Q}$(AR) ru $@ $<
	@echo " RANLIB $@"
	${Q}$(RANLIB) $@

install: $(LIBS)
	@echo " CP     $< -> $(INSTALL_LIB)"
	${Q}mkdir -p $(INSTALL_LIB)
	${Q}cp $(PWD)/$< $(INSTALL_LIB)/$<
	@echo " CP     $(<:%.a=%.so) -> $(INSTALL_LIB)"
	${Q}cp $(PWD)/$(<:%.a=%.so) $(INSTALL_LIB)/$(<:%.a=%.so)
	@echo " CP     ${HDRS} -> $(INSTALL_INC)"
	${Q}mkdir -p $(INSTALL_INC)
	${Q}cp ${HDRS} $(INSTALL_INC)

%.d: %.c
	@echo " DEP    $@"
	@set -e; rm -f $@; \
	$(CC) -MM -shared $(INCS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(DEPS)

clean:
	@echo " CLEAN"
	${Q}rm -f ${OBJ} ${LIBS} ${DEPS}

echoarch:
	@echo "Make for $(OS)-$(ARCH)"

.PHONY: clean echoarch
