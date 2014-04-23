#
# Execute 'make' to create prucode.bin and mytest
# Other options:
# make clean
# make all
# make pru
# make project
# make prucode
# make clean
#

pru = prucode
project = ledtest

LIB_PATH = .
LIBRARIES = pthread prussdrv
INCLUDES = -I. ${LIB_PATH}

SOURCES =  ledtest.cpp

EXTRA_DEFINE =
CCCFLAGS = $(EXTRA_DEFINE)
CC = gcc
CFLAGS = $(EXTRA_DEFINE)
PASM = pasm

all : $(pru) $(project)
pru : $(pru)
project: $(project)

$(project) : $(project:%=%.cpp)
	$(CC) $(CFLAGS) -c -o $@.o $@.cpp
	$(CC) $@.o $(LIB_PATH:%=-L%) $(LIBRARIES:%=-l%) -o $@

clean :
	rm -rf *.o *.bin $(project) core *~

$(pru) : $(pru:%=%.p)
	$(PASM) -b $@.p

.SUFFIXES: .c.d

%.d: %.c
	$(SHELL) -ec "$(CC) -M $(CPPFLAGS) $< | sed 's/$*\\.o[ :]*/$@ &/g' > $@" -include $(SOURCES:.c=.d)

