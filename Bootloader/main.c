///@file main.c
///@brief this is the main file where the assembly will call upon start

#include "io.h" //io functions
#include "ext2.h" //struct information for the filesystem


int load(char *name[], u16 segment) {
	extern char buf[BlockSize];
	extern char buf2[BlockSize];
	u16 iblk, i, ino;
	u32 *up;
	INODE *ip;

	getBlock(2, buf); //check for return value (no error checks currently)
	iblk = (u16)( (GD *)buf )->bg_inode_table; //start location

	getBlock(iblk, buf); //now grabbing that block
	ip = (INODE *)buf + 1; //getting the inode information

	/*temp = strtok(temp, "/");
	while (temp) {
		ino = search(ip, temp);
		if (ino < 2) error();
		getBlock( INUMBER(ino, iblk), buf);
		ip = (INODE *)buf + OFFSET(ino);
		temp = strtok(0, "/");
	}*/

	for (i = 0; i < 2; i++) {
		ino = search(ip, name[i]); //search for the different parts of name
		if (ino < 0) error();
		getBlock( INUMBER(ino, iblk) , buf);
		ip = (INODE *)buf + OFFSET(ino);
	}
	getBlock( (u16)ip->i_block[12], buf2 );
	//setes to 0x1000 before incrementing
	setes(segment);
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

}

///@brief this is the entry point of the program
int main(void) {
	// variable declarations
	char *name[2];
	name[0] = "boot";
	name[1] = "mtx0";
	//initialize name
	puts("boot mtx ");
	load(name, 0x1000);

	puts("go?");
	getc();
}
