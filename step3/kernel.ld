SECTIONS
{
 . = 0x00;
 .text : {
 build/exception.o(.text)
 build/startup.o(.text)
 build/irq.o(.text)
 build/*(.text)
 }
 . = ALIGN(4);
 .data : {
 build/*(.data)
 }
 . = ALIGN(4);
 _bss_start = .;
 .bss . : {
 build/*(.bss COMMON)
 }
 . = ALIGN(4);
 _bss_end = .;
 . = ALIGN(8);
 . = . + 0x1000;
 stack_top = .;
 irq_stack_top = .;
}