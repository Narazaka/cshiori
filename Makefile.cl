CC = cl
CFLAGS = /Ox /GL
LDFLAGS = /MT
all:
	@echo shiori/shiolink
shiori: shiori.c
	$(CC) /o shiori.dll $< $(CFLAGS) $(LDFLAGS) /LD
shiolink: shiolink.c
	$(CC) /o shiori.exe $< $(CFLAGS) $(LDFLAGS)
doc: $(wildcard *.h)
	doxygen
