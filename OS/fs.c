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
char buf[BlockSize];
	/*			LITTLE ENDIAN
	*	 _____________________________
	*	|			   |			  |
	*	|	LOW		   |	HIGH	  |
	*	|	BYTE	   |	BYTE	  |
	*	|______________|______________|
	*/

int put_byte(u8 byte, u16 segment, u16 offset) {
	u16 ds = getds();    // save DS
	setds(segment);      // set DS to segment
	*(u8 *)offset = byte;
	setds(ds); 
}

u8 get_byte(u16 segment, u16 offset) {
	u8 byte;
	u16 ds = getds();    // getds() in assembly returns DS value 
	setds(segment);      // set DS to segment 
	byte = *(u8 *)offset;
	setds(ds);           // setds() in assembly restores DS
	return byte;
}


int put_word(u16 word, u16 segment, u16 offset) {
	put_byte( word & 0x00FF , segment, offset);
	put_byte( (word & 0xFF00) >> 8, segment, offset + 1);

	/*u16 ds = getds();
	setds(segment);
	*(u16 *)offset = word;
	setds(ds);*/

	return 0;


}

u16 get_word(u16 segment, u16 offset) {
	u16 word;
	word = get_byte(segment, offset);
	word += (get_byte(segment, offset + 1) << 8);
	
	return word;
	
	/*u16 ds = getds();
	setds(segment);
	word = *(u16 *)offset;
	setds(ds);*/

	//return word;
}


///@brief This function is responsible for receiving the block information from the disk
///@param blk This should be an integer (u16) that denotes the block number that is being received
///@param buf This is a string that the block information will be stored into
///@returns This will return a 0 for success and readfd will print if error
u16 getBlock(u16 blk, char *dbuf) {
	readfd( blk / 18, ( blk % 18 ) / 9,  ( ( blk % 18 ) % 9 ) << 1, dbuf); //this function is defined in bs.s, it will get the block info and put it into buf
	return 0; //signify succesful completion
}

int search(char *name, INODE *ip) {
	char sbuf[BlockSize];
	int index; char c; //index variable and temp char variable
	DIR *dp; //initialize a directory pointer
	//printf("Searching for.... %s\n\r", name); getc();
	for (index = 0; index < 12; index++) { //loop through all the possible blocks
		//if (index == 12 || index == 13 || index == 14) error(); //single, double, and triple indirect not being handled currently
		if ( (u16)ip->i_block[index] ) { //make sure that this block contains information and does not just contain empty space
			getBlock((u16)ip->i_block[index], sbuf);
			dp = (DIR *)sbuf;
			while ( (char *)dp < sbuf + BlockSize ) { //loop through the buffer, making sure not to go over the total size
				c = dp->name[ dp->name_len ];
				dp->name[ dp->name_len ] = 0;
				//printf("%s",dp->name); printf(" "); //print the name of the record we are checking
				if ( strcmp(dp->name, name) == 0 ) { //found what we are looking for
					//no need to restore dp since it is only a local variable
					//printf("\n\r"); //print newline to show that we found it
					return (u16)dp->inode;
				}
				dp->name[ dp->name_len ] = c; // assign it back the original value
				dp = (char *)dp + dp->rec_len;
			} // end of while loop looking for name in current block
		} // end of if iblock
		printf("Error! %s not found!\n\r", name);
		return -1;
	} // end of index for loop
}

int load(char *filename, int segment) {
	char *temp;
	char file[128];//, buf[BlockSize];

	int BufIndex = 0, offset = 0, iblk = -1;
	int ino = 0, dssize = 0, bsize = 0, hsize = sizeof( struct header), tsize = 0, i = 0;
	
	u16 block;
	u32 *up;
	
	GD *gp;
	INODE *ip;
	struct header *h;

	if (filename == 0) {
		printf("No file entered!\n\r");
		return 1;
	}
	strncpy(file, filename, 32);
	temp = &file[0];

	printf("Loading \"%s\".... ", temp);
	getBlock(2, buf);

	gp = (GD *)buf;
	iblk = gp->bg_inode_table;

	//printf("Begin block: %d\n\r", iblk);
	
	getBlock(iblk, buf);
	ip = (INODE *)buf + 1;
	temp = strtok(temp, "/");
	while (temp) {
		ino = search(temp, ip);
		if (ino < 0) return 1;
		getBlock( INUMBER(ino, iblk), buf);
		ip = (INODE *)buf + OFFSET(ino);
		temp = strtok(0, "/");
	}

	//printf("Inode number: %d\n\r", ino);
	block = ip->i_block[0];
	//printf("Block 0: %d\n\r", block); getc();

	getBlock(block, buf);
	h = (struct header *)buf;

	dssize = h->dsize + h->tsize;
	bsize = h->bsize;
	tsize = dssize + bsize + hsize;
	getBlock(block, buf);
	//i is the index for the loaded file data
	//first 32 bytes are for header, so we are skipping that
	printf("\n\rTotal Size: %d\n\rBlock Size: %d\n\rHeader Size: %d\n\r", tsize, BlockSize, hsize);
	/*for (i = hsize; i < tsize; 	i++,
								BufIndex = i % BlockSize,
								block = i / BlockSize,
								offset = i - hsize,
								(BufIndex) ? i = i : getBlock( ip->i_block[block], buf ),
								put_byte( (i < tsize - bsize) ? buf[BufIndex] : 0, segment, offset )
 		);*/
	for(i = hsize; i < tsize; i++) {
		if (i % BlockSize == 0) getBlock(ip->i_block[i / BlockSize], buf);
		if (i < (tsize - bsize) ) put_byte(buf[i % BlockSize], segment, i - hsize);
		else put_byte(0, segment, i - hsize);
	}


	printf("Done\n\r");
	return 0;
}

