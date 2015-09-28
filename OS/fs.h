#ifndef _FS_H
#define _FS_H

#define INUMBER(y,z) (y - 1) / 8 + z
#define OFFSET(y) (y - 1) % 8

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


int setds(u16 segment);
u16 getds();
int setes(u16 segment);
int inces();

int put_byte(u8 byte, u16 segment, u16 offset);
int put_word(u16 word, u16 segment, u16 offset);

u8 get_byte(u16 segment, u16 offset);
u16 get_word(u16 segment, u16 offset);

int search(char *name, INODE *ip);
int load(char *filename, int segment);

#endif
