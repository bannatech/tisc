#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXSYMBOLS   1000
#define MAXSYMBOLLEN 100
#define MAXLINELEN   100
#define MAX_INSTYPES 4

/*
 OP CODE SPEC
  4 bit integers cut in half,
  LSB is postfix, MSB is prefix (or C)

 C B A OP when OP < 3 otherwise:
 OPB A OP
*/

#define ADD  0
#define NAND 1
#define OR   2
#define CIN  2
#define LLI  3
#define LUI  7
#define EXT  11 // extended instructions
#define CMP  15

char symbols[MAXSYMBOLS][MAXSYMBOLLEN];
int  addresses[MAXSYMBOLS];
int  validSymbols = 0;

char *formats[MAX_INSTYPES][8] =
{
	// 0 arg instructions
	{
		"push",
		"pop",
		"pcr",
		NULL
	},
	// 1 arg instructions
	{
		"lli",
		"lui",
		"jmp",
		"lb",
		"sb",
		"sp",
		".fill",
		NULL
	},
	// 2 arg instructions
	{
		"cmp",
		NULL
	},
	// 3 arg instructions
	{
		"add",
		"nand",
		"or",
		"cin",
		NULL
	}
};

/* char *parse -> parses file and sets label, opcode, and args
   accordingly */
char *parse(FILE *file, char *line, char **label, char **opcode,
            char **arg0, char **arg1, char **arg2)
{
	char *str, *first;
	str = fgets(line, MAXLINELEN, file);
	if (str != NULL)
	{
		first = strtok(line, " \t\n");
		if (first == NULL || first[0] == '#')
		{
			return parse(file, line, label, opcode, arg0, arg1, arg2);
		} else if (first[strlen(first) - 1] == ':')
		{
			*label = first;
			*opcode = strtok(NULL, " \t\n");
			first[strlen(first) - 1] = '\0';
		} else
		{
			*label = NULL;
			*opcode = first;
		}
		*arg0 = strtok(NULL, " \t\n");
		*arg1 = strtok(NULL, " \t\n");
		*arg2 = strtok(NULL, " \t\n");
	}
	
	return(str);
}

int labelext(const char *label)
{
	int i, status = 0;

	for (i = 0; i < validSymbols; i++)
		if (strcmp(symbols[i], label) == 0) status = 1;

	return status;
}

int validins(int op, int format, const char *opcode, char *arg[])
{
	int status = 0;
	if (strcmp(formats[format][op], opcode) == 0)
	{
		status = 1;
		if (!(format == 0 && arg[0] == NULL && arg[1] == NULL && arg[2] == NULL ||
		      format == 1 && arg[0] != NULL && arg[1] == NULL && arg[2] == NULL ||
		      format == 2 && arg[0] != NULL && arg[1] != NULL && arg[2] == NULL ||
		      format == 3 && arg[0] != NULL && arg[1] != NULL && arg[2] != NULL ))
		{
			status = 2;
		}
	}

	return status;
}

/* int preprocess -> 1st pass over file, links symbols to address
   and reports syntax errors, fails if returns -1 */
int preprocess(int address, char *label, char *opcode,
               char *arg0, char *arg1, char *arg2)
{
	int i,op;
	int status = 0;

	// arguement check
	for (i = 0; i < MAX_INSTYPES; i++)
	{
		for (op = 0; formats[i][op] != NULL; op++)
		{
			char *args[3] = {arg0, arg2, arg2};
			if (status == 0) status = validins(op, i, opcode, args);
		}
	}
	switch (status)
	{
		case 0:
			printf("Error:%i: instruction '%s' does not exist\n",address, opcode);
			status = 1;
		case 2:
			printf("Error:%i: not enough arguements for '%s'\n",address, opcode);
			status = 1;
	}

	//duplicate label check
	if (label != NULL)
	{
		if (labelext(label) == 1)
		{
			printf("Error:%i: label exists '%s'\n",address, label);
		} else
		{
			strcpy(symbols[validSymbols], label);
			addresses[validSymbols] = address;
			validSymbols++;
		}
	}

	return status;
}
/* int process -> 2nd pass over file, instrutions are turned into
   machine code with symbols filled in as adresses, 
   fails if returns -1 */
int process(int address, FILE *outfile, char *label, char *opcode,
            char *arg0, char *arg1, char *arg2)
{
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "error useage: %s <assembly-code-file> <machine-code-file> \n", argv[0]);
		exit(1);
	}

	char *input,  *output;
	FILE *inputf, *outputf;
	char *labels, *opcodes, *args0, *args1, *args2;
	char lines[MAXLINELEN+1];
	
	int address = 0;

	input = argv[1];
	output = argv[2];

	inputf = fopen(input, "r");
	if (inputf == NULL)
	{
		fprintf(stderr, "Error opening file '%s'", input);
		exit(1);
	}
	outputf = fopen(output, "w");
	if (outputf == NULL)
	{
		fprintf(stderr, "Error opening file '%s'", output);
		exit(1);
	}

	while (parse(inputf, lines, &labels, &opcodes, &args0, &args1, &args2) != NULL)
	{
		if (preprocess(address, labels, opcodes, args0, args1, args2) == 0) exit(1);
		address++;
	}

	rewind(inputf);
	address = 0;

	while (parse(inputf, lines, &labels, &opcodes, &args0, &args1, &args2) != NULL)
	{
		if (process(address, outputf, labels, opcodes, args0, args1, args2) == 0) exit(1);
		address++;
	}
	fclose(inputf);
	fclose(outputf);
	return 1;
}
