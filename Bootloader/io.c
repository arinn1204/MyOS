///@file io.c
///@brief This file contains the io functions that are needed by the 
///bootloader to function properly and to alert the user as to what is happening


#include "io.h" // this is needed to hold the function templates
#include "ext2.h" //includes structure information
///@brief This function will make a call to the bios to  print s to the screen
///@param STR this will contain the string that is wanted to print
///@returns This will return 0 upon success
int puts(char *str) {
	while (*str) //loop through the string
		putc(*str++); //else just print the character to the screen
	return 0; //return 0 to signify successful completion
}

///@brief This function will make a call to BIOS and receive a string and places it into str
///@param STR this will contain the string that was received by the user
///@returns This will return 0 for success, str will be 0 if no characters received
int gets(char str[ ]) {
	char c; //character being received by BIOS
	u8 index = 0; //index variable used to loop through str
	while( ( c = getc() ) != '\r' ) //loop through until an enter is received
		str[index++] = c, putc(c); //assign location to string at index this will also print to the screen what is being typed

	str[index] = 0; //make sure the last character is a NULL
	return 0; //0 for success
}

///@brief This function is responsible for receiving the block information from the disk
///@param blk This should be an integer (u16) that denotes the block number that is being received
///@param buf This is a string that the block information will be stored into
///@returns This will return a 0 for success and readfd will print if error
u16 getBlock(u16 blk, char *buf) {
	readfd( blk / 18, ( blk % 18 ) / 9,  ( ( blk % 18 ) % 9 ) << 1, buf); //this function is defined in bs.s, it will get the block info and put it into buf
	return 0; //signify succesful completion
}



///@brief This function is responsible for searching the filesystem for a specific file, it will return the inode number
///@param ip This is responsible for holding the inode information of the directory that holds the name being searched for
///@param name This is the name that is being searched for
///@returns  Inode number upon success
u16 search(INODE *ip, char *name) {
	extern char buf2[BlockSize]; //this will be the buffer that is read in
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
				puts(dp->name); putc(' '); //print the name of the record we are checking
				if ( strcmp(dp->name, name) == 0 ) { //found what we are looking for
					//no need to restore dp since it is only a local variable
					puts("\n\r"); //print newline to show that we found it
					return (u16)dp->inode;
				}
				dp->name[ dp->name_len ] = c; // assign it back the original value
				dp = (char *)dp + dp->rec_len;
			} // end of while loop looking for name in current block
		} // end of if iblock

	} // end of index for loop
	error();
}

