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
	int (*assemble)(struct InstructionDefintion*, char* arg[3], uint8_t*);
} InstructionDefinition_t;

int assemble_default(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer)
{
	write_buffer[0] = definition->opcode_mask;

	return 0;
}

InstructionDefinition_t definitions[TOT_INSTRUCTIONS] =
{
	{ "push",  0, 1, 0x93, assemble_default },
	{ "pop",   0, 1, 0x53, assemble_default },
	{ "pcr",   0, 1, 0xA3, assemble_default },
	{ "li",    1, 1, 0x00, assemble_default },
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
int parse(int* line_number, FILE *file, char *line, char **label, char **opcode,
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
			*line_number = *line_number + 1;
			return parse(line_number, file, line, label, opcode, arg);
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

		*line_number = *line_number + 1;
	}

	return success;
}

/* Checks if instruction is valid
 *
 *  return -1 -> too few arguements
 *  return  0 -> instruction is valid
 *  return  1 -> too many arguements
 */
int validins(InstructionDefinition_t *ins, const char *opcode, char *arg[])
{
	int status = 0;

	int supp_args;

	for (supp_args = 0; supp_args < 3; supp_args++)
	{
		if (arg[supp_args] == NULL)
		{
			break;
		}
	}

	if (ins->arguements > supp_args)
	{
		status = -1; // Too few args, fail
	}
	else
	if (ins->arguements < supp_args)
	{
		status = 1; // Too many args, fail
	}

	return status;
}
InstructionDefinition_t* getInstructionFromOpcode(const char *opcode)
{
	InstructionDefinition_t* return_value = NULL;

	int i;
	for (i = 0; i < TOT_INSTRUCTIONS; i++)
	{
		if (strcmp(definitions[i].instructionLabel, opcode) == 0)
		{
			return_value = &definitions[i];
			break;
		}
	}

	return return_value;
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
		if (labelexists(label) == 0)
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
int preprocess(int line, int *address, char *label, char *opcode,
               char *arg[3])
{
	int status = 1;

	InstructionDefinition_t* ins = getInstructionFromOpcode(opcode);

	if (ins != NULL)
	{
		switch (validins(ins, opcode, arg))
		{
		case -1:
			printf("Error:%i: too few arguements for '%s'\n",
			              line,                       opcode);
			status = 1;
			break;
		case 1:
			printf("Error:%i: too many arguements for '%s'\n",
			              line,                        opcode);
			status = 1;
			break;
		case 0:
			status = 0;
			break;
		}
	}
	else
	{
		printf("Error:%i: instruction '%s' does not exist\n",
		              line,            opcode);
		status = 1;
	}

	if (status == 0)
	{
		if (process_label(label, *address) == 0)
		{
			printf(
				"Error:%i: re-use of existing label '%s'\n",
				       line,                         label );

			status = 1;
		}
	}

	if (status == 0)
	{
		*address = *address + ins->instructionLength;
	}

	return status;
}

/* int process -> 2nd pass over file, instructions are turned into
   machine code with symbols filled in as adresses,
   fails if returns -1 */
int process(int line, int* address, uint8_t *buffer, char *label, char *opcode,
            char *arg[3])
{
	int status = 0;

	InstructionDefinition_t* ins = getInstructionFromOpcode(opcode);

	if (ins != NULL)
	{
		status = ins->assemble(ins, arg, buffer + *address);
	}

	if (status == 0)
	{
		*address = *address + ins->instructionLength;
	}
	

	return status;
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
	char *label, *opcodes, *args[3];
	char line[MAX_LINE_LEN + 1];
	int address = 0, line_number = 0;

	input = argv[1];
	inputf = fopen(input, "r");

	if (inputf == NULL || ferror(inputf))
	{
		fprintf(stderr, "Error opening file '%s'", input);
		goto DITCH;
	}

	output = argv[2];
	outputf = fopen(output, "w");

	if (outputf == NULL || ferror(outputf))
	{
		fprintf(stderr, "Error opening file '%s'", output);
		goto DITCH;
	}

	while (parse(&line_number, inputf, line, &label, &opcodes, args))
	{
		if (preprocess(line_number, &address, label, opcodes, args))
		{
			goto DITCH;
		}
	}

	rewind(inputf);

	int full_size = address;

	uint8_t* w_buffer = (uint8_t*)malloc(sizeof(uint8_t) * full_size);

	if (w_buffer != NULL)
	{
		address = 0;
		line_number = 0;
	
		while (parse(&line_number, inputf, line, &label, &opcodes, args))
		{
			if (process(line_number, &address, w_buffer, label, opcodes, args))
			{
				goto DITCH;
			}
		}

		if (fwrite(w_buffer, sizeof(uint8_t), full_size, outputf) == full_size)
		{
			printf("Wrote %i bytes to file '%s'\r\n", 
			              full_size,        output);
		}
		else
		{
			printf("Failed to write %i bytes to file '%s'!\r\n",
			                        full_size,        output);
		}
	}

DITCH:
	if (inputf != NULL) fclose(inputf);
	if (outputf != NULL) fclose(outputf);

	return 1;
}
