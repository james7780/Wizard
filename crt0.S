	.section .init.100,"axR",@progbits
	.globl _start
	.globl main
_start:
__do_init_stack:
  lda #mos16lo(__stack)
  sta __rc0
  lda #mos16hi(__stack)
  sta __rc1
  jmp main
