typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#include "fs.h"
#include "ext2.h"


typedef struct ext2_group_desc GD;	
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;

#define NULL 0
#define BlockSize 1024

int put_word(u16 word, u16 segment, u16 offset) {
	u16 ds = getds();
	setds(segment);
	*(u16 *)offset = word;
	setds(ds);
}

u16 get_word(u16 segment, u16 offset) {
	u16 word;
	u16 ds = getds();
	setds(segment);

	word = *(u16 *)offset;
	setds(ds);
	return word;

}


char *strtok(char *str, char delim) {
	static char *save;
	char *tmp = str;
	int i;

	//kprintf("String: %s Delim: %c\n\r", (str == 0) ? save : str, delim); kgetc();

	if (str == 0) {
		str = save;
		if (save == 0) return 0;

	}
	else {
		for(i = 0; i < strlen(str); i++ ) {
			if ( *(str + i) == delim) {
				//kprintf("Delim at location: %d\n\r", i);
				str += i;
				break;
			}
		}
	}

	for(i = 1; i < strlen(str); i++) {
		if (*(tmp + i) == delim) {
			tmp += i;
			//kprintf("New tmp string: %s\n\r", tmp); kgetc();
			break;
		}
	}
	if(tmp) {
		*tmp = '\0';
		save = tmp + 1;
		//kprintf("Save string: %s Return string: %s\n\r", save, str);
	}
	else {
		save = 0;
	}
	return (*str == delim) ? str + 1 : str;
}


///@brief This function is responsible for receiving the block information from the disk
///@param blk This should be an integer (u16) that denotes the block number that is being received
///@param buf This is a string that the block information will be stored into
///@returns This will return a 0 for success and readfd will print if error
u16 getBlock(u16 blk, char *buf) {
	readfd( blk / 18, ( blk % 18 ) / 9,  ( ( blk % 18 ) % 9 ) << 1, buf); //this function is defined in bs.s, it will get the block info and put it into buf
	return 0; //signify succesful completion
}

int search(char *name, INODE *ip) {
	char buf2[BlockSize];
	int index; char c; //index variable and temp char variable
	DIR *dp; //initialize a directory pointer
	for (index = 0; index < 12; index++) { //loop through all the possible blocks
		//if (index == 12 || index == 13 || index == 14) error(); //single, double, and triple indirect not being handled currently
		if ( (u16)ip->i_block[index] ) { //make sure that this block contains information and does not just contain empty space
			getBlock((u16)ip->i_block[index], buf2);
			dp = (DIR *)buf2;

			while ( (char *)dp < buf2 + BlockSize ) { //loop through the buffer, making sure not to go over the total size
				c = dp->name[ dp->name_len ];
				dp->name[ dp->name_len ] = 0;
				kprintf("%s",dp->name); kprintf(" "); //print the name of the record we are checking
				if ( strcmp(dp->name, name) == 0 ) { //found what we are looking for
					//no need to restore dp since it is only a local variable
					kprintf("\n\r"); //print newline to show that we found it
					return (u16)dp->inode;
				}
				dp->name[ dp->name_len ] = c; // assign it back the original value
				dp = (char *)dp + dp->rec_len;
			} // end of while loop looking for name in current block
		} // end of if iblock
		return -1;
	} // end of index for loop
}

int load(char *filename, int segment) {
	char *temp, *save, buf1[BlockSize];
	int iblk, ino;
	GD *gp;
	INODE *ip;

	if (filename == 0) {
		kprintf("No file entered!\n\r");
		return 0;
	}
	getBlock(2, buf1);
	gp = (GD *)buf1;
	iblk = gp->bg_inode_table;
	kprintf("Begin block: %d\n\r", iblk);

	getBlock(iblk, buf1);
	ip = (INODE *)buf1 + 1;
	kprintf("Path: %s\n\r", filename);

	for (temp = strtok(filename, '/'); temp; temp = strtok(0, '/') ) {
		kprintf("%s\n\r", temp); kgetc();
		ino = search(temp, ip);
		if (ino < 0) return 0;
		getBlock( INUMBER(ino, iblk), buf1);
		ip = (INODE *)buf1 + OFFSET(ino);
	}

	kprintf("Found it! Inode is at %d\n\r", ino);
	kgetc();

	return 1;
}