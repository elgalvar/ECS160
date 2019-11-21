#include<stdio.h>
#include<stdlib.h>

const int MAXCHAR = 1024;
const int MAXLINES = 20000;

struct Value
{
	char* names;
	int count;	
};

int getValues(Value *values) {

}

int main(int argc, char const *argv[])
{
	Value values[MAXLINES];
	getValues(values);

	return 0;
}