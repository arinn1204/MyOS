///@file main.c
///@brief this is the main file where the assembly will call upon start

#include "../Shared/io.h" //io functions
#include "ext2.h" //struct information for the filesystem


///@brief this is the entry point of the program
int main(void) {
	// variable declarations
	char *name[2];
	char test[1];
	extern char buf[BlockSize];
	extern char buf2[BlockSize];
	u16 i, ino, iblk;
	u32 *up;
	GD *gp;
	INODE *ip;
	//initialize name
	name[0] = "boot";
	name[1] = "mtx"; 
	puts("boot mtx:\n\r");
	//putc('/'); puts(name[0]); putc('/'); puts(name[1]); //print boot location

	getBlock(2, buf); //check for return value (no error checks currently)
	gp = (GD *)buf; //get the global descriptor
	iblk = (u16)gp->bg_inode_table; //start location

	//putc('0' + iblk);
	//puts("\n\r");


	getBlock(iblk, buf); //now grabbing that block
	ip = (INODE *)buf + 1; //getting the inode information



	for (i = 0; i < 2; i++) {
		ino = search(ip, name[i]); //search for the different parts of name
		if (ino < 0) error();
		getBlock( INUMBER(ino, iblk) , buf);
		ip = (INODE *)buf + OFFSET(ino);
	}
	getBlock( (u16)ip->i_block[12], buf2 );
	//setes to 0x1000 before incrementing
	setes(0x1000);
	//incrementing the es register 11 times (why not just call inces 11 times?)
	for (i = 0; i < 12; i++) {
		getBlock( (u16)ip->i_block[i], 0 );
		putc('*');
		inces();
	}
		//incrementing ec again, for all of the indirect blocks
	if (ip->i_block[12]) {
		up = (u32 *)buf2;
		while (*up) {
			getBlock( (u16)*up, 0);
			putc('.');
			inces();
			up++;
		}
	}

	puts("\n\rgo?");
	getc();
}
