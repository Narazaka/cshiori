CC = tcc/tcc
CFLAGS = -O3 -DNDEBUG -Wall
LDFLAGS = 
all:
	@echo shiori/shiolink/run
shiori: shiori.c
	$(CC) -o shiori.dll $< $(CFLAGS) $(LDFLAGS) -shared
shiolink: shiolink.c
	$(CC) -o shiori.exe $< $(CFLAGS) $(LDFLAGS)
run: shiolink.c
	$(CC) -run $< $(CFLAGS) $(LDFLAGS)
doc: $(wildcard *.h)
	doxygen
