#include "arch/x86_64/gdt.h"
#include "arch/x86_64/mmu.h"
#include "kernel.h"
#include "sizes.h"
#include "multiboot2.h"
#include "arch/x86_64/msr.h"

/ First define a couple variables for the multiboot header
.SET HEADER_LENGTH, header_end - header_start
.SET CHECKSUM, -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + HEADER_LENGTH)
.SET MEMINFO 1 << 1

/ Define the multiboot header itself. It stores:
/  - the magic number 0x1BADB002
/  - flags - requests or requires of a boot loader. 
/    Bits 0-15 indicate requirements. If the bootloader doesn't understand
/    them, the OS image fails to load.
/    Bits 16-31 indicate optional features. Not understanding them doesn't prevent loading the image.
/    Bit 0 says that all boot modules lodaded along with the OS should be aligned on 4KB page boundaries.
/    Bit 1 says that information on available memory via at least the 'mem_*' fields of the multiboot information stucture must me included.
/    Bit 2 says that information about the video mode table must be available to the kernel.
/    Bit 16 says that fields in the header at offsets 12-28 are valid and the bootloader should use them instead of
/    the fields in the actual executable header to calculate where to load the OS image.
/  - checksum - a value which, when added to the magic number and flags, should result in 0
.section .multiboot
header_start:
    .long MULTIBOOT2_HEADER_MAGIC
    .long MULTIBOOT_ARCHITECTURE_I386
    .long HEADER_LENGTH
    .long CHECKSUM

    // multiboot tags go here

    .short MULTIBOOT_HEADER_TAG_END
    .short 0    // flags, none set
    .long 8     // size, including itself (short + short + long)
header_end:

/ Start of 32b code
.code32

/ Define global symbols. The first argument is the symbol itself, the second is its value,
/ the third (optional) is its alignment
.section .bss
.comm pml4, PML4_SIZE, PML4_ALIGNMENT
.comm low_pdpt, PDPT_SIZE, PDPT_ALIGNMENT
.comm high_pdpt, PDPT_SIZE, PDPT_ALIGNMENT
.comm low_page_directory_table, PAGE_DIRECTORY_SIZE, PAGE_DIRECTORY_ALIGNMENT
.comm high_page_directory_table, PAGE_DIRECTORY_SIZE, PAGE_DIRECTORY_ALIGNMENT
.comm tmp_stack, KERNEL_BOOT_STACK_SIZE, KERNEL_BOOT_STACK_ALIGNMENT

.data
/ Define the GDT. The first entry in the table is a NULL entry - all zeroes.
/ The second is the kernel. It sets the flag to say it's 64b, sets the access to say
/ that it's present (loaded), has ring 0 privilege and is executable.
.align GDT_TABLE_ALIGNMENT
gdt_table:
        .8byte GDT_FIRST_ENTRY
        .8byte GDT_KERNEL_ENTRY

/ Since some of the table remains empty, this skips the empty part of the table (to leave it free?)
gdt_table_end:
        .skip (GDT_TABLE_SIZE - (gdt_table_end - gdt_table))

/ This is the GDT descriptor. It holds the size of the table (- 1) and the linear address of the table itself.
gdt_ptr:
         .short GDT_TABLE_SIZE - 1
         .long gdt_table

/ Next is the text segment and the 32b start function.
.section .text
.global _start
.type _start, @function
_start:
        / Find out the actual size of memory
        /call do_e820



        / Save 2 x kernel boot stack size in the ESP register
        movl $tmp_stack + KERNEL_BOOT_STACK_SIZE, %esp

        / Save the low page directory pointer table (pdpt) size in EAX.
        / Then, OR it with whether the MMU is present and writeable (both 1).
        / Then move this result to the PML4 table at offset of the physical start of kernel * the size of a PML4 entry
        movl $low_pdpt, %eax
        or $(MMU_PRESENT | MMU_WRITABLE), %eax
        movl %eax, pml4 + (PML4_ADDR_TO_ENTRY_INDEX(KERNEL_PHYSICAL_START) * PML4_ENTRY_SIZE)

        / Save the high PDPT size in EAX.
        / OR the size with MMU flags.
        / Move the result to the PML4 table with offset of virtual start of kernel * size of a PML4 entry
        movl $high_pdpt, %eax
        or $(MMU_PRESENT | MMU_WRITABLE), %eax
        movl %eax, pml4 + (PML4_ADDR_TO_ENTRY_INDEX(KERNEL_VIRTUAL_START) * PML4_ENTRY_SIZE)

        / Save the size of a low page directory table (PDT) in EAX.
        / OR it with MMU flags
        / Move the result to the low PDPT at offset of kernel's physical start * PDPT entry size
        movl $low_page_directory_table, %eax
        or $(MMU_PRESENT | MMU_WRITABLE), %eax
        movl %eax, low_pdpt + (PDPT_ADDR_TO_ENTRY_INDEX(KERNEL_PHYSICAL_START) * PDPT_ENTRY_SIZE)

        / Save the size of high PDT in EAX.
        / OR it with MMU flags.
        / Move the result to the high PDPT at offset of kernel's virtual start * PDPT entry size
        movl $high_page_directory_table, %eax
        or $(MMU_PRESENT | MMU_WRITABLE), %eax
        movl %eax, high_pdpt + (PDPT_ADDR_TO_ENTRY_INDEX(KERNEL_VIRTUAL_START) * PDPT_ENTRY_SIZE)

        / Reset the ECX register
        mov $0, %ecx

        / Store the physical end of the kernel in the ESI register. ESI is a source register.
        / Shift the kernel end address by two megabytes to the right (make it smaller?).
        / Add 1 to the resulting address.
        movl $_kernel_physical_end, %esi
        shrl $TWO_MEGABYTES_SHIFT, %esi
        addl $1, %esi

do_e820:
        mov $0x8004, %di
        movl $0, %ebx
        movw $0, %bp
        movl $0x534D4150, %edx
        movl $0xE820, %eax
        movl $1, %es:20(%di)
        movl $24, %ecx
        int $0x15

        / Carry set on the first call means unsupported function (?)
        jc e820_failed

        / Reset the EDX register to make sure it's not trashed.
        / On success, EAX will also be set to SMAP. If it's a failure, just go to the
        / failure handling method.
        movl $0x534D4150, %edx
        cmpl %edx, %eax
        jne e820_failed
        
        / If it's a sucess, do a bitwise AND of EBX with itself. If the result is 0,
        / it means that the list is only 1 entry long and is worthless, so we can quit again (?).
        / Otherwise, do stuff.
        testl %ebx, %ebx
        je e820_failed
        jmp e820_jumpin


/ https://github.com/cirosantilli/x86-bare-metal-examples/blob/master/bios_detect_memory.S
e820_failed:
        stc
        ret

e820_jumpin:
        / Skip if the entry is 0 (stored in the ECX register)
        jcxz e820_skip

        / If this is a 24 byte long responce for ACPI, read it?
        cmpb $20, %cl
        jbe e820_notext

        / Not sure what this does
        testb $1, %es:29(%di)
        je e820_skip

e820_skip:
        / If EBX becomes 0, the list is complete.
        testl %ebx, %ebx
        jne e820_step

e820_step:
        movl $0xE820, %eax
        movl $1, %es:20(%di)
        movl $24, %ecx
        int $0x15

        / If the end of the list is reached, finish up
        jc e820_finish

        / Otherwise, restore the EDX register again and keep going
        movl $0x534D4150, %edx

e820_finish:
        / TODO: Store the entry count (from BP)
        / Clear the carry and return
        clc
        ret

e820_notext:
        mov %ecx, %es:8(%di)
        or %ecx, %es:12(%di)

        / If the length is 0, skip the entry
        jz e820_skip

        / If this is a good entry, increment the count and move to the next entry
        incw %bp
        addw $24, %di

/ Set up the page directory table
page_directory_table_loop:
        / Save the size of 2MB in the EAX register
        / It's then multiplied with the counter (which should be 0 during the first run), the lower order 32 bits are stored in EAX
        / The lower order 32 bits are ORed with MMU flags
        / The result is stored in the low PDT and high PDT
        movl $TWO_MEGABYTES, %eax
        mul %ecx
        or $(MMU_PRESENT | MMU_WRITABLE | MMU_PDE_TWO_MB), %eax
        / Move to low PDT at offset 0 (the empty argument), index stored in ECX, size of PDE size
        movl %eax, low_page_directory_table(, %ecx, PAGE_DIRECTORY_ENTRY_SIZE)
        movl %eax, high_page_directory_table(, %ecx, PAGE_DIRECTORY_ENTRY_SIZE)

        / The counter is increased. If the counter is not yet the same as the ESI register
        / (which stores the size of the kernel, essentially), it loops again
        inc %ecx
        cmp %esi, %ecx
        jne page_directory_table_loop  // if not equal redo loop

        / If we've mapped the entire kernel, we store the addres of PML4 in EAX
        / Then move EAX to CR3. The address of the page directory (?) is saved in the control register
        movl $pml4, %eax
        movl %eax, %cr3

        / Save the kernel cr4 flag in EAX. It sets bit 5 which enables extending physical addresses to 36b in the page tables
        movl $KERNEL_CR4, %eax
        movl %eax, %cr4

        / MSR - model specific register. Used for debugging, monitoring and toggling CPU features.
        / The EFER register controls certain long mode features.
        / Bit 8 is enabled - long mode enabled (LME)
        / The MSR register is read
        / It's them ORed with a long mode enable MSR flag and written back.
        movl $MSR_EFER, %ecx
        rdmsr
        or $MSR_EFER_LME, %eax
        wrmsr

        / The CR0 register flag is moved into EAX.
        / It has bit 31 set - enable paging,
        / bit 4 set - aloowed to specify the extension type,
        / bit 0 set - enabled protected mode.
        / This value is saved in the CR0 register.
        movl $KERNEL_CR0, %eax
        movl %eax, %cr0

        / Saves the limit and base of our GDT in the GDT register
        lgdt gdt_ptr

        / Jumps to the 64b start function.
        / Kernel GDT entry * GDT entry size defines the code segment to switch to. It sets the CS (code segment) register.
        / _start64 is the address of the function we want to jump to.
        / We need this jump to activate protected mode, since the CS register needs to hold the selected for a code segment in the GDT
        ljmp $(KERNEL_GDT_ENTRY * GDT_ENTRY_SIZE), $_start64

        / Safety net - shouldn't reach this code
        cli
        hlt

/ Start of 64b code.
.code64

/ Define a global function to load the idt
.global idt_load
idt_load:
        lidt idtp
        ret

/ Define global interrupt service routine (ISR) functions
.global _isr0
.global _isr1
.global _isr2
.global _isr3
.global _isr4
.global _isr5
.global _isr6
.global _isr7
.global _isr8
.global _isr9
.global _isr10
.global _isr11
.global _isr12
.global _isr13
.global _isr14
.global _isr15
.global _isr16
.global _isr17
.global _isr18
.global _isr19
.global _isr20
.global _isr21
.global _isr22
.global _isr23
.global _isr24
.global _isr25
.global _isr26
.global _isr27
.global _isr28
.global _isr29
.global _isr30
.global _isr31

/ Now define the actual contents of each of them
/ Divide by 0 exception
_isr0:
        / Disable interrupts, push dummy error code
        / on the stack, push the interrupr number and jump to a common handler.
        cli
        push $0
        push $0
        jmp isr_common_stub

/ Debug exception
_isr1:
        cli
        push $0
        push $1
        jmp isr_common_stub

/ Non-maskable interrupt exception
_isr2:
        cli
        push $0
        push $2
        jmp isr_common_stub

/ Breakpoint exception
_isr3:
        cli
        push $0
        push $3
        jmp isr_common_stub

/ Into detected overflow exception
_isr4:
        cli
        push $0
        push $4
        jmp isr_common_stub

/ Out of bounds exception
_isr5:
        cli
        push $0
        push $5
        jmp isr_common_stub

/ Invalid opcode exception
_isr6:
        cli
        push $0
        push $6
        jmp isr_common_stub

/ No coprocessor exception
_isr7:
        cli
        push $0
        push $7
        jmp isr_common_stub

/ Double fault exception
_isr8:
        cli
        / We don't push an error code because it's already pushed
        push $8
        movq $8, %rsi
        jmp isr_common_stub

/ Coprocessor segment overrun exception
_isr9:
        cli
        push $0
        push $9
        jmp isr_common_stub

/ Bad TSS exception
_isr10:
        cli
        / We don't push an error code because it's already pushed
        push $10
        push $10
        jmp isr_common_stub

/ Segment not present exception
_isr11:
        cli
        / We don't push an error code because it's already pushed
        push $11
        jmp isr_common_stub

/ Stack fault exception
_isr12:
        cli
        / We don't push an error code because it's already pushed
        push $12
        jmp isr_common_stub

/ General protection fault exception
_isr13:
        cli
        / We don't push an error code because it's already pushed
        push $13
        jmp isr_common_stub

/ Page fault exception
_isr14:
        cli
        / We don't push an error code because it's already pushed
        push $14
        jmp isr_common_stub

/ Unknown interrupt exception
_isr15:
        cli
        push $0
        push $15
        jmp isr_common_stub

/ Coprocessor fault exception
_isr16:
        cli
        push $0
        push $16
        jmp isr_common_stub

/ Alignment check exception
_isr17:
        cli
        push $0
        push $17
        jmp isr_common_stub

/ Machine check exception
_isr18:
        cli
        push $0
        push $18
        jmp isr_common_stub

/ Reserved exception
_isr19:
        cli
        push $0
        push $19
        jmp isr_common_stub

/ Reserved exception
_isr20:
        cli
        push $0
        push $20
        jmp isr_common_stub

/ Reserved exception
_isr21:
        cli
        push $0
        push $21
        jmp isr_common_stub

/ Reserved exception
_isr22:
        cli
        push $0
        push $22
        jmp isr_common_stub

/ Reserved exception
_isr23:
        cli
        push $0
        push $23
        jmp isr_common_stub

/ Reserved exception
_isr24:
        cli
        push $0
        push $24
        jmp isr_common_stub

/ Reserved exception
_isr25:
        cli
        push $0
        push $25
        jmp isr_common_stub

/ Reserved exception
_isr26:
        cli
        push $0
        push $26
        jmp isr_common_stub

/ Reserved exception
_isr27:
        cli
        push $0
        push $27
        jmp isr_common_stub

/ Reserved exception
_isr28:
        cli
        push $0
        push $28
        jmp isr_common_stub

/ Reserved exception
_isr29:
        cli
        push $0
        push $29
        jmp isr_common_stub

/ Reserved exception
_isr30:
        cli
        push $0
        push $30
        jmp isr_common_stub

/ Reserved exception
_isr31:
        cli
        push $0
        push $31
        jmp isr_common_stub

/ Define an external C-function
/external fault_handler

/ Define the common ISR stub. It saves the processor state, sets up for
/ kernel mode segments, calls the C fault handler and restores the stack frame
isr_common_stub:
        / Registers r9, r8, rdx, rcx, rdi, rsi will be set as the first six arguments
        / of the C function, so we don't need to push them on the stack. Only push the
        / remaining 3 that could be srambled
        push %r11
        push %r10
        push %rax

        movw $0, %ax   //kernel data segment descriptor?
        movw %ax, %ds
        movw %ax, %es
        movw %ax, %fs
        movw %ax, %gs

        call fault_handler

        pop %rbx
        movw %bx, %ds
        movw %bx, %es
        movw %bx, %fs
        movw %bx, %gs

        pop %rax
        pop %r10
        pop %r11

        add $8, %rsp
        sti
        iret

/ Define the IRQs
_irq0:
        cli
        push $0
        push $32
        jmp irq_common_stub
_irq1:
        cli
        push $0
        push $33
        jmp irq_common_stub
_irq2:
        cli
        push $0
        push $34
        jmp irq_common_stub
_irq3:
        cli
        push $0
        push $35
        jmp irq_common_stub
_irq4:
        cli
        push $0
        push $36
        jmp irq_common_stub
_irq5:
        cli
        push $0
        push $37
        jmp irq_common_stub
_irq6:
        cli
        push $0
        push $38
        jmp irq_common_stub
_irq7:
        cli
        push $0
        push $39
        jmp irq_common_stub
_irq8:
        cli
        push $0
        push $40
        jmp irq_common_stub
_irq9:
        cli
        push $0
        push $41
        jmp irq_common_stub
_irq10:
        cli
        push $0
        push $42
        jmp irq_common_stub
_irq11:
        cli
        push $0
        push $43
        jmp irq_common_stub
_irq12:
        cli
        push $0
        push $44
        jmp irq_common_stub
_irq13:
        cli
        push $0
        push $45
        jmp irq_common_stub
_irq14:
        cli
        push $0
        push $46
        jmp irq_common_stub
_irq15:
        cli
        push $0
        push $47
        jmp irq_common_stub

irq_common_stub:
        / Replacement for pusha - save some registers on the stack
        push %rax
        push %rcx
        push %rdx
        push %rbx
        push %rbp
        push %rsi
        push %rdi

        movw $0, %ax   //kernel data segment descriptor?
        movw %ax, %ds
        movw %ax, %es
        movw %ax, %fs
        movw %ax, %gs

        /call irq_handler

        pop %rbx
        movw %bx, %ds
        movw %bx, %es
        movw %bx, %fs
        movw %bx, %gs

        pop %rdi
        pop %rsi
        pop %rbp
        pop %rbx
        pop %rdx
        pop %rcx
        pop %rax

        add $8, %rsp
        sti
        iret

/ The 64b start function.
.global _start64
.type _start64, @function
_start64:
        / Clear all the segment selectors
        movw $0, %ax
        movw %ax, %ds
        movw %ax, %es
        movw %ax, %fs
        movw %ax, %gs
        movw %ax, %ss

        //set the parameter for Kernel_Main
        movq %rbx, %rdi

        / Go into C code
        call Kernel_Main

        / Safety net - shouldn't reach this code
        cli
        hlt

/ Not sure what this is
1:
        jmp 1b