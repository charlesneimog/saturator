NAME=saturator
CSYM=saturator

STATIC_LIBRARIES=saturator_vu~.o saturator_thresh~.o saturator_td.o saturator~.o
DINAMIC_LIBRARES=
HELP_PD=help-saturator~.pd

UNIVERSAL=-arch i386 -arch x86_64

current: pd_linux

# ----------------------- NT -----------------------
#pd_nt: $(NAME).dll

#.SUFFIXES: .dll

#NTCFLAGS = -DPD -DNT -O3 -funroll-loops -fomit-frame-pointer \
#    -Wall -W -Wno-shadow -Wstrict-prototypes \
#    -Wno-unused -Wno-parentheses -Wno-switch


#NTINCLUDE =

#.c.dll:
#    gcc $(NTCFLAGS) $(NTINCLUDE) -o $*.o -c $*.c
#    gcc $(NTCFLAGS) $(NTINCLUDE) -o saturator~.o -c saturator~.c
#    gcc $(NTCFLAGS) $(NTINCLUDE) -o saturator_td.o -c saturator_td.c
#    gcc $(NTCFLAGS) $(NTINCLUDE) -o saturator_vu~.o -c saturator_vu~.c
#    gcc $(NTCFLAGS) $(NTINCLUDE) -o saturator_thresh~.o -c saturator_thresh~.c
#    gcc  $(DINAMIC_LIBRARES) -Lpathtopdlib -lpd -shared -o $*.dll $(STATIC_LIBRARIES) $*.o -W1
#    strip --strip-unneeded $*.dll



# ----------------------- IRIX 5.x -----------------------

pd_irix5: $(NAME).pd_irix5

.SUFFIXES: .pd_irix5

SGICFLAGS5 = -o32 -DPD -DUNIX -DIRIX -O2

SGIINCLUDE =  -I../../src

.c.pd_irix5:
	cc $(SGICFLAGS5) $(SGIINCLUDE) -o $*.o -c $*.c
	ld -elf -shared -rdata_shared -o $*.pd_irix5 $*.o
	rm $*.o

# ----------------------- IRIX 6.x -----------------------

pd_irix6: $(NAME).pd_irix6

.SUFFIXES: .pd_irix6

SGICFLAGS6 = -n32 -DPD -DUNIX -DIRIX -DN32 -woff 1080,1064,1185 \
	-OPT:roundoff=3 -OPT:IEEE_arithmetic=3 -OPT:cray_ivdep=true \
	-Ofast=ip32

.c.pd_irix6:
	cc $(SGICFLAGS6) $(SGIINCLUDE) -o $*.o -c $*.c
	ld -n32 -IPA -shared -rdata_shared -o $*.pd_irix6 $*.o
	rm $*.o

# ----------------------- LINUX i386 -----------------------

pd_linux: $(NAME).pd_linux

.SUFFIXES: .pd_linux

LINUXCFLAGS = -DPD  -DUNIX -DICECAST -O3 -funroll-loops -fomit-frame-pointer \
    -Wall -W -Wno-shadow -Wstrict-prototypes \
    -Wno-unused -Wno-parentheses -Wno-switch #-Werror

LINUXINCLUDE =

.c.pd_linux:
	cc $(LINUXCFLAGS) $(LINUXINCLUDE) -o $*.o -c $*.c
	cc $(LINUXCFLAGS) $(LINUXINCLUDE) -o saturator~.o -c saturator~.c
	cc $(LINUXCFLAGS) $(LINUXINCLUDE) -o saturator_td.o -c saturator_td.c
	cc $(LINUXCFLAGS) $(LINUXINCLUDE) -o saturator_vu~.o -c saturator_vu~.c
	cc $(LINUXCFLAGS) $(LINUXINCLUDE) -o saturator_thresh~.o -c saturator_thresh~.c
	ld -export_dynamic   $(DINAMIC_LIBRARES) -shared -o $*.pd_linux $(STATIC_LIBRARIES) $*.o -lc -lm
	strip --strip-unneeded $*.pd_linux

# ----------------------- Mac OSX -----------------------

pd_darwin: $(NAME).pd_darwin

.SUFFIXES: .pd_darwin

DARWINCFLAGS = -DPD -DDARWIN -O2 -Wall -W -Wshadow -Wstrict-prototypes \
    -Wno-unused -Wno-parentheses -Wno-switch $(UNIVERSAL)
DARWININCLUDE=$(LINUXINCLUDE) -I/sw/include

DARWIN_LIBS=-L/sw/lib $(UNIVERSAL)


.c.pd_darwin:
	cc $(DARWINCFLAGS) $(DARWININCLUDE) -o $*.o -c $*.c
	cc $(DARWINCFLAGS) $(DARWININCLUDE) -o saturator~.o -c saturator~.c
	cc $(DARWINCFLAGS) $(DARWININCLUDE) -o saturator_td.o -c saturator_td.c
	cc $(DARWINCFLAGS) $(DARWININCLUDE) -o saturator_vu~.o -c saturator_vu~.c
	cc $(DARWINCFLAGS) $(DARWININCLUDE) -o saturator_thresh~.o -c saturator_thresh~.c
	cc -bundle -undefined suppress  -flat_namespace $(DARWIN_LIBS)  $(DINAMIC_LIBRARES) -o $*.pd_darwin $(STATIC_LIBRARIES) $*.o 


# ----------------------------------------------------------

all: current

install:
	cp help-*.pd ../../doc/5.reference

cp:
	rm -f *.o *.pd_* so_locations
	
clean:
	rm *.o
	rm $(NAME).pd*	
