#ifndef _ARCH_H_
#define _ARCH_H_

#include <sgos.h>
#include <gdt_const.h>

#define SYSTEM_INTERRUPT 0xA0
#define PAGEFAULT_INTERRUPT 14

#define PAGE_SIZE	4096
#define PAGE_SIZE_BITS 12
#define KERNEL_BASE	0xC0000000
#define PROC_PAGE_DIR_BASE	0xE0000000
#define PROC_PAGE_TABLE_MAP	0xBFC00000

// gdt 
// segment
typedef struct SEGMENT_DESC
{
	t_16	limit_low;
	t_16	base_low;
	t_8		base_mid;
	t_8		attr1;
	t_8		limit_high_attr2;
	t_8		base_high;
}SEGMENT_DESC;
// 门描述符
typedef struct GATE_DESC
{
	t_16	offset_low;
	t_16	selector;
	t_8		dcount;	
	t_8		attr;
	t_16	offset_high;
}GATE_DESC;
// GDT位置描述符
typedef struct GDT_ADDR {
	t_16	limit __attribute__((packed));
	t_32	addr __attribute__((packed));
}GDT_ADDR;

typedef struct TSS {
	t_32	back_link;
	t_32	esp0;	//kernel esp
	t_32	ss0;
	t_32	esp1;
	t_32	ss1;
	t_32	esp2;
	t_32	ss2;
	t_32	cr3;
	t_32	eip;
	t_32	flags;
	t_32	eax, ecx ,edx, ebx;
	t_32	esp;
	t_32	ebp;
	t_32	esi;
	t_32	edi;
	t_32	es;
	t_32	cs;
	t_32	ss;
	t_32	ds;
	t_32	fs;
	t_32	gs;
	t_32	ldt;
	t_16	trap;
	t_16	iobase;	
	/* I/O位图基址大于或等于TSS段界限，就表示没有I/O许可位图 */
	/*t_8	iomap[2];*/
}TSS;

typedef struct I386_REGISTERS
{
	t_32	gs, fs, es, ds;
	t_32	edi, esi, ebp, kesp, ebx, edx, ecx, eax;
	t_32	int_no, err_code;
	t_32	eip, cs, eflags, esp, ss;
}I386_REGISTERS;	//19*4=76 Bytes

//paging
typedef union PAGE_TABLE{
	t_32	v;
	struct{
		unsigned present:1;
		unsigned write:1;
		unsigned user:1;
		unsigned unused1:2;
		unsigned access:1;
		unsigned unused2:2;
		unsigned global:1;
		unsigned share:1;
		unsigned allocated:1;
		unsigned copy_on_write:1;
		unsigned phys_addr:20;
	}a;
}PAGE_TABLE, PAGE_DIR ;
#define P_PRESENT	(1<<0)	//页面存在
#define P_WRITE		(1<<1)	//页面可写
#define P_USER		(1<<2)	//页面为用户级
#define P_ACCESS	(1<<5)	//页面被访问过

// (端口, 数据)
#define out_byte_wait(port,data) \
__asm__ __volatile__ ( "out %%al , %%dx\n\tjmp 1f\n1:\tjmp 1f\n1:" : : "a"( data ) , "d"( port ) );
#define out_word_wait(port,data) \
__asm__ __volatile__ ( "out %%ax , %%dx\n\tjmp 1f\n1:\tjmp 1f\n1:" : : "a"( data ) , "d"( port ) );
#define out_byte(port,data) \
__asm__ __volatile__ ( "out %%al , %%dx" : : "a"( data ) , "d"( port ) );
#define out_word(port,data) \
__asm__ __volatile__ ( "out %%ax , %%dx" : : "a"( data ) , "d"( port ) );
#define out_dword(port,data) \
__asm__ __volatile__ ( "out %%eax , %%dx" : : "a"( data ) , "d"( port ) );
// port.c
t_16 in_word( t_16 port );
t_32 in_dword( t_16 port );
t_8 in_byte( t_16 port );

//gdt
void set_gate( int vector, uchar desc_type, void* handle );
void set_gdt_desc( int vector, uint base, uint limit, uint attribute );
void set_idt_desc( int vector, uchar desc_type, void* handler );
void set_ldt_desc( SEGMENT_DESC *desc, uint base, uint limit, uint attribute );
//isr
void isr_init();
void isr_dumpcpu( const I386_REGISTERS *r );
void isr_uninstall( int isr );
int isr_install( int isr, void (*handler)(int err_code, const I386_REGISTERS *r) );
//irq
void irq_init();
void irq_mask( int irq, int enabled );
void irq_uninstall( int irq );
int irq_install( int irq, void (*handler)() );
//syscall
int syscall_interrupt();
//init
int machine_init();
//page
int page_init( uint size );
void load_page_dir(uint phys_addr);
void dump_phys_pages();
void free_phys_page( uint addr );
uint get_phys_page();
void reflush_pages();
void load_page_dir(uint phys_addr);
//map
void map_pages( uint vir_addr, uint phys_addr, uint size, uint attr );
void unmap_pages( uint vir_addr, uint size );
//dir
uint get_page_dir();
void free_page_dir(uint addr);
//rtc
void rtc_init();

#endif
