CC = tcc/tcc
CFLAGS = -O3 -DNDEBUG -Wall
LDFLAGS = 
all:
	@echo shiori/shiolink/run
shiori: shiori.c
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -o shiori.dll $<
shiolink: shiolink.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o shiori.exe $<
run: shiolink.c
	$(CC) $(CFLAGS) $(LDFLAGS) -run $<
doc: $(wildcard *.h)
	doxygen
