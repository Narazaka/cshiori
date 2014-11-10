CC = cl
CFLAGS = /Ox /GL
LDFLAGS = /MT
all:
	@echo shiori/shiolink
shiori: shiori.c
	$(CC) $(CFLAGS) $(LDFLAGS) /LD /OUT:shiori.dll $<
shiolink: shiolink.c
	$(CC) $(CFLAGS) $(LDFLAGS) /OUT:shiori.exe $<
doc: $(wildcard *.h)
	doxygen
