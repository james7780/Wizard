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
        ROM1 : ORIGIN = 53, LENGTH = 32K
	lyx_header (wxar) : ORIGIN = 0, LENGTH = 53
	/* exe */
	code (wxa!r) : ORIGIN = 0x200, LENGTH = 0x1000
	ram  : ORIGIN = 0x1000, LENGTH = 0x1000
}

SECTIONS {

    .lyx : {
     INCLUDE ml.enc.ld
     BYTE((CODE_SIZE+255)/256)
     } > lyx_header

CODE_SIZE = SIZEOF(.text);

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
