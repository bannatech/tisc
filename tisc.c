#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* The TISC Version */
#define VERSION_STRING "v2.4"
#define TOT_INSTRUCTIONS 46
#define MAX_SYMBOLS    1000
#define MAX_SYMBOL_LEN 100
#define MAX_LINE_LEN   256
#define MAX_PGR_SIZE   0xFFFF
#define MAX_LNI_SIZE   16
#define MAX_MNB_SIZE   16

#define EMPTY       ""
#define NR          0x0
#define NR_STRING   "NIL"
#define GR_A        0x1
#define GR_A_STRING "GRA"
#define GR_B        0x2
#define GR_B_STRING "GRB"
#define GR_C        0x3
#define GR_C_STRING "GRC"

#define INS_TOKENIZER " \t\n"
#define STR_TOKENIZER "\n"

int  validSymbols = 0;
int  addresses[MAX_SYMBOLS];
char symbols[MAX_SYMBOLS][MAX_SYMBOL_LEN];

typedef struct InstructionDefintion
{
	char* instructionLabel;
	int (*assemble)(struct InstructionDefintion*, char* arg[3], uint8_t*, int);
	uint8_t arguements;
	uint8_t instructionLength;
	uint8_t opcode_mask;
} InstructionDefinition_t;

typedef enum InsCheck
{
	TOOFEW,
	VALID,
	TOOMANY,
} InsCheck_t;

/* Checks if instruction is valid
 *
 *  return -1 -> too few arguements
 *  return  0 -> instruction is valid
 *  return  1 -> too many arguements
 */
InsCheck_t validins(InstructionDefinition_t *ins, const char *opcode, char *arg[])
{
	InsCheck_t status = VALID;

	int supp_args;

	for (supp_args = 0; supp_args < 3; supp_args++)
	{
		if (arg[supp_args] != NULL && strcmp(arg[supp_args], EMPTY) == 0)
		{
			break;
		}
	}

	int target_args = (ins->arguements < 4) ? ins->arguements : 1;

	if (target_args > supp_args)
	{
		status = TOOFEW; // Too few args, fail
	}
	else if (target_args < supp_args)
	{
		status = TOOMANY; // Too many args, fail
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
		if (strcmp(symbols[i], label) == 0)
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
		if (strcmp(symbols[i], label) == 0)
		{
			addresses[i] = address & 0xFF;
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
	if (string == NULL) {
		return 0xFF;
	}
	if (strncmp(string, GR_A_STRING, strlen(GR_A_STRING)) == 0)
	{
		return GR_A;
	}
	if (strncmp(string, GR_B_STRING, strlen(GR_B_STRING)) == 0)
	{
		return GR_B;
	}
	if (strncmp(string, GR_C_STRING, strlen(GR_C_STRING)) == 0)
	{
		return GR_C;
	}
	if (strncmp(string, NR_STRING, strlen(NR_STRING)) == 0)
	{
		return NR;
	}

	return 0xFF;
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

int assemble_0arg(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer,
	int address)
{
	write_buffer[0] = definition->opcode_mask;

	return 1;
}

int assemble_1arg(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer,
	int address)
{
	uint8_t argB = getRegisterEnumeration(arg[0]);

	write_buffer[0] = definition->opcode_mask | (argB << 4);

	return 1;
}

int assemble_1argc(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer,
	int address)
{
	uint8_t argC = getRegisterEnumeration(arg[0]);

	write_buffer[0] = definition->opcode_mask | (argC << 6);

	return 1;
}

int assemble_2arg(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer,
	int address)
{
	uint8_t argA, argB;

	argA = getRegisterEnumeration(arg[0]);
	argB = getRegisterEnumeration(arg[1]);

	write_buffer[0] = definition->opcode_mask;

	write_buffer[0] |= ( argA << 2 ) | ( argB << 4);

	return 1;
}

int assemble_2argb(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer,
	int address)
{
	uint8_t argB, argC;

	argB = getRegisterEnumeration(arg[0]);
	argC = getRegisterEnumeration(arg[1]);

	write_buffer[0] = definition->opcode_mask;

	write_buffer[0] |= ( argB << 4) | ( argC << 6);

	return 1;
}

int assemble_2arga(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer,
	int address)
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
	char* arg[3], uint8_t* write_buffer,
	int address)
{
	uint8_t argA, argB, argC;

	argA = getRegisterEnumeration(arg[0]);
	argB = getRegisterEnumeration(arg[1]);
	argC = getRegisterEnumeration(arg[2]);

	write_buffer[0] = definition->opcode_mask;

	write_buffer[0] |= ( argA << 2 ) | ( argB << 4) | ( argC << 6);

	return 1;
}

int assemble_mnb(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer,
	int address)
{
	int return_value = 0;

	int byte_length = stringToInteger(arg[0]);

	if (byte_length > 0 && byte_length <= MAX_MNB_SIZE)
	{
		write_buffer[0] = definition->opcode_mask | (byte_length - 1) << 2;

		return_value = 1;
	}
	else
	{
		printf("FATAL: Value provided is invalid: (%i)", byte_length);
		printf("FATAL: Value must be <= %i && > 0", MAX_MNB_SIZE);
	}

	return return_value;
}

int assemble_li(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer,
	int address)
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
	char* arg[3], uint8_t* write_buffer,
	int address)
{
	int return_value = 0;

	int byte_len = 0;
	int bytes[MAX_LNI_SIZE];
	int arg_str_len = strlen(arg[0]);
	if (arg_str_len == 0) {
		printf("FATAL: must provide byte array length > 1\n");
		return 1;
	}
	char* arg_str = (char*)malloc(sizeof(char) * arg_str_len + 1);
	char* byte_str = strcpy(arg_str, arg[0]);
	byte_str = strtok(byte_str, ",");
	while (byte_str != NULL)
	{
		int label = label_address(arg[0]);
		int value = -1;

		if (label != -1) {
			value = label;
		} else {
			value = stringToInteger(byte_str);
		}

		if (value > 255 || value < 0)
		{
			byte_len = -1;
			printf("FATAL: value provided is invalid %i\n", value);
			break;
		}

		byte_str = strtok(NULL, ",");
		if (byte_len <= MAX_LNI_SIZE)
		{
			bytes[byte_len] = value;
		}
		byte_len++;

	}
	free(arg_str);

	if (byte_len < 0) {
		printf("FATAL: error while parsing");
	}
	else if (byte_len <= 1)
	{
		printf("FATAL: must provide byte array length > 1\n");
	} else if ((address&0xFF) > ((address+byte_len)&0xFF)) {
		printf("FATAL: instruction overflows least significant address byte: %04x -> %04x\n", address, (address+byte_len));
		printf("FATAL: this may cause unintended behavior, realign instruction\n");
		return_value = 0;
	}
	else if (byte_len > 1 && byte_len <= MAX_LNI_SIZE)
	{
		definition->instructionLength = byte_len + 1;

		write_buffer[0] = definition->opcode_mask | (byte_len - 1) << 2;

		for (int i = 0; i < byte_len; i++)
		{
			write_buffer[i + 1] = bytes[i];
		}
		return_value = 1;
	}
	else
	{
		printf("FATAL: byte array has too many elements. length must be <= %i\n", MAX_LNI_SIZE);
		printf("FATAL:                                           length  = %i\n", byte_len);
	}

	return return_value;
}

int assemble_jmp(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer,
	int address)
{
	int return_value = 0;

	int jump_address = label_address(arg[0]);

	if (jump_address != -1)
	{
		write_buffer[0] = definition->opcode_mask;
		write_buffer[1] = jump_address;

		return_value = 1;
	}
	else
	{
		printf("Could not find label %s\n", arg[0]);
	}

	return return_value;
}

int zero(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer,
	int address)
{
	int return_value = 0;

	uint8_t argA;

	argA = getRegisterEnumeration(arg[0]);

	write_buffer[0] = definition->opcode_mask;

	write_buffer[0] |= ( argA << 2) | ( argA << 6);

	return 1;
}

int space(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer,
	int address)
{
	int return_value = 0;
	int target_address = stringToInteger(arg[0]);

	if (target_address < MAX_PGR_SIZE && (target_address > address))
	{
		definition->instructionLength = target_address - address;
		return_value = 1;
	}
	else if (target_address <= address) {
		printf("Space preceeds current program address (%i)\n", address);
	}
	else
	{
		printf("address must be less than %i\n", MAX_PGR_SIZE);
	}

	return return_value;
}

int getlabel(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer,
	int address)
{
	int return_value = 0;

	int label = label_address(arg[0]);

	if (label != -1)
	{
		write_buffer[0] = definition->opcode_mask;
		write_buffer[1] = label;

		return_value = 1;
	}
	else
	{
		printf("Could not find label %s\n", arg[0]);
	}

	return return_value;
}

int load_string(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer,
	int address, int direction)
{
	int return_value = 0;

	char* raw_value = arg[0];
	int raw_len = strlen(raw_value);

	int str_len = 0;
	for (int i = 0; i < raw_len; i++)
	{
		if (raw_value[i] != '\\')
		{
			str_len += 1;
		}
	}

	if (str_len < MAX_LNI_SIZE)
	{
		definition->instructionLength = 1 + str_len;
	}
	// if our last "lni" instruction is going to only move one byte,
	// it will end up in GRA, forcing us to add an extra instruction
	else if (str_len % (MAX_LNI_SIZE) == 1)
	{
		definition->instructionLength = 2 + str_len + (str_len / MAX_LNI_SIZE);
	}
	else
	{
		definition->instructionLength = 1 + str_len + (str_len / MAX_LNI_SIZE);
	}

	if (str_len == 0)
	{
		printf("FATAL: must provide string\n");
	}
	else if ((address&0xFF) > ((address+definition->instructionLength)&0xFF))
	{
		printf("FATAL: instruction overflows least significant address byte: %04x -> %04x\n", address, (address+str_len));
		printf("FATAL: this may cause unintended behavior, realign instruction\n");
	}
	else if (str_len > 1)
	{
		char *value;
		if (raw_len != str_len)
		{
			value = (char*)malloc(sizeof(char) * str_len);
			int val_i = 0;
			for (int i = 0; i < raw_len; i++)
			{
				if (raw_value[i] != '\\')
				{
					value[val_i] = raw_value[i];
					val_i += 1;
				}
				else if (i + 1 < raw_len)
				{
					char code = raw_value[i + 1];
					switch (code) {
					case '0':
						value[val_i] = '\0';
						break;
					case 'n':
						value[val_i] = '\n';
						break;
					case 'r':
						value[val_i] = '\r';
						break;
					default:
						value[val_i] = code;
						break;
					}
					val_i += 1;
					i++;
				}
			}
		}
		else {
			value = raw_value;
		}

		int buf_i = 0;
		for (int i = 0; i < str_len; i += MAX_LNI_SIZE)
		{
			int write_len = str_len - i;
			if (write_len > MAX_LNI_SIZE)
			{
				write_len = MAX_LNI_SIZE;
			}

			write_buffer[buf_i] = definition->opcode_mask | (write_len - 1) << 2;

			buf_i += 1;

			for (int x = 0; x < write_len; x++)
			{
				int v_i = (direction) ? str_len - 1 - (i + x) : i + x;
				write_buffer[buf_i] = (uint8_t)value[v_i];
				buf_i += 1;
			}

			// push the last remaining byte, because the value is at GRA
			if (write_len == 1)
			{
				write_buffer[buf_i] = 0x93;
			}

		}

		if (raw_len != str_len)
		{
			free(value);
		}

		return_value = 1;
	}
	else
	{
		printf("FATAL: must provide string length > 1\n");
	}

	return return_value;
}

int load_string_f(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer,
	int address)
{
	return load_string(definition, arg, write_buffer, address, 1);
}

int load_string_r(
	InstructionDefinition_t *definition,
	char* arg[3], uint8_t* write_buffer,
	int address)
{
	return load_string(definition, arg, write_buffer, address, 0);
}

InstructionDefinition_t definitions[TOT_INSTRUCTIONS] =
{
	// abstract instructions - assembler specific
	//opcode,     assembler,   args,len,byte
	{ "string",   load_string_f , 4, 1, 0x43, },
	{ "string_r", load_string_r , 4, 1, 0x43, },
	{ "getlabel", getlabel      , 1, 2, 0x43, },
	{ "space",    space         , 1, 0, 0x00, },
	{ "zero",     zero          , 1, 1, 0x01, },
	{ "mov",      assemble_2arga, 2, 1, 0x00, },
	// 1:1 instructions
	//opcode,       assembler,  args,len,byte
	{ "push",       assemble_0arg , 0, 1, 0x93, },
	{ "pop",        assemble_0arg , 0, 1, 0xA3, },
	{ "peek",       assemble_0arg , 0, 1, 0xB3, },
	{ "lbs",        assemble_0arg , 0, 1, 0x87, },
	{ "sbs",        assemble_0arg , 0, 1, 0x8B, },
	{ "sps",        assemble_0arg , 0, 1, 0x8F, },
	{ "sop_add",    assemble_0arg , 0, 1, 0x00, },
	{ "sop_sub",    assemble_0arg , 0, 1, 0x10, },
	{ "sop_and",    assemble_0arg , 0, 1, 0x20, },
	{ "sop_xor",    assemble_0arg , 0, 1, 0x30, },
	{ "sop_pcnt",   assemble_0arg , 0, 1, 0x40, },
	{ "sop_cin",    assemble_0arg , 0, 1, 0x50, },
	{ "sop_lsh",    assemble_0arg , 0, 1, 0x60, },
	{ "sop_rsh",    assemble_0arg , 0, 1, 0x70, },
	{ "goto",       assemble_0arg , 0, 1, 0x80, },
	{ "pcr",        assemble_0arg , 0, 1, 0x90, },
	{ "seg",        assemble_0arg , 0, 1, 0xA0, },
	{ "mnb_noincr", assemble_0arg , 0, 1, 0xB0, },
	{ "mnb_incr",   assemble_0arg , 0, 1, 0xC0, },
	{ "mnb_decr",   assemble_0arg , 0, 1, 0xD0, },
	{ "mnb_store",  assemble_0arg , 0, 1, 0xE0, },
	{ "mnb_load",   assemble_0arg , 0, 1, 0xF0, },
	{ "mnb",        assemble_mnb  , 4, 1, 0x03, },
	{ "lni",        assemble_lni  , 1, 1, 0x43, },
	{ "li",         assemble_li   , 1, 2, 0x43, },
	{ "jmp",        assemble_jmp  , 1, 2, 0x83, },
	{ "lb",         assemble_1arg , 1, 1, 0x87, },
	{ "sb",         assemble_1arg , 1, 1, 0x8B, },
	{ "sp",         assemble_1arg , 1, 1, 0x8F, },
	{ "cmp",        assemble_2arg , 2, 1, 0xC3, },
	{ "or",         assemble_3arg , 3, 1, 0x00, },
	{ "nand",       assemble_3arg , 3, 1, 0x01, },
	{ "and",        assemble_2argb, 2, 1, 0x01, },
	{ "xor",        assemble_2arga, 2, 1, 0x01, },
	{ "ptradd",     assemble_1argc, 1, 1, 0x01, },
	{ "op",         assemble_3arg , 3, 1, 0x02, },
	{ "incr",       assemble_2argb, 2, 1, 0x02, },
	{ "decr",       assemble_2arga, 2, 1, 0x02, },
	{ "one",        assemble_1argc, 1, 1, 0x02, },
};

InstructionDefinition_t* getInstructionFromOpcode(const char *opcode)
{
	InstructionDefinition_t* return_value = NULL;

	int i;
	for (i = 0; i < TOT_INSTRUCTIONS; i++)
	{
		char *label = definitions[i].instructionLabel;
		if (strlen(label) == strlen(opcode) &&
		    strncmp(label, opcode, strlen(label)) == 0)
		{
			return_value = &definitions[i];
			break;
		}
	}

	return return_value;
}

void printnear(int target, FILE *file, char *line)
{
	int start = target - 1;
	int end = target + 1;
	char *str;
	int ln = 0;

	long pos = ftell(file);
	rewind(file);

	do {
		str = fgets(line, MAX_LINE_LEN, file);
		ln++;
		if (str != NULL && (ln >= start && ln <= end))
		{
			int it = ln == target;
			printf(" %s%04i | %s", (it) ? "---> ":"     ", ln, str);
		}
	} while (str != NULL);
	printf("\n");
	if (pos != -1L) {
		fseek(file, pos, SEEK_SET);
	}
}

/* char *parse -> parses file and sets label, opcode, and args
   accordingly */
int parse(int* line_number, FILE *file, char *line, char **label, char **opcode, char *arg[3])
{
	*opcode = NULL;
	*label  = NULL;
	arg[0]  = EMPTY;
	arg[1]  = EMPTY;
	arg[2]  = EMPTY;

	int success, loop = 0;
	char *str, *first;
	do {
		str = fgets(line, MAX_LINE_LEN, file);
		*line_number = *line_number + 1;
		if (str != NULL)
		{
			first = strtok(line, INS_TOKENIZER);
		}
		success = str != NULL;
		loop = success && (first == NULL || first[0] == '#');
	} while (loop == 1);

	if (success && first != NULL)
	{
		// We determine if we are working with a label, instead of an opcode
		if (first[strlen(first) - 1] == ':')
		{
			*opcode = strtok(NULL, INS_TOKENIZER);
			if (*opcode != NULL)
			{
				first[strlen(first) - 1] = '\0';
			}
			*label = first;
		}
		else
		{
			*label = NULL;
			*opcode = first;
		}

		if (*opcode != NULL)
		{
			InstructionDefinition_t *ins = getInstructionFromOpcode(*opcode);
			if (ins != NULL)
			{
				if (ins->arguements == 4)
				{
					arg[0] = strtok(NULL, STR_TOKENIZER);
				} else {
					for (int i = 0; i < ins->arguements; i++)
					{
						arg[i] = strtok(NULL, INS_TOKENIZER);
					}
				}
			}
		}
	}

	return success;
}

/* int labelprocess -> 1st pass over file, defines all labels
 */
int labelprocess(int line, int *address, char *label, char *opcode, char *arg[3])
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
int preprocess(int line, int *address, char *label, char *opcode, char *arg[3])
{
	int status = 0;

	if (opcode != NULL)
	{
		InstructionDefinition_t* ins = getInstructionFromOpcode(opcode);

		if (ins != NULL)
		{
			switch (validins(ins, opcode, arg))
			{
			case TOOFEW:
				printf("Error:%i: too few arguements for '%s', expecting %i\n",
				              line,                       opcode,        ins->arguements);
				status = 0;
				break;
			case TOOMANY:
				printf("Error:%i: too many arguements for '%s', expecting %i\n",
				              line,                        opcode,        ins->arguements);
				status = 0;
				break;
			case VALID:
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
			uint8_t test_buffer[256];
			status = ins->assemble(ins, arg, test_buffer, *address);

			*address = *address + ins->instructionLength;
		}
	}
	else
	{
		printf("Error:%i: Unspecified opcode\n", line);
		status = 0;
	}

	return status;
}

/* int process -> 3rd pass over file, instructions are turned into
   machine code with symbols filled in as adresses,
   fails if returns -1 */
int process(
	int line, int* address, uint8_t *buffer, char *label, char *opcode, char *arg[3])
{
	int status = 0;

	InstructionDefinition_t* ins = getInstructionFromOpcode(opcode);

	if (ins != NULL && ins->assemble != NULL)
	{
		status = ins->assemble(ins, arg, buffer + *address, *address);
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

	printf("%s line  addr:out>op \targs\n", label_str);
	free(label_str);
}

/* void print_instruction -> prints the instruction with some helpful information
   about the source line number, the physical address, the label the instruction has,
   and the args for that instruction
*/
void print_instruction(int line, int begin_address, int end_address, uint8_t *buffer, char *label, char *opcode, char* args[3], int label_width)
{
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

	int this_size = (int)(end_address - begin_address);
	uint8_t *this_buffer = buffer + begin_address;

	printf("%s %04i  %04x:%02x| %s  %s %s %s\n", label_str, line + 1, begin_address, this_buffer[0], opcode, args[0], args[1], args[2]);
	int eqflag = 0;
	for (int i = 0; i < label_width; i++)
	{
		label_str[i] = ' ';
	}
	label_str[label_width] = '\0';
	for (int i = 1; i < this_size; i++)
	{
		int dupe = i > 1 && i + 1 < this_size && this_buffer[i-1] == this_buffer[i] && this_buffer[i] == this_buffer[i+1];
		if (eqflag == 0 || dupe == 0) {
			printf("%s       %04x:%02x|%s\n", label_str, begin_address + i, this_buffer[i], (dupe) ? " ~" : "");
		}
		eqflag = dupe;
	}

	free(label_str);
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

	char *input = argv[1];
	printf("Assembling tac file: '%s' TISC %s\n", input, VERSION_STRING);
	FILE *inputf = fopen(input, "r");
	if (inputf == NULL || ferror(inputf))
	{
		printf("Error opening file '%s'\n", input);
		goto DITCH;
	}

	char *output = argv[2];
	FILE *outputf = fopen(output, "w");
	if (outputf == NULL || ferror(outputf))
	{
		printf("Error opening file '%s'\n", output);
		goto DITCH;
	}

	char *label   = EMPTY;
	char *opcodes = EMPTY;
	char *args[3] = { EMPTY, EMPTY, EMPTY };
	char line[MAX_LINE_LEN + 1];
	for (int i = 0; i < MAX_LINE_LEN+1; i++) {
		line[i] = '\0';
	}

	int address     = 0;
	int line_number = 0;
	int label_err   = 0;
	while (parse(&line_number, inputf, line, &label, &opcodes, args))
	{
		if (labelprocess(line_number, &address, label, opcodes, args) == 0)
		{
			printnear(line_number, inputf, line);
			label_err = 1;
		}
	}

	address = 0;
	line_number = 0;
	rewind(inputf);

	int preprocess_err = 0;
	while (parse(&line_number, inputf, line, &label, &opcodes, args))
	{
		if (preprocess(line_number, &address, label, opcodes, args) == 0)
		{
			printnear(line_number, inputf, line);
			label_err = 1;
		}
	}
	if (preprocess_err == 1 || label_err == 1)
	{
		goto CLOSEFILES;
	}

	rewind(inputf);

	int full_size = address;

	if (full_size > MAX_PGR_SIZE)
	{
		printf("FATAL: Program exceeds maximum size!\n");
		printf("FATAL: program size=%i max=%i\n", full_size, MAX_PGR_SIZE);
		goto CLOSEFILES;
	}

	uint8_t* w_buffer = (uint8_t*)malloc(sizeof(uint8_t) * full_size);
	for (int i = 0; i < full_size; i++) {
		w_buffer[i] = 0;
	}

	if (w_buffer != NULL)
	{
		address = 0;
		line_number = 0;

		int label_width = longest_label();
		print_instruction_header(label_width);

		int process_err = 0;
		while (parse(&line_number, inputf, line, &label, &opcodes, args))
		{
			int begin_address = address;
			int begin_line = line_number;
			if (process(line_number, &address, w_buffer, label, opcodes, args) == 0)
			{
				printnear(line_number, inputf, line);
				printf("Process: Error on line #%i\n", line_number);
				process_err = 1;

				continue;
			}

			print_instruction(begin_line - 1, begin_address, address, w_buffer, label, opcodes, args, label_width);
		}
		if (process_err == 1)
		{
			free(w_buffer);
			goto CLOSEFILES;
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
