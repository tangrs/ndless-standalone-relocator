GCC = gcc
LD = gcc
GCCFLAGS = -I/opt/local/include/
LDFLAGS = -L/opt/local/lib/ -lelf
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
EXE = makeself

all: $(EXE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
	$(LD) $^ -o $(EXE) $(LDFLAGS)

clean:
	rm -f *.o $(EXE)
