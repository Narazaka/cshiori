CC = cl
CFLAGS = /Ox /GL
LDFLAGS = /MT
all:
	@echo shiori/shiolink
shiori: shiori.c
	$(CC) $(CFLAGS) $(LDFLAGS) /LD /o shiori.dll $<
shiolink: shiolink.c
	$(CC) $(CFLAGS) $(LDFLAGS) /o shiori.exe $<
doc: $(wildcard *.h)
	doxygen
