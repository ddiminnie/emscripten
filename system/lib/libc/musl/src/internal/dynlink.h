#ifndef _INTERNAL_RELOC_H
#define _INTERNAL_RELOC_H

#ifdef __EMSCRIPTEN__
// Declare `struct dso` in this header so that it is visible to gen_struct_info.

#pragma once

#include <emscripten/emscripten.h>

struct dso {
  struct dso *next, *prev;

  // For async mode
  em_dlopen_callback onsuccess;
  em_arg_callback_func onerror;
  void* user_data;

  // Flags used to open the library.  We need to cache these so that other
  // threads can mirror the open library state.
  int flags;

  // Location in memory/table of static data/static function addresses
  // The first thread to load a given module alloces the memory and table
  // address space and then sets this field to non-zero.
  uint8_t mem_allocated;
  void* mem_addr;
  size_t mem_size;
  void* table_addr;
  size_t table_size;

  // Flexible array; must be final element of struct
  char name[];
};

#else

#include <features.h>
#include <elf.h>
#include <stdint.h>

#if UINTPTR_MAX == 0xffffffff
typedef Elf32_Ehdr Ehdr;
typedef Elf32_Phdr Phdr;
typedef Elf32_Sym Sym;
#define R_TYPE(x) ((x)&255)
#define R_SYM(x) ((x)>>8)
#define R_INFO ELF32_R_INFO
#else
typedef Elf64_Ehdr Ehdr;
typedef Elf64_Phdr Phdr;
typedef Elf64_Sym Sym;
#define R_TYPE(x) ((x)&0x7fffffff)
#define R_SYM(x) ((x)>>32)
#define R_INFO ELF64_R_INFO
#endif

/* These enum constants provide unmatchable default values for
 * any relocation type the arch does not use. */
enum {
	REL_NONE = 0,
	REL_SYMBOLIC = -100,
	REL_GOT,
	REL_PLT,
	REL_RELATIVE,
	REL_OFFSET,
	REL_OFFSET32,
	REL_COPY,
	REL_SYM_OR_REL,
	REL_DTPMOD,
	REL_DTPOFF,
	REL_TPOFF,
	REL_TPOFF_NEG,
	REL_TLSDESC,
	REL_FUNCDESC,
	REL_FUNCDESC_VAL,
};

struct fdpic_loadseg {
	uintptr_t addr, p_vaddr, p_memsz;
};

struct fdpic_loadmap {
	unsigned short version, nsegs;
	struct fdpic_loadseg segs[];
};

struct fdpic_dummy_loadmap {
	unsigned short version, nsegs;
	struct fdpic_loadseg segs[1];
};

#include "reloc.h"

#ifndef FDPIC_CONSTDISP_FLAG
#define FDPIC_CONSTDISP_FLAG 0
#endif

#ifndef DL_FDPIC
#define DL_FDPIC 0
#endif

#ifndef DL_NOMMU_SUPPORT
#define DL_NOMMU_SUPPORT 0
#endif

#if !DL_FDPIC
#define IS_RELATIVE(x,s) ( \
	(R_TYPE(x) == REL_RELATIVE) || \
	(R_TYPE(x) == REL_SYM_OR_REL && !R_SYM(x)) )
#else
#define IS_RELATIVE(x,s) ( ( \
	(R_TYPE(x) == REL_FUNCDESC_VAL) || \
	(R_TYPE(x) == REL_SYMBOLIC) ) \
	&& (((s)[R_SYM(x)].st_info & 0xf) == STT_SECTION) )
#endif

#ifndef NEED_MIPS_GOT_RELOCS
#define NEED_MIPS_GOT_RELOCS 0
#endif

#ifndef DT_DEBUG_INDIRECT
#define DT_DEBUG_INDIRECT 0
#endif

#define AUX_CNT 32
#define DYN_CNT 32

typedef void (*stage2_func)(unsigned char *, size_t *);
typedef _Noreturn void (*stage3_func)(size_t *);

#endif // __EMSCRIPTEN__

#endif
