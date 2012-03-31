GCC = nspire-gcc
LD = $(GCC)
LIBS = -lm
GCCFLAGS = -O3 -Wall -W -mcpu=arm926ej-s
LDFLAGS = -nostartfiles  -T "ldscript" -static
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
EXE = startup.bin
OBJS = startup.o

DISTDIR = .
vpath %.tns $(DISTDIR)

all: $(EXE)

%.o: %.S
	$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $(@:.bin=.elf)
	$(OBJCOPY) -O binary $(@:.bin=.elf) $@

clean:
	rm -f *.o *.elf
	rm -f $(EXE)
