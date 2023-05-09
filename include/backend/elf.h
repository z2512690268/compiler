#pragma once

#include <cstdint>
typedef uint32_t __u32;
typedef uint16_t __u16;
typedef uint64_t __u64;
typedef int32_t __s32;
typedef int16_t __s16;
typedef int64_t __s64;

/* 32-bit ELF base types. */
typedef __u32	Elf32_Addr;
typedef __u16	Elf32_Half;
typedef __u32	Elf32_Off;
typedef __s32	Elf32_Sword;
typedef __u32	Elf32_Word;

/* 64-bit ELF base types. */
typedef __u64	Elf64_Addr;
typedef __u16	Elf64_Half;
typedef __s16	Elf64_SHalf;
typedef __u64	Elf64_Off;
typedef __s32	Elf64_Sword;
typedef __u32	Elf64_Word;
typedef __u64	Elf64_Xword;
typedef __s64	Elf64_Sxword;

/* These constants are for the segment types stored in the image headers */
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6
#define PT_TLS     7               /* Thread local storage segment */
#define PT_LOOS    0x60000000      /* OS-specific */
#define PT_HIOS    0x6fffffff      /* OS-specific */
#define PT_LOPROC  0x70000000
#define PT_HIPROC  0x7fffffff
#define PT_GNU_EH_FRAME	(PT_LOOS + 0x474e550)
#define PT_GNU_STACK	(PT_LOOS + 0x474e551)
#define PT_GNU_RELRO	(PT_LOOS + 0x474e552)
#define PT_GNU_PROPERTY	(PT_LOOS + 0x474e553)


/* ARM MTE memory tag segment type */
#define PT_AARCH64_MEMTAG_MTE	(PT_LOPROC + 0x2)

/*
 * Extended Numbering
 *
 * If the real number of program header table entries is larger than
 * or equal to PN_XNUM(0xffff), it is set to sh_info field of the
 * section header at index 0, and PN_XNUM is set to e_phnum
 * field. Otherwise, the section header at index 0 is zero
 * initialized, if it exists.
 *
 * Specifications are available in:
 *
 * - Oracle: Linker and Libraries.
 *   Part No: 817–1984–19, August 2011.
 *   https://docs.oracle.com/cd/E18752_01/pdf/817-1984.pdf
 *
 * - System V ABI AMD64 Architecture Processor Supplement
 *   Draft Version 0.99.4,
 *   January 13, 2010.
 *   http://www.cs.washington.edu/education/courses/cse351/12wi/supp-docs/abi.pdf
 */
#define PN_XNUM 0xffff

/* These constants define the different elf file types */
#define ET_NONE   0
#define ET_REL    1
#define ET_EXEC   2
#define ET_DYN    3
#define ET_CORE   4
#define ET_LOPROC 0xff00
#define ET_HIPROC 0xffff

/* This is the info that is needed to parse the dynamic section of the file */
#define DT_NULL		0
#define DT_NEEDED	1
#define DT_PLTRELSZ	2
#define DT_PLTGOT	3
#define DT_HASH		4
#define DT_STRTAB	5
#define DT_SYMTAB	6
#define DT_RELA		7
#define DT_RELASZ	8
#define DT_RELAENT	9
#define DT_STRSZ	10
#define DT_SYMENT	11
#define DT_INIT		12
#define DT_FINI		13
#define DT_SONAME	14
#define DT_RPATH	15
#define DT_SYMBOLIC	16
#define DT_REL	        17
#define DT_RELSZ	18
#define DT_RELENT	19
#define DT_PLTREL	20
#define DT_DEBUG	21
#define DT_TEXTREL	22
#define DT_JMPREL	23
#define DT_ENCODING	32
#define OLD_DT_LOOS	0x60000000
#define DT_LOOS		0x6000000d
#define DT_HIOS		0x6ffff000
#define DT_VALRNGLO	0x6ffffd00
#define DT_VALRNGHI	0x6ffffdff
#define DT_ADDRRNGLO	0x6ffffe00
#define DT_ADDRRNGHI	0x6ffffeff
#define DT_VERSYM	0x6ffffff0
#define DT_RELACOUNT	0x6ffffff9
#define DT_RELCOUNT	0x6ffffffa
#define DT_FLAGS_1	0x6ffffffb
#define DT_VERDEF	0x6ffffffc
#define	DT_VERDEFNUM	0x6ffffffd
#define DT_VERNEED	0x6ffffffe
#define	DT_VERNEEDNUM	0x6fffffff
#define OLD_DT_HIOS     0x6fffffff
#define DT_LOPROC	0x70000000
#define DT_HIPROC	0x7fffffff

/* This info is needed when parsing the symbol table */
#define STB_LOCAL  0
#define STB_GLOBAL 1
#define STB_WEAK   2

#define STT_NOTYPE  0
#define STT_OBJECT  1
#define STT_FUNC    2
#define STT_SECTION 3
#define STT_FILE    4
#define STT_COMMON  5
#define STT_TLS     6

#define ELF_ST_BIND(x)		((x) >> 4)
#define ELF_ST_TYPE(x)		((x) & 0xf)
#define ELF32_ST_BIND(x)	ELF_ST_BIND(x)
#define ELF32_ST_TYPE(x)	ELF_ST_TYPE(x)
#define ELF64_ST_BIND(x)	ELF_ST_BIND(x)
#define ELF64_ST_TYPE(x)	ELF_ST_TYPE(x)

#define EI_NIDENT	16

typedef struct elf32_hdr
{
	  unsigned char	e_ident[EI_NIDENT];	/* Magic number and other info */
	  Elf32_Half	e_type;			/* Object file type */
	  Elf32_Half	e_machine;		/* Architecture */
	  Elf32_Word	e_version;		/* Object file version */
	  Elf32_Addr	e_entry;		/* Entry point virtual address */
	  Elf32_Off	e_phoff;		/* Program header table file offset */
	  Elf32_Off	e_shoff;		/* Section header table file offset */
	  Elf32_Word	e_flags;		               /* Processor-specific flags */
	  Elf32_Half	e_ehsize;		/* ELF header size in bytes */
	  Elf32_Half	e_phentsize;		/* Program header table entry size */
	  Elf32_Half	e_phnum;		/* Program header table entry count */
	  Elf32_Half	e_shentsize;		/* Section header table entry size */
	  Elf32_Half	e_shnum;		/* Section header table entry count */
	  Elf32_Half	e_shstrndx;		/* Section header string table index */
} Elf32_Ehdr;

typedef struct elf32_phdr{
	  Elf32_Word	p_type;    /* Magic number and other info */
	  Elf32_Off	p_offset;
	  Elf32_Addr	p_vaddr;
	  Elf32_Addr	p_paddr;
	  Elf32_Word	p_filesz;
	  Elf32_Word	p_memsz;
	  Elf32_Word	p_flags;
	  Elf32_Word	p_align;
} Elf32_Phdr;

typedef struct elf32_shdr{
	    Elf32_Word sh_name;   //节区名，名字是一个 NULL 结尾的字符串。
	    Elf32_Word sh_type;    //为节区类型
	    Elf32_Word sh_flags;    //节区标志
	    Elf32_Addr sh_addr;    //节区的第一个字节应处的位置。否则，此字段为 0。
	    Elf32_Off sh_offset;    //此成员的取值给出节区的第一个字节与文件头之间的偏移。
	    Elf32_Word sh_size;   //此 成 员 给 出 节 区 的 长 度 （ 字 节 数 ）。
	    Elf32_Word sh_link;   //此成员给出节区头部表索引链接。其具体的解释依赖于节区类型。
	    Elf32_Word sh_info;       //此成员给出附加信息，其解释依赖于节区类型。
	    Elf32_Word sh_addralign;    //某些节区带有地址对齐约束.
	    Elf32_Word sh_entsize;    //给出每个表项的长度字节数。
}Elf32_Shdr;