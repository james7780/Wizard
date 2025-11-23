__stack_size = 1k;

__rc0 = 0;
__rc1 = 1;
__rc2 = 2;
__rc3 = 3;
__rc4 = 4;
__rc5 = 5;
__rc6 = 6;
__rc7 = 7;
__rc8 = 8;
MEMORY {
       /* load */
        ROM1 : ORIGIN = 10, LENGTH = 32K
	bll_header (wxar) : ORIGIN = 0, LENGTH = 10
	/* exe */
	code (wxa!r) : ORIGIN = 0x200, LENGTH = 0x1000
	ram  : ORIGIN = 0x1000, LENGTH = 0x1000
}

SECTIONS {

   .bll : {
   	BYTE(0x80)
	BYTE(0x08)
	BYTE(ORIGIN(code)>>8)
	BYTE(ORIGIN(code) & 0xff)
	BYTE(CODE_SIZE >> 8)
	BYTE(CODE_SIZE & 0xff)
	BYTE(0x42)
	BYTE(0x53)
	BYTE(0x39)
	BYTE(0x33)
   } > bll_header

CODE_SIZE = SIZEOF(.text)+10;

 .text : {
      *(.init*)
      *(.text*)
     } > code AT > ROM1
  .bss (NOLOAD) : {
       *(.bss*)
       . += __stack_size;
       __stack = .;
  } > ram

}
