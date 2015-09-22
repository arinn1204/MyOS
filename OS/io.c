#include "proc.h"
#include "io.h"

int BASE = 10;

char *table = "0123456789ABCDEF";

/**
*/
int getI() {
	int ret = 0;
	char c;
	while( ( c = kgetc() ) != '\r' )  {
		kputc(c);
		if (c < '0')	break;
		else if (c > '9') break;
		ret *= 10;
		ret += (c - '0');
	}

	return ret;
}

///@brief This function will make a call to the bios to  print s to the screen
///@param STR this will contain the string that is wanted to print
///@returns This will return 0 upon success
int kputs(char *str) {
	while (*str) {//loop through the string
		kputc(*str); 
		str++; //else just print the character to the screen
	}
	return 0; //return 0 to signify successful completion
}


/**@brief This function is the core of all of the print statements. This function is designed to use the global variable table
* in order to print the number that is passed in through x.
* @param X This is the number that is being passed in that the user wants to print
*/
int rpu(int x) {
	char c;
	if(x) { //if x is greater than 0
		c = table[x % BASE]; //assign the char to whatever it should be based on the global table
		rpu(x/BASE); //recursively call self
		kputc(c); //now put the character received on screen
	}
	return 0;
}


/**@brief This function is tasked with printing unsigned 32-bit integers.
*   @param X This is the unsigned integer that the user wants printed
*/
int printu(int x) {
	BASE = 10; //assign the base a value
	if ( ! x ) {
		kputc('0'); // if x < 0 then put 0 to screen
	}
	else { //otherwise call rpu
		rpu(x);
	}

	return 0;
}
/** @brief This function will print a 32-bit signed integer, if the number is a negative number then it will print the negative sign first
* @param X this is the signed integer that will be printed
*/
int printd(int x) {
	BASE = 10;
	if ( ! x ) {
		kputc('0'); //print 0 if nothing passed in
	}
	else {
		if (x < 0) {
			kputc('-'); //print - if less than 0
			rpu(x*-1); //call rpu but make the number positive
		}
		else {
			rpu(x); //call rpu like printu does
		}
	}
	return 0;
}
/** 	@brief This function is responsible for printing octal numbers, the format will be 0number, so if
*	the user wants to print (octal) 71217, then it will be displayed as 071217
*	@param X this is the octal number that is being passed in for printing
*/

int printo(int x) {
	BASE = 8; //set the base to 8
	if ( ! x ) {
		kputc('0'); //print 0 if 0 is passed in
	}
	else {
		kputc('0'); //print the leading 0
		rpu(x); //call rpu like normal (see the pattern here)
	}
	return 0;
}

/**
* @brief This function is responsible for printing hexadecimal numbers. The format will be 0xNUMBER
* for example, if someone wants to print EF53, the displayed value would be 0xEF53
* @param X This is the number to be printed
*/
int printx(int x) {
	BASE = 16;
	if ( ! x ) {
		//print 0x0 instead of just 0
		kputc('0');
		kputc('x');
		kputc('0');
	}	
	else {
		//print 0x
		kputc('0');
		kputc('x');
		rpu(x); //call rpu like normal
	}

	return 0;
}

/**
* @brief This function will be the one that will actually be called by the user, this is printf
* Currently printf takes in only 6 different options with no formatting options
* @param FMT this is the buffer that is actually passed in
* @param ... This will work like a normal printf function works, so it will take in an undetermined number of parameters
* these parameters are to be combined with a % in the string passed in like a normal string
* for example: ("I am %d years old and my name is %s", 21, "Aaron") Will print to: "I am 21 years old and my name is Aaron"
*/
int kprintf(char fmt[], ...) {
	char *cp=fmt; //set pointer to the string being passed in
	int *ip = (int *)&fmt + 1; //set pointer to the next item on the stack

	while(*cp) { //loop through until NULL is received
		if (*cp == '%') { //if it is equal to a % sign, then special operation
			cp++; //inc the char pointer
			switch(*cp) { //switch statement to call the above functions
				case 'c':
					kputc( (char)*ip );
					break;
				case 's':
					kputs( (char *)*ip);
					break;
				case 'd':
					printd( (int)*ip );
					break;
				case 'u':
					printu( (int)*ip );
					break;
				case 'o':
					printo( (int)*ip );
					break;
				case 'x':
					printx( (int)*ip );
					break;
				default:
					kputc( *cp ); //default print the option attempted to pass in
					kputs(" is not a supported case at this time.\n\r"); //print error string
					break;

			}
			ip++; //increment the pointer to the options on the stack
		}
		else if ( *cp == '\n' ) { //if cp is \n then print \n\r
			kputc('\n');
			kputc('\r');
		}
		else { //else it has to be just a character, so print it
			kputc(*cp);
		}

		cp++; //now continue with the next char
	}

	return 0;
}


/**
*/

int printList(char *name, PROC *list) {
	PROC *p = list;

	kprintf("%s", name);
	kprintf("= ");

	while( p ) {
		kprintf("%d ->", p->pid);
		p = p->next;
	}
	kprintf("NULL\n\r");
	return 0;
}

/**
*/
int printQueue(char *name, PROC *queue) {
	PROC *q = queue;

	kprintf("%s", name);
	kprintf("= ");

	while( q ) {
		kprintf("%d ->", q->pid);
		q = q->next;
	}
	kprintf("NULL\n\r");

	return 0;
}
