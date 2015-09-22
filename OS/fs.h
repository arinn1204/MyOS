#ifndef _FS_H
#define _FS_H

struct header {
	u32 ID_space; // 0x4100301:combined I&D|0x4200301:separate I&D
	u32 magic_number; // 0x00000020
	u32 tsize;        // code section size in bytes    
	u32 dsize;        // initialized data section size in bytes
	u32 bsize;        // bss section size in bytes     
	u32 zero;         // 0 
	u32 total_size;   // total memory size, including heap
	u32 symbolTable_size; // only if symbol table is present
};

typedef struct header header;

#endif
