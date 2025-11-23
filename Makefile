# JH batch file to make "Wizard of WASD" for llvm-mos 22.3
# run setenv in parent folder before making

CFLAGS=
LDFLAGS=
LD=mos-lynx-bll-clang -mlto-zp=20 -Wl,-Map,$@.map
# NOTE: -mlto-zp=32 above prevents clang from putting const char arrays in ZP! 

all:	wizard.o

wizard.o : wizard.c lynxlib.c drawtext.c audio.c
	$(LD) wizard.c lynxlib.c drawtext.c audio.c -o wizard.o

# Use lyxass method to make .lnx file
wizard.lnx: wizard.o
	copy /b bll.enc + wizard.o wizard.lyx
	make_lnx wizard.lyx -b0 256K

run:
	C:\Emulators\Mednafen\mednafen.exe wizard.o

runx:
	C:\Emulators\Mednafen\mednafen.exe wizard.lnx

.PHONY: clean
clean:
	del -f *.lnx
	del -f *.lyx
	del -f *.elf
	del -f *.map
	del -f *.o


