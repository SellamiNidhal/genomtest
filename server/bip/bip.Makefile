<'
#
# Copyright (c) 2012 LAAS/CNRS
# All rights reserved.
#
# Permission to use, copy, modify, and distribute this software for any purpose
# with or without   fee is hereby granted, provided   that the above  copyright
# notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS  SOFTWARE INCLUDING ALL  IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR  BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR  ANY DAMAGES WHATSOEVER RESULTING  FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION,   ARISING OUT OF OR IN    CONNECTION WITH THE USE   OR
# PERFORMANCE OF THIS SOFTWARE.
#
#                                            Anthony Mallet on Tue Jan  3 2012
#

# check arguments
if {[llength $argv] != 2} { error "expected arguments: types input" }
lassign $argv types input

# compute handy shortcuts
set component [dotgen component]
set comp [$component name]
'>


# This is very crude for now and use cmake/make to produce the library
# and Deploy stuff I need to link with.  I will improve this later and get the
# compilation code properly done in automake integrated in this Makefile.

export BIP2_HOME=/home/nidhal/work/bip/rt-multi/BIP.linux.x86

CFLAGS+=-I/home/nidhal/openrobots/include
all:  <"$comp">.bip.o

<"$comp">.bip.C: <"$comp">.bip
	${BIP2_HOME}/bin/bipc --genC-multi -f <"$comp">.bip

<"$comp">.bip.o: <"$comp">.bip.C
	make -f <"$comp">.bip.mk [debug=yes] <"$comp">.bip.o
clean:
	rm -rf <"$comp">.bip.C <"$comp">.bip.h  <"$comp">.bip.mk <"$comp">.bip.o



# old version for the new syntaxe...
# mkdir -p output
# bipc.sh -I . -p <"$comp">Package -d "<"$comp">Compound()"  --gencpp-output output   --gencpp-cc-I ext-cpp
# (cd output; cmake . ; make)
