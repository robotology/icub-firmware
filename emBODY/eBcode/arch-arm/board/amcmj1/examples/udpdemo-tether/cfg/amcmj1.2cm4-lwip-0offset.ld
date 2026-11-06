/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 */

/*
 * GNU linker script (for ld.lld of the Arm Toolchain for Embedded, ATfE) of the CM4 of the amcmj1 (STM32H747).
 * It is the equivalent of amcmj1.2cm4-lwip-0offset.sct, the scatter file used by armlink (AC6):
 *
 * - FLASH: second bank @ 0x08100000 for 512k. the vector table (section RESET) comes first.
 * - RAM:   SRAM1 + SRAM2 @ 0x10000000 for 256k minus a hole of 64 bytes on top (0x1003FFC0), kept for the IPC.
 *          the linker places in here all the data, as .ANY (+RW +ZI) does in the scatter file. the stack and the heap
 *          are the arrays of the startup (sections .stack.amcmj1 and .heap.amcmj1), so their size is set in the startup.
 * - lwIP heap: no region. lwipopts.h does not define LWIP_RAM_HEAP_POINTER, so the heap of lwIP is its array ram_heap,
 *   which goes in RAM w/ the other data. a region @ 0x30004000 would overlap RAM: for the CM4, 0x10000000 is an alias of
 *   the SRAM1 of D2 @ 0x30000000, so 0x30004000 is the same memory as 0x10004000.
 * - ETH DMA: descriptors and RX pool @ 0x30040000 (SRAM3 of D2), in the sections that the ETH driver names.
 *
 * It also defines the symbols that the startup (stm32hal.h7.startup.amcmj1.c, ATfE branch) and picolibc need:
 * __data_source / __data_start / __data_size, __bss_start / __bss_size, __tls_base (picolibc keeps errno in thread
 * local storage), __heap_start / __heap_end (used by malloc() via sbrk()), __stack, and the zero-initialised regions
 * of the ETH DMA (__eth_dma_start / __eth_dma_end), which armlink zeroes by itself.
 *
 * The TLS part follows picolibc.ld of ATfE: .tdata right after .data (so it is copied with .data) and .tbss right
 * before .bss (so it is zeroed with .bss).
 */

ENTRY(Reset_Handler)

MEMORY
{
    FLASH       (rx)  : ORIGIN = 0x08100000, LENGTH = 0x00080000
    RAM         (rwx) : ORIGIN = 0x10000000, LENGTH = 0x0003FFC0
    ETH_RXDSC   (rw)  : ORIGIN = 0x30040000, LENGTH = 0x00000080
    ETH_TXDSC   (rw)  : ORIGIN = 0x30040080, LENGTH = 0x00000080
    ETH_RXPOOL  (rw)  : ORIGIN = 0x30040100, LENGTH = 0x00003F00
}

SECTIONS
{
    /* -- flash */

    .text :
    {
        KEEP(*(RESET))
        *(.text .text.*)
        *(.glue_7 .glue_7t .vfp11_veneer .v4_bx)
        KEEP(*(.init .fini))

        /* the constructors of the static objects (C++ dynamic initialisation). __libc_init_array() of picolibc calls
           the functions from __bothinit_array_start to __bothinit_array_end, i.e. .preinit_array and .init_array:
           these two symbols are needed (picolibc refers to them as weak, so without them the link does not fail
           but no constructor runs) */
        . = ALIGN(4);
        PROVIDE_HIDDEN(__bothinit_array_start = .);
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN(__preinit_array_end = .);
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
        KEEP(*(.init_array .ctors))
        PROVIDE_HIDDEN(__init_array_end = .);
        PROVIDE_HIDDEN(__bothinit_array_end = .);
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
        KEEP(*(.fini_array .dtors))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } > FLASH

    .rodata :
    {
        *(.rodata .rodata.*)
        . = ALIGN(8);
    } > FLASH

    /* unwind tables: the code is built with -fno-exceptions, but the C++ library may still bring some entries */
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH

    /* -- ram: data copied from flash at startup */

    .data :
    {
        *(.data .data.*)
        . = ALIGN(8);
    } > RAM AT > FLASH

    .tdata : ALIGN(8)
    {
        *(.tdata .tdata.*)
        . = ALIGN(8);
    } > RAM AT > FLASH

    __data_start  = ADDR(.data);
    __data_source = LOADADDR(.data);
    __data_end    = ADDR(.tdata) + SIZEOF(.tdata);
    __data_size   = __data_end - __data_start;

    /* -- ram: data zeroed at startup */

    .tbss (NOLOAD) : ALIGN(8)
    {
        *(.tbss .tbss.*)
        . = ALIGN(8);
    } > RAM

    .bss (NOLOAD) :
    {
        *(.bss .bss.* COMMON)
        . = ALIGN(8);
    } > RAM

    __bss_start = ADDR(.tbss);
    __bss_end   = ADDR(.bss) + SIZEOF(.bss);
    __bss_size  = __bss_end - __bss_start;

    /* thread local storage of picolibc (errno): one block, the one of the data above. the startup calls _set_tls() */
    __tls_align = MAX(ALIGNOF(.tdata), ALIGNOF(.tbss));
    __tls_base  = SIZEOF(.tdata) ? ADDR(.tdata) : ADDR(.tbss);
    __arm32_tls_tcb_offset = MAX(8, __tls_align);

    /* -- ram: heap and stack of the startup. not zeroed, as nobody needs it */

    .heap (NOLOAD) : ALIGN(8)
    {
        __heap_start = .;
        KEEP(*(.heap.amcmj1))
        __heap_end = .;
    } > RAM

    .stack (NOLOAD) : ALIGN(8)
    {
        __stack_bottom = .;
        KEEP(*(.stack.amcmj1))
        __stack = .;
    } > RAM

    __end = .;
    PROVIDE(end = .);
    PROVIDE(_end = .);

    /* -- other rams */

    .eth_rxdsc (NOLOAD) :
    {
        *(.RxDescripSection)
    } > ETH_RXDSC

    .eth_txdsc (NOLOAD) :
    {
        *(.TxDescripSection)
    } > ETH_TXDSC

    .eth_rxpool (NOLOAD) :
    {
        *(.Rx_PoolSection)
        . = ALIGN(8);       /* so that the startup zeroes it all with words */
    } > ETH_RXPOOL

    __eth_dma_start = ADDR(.eth_rxdsc);
    __eth_dma_end   = ADDR(.eth_rxpool) + SIZEOF(.eth_rxpool);

    /* -- not loaded */

    /DISCARD/ :
    {
        *(.note .note.*)
        *(.eh_frame .eh_frame.*)
    }

    .ARM.attributes 0 : { *(.ARM.attributes) }
}
