#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define TOT_INSTRUCTIONS 15
#define MAX_SYMBOLS    1000
#define MAX_SYMBOL_LEN 100
#define MAX_LINE_LEN   100
#define MAX_INSTYPES   4

#define ADD  0
#define NAND 1
#define OR   2
#define CIN  2
#define LLI  3
#define LUI  7
#define EXT  11
#define CMP  15

typedef struct InstructionDefintion
{
	char* instructionLabel;
	int arguements;
	int instructionLength;
	uint8_t opcode_mask;
	uint8_t** (*assemble)(struct InstructionDefintion, uint8_t*);
} InstructionDefinition_t;

uint8_t** assemble_default(
	InstructionDefinition_t definition,
	uint8_t* arg)
{
	uint8_t** return_value = (uint8_t**)malloc(sizeof(uint8_t*) * 1);

	return_value[0] = &definition.opcode_mask;

	return return_value;
}

InstructionDefinition_t definitions[TOT_INSTRUCTIONS] =
{
	{ "push",  0, 1, 0x93, assemble_default },
	{ "pop",   0, 1, 0x53, assemble_default },
	{ "pcr",   0, 1, 0xA3, assemble_default },
	{ "li",    1, 2, 0x00, assemble_default },
	{ "lli",   1, 1, 0x03, assemble_default },
	{ "lui",   1, 1, 0x43, assemble_default },
	{ "jmp",   1, 2, 0x83, assemble_default },
	{ "lb",    1, 1, 0x87, assemble_default },
	{ "sb",    1, 1, 0x8B, assemble_default },
	{ "sp",    1, 1, 0x8F, assemble_default },
	{ "cin",   2, 1, 0x02, assemble_default },
	{ "cmp",   2, 1, 0xC3, assemble_default },
	{ "or",    2, 1, 0x00, assemble_default },
	{ "nand",  2, 1, 0x01, assemble_default },
	{ "add",   2, 1, 0x02, assemble_default },
};

int  validSymbols = 0;
int  addresses[MAX_SYMBOLS];
char symbols[MAX_SYMBOLS][MAX_SYMBOL_LEN];

/* char *parse -> parses file and sets label, opcode, and args
   accordingly */
int parse(FILE *file, char *line, char **label, char **opcode,
            char *arg[3])
{
	int success = 0;

	char *str, *first;
	str = fgets(line, MAX_LINE_LEN, file);

	if (str != NULL)
	{
		first = strtok(line, " \t\n");

		if (first == NULL || first[0] == '#')
		{
			return parse(file, line, label, opcode, arg);
		}
		else if (first[strlen(first) - 1] == ':')
		{
			*label = first;
			*opcode = strtok(NULL, " \t\n");
			first[strlen(first) - 1] = '\0';

			success = 1;
		}
		else
		{
			*label = NULL;
			*opcode = first;

			success = 1;
		}

		if (success)
		{
			arg[0] = strtok(NULL, " \t\n");
			arg[1] = strtok(NULL, " \t\n");
			arg[2] = strtok(NULL, " \t\n");
		}
	}
	
	return success;
}

int validins(const char *opcode, char *arg[])
{
	int status = 0;
	
	for (int i = 0; i < TOT_INSTRUCTIONS; i++)
	{
		// does that instruction label match what we're looking for?
		if (strcmp(definitions[i].instructionLabel, opcode) == 0)
		{
			int supp_args;
			for (supp_args = 0; supp_args < 3; supp_args++)
			{
				if (arg[supp_args] == NULL)
				{
					break;
				}
			}

			if (definitions[i].arguements > supp_args)
			{
				status = 2; // Too few args, fail
			}
			else
			if (definitions[i].arguements < supp_args)
			{
				status = 3; // Too many args, fail
			}
			else
			{
				status = 1; // Success!
			}
			break;
		}
	}

	return status;
}

int processins(int op, const char *opcode, char* arg[])
{
	return 0;
}

int labelexists(const char *label)
{
	int i, status = 0;

	for (i = 0; i < validSymbols; i++)
	{
		if (strcmp(symbols[i], label) == 0)
		{
			status = 1;
		}
	}

	return status;
}

int process_label(char *label, int address)
{
	int status = 0;

	if (label != NULL)
	{
		//duplicate label check
		if (labelexists(label) == 1)
		{
			printf(
				"Error:%i: re-use of existing label '%s'\n",
				       address,                      label );
		}
		else
		{
			//add label to table 'o' label
			strcpy(symbols[validSymbols], label);
			addresses[validSymbols] = address;
			validSymbols++;

			status = 1;
		}
	}
	else
	{
		status = 1;
	}

	return status;
}

/* int preprocess -> 1st pass over file, links symbols to address
   and reports syntax errors, fails if returns -1 */
int preprocess(int address, char *label, char *opcode,
               char *arg[3])
{

	int i,op;
	int status = 0;

	status = validins(opcode, arg);

	switch (status)
	{
		case 0:
			printf("Error:%i: instruction '%s' does not exist\n",
			              address,         opcode);
			status = 0;
			break;
		case 2:
			printf("Error:%i: too few arguements for '%s'\n",
			              address,                       opcode);
			status = 0;
			break;
		case 3:
			printf("Error:%i: too many arguements for '%s'\n",
			              address,                       opcode);
			status = 0;
			break;
	}

	if (status == 1)
	{
		status = process_label(label, address);
	}

	return status;
}

/* int process -> 2nd pass over file, instrutions are turned into
   machine code with symbols filled in as adresses, 
   fails if returns -1 */
int process(int address, FILE *outfile, char *label, char *opcode,
            char *arg[3])
{
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf(stderr,
			"do: %s <assembly-code-file> <machine-code-file> \n",
			    argv[0]);

		exit(1);
	}

	char *input,  *output;
	FILE *inputf, *outputf;
	char *labels, *opcodes, *args[3];
	char line[MAX_LINE_LEN + 1];
	int address = 0;

	input = argv[1];
	inputf = fopen(input, "r");

	if (inputf == NULL)
	{
		fprintf(stderr, "Error opening file '%s'", input);
		exit(1);
	}

	output = argv[2];
	outputf = fopen(output, "w");

	if (outputf == NULL)
	{
		fprintf(stderr, "Error opening file '%s'", output);
		exit(1);
	}

	while (parse(inputf, line, &labels, &opcodes, args) == 1)
	{
		if (preprocess(address, labels, opcodes, args) == 0)
		{
			exit(1);
		}
		address++;
	}

	rewind(inputf);
	address = 0;

	while (parse(inputf, line, &labels, &opcodes, args) == 1)
	{
		if (process(address, outputf, labels, opcodes, args) == 0)
		{
			exit(1);
		}
		address++;
	}

	fclose(inputf);
	fclose(outputf);

	return 1;
}
