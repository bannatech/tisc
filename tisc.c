#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define TOT_INSTRUCTIONS 26
#define MAX_SYMBOLS    1000
#define MAX_SYMBOL_LEN 100
#define MAX_LINE_LEN   100
#define MAX_INSTYPES   4
#define MAX_PGR_SIZE   0xFF

#define NR          0x0
#define NR_STRING   "NIL"
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

void add_label(const char* label, int address)
{
	//add label to table 'o' label
	strcpy(symbols[validSymbols], label);
	addresses[validSymbols] = address;
	validSymbols++;
}

int longest_label()
{
	int i, labelLen = 0;

	for (i = 0; i < validSymbols; i++)
	{
		int len = strlen(symbols[i]);
		if (len > labelLen)
		{
			labelLen = len;
		}
	}

	return (labelLen > 5) ? labelLen : 5;
}

int label_address(const char *label)
{
	int i, address = -1;

	for (i = 0; i < validSymbols; i++)
	{
		if (strncmp(symbols[i], label, strlen(symbols[i])) == 0)
		{
			address = addresses[i];
			break;
		}
	}

	return address;
}

void update_label(const char* label, int address)
{
	int i;

	for (i = 0; i < validSymbols; i++)
	{
		if (strncmp(symbols[i], label, strlen(symbols[i])) == 0)
		{
			addresses[i] = address;
			break;
		}
	}
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

int process_label_initial(char *label, int address)
{
	int status = 0;

	if (label != NULL)
	{
		//duplicate label check
		if (labelexists(label) == 0)
		{
			add_label(label, address);

			status = 1;
		}
	}
	else
	{
		status = 1;
	}

	return status;
}

int process_label_final(char *label, int address)
{
	int status = 0;

	if (label != NULL)
	{
		//make sure the label exists 
		if (labelexists(label) == 1)
		{
			update_label(label, address);
			
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
	if ((string != NULL) && (strncmp(string, GR_A_STRING, strlen(GR_A_STRING)) == 0))
	{
		return GR_A;
	} else
	if ((string != NULL) && (strncmp(string, GR_B_STRING, strlen(GR_B_STRING)) == 0))
	{
		return GR_B;
	} else
	if ((string != NULL) && (strncmp(string, GR_C_STRING, strlen(GR_C_STRING)) == 0))
	{
		return GR_C;
	} else
	if ((string != NULL) && (strncmp(string, NR_STRING, strlen(NR_STRING)) == 0))
	{
		return NR;
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

int assemble_cin(
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

int assemble_mov(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer)
{
	uint8_t argA, argC;

	argA = getRegisterEnumeration(arg[0]);
	argC = getRegisterEnumeration(arg[1]);

	write_buffer[0] = definition->opcode_mask;

	write_buffer[0] |= ( argA << 2) | ( argC << 6);
	
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

int stringToInteger(char* str)
{
	int return_value = -1;

	if (strncmp("0x", str, 2) == 0)
	{
		return_value = (int)strtol(str+2, NULL, 16);
	}
	else if (strncmp("0b", str, 2) == 0)
	{
		return_value = (int)strtol(str+2, NULL, 2);
	}
	else
	{
		return_value = atoi(str);
	}

	return return_value;
}

int assemble_lli(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer)
{
	int return_value = 0;
	
	int immediate_value = stringToInteger(arg[0]);

	if ((immediate_value & 0xF0) == 0)
	{
		write_buffer[0] = definition->opcode_mask | immediate_value << 2;

		return_value = 1;
	}
	else
	{
		printf("FATAL: Value provided is invalid: (%i)", immediate_value);
		printf("FATAL: Value must not be > 15");
	}

	return return_value;
}

int assemble_li(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer)
{
	int return_value = 0;
	
	int immediate_value = stringToInteger(arg[0]);

	if (immediate_value < 256 && immediate_value >= 0)
	{
		write_buffer[0] = definition->opcode_mask;
		write_buffer[1] = immediate_value;

		return_value = 1;
	}
	else
	{
		printf("FATAL: value provided is invalid %i\n", immediate_value);
		printf("FATAL: value must be < 256 && >= 0\n");
	}

	return return_value;
}

int assemble_lni(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer)
{
	int return_value = 0;

	int byte_len = 0;
	int bytes[16];

	char* byte_str = strtok(arg[0], ",");

	while (byte_str != NULL)
	{
		int value = stringToInteger(byte_str);

		if (value > 255 || value < 0)
		{
			byte_len = 0;
			printf("FATAL: value provided is invalid %i\n", value);
			break;
		}

		byte_str = strtok(NULL, ",");
		bytes[byte_len] = value;
		byte_len++;

		if (byte_len > 16)
		{
			byte_len = 0;
			printf("FATAL: byte array has too many elements. length must be <= 16\n");
			break;
		}
	}

	if (byte_len <= 1)
	{
		printf("FATAL: must provide byte array length > 1\n");
	}

	if (byte_len > 1)
	{
		definition->instructionLength = byte_len + 1;

		write_buffer[0] = definition->opcode_mask | (byte_len - 1) << 2;

		for (int i = 1; (i-1) < byte_len; i++)
		{
			write_buffer[i] = bytes[i - 1];
		}

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
	else
	{
		printf("Could not find label %s\n", arg[0]);
	}

	return return_value;
}

InstructionDefinition_t definitions[TOT_INSTRUCTIONS] =
{
	{ "nop",       0, 1, 0x00, assemble_0arg },
	{ "push",      0, 1, 0x93, assemble_0arg },
	{ "pop",       0, 1, 0xA3, assemble_0arg },
	{ "pcr",       0, 1, 0xB3, assemble_0arg },
	{ "sop_add",   0, 1, 0x00, assemble_0arg },
	{ "sop_sub",   0, 1, 0x10, assemble_0arg },
	{ "sop_and",   0, 1, 0x20, assemble_0arg },
	{ "sop_xor",   0, 1, 0x30, assemble_0arg },
	{ "sop_xnor",  0, 1, 0x40, assemble_0arg },
	{ "sop_cin",   0, 1, 0x50, assemble_0arg },
	{ "sop_lsh",   0, 1, 0x60, assemble_0arg },
	{ "sop_rsh",   0, 1, 0x70, assemble_0arg },
	{ "ptrinc",    0, 1, 0xF0, assemble_0arg },
	{ "lli",       1, 1, 0x03, assemble_lli },
	{ "lni",       1, 1, 0x43, assemble_lni },
	{ "li",        1, 2, 0x43, assemble_li  },
	{ "jmp",       1, 2, 0x83, assemble_jmp },
	{ "lb",        1, 1, 0x87, assemble_1arg },
	{ "sb",        1, 1, 0x8B, assemble_1arg },
	{ "sp",        1, 1, 0x8F, assemble_1arg },
	{ "cin",       2, 1, 0x02, assemble_cin },
	{ "mov",       2, 1, 0x00, assemble_mov },
	{ "cmp",       2, 1, 0xC3, assemble_2arg },
	{ "or",        3, 1, 0x00, assemble_3arg },
	{ "nand",      3, 1, 0x01, assemble_3arg },
	{ "op",        3, 1, 0x02, assemble_3arg }
};

InstructionDefinition_t* getInstructionFromOpcode(const char *opcode)
{
	InstructionDefinition_t* return_value = NULL;

	int i;
	for (i = 0; i < TOT_INSTRUCTIONS; i++)
	{
		if (strncmp(definitions[i].instructionLabel, opcode, strlen(definitions[i].instructionLabel)) == 0)
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

/* int labelprocess -> 1st pass over file, defines all labels
 */
int labelprocess(int line, int *address, char *label, char *opcode,
			   char *arg[3])
{

	int status = 1;

	if (process_label_initial(label, 0) == 0)
	{
		printf(
			"Error:%i: re-use of existing label '%s'\n",
				   line,                         label );

		status = 0;
	}

	return status;
}

/* int preprocess -> 2nd pass over file, links symbols to address
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
			printf("Something Weird!\n");
			break;
		}
	}
	else
	{
		printf("Error:%i: instruction '%s' does not exist\n",
					  line,            opcode);
		status = 0;
	}

	process_label_final(label, *address);

	if (status)
	{
		uint8_t test_buffer[32];
		status = ins->assemble(ins, arg, test_buffer);

		*address = *address + ins->instructionLength;
	}

	return status;
}

/* int process -> 3rd pass over file, instructions are turned into
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

void print_instruction_header(int label_width)
{
	char* label_str = (char*)malloc(sizeof(char) * (label_width + 1));
	for (int i = 0; i < label_width; i++)
	{
		label_str[i] = ' ';
	}
	label_str[label_width] = '\0';
	strncpy(label_str, "label", 5);

	printf("ln# [addr]:%s <op> <args>\n", label_str);
	free(label_str);
}

/* void print_instruction -> prints the instruction with some helpful information
   about the source line number, the physical address, the label the instruction has,
   and the args for that instruction
*/
void print_instruction(int line, int* address, char *label, char *opcode, char *arg[3], int label_width)
{
	int len = 0;
	for (int i = 0; arg[i] != NULL && i < 3; i++)
	{
		len += strlen(arg[i]) + 1;
	}
	char* arg_str = "";
	if (len > 0)
	{
		arg_str = (char*)malloc(sizeof(char) * len);

		for (int i = 0; i < len; i++)
		{
			arg_str[i] = '\0';
		}

		for (int i = 0; arg[i] != NULL && i < 3; i++)
		{
			sprintf(arg_str, "%s%s\t", arg_str, arg[i]);
		}	
	}

	char* label_str = (char*)malloc(sizeof(char) * (label_width + 1));
	for (int i = 0; i < label_width; i++)
	{
		label_str[i] = ' ';
	}
	label_str[label_width] = '\0';

	if (label != NULL)
	{
		strncpy(label_str, label, strlen(label));
	}

	printf("%03i [0x%02x]:%s %s\t%s\n", line, *address, label_str, opcode, arg_str);
	free(label_str);
	if (strlen(arg_str) != 0)
	{
		free(arg_str);
	}
}

int output_file(FILE* output, uint8_t* bytes, int size)
{
	fprintf(output, "v2.0 raw");

	for (int i = 0; i < size; i++)
	{
		fprintf(output, "%s%x", (i % 8 == 0) ? "\n" : " ", bytes[i]);
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
	FILE *inputf = NULL, *outputf = NULL;
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
	printf("Assembling tac file: '%s'\n", input);

	while (parse(&line_number, inputf, line, &label, &opcodes, args))
	{
		if (labelprocess(line_number, &address, label, opcodes, args) == 0)
		{
			printf("Labelprocess: Error on line #%i\n", line_number);

			goto CLOSEFILES;
		}
	}

	address = 0;
	line_number = 0;

	rewind(inputf);

	while (parse(&line_number, inputf, line, &label, &opcodes, args))
	{
		if (preprocess(line_number, &address, label, opcodes, args) == 0)
		{
			printf("Preprocess: Error on line #%i\n", line_number);

			goto CLOSEFILES;
		}
	}

	rewind(inputf);

	int full_size = address;

	if (full_size > MAX_PGR_SIZE)
	{
		printf("FATAL: Program exceeds maximum size!\n");
		goto CLOSEFILES;
	}
	
	uint8_t* w_buffer = (uint8_t*)malloc(sizeof(uint8_t) * full_size);

	if (w_buffer != NULL)
	{
		address = 0;
		line_number = 0;

		int label_width = longest_label();

		print_instruction_header(label_width);

		while (parse(&line_number, inputf, line, &label, &opcodes, args))
		{
			print_instruction(line_number - 1, &address, label, opcodes, args, label_width);

			if (process(line_number, &address, w_buffer, label, opcodes, args) == 0)
			{
				printf("Process: Error on line #%i\n", line_number);
				free(w_buffer);
				goto CLOSEFILES;
			}
		}

		// Output Logisim raw v2.0 format

		printf("Finished assembling tac file: '%s', program size: %i byte%s\n", input, full_size, (full_size > 1) ? "s":"");

		output_file(outputf, w_buffer, full_size);

		free(w_buffer);
	}

CLOSEFILES:
	if (inputf != NULL) fclose(inputf);
	if (outputf != NULL) fclose(outputf);

DITCH:
	return 1;
}
