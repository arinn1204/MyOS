///@file io.h
///@brief This file contains the function declerations for io.c

#ifndef _IO_H // make sure this isn't being included multiple times
#define _IO_H

//The following are constants used in this file
#define BlockSize 1024


//Mailmans Algorithm
#define INUMBER(y,z) (y - 1) / 8 + z
#define OFFSET(y) (y - 1) % 8

//The following lines will shorten the names to make typing them out easier
typedef struct ext2_group_desc GD;	
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;

//makes typing easier
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

char buf[BlockSize];
char buf2[BlockSize];
//function prototypes
int puts(char * );
int gets(char [] );
u16 getBlock(u16 , char *);
u16 search(INODE *, char *);


#endif  //_IO_H