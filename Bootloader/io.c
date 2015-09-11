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

#ifdef _BOOT_

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


#else

int BASE;

char *table = "0123456789ABCDEF";
/**@brief This function is the core of all of the print statements. This function is designed to use the global variable table
* in order to print the number that is passed in through x.
* @param X This is the number that is being passed in that the user wants to print
*/
u8 rpu(u8 x) {
	char c;
	if(x) { //if x is greater than 0
		c = table[x % BASE]; //assign the char to whatever it should be based on the global table
		rpu(x/BASE); //recursively call self
		putc(c); //now put the character received on screen
	}
}
/**@brief This function is tasked with printing unsigned 32-bit integers.
*   @param X This is the unsigned integer that the user wants printed
*/
u8 printu(u32 x) {
	BASE = 10; //assign the base a value
	if ( ! x ) {
		putc('0'); // if x < 0 then put 0 to screen
	}
	else { //otherwise call rpu
		rpu(x);
	}

	return 0;
}
/** @brief This function will print a 32-bit signed integer, if the number is a negative number then it will print the negative sign first
* @param X this is the signed integer that will be printed
*/
u8 printd(u32 x) {
	BASE = 10;
	if ( ! x ) {
		putc('0'); //print 0 if nothing passed in
	}
	else {
		if (x < 0) {
			putc('-'); //print - if less than 0
			rpu(x*-1); //call rpu but make the number positive
		}
		else {
			rpu(x); //call rpu like printu does
		}
	}
}
/** 	@brief This function is responsible for printing octal numbers, the format will be 0number, so if
*	the user wants to print (octal) 71217, then it will be displayed as 071217
*	@param X this is the octal number that is being passed in for printing
*/

u8 printo(u32 x) {
	BASE = 8; //set the base to 8
	if ( ! x ) {
		putc('0'); //print 0 if 0 is passed in
	}
	else {
		putc('0'); //print the leading 0
		rpu(x); //call rpu like normal (see the pattern here)
	}
}

/**
* @brief This function is responsible for printing hexadecimal numbers. The format will be 0xNUMBER
* for example, if someone wants to print EF53, the displayed value would be 0xEF53
* @param X This is the number to be printed
*/
u8 printx(u32 x) {
	BASE = 16;
	if ( ! x ) {
		//print 0x0 instead of just 0
		putc('0');
		putc('x');
		putc('0');
	}	
	else {
		//print 0x
		putc('0');
		putc('x');
		rpu(x); //call rpu like normal
	}
}

/**
* @brief This function will be the one that will actually be called by the user, this is printf
* Currently printf takes in only 6 different options with no formatting options
* @param FMT this is the buffer that is actually passed in
* @param ... This will work like a normal printf function works, so it will take in an undetermined number of parameters
* these parameters are to be combined with a % in the string passed in like a normal string
* for example: ("I am %d years old and my name is %s", 21, "Aaron") Will print to: "I am 21 years old and my name is Aaron"
*/
int printf(char fmt[], ...) {
	char *cp=fmt; //set pointer to the string being passed in
	int *ip = (int *)&fmt + 1; //set pointer to the next item on the stack

	while(*cp) { //loop through until NULL is received
		if (*cp == '%') { //if it is equal to a % sign, then special operation
			cp++; //inc the char pointer
			switch(*cp) { //switch statement to call the above functions
				case 'c':
					putc( (char)*ip );
					break;
				case 's':
					puts((char*)*ip);
					break;
				case 'd':
					printd( (u8)*ip );
					break;
				case 'u':
					printu( (u8)*ip );
					break;
				case 'o':
					printo( (u8)*ip );
					break;
				case 'x':
					printx( (u8)*ip );
					break;
				default:
					putc( *cp ); //default print the option attempted to pass in
					puts(" is not a supported case at this time.\n\r"); //print error string

			}
			ip++; //increment the pointer to the options on the stack
		}
		else if ( *cp == '\n' ) { //if cp is \n then print \n\r
			putc('\n');
			putc('\r');
		}
		else { //else it has to be just a character, so print it
			putc(*cp);
		}

		cp++; //now continue with the next char
	}
}



#endif //_BOOT_
