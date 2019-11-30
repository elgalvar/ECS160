#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//in C the conventional way to define a constant is by the #define macro
//note: if we define a string, we should define it as char str[MAXCHAR + 1],
//considering the '\0'
#define MAXCHAR 1024
#define MAXLINES 20000

typedef struct {
	char *name;
	int count;	
}Value;

//whenever the program encounters an invalid situation
//just call this function
void invalid(void){
	printf("Invalid Input Format\n");
	exit(EXIT_SUCCESS);
}


//strip the potential double quotes at the beginning and the end of raw_field
//if raw_field only has double quote on one side, for example "content or content"
//this function will print "Invalid Input Format" and exit the program
void stripQuote(char *raw_field, int *p_is_quoted){
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
			*p_is_quoted = 1;
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
		*p_is_quoted = 0;
	}
}

//input: a line representing the header of the csv file, a pointer to a mark
//output: an integer representing the index of "name" column
int getNameIndex(const char *line, int *p_is_quoted){
	
	int length = strlen(line);
	if(length < 4 || length > MAXCHAR){
		//line should at least contain 4 chars: n, a, m and e
		invalid();
	}
	int last_comma_index = -1;
	//the number of commas we have found
	int comma_num = 0;
	//has found the name column before
	int has_found = 0;
	//name column index
	int name_col_index;
	for(int i = 0; i < length; i++){
		if(line[i] == ','){
			//find one comma
			comma_num ++;
			//prepare a field to store that substring
			//between this comma and the last comma we met
			char field[MAXCHAR + 1] = {0};
			if(last_comma_index == -1){
				//we have never found any comma before
				memmove(field, line, i);
				//since field was initialized to 0 first
				//we don't need to append \0
				int is_quoted;
				stripQuote(field, &is_quoted);
				if(strcmp(field, "name") == 0){
					//we have found "name" column!
					//it is the first column
					has_found = 1;
					name_col_index = 0;
					*p_is_quoted = is_quoted;
				}
				//update last_comma_index
				last_comma_index = i;
			}
			else{
				//in this case we have found at least one comma before
				memmove(field, &line[last_comma_index + 1], i - last_comma_index - 1);
				//since field was initialized to 0 first
				//we don't need to append \0
				int is_quoted;
				stripQuote(field, &is_quoted);
				if(strcmp(field, "name") == 0){
					//we have found "name" column
					if(has_found == 1){
						//we have found it before
						invalid();
					}

					has_found = 1;
					name_col_index = comma_num - 1;
					*p_is_quoted = is_quoted;
				}

				//update last_comma_index
				last_comma_index = i;
			}
		}
	}
	
	char field[MAXCHAR + 1] = {0};
	strcpy(field, &line[last_comma_index + 1]);
	//if the csv file comes from windows system
	//we need to strip the \n character
	int tail_length = strlen(field);
	if(tail_length > 0){
		if(field[tail_length - 1] == '\r'){
			field[tail_length - 1] = '\0';
		}
	}
	int is_quoted;
	stripQuote(field, &is_quoted);
	if(strcmp(field, "name") == 0){
		//"name" column is the last column
		if(has_found == 1){
			invalid();
		}
		name_col_index = comma_num;
		has_found = 1;
		*p_is_quoted = is_quoted;
	}

	if(has_found == 0){
		invalid();
		//no name column
	}

	return name_col_index;
}


//get the nameid-th field in line and strip it
//if line is empty or if there is anything else wrong
//that prevents us from getting the nameid-th field
//invalid() would be called
//If the line ends with a comma and we are supposed to extract the field after it, it is an empty name
void getName(int nameid, char *name, const char *line, int is_quoted){
	int length = strlen(line);
	if(length == 0 || length > MAXLINES){
		invalid();
	}
	//we search the line from left to right for commas
	//current position in line we need to look at
	int cur_pos = 0;
	if(nameid > 0){
		//first pass (nameid) commas
		//number of commas we need to pass
		int num_commas = nameid;
		while(num_commas > 0){
			if(line[cur_pos] == '\0'){
				//we have reached the end
				invalid();
			}
			else if(line[cur_pos] == ','){
				cur_pos ++;
				num_commas --;
			}
			else{
				cur_pos ++;
			}
		}

		if(line[cur_pos] == '\0'){
			//we have found enough commas
			//but we have already reached the end
			if(is_quoted == 1){
				// we expect a quoted name, which is an invalid case
				invalid();
			}
			else{
				// return an empty name
				name[0] = '\0';
				return;
			}
		}
	}

	char *pnext_comma = strchr(&line[cur_pos], ',');
	int name_length = 0;
	if(pnext_comma == NULL){
		// no more comma
		// name column is the last column
		name_length = length - cur_pos;
		if(line[length - 1] == '\r'){
			//handle the case when the csv file comes from window system
			//excluding \r character
			name_length --;
		}
	}
	else{
		name_length = pnext_comma - &line[cur_pos];
	}

	memmove(name, &line[cur_pos], name_length);
	name[name_length] = '\0';
	int name_is_quoted;
	stripQuote(name, &name_is_quoted);
	if(name_is_quoted != is_quoted){
		invalid();
	}
}


//extract a line from stream
//this function will discard the end line character it meets
void getLine(char *line, FILE *stream){
	int count = 0;
	int ch = 0;
	while(1){
		ch = fgetc(stream);
		if(ch == EOF){
			//either we have reached the end of the file
			//or some error happened
			if(feof(stream) != 0){
				//we have reached the end of the file
				break;
			}
			else{
				invalid();
			}
		}
		else if(ch == '\n'){
			break;
		}
		else{
			//in other cases, we got a normal character
			count ++;
			//check if what we got so far exceeds MAXCHAR
			if(count > MAXCHAR){
				invalid();
			}
			else{
				//append it to line
				line[count - 1] = ch;
			}
		}
	}

	//append '\0' to the end
	line[count] = '\0';
}

//add a record to value_array
//if the name is not in value_array, return 1
//else return 0
int addRecord(const char *name, Value *value_array, int array_length){
	for(int i = 0; i < array_length; i++){
		if(strcmp(name, value_array[i].name) == 0){
			value_array[i].count++;
			return 0;
		}
	}
	//we cannot directly assign name to the corresponding field in value_array
	//since name is a pointer, if we do that, both will point to same address.
	int length = strlen(name);
	value_array[array_length].name = (char *) malloc(length + 1);
	strcpy(value_array[array_length].name, name);
	value_array[array_length].count = 1;
	return 1;
}


//return the actual length of value_array (or the actual valid number of values in value_array)
//warning: the name field are assigned by malloc, so use free() in the end
//Now my implementation rejects an empty line in the middle
int getValues(Value *value_array, const char *path) {
	char line[MAXCHAR + 1] = {0};
	//a marker indicating whether the name column is quoted
	//if it is, is_quoted = 1. Otherwise it is 0
	int is_quoted;
	FILE *stream = fopen(path, "r");
	if(stream == NULL){
		//read error
		invalid();
	}
	
	//first extract the position of name column
	getLine(line, stream);
	int nameid = getNameIndex(line, &is_quoted);

	//then read one line after another
	//number of values we have filled value_array with
	int num_vals = 0;
	//number of lines we have read, including header
	int num_lines = 1;
	//if there is an empty line in the middle, it's of invalid format.
	int empty_line_warning = 0;
	char name[MAXCHAR + 1] = {0};
	while(1){
		getLine(line, stream);
		if(feof(stream) && line[0] == '\0'){
			//nothing is read. We have already reached the end.
			break;
		}
		num_lines ++;
		if(num_lines > MAXLINES){
			//the file has more than MAXLINES lines, which is invalid
			invalid();
		}

		if(empty_line_warning == 1){
			invalid();
		}

		if(line[0] == '\0'){
			//an empty line
			//set empty_line_warning
			empty_line_warning = 1;
			continue;
		}

		getName(nameid, name, line, is_quoted);
		num_vals += addRecord(name, value_array, num_vals);
	}

	if(fclose(stream) == EOF){
		//close error
		invalid();
	}
	return num_vals;
}

// used as the function in qsort to compare elements
// returns an integer to decide which element goes first in the array
int comparator(const void *p, const void *q) {
	const Value *leftValue = p;
	const Value *rightValue = q;

	// the greater of the two counts will be put first
	return (rightValue->count - leftValue->count);
}

// prints out the top ten users with the largest number of tweets 
// in descending order
void getTopTen(Value *values, int array_length) {
	// second parameter should be the length of the actual array size
	// to only sort values that are not equal to NULL
	qsort((void*)values, array_length, sizeof(values[0]), comparator);

	// the number of loops we will go over
	int num_loops = 10;

	// handle the case when there are less 10 tweeters in the file
	if(array_length < num_loops){
		num_loops = array_length;
	}

	for(int i = 0; i < num_loops; i++) {
		printf("%s: %d\n", values[i].name, values[i].count);
	}
}


int main(int argc, char const *argv[]){	
	if(argc < 1){
		invalid();
	}

	Value values[MAXLINES];
	//argv[1] should be the path of the csv file
	int array_length = getValues(values, argv[1]);
	getTopTen(values, array_length);

	for(int i = 0; i < array_length; i++){
		free(values[i].name);
	}
	return 0;
}