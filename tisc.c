#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define TOT_INSTRUCTIONS 16
#define MAX_SYMBOLS    1000
#define MAX_SYMBOL_LEN 100
#define MAX_LINE_LEN   100
#define MAX_INSTYPES   4
#define MAX_PGR_SIZE   0xFF

#define GR_N        0x0
#define GR_N_STRING "NUL"
#define GR_A        0x1
#define GR_A_STRING "GRA"
#define GR_B        0x2
#define GR_B_STRING "GRB"
#define GR_C        0x3
#define GR_C_STRING "GRC"

int  validSymbols = 0;
int  addresses[MAX_SYMBOLS];
char symbols[MAX_SYMBOLS][MAX_SYMBOL_LEN];

typedef struct InstructionDefintion
{
	char* instructionLabel;
	int arguements;
	int instructionLength;
	uint8_t opcode_mask;
	int (*assemble)(struct InstructionDefintion*, char* arg[3], uint8_t*);
} InstructionDefinition_t;

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

int label_address(const char *label)
{
	int i, address = -1;

	for (i = 0; i < validSymbols; i++)
	{
		if (strcmp(symbols[i], label) == 0)
		{
			address = addresses[i];
			break;
		}
	}

	return address;
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

uint8_t getRegisterEnumeration(char* string)
{
	if ((string != NULL) && (strcmp(string, GR_A_STRING) == 0))
	{
		return GR_A;
	} else
	if ((string != NULL) && (strcmp(string, GR_B_STRING) == 0))
	{
		return GR_B;
	} else
	if ((string != NULL) && (strcmp(string, GR_C_STRING) == 0))
	{
		return GR_C;
	} else
	if ((string != NULL) && (strcmp(string, GR_N_STRING) == 0))
	{
		return GR_N;
	}

	return 0xFF;
}

int assemble_0arg(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer)
{
	write_buffer[0] = definition->opcode_mask;

	return 1;
}

int assemble_1arg(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer)
{
	uint8_t argB = getRegisterEnumeration(arg[0]);
	
	write_buffer[0] = definition->opcode_mask | (argB << 4);

	return 1;
}

int assemble_2arg(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer)
{
	uint8_t argA, argB;

	argA = getRegisterEnumeration(arg[0]);
	argB = getRegisterEnumeration(arg[1]);

	write_buffer[0] = definition->opcode_mask;

	write_buffer[0] |= ( argA << 2 ) | ( argB << 4);
	
	return 1;
}

int assemble_2arg_alt(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer)
{
	uint8_t argB, argC;

	argB = getRegisterEnumeration(arg[0]);
	argC = getRegisterEnumeration(arg[1]);

	write_buffer[0] = definition->opcode_mask;

	write_buffer[0] |= ( argB << 4) | ( argC << 6);
	
	return 1;
}

int assemble_3arg(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer)
{
	uint8_t argA, argB, argC;

	argA = getRegisterEnumeration(arg[0]);
	argB = getRegisterEnumeration(arg[1]);
	argC = getRegisterEnumeration(arg[2]);

	write_buffer[0] = definition->opcode_mask;

	write_buffer[0] |= ( argA << 2 ) | ( argB << 4) | ( argC << 6);
	
	return 1;
}

int assemble_immediate(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer)
{
	int return_value = 0;
	
	int immediate_value = atoi(arg[0]);

	if (immediate_value < 0x0F && immediate_value >= 0)
	{
		write_buffer[0] = definition->opcode_mask | immediate_value << 2;

		return_value = 1;
	}

	return return_value;
}

int assemble_li(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer)
{
	int return_value = 0;
	
	int immediate_value = atoi(arg[0]);

	if (immediate_value <= 0xFF && immediate_value >= 0)
	{
		write_buffer[0] = 0x03 | (immediate_value & 0x0F) << 2;
		write_buffer[1] = 0x43 | ((immediate_value & 0xF0) >> 4) << 2;
		
		return_value = 1;
	}

	return return_value;
}

int assemble_jmp(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer)
{
	int return_value = 0;

	int address = label_address(arg[0]);

	if (address != -1)
	{
		write_buffer[0] = definition->opcode_mask;
		write_buffer[1] = address;

		return_value = 1;
	}

	return return_value;
}

InstructionDefinition_t definitions[TOT_INSTRUCTIONS] =
{
	{ "push",  0, 1, 0x93, assemble_0arg },
	{ "pop",   0, 1, 0xA3, assemble_0arg },
	{ "pcr",   0, 1, 0xB3, assemble_0arg },
	{ "li",    1, 2, 0x00, assemble_li },
	{ "lli",   1, 1, 0x03, assemble_immediate },
	{ "lui",   1, 1, 0x43, assemble_immediate },
	{ "jmp",   1, 2, 0x83, assemble_jmp },
	{ "lb",    1, 1, 0x87, assemble_1arg },
	{ "sb",    1, 1, 0x8B, assemble_1arg },
	{ "sp",    1, 1, 0x8F, assemble_1arg },
	{ "cin",   2, 1, 0x02, assemble_2arg_alt },
	{ "mov",   2, 1, 0x00, assemble_2arg_alt },
	{ "cmp",   2, 1, 0xC3, assemble_2arg },
	{ "or",    3, 1, 0x00, assemble_3arg },
	{ "nand",  3, 1, 0x01, assemble_3arg },
	{ "add",   3, 1, 0x02, assemble_3arg },
};
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

/* int preprocess -> 1st pass over file, links symbols to address
   and reports syntax errors, fails if returns -1 */
int preprocess(int line, int *address, char *label, char *opcode,
			   char *arg[3])
{
	int status = 0;

	InstructionDefinition_t* ins = getInstructionFromOpcode(opcode);

	if (ins != NULL)
	{
		switch (validins(ins, opcode, arg))
		{
		case -1:
			printf("Error:%i: too few arguements for '%s'\n",
						  line,                       opcode);
			status = 0;
			break;
		case 1:
			printf("Error:%i: too many arguements for '%s'\n",
						  line,                        opcode);
			status = 0;
			break;
		case 0:
			status = 1;
			break;
		default:
			status = 0;
			printf("Something Weird!");
			break;
		}
	}
	else
	{
		printf("Error:%i: instruction '%s' does not exist\n",
					  line,            opcode);
		status = 0;
	}

	if (status)
	{
		if (process_label(label, *address) == 0)
		{
			printf(
				"Error:%i: re-use of existing label '%s'\n",
					   line,                         label );

			status = 0;
		}
	}

	if (status)
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

	if (ins != NULL && ins->assemble != NULL)
	{
		status = ins->assemble(ins, arg, buffer + *address);
	}

	if (status)
	{
		*address = *address + ins->instructionLength;
	}
	
	return status;
}

int output_file(FILE* output_file, uint8_t* bytes, int size)
{
	fprintf(output_file, "v2.0 raw\n");

	for (int i = 0; i < size; i++)
	{
		fprintf(output_file, "%x%s", bytes[i], (i % 8 == 0 && i != 0) ? "\n" : " ");
	}
	return 1;
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
		printf("Error opening file '%s'\n", input);
		goto DITCH;
	}

	output = argv[2];
	outputf = fopen(output, "w");

	if (outputf == NULL || ferror(outputf))
	{
		printf("Error opening file '%s'\n", output);
		goto DITCH;
	}

	while (parse(&line_number, inputf, line, &label, &opcodes, args))
	{
		if (preprocess(line_number, &address, label, opcodes, args) == 0)
		{
			printf("Preprocess: Error on line #%i\n", line_number);

			goto DITCH;
		}
	}

	rewind(inputf);

	int full_size = address;

	if (full_size > MAX_PGR_SIZE)
	{
		printf("FATAL: Program exceeds maximum size!\n");
		goto DITCH;
	}
	
	uint8_t* w_buffer = (uint8_t*)malloc(sizeof(uint8_t) * full_size);

	if (w_buffer != NULL)
	{
		address = 0;
		line_number = 0;
	
		while (parse(&line_number, inputf, line, &label, &opcodes, args))
		{
			if (process(line_number, &address, w_buffer, label, opcodes, args) == 0)
			{
				printf("Process: Error on line #%i\n", line_number);
				goto DITCH;
			}
		}

		// Output Logisim raw v2.0 format

		output_file(outputf, w_buffer, full_size);
	}

DITCH:
	if (inputf != NULL) fclose(inputf);
	if (outputf != NULL) fclose(outputf);

	return 1;
}
