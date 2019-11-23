#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//in C the conventional way to define a constant is by the #define macro
#define MAXCHAR 1024
#define MAXLINES 20000

typedef struct{
	char* name;
	int count;	
}Value;

//whenever the program encounters an invalid situation
//just call this function
void invalid(void){
	printf("Invalid Input Format");
	exit(EXIT_SUCCESS);
}


//strip the potential double quotes at the beginning and the end of raw_field
//if raw_field only has double quote on one side, for example "content or content"
//this function will print "Invalid Input Format" and exit the program
void strip_quote(char *raw_field){
	int length = strlen(raw_field);
	if(length == 0){
		//do nothing
		return;
	}

	if(raw_field[0] == '\"'){
		//raw_field begins with a double quote

		if(raw_field[length - 1] == '\"'){
			//raw_field ends with a double quote as well

			if(length == 1){
				//the beginning and the ending double quote are the same one
				//invalid format
				invalid();
			}
			//now length >= 2
			//memmove is a safer function compared to strcpy or memcpy
			//in the sense that the source and the destination could overlap
			memmove(raw_field, raw_field + 1, length - 2);
			//add null character
			raw_field[length - 2] = '\0';
		}
		else{
			//raw_field does not end with a double quote
			//invalid format
			invalid();
		}
	}
	else{
		//raw_field does not begin with a double quote

		if(raw_field[length - 1] == '\"'){
			//raw_field ends with a double quote
			//invalid format
			invalid();
		}
		
		//now raw_field does not end with a double quote
		//do nothing
	}
}


int getValues(Value *values, const char *path) {
	//unfinished
	return -1;
}

int main(int argc, char const *argv[]){	
	if(argc < 1){
		invalid();
	}
	//argv[1] should be the path of the csv file
	Value values[MAXLINES];
	getValues(values, argv[1]);

	return 0;
}