
/* AUTO-GENERATED by gen_isr_tables.py, do not edit! */

#include <toolchain.h>
#include <linker/sections.h>
#include <sw_isr_table.h>
#include <arch/cpu.h>

#if defined(CONFIG_GEN_SW_ISR_TABLE) && defined(CONFIG_GEN_IRQ_VECTOR_TABLE)
#define ISR_WRAPPER ((u32_t)&_isr_wrapper)
#else
#define ISR_WRAPPER NULL
#endif

u32_t __irq_vector_table _irq_vector_table[39] = {
	ISR_WRAPPER,
	110227,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
	ISR_WRAPPER,
};
struct _isr_table_entry __sw_isr_table _sw_isr_table[39] = {
	{(void *)0x0, (void *)0x6071},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)0x2ccb1},
	{(void *)0x2000e1d8, (void *)0x2cacd},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)0x1ae59},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x20002f4c, (void *)0x2d34d},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)0x61ad},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)0x1ae8d},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
	{(void *)0x0, (void *)&_irq_spurious},
};
