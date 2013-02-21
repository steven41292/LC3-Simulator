/* LC3 simulator file.
 * Complete the functions!
 * This is a front-end to the code you wrote in lc3.c
 *
 * Name: Ryan Goodrich
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lc3.h"

void cmd_runprompt(lc3machine* mach, char *);
void cmd_registers(lc3machine* mach);
void cmd_dump(lc3machine* mach, int start, int end);
void cmd_setaddr(lc3machine* mach, int address, short value);
void cmd_setreg(lc3machine* mach, int reg, short value);

/* FIXME: change this to be a good prompt string */
#define PROMPT "(LC3) "

int main(int argc, char **argv) {
	FILE *prog;
	lc3machine mach;

	/* We expect only one argument for the program... */
	if (argc != 2) {
		fprintf(stderr, "Usage: %s file.obj\n", argv[0]);
		return 1;
	}
	/* We want to open the file and make sure that it exists. */
	prog = fopen(argv[1], "rb");
	if (!prog) {
		fprintf(stderr, "Could not find file %s\n", argv[1]);
		return 2;
	}

	/* Make a call to lc3_init to initialize your lc3machine */
    lc3_init(&mach);
    
	/* Make a call to lc3_load to load the program */
    lc3_load(&mach, prog);
    fclose(prog);
    
	/* Print out start of program */
	printf("LC-3 simulator and debugger\n");
	printf("Written by Ryan Goodrich\n");

	/* Run this loop until we are told to stop debugging. */
    unsigned char emptyPrompt = 0;
    char userInput[30];
    int letter;
    int inputIndex = 0;
	while (1) {
		
        // check if there is a prompt on the screen
        if (!emptyPrompt)
            printf("%s", PROMPT), emptyPrompt = 1;
        
        else {
            
            // get the user input
            while ((letter = getchar()) != '\n')
                userInput[inputIndex < 30 ? inputIndex ++ : inputIndex] = letter;
            
            // check if input is exceeded buffer
            if (inputIndex > 29)
                printf("ERR: Input overflow\n");
            
            // run command if valid
            else {
                
                // clean out the rest of the buffer
                if (inputIndex) {
                    for (int i = inputIndex; i < 30; i ++)
                        userInput[i] = 0;
                }
                    
                // run command
                cmd_runprompt(&mach, userInput);
            }
            
            // rest input and prompt
            emptyPrompt = inputIndex = 0;
        }
	}

	return 0;
}

/** cmd_runprompt
 *
 * Determine what the user input is and run 
 *
 * @param mach A pointer to the lc3 simulator
 * @param prompt A pointer to the string the user provided
 * @param lastCommand The last command index to be run
 */
void cmd_runprompt(lc3machine* mach, char * prompt) 
{
    // copy prompt
    char promptCopy[30];
    strcpy(promptCopy, prompt);
    
    // break appart command by spaces
    char * command;
    command = strtok(promptCopy, " ");
    
    // check if command is a quit
    if (strcmp(command, "quit") == 0 || strcmp(command, "q") == 0)
        exit(0);
    
    // check if command is to step
    else if (strcmp(command, "step") == 0 || strcmp(command, "s") == 0) {
        
        // get number of lines to step
        command = strtok(NULL, " ");
        int lines = command == NULL ? 1 : atoi(command);
        lc3_run(mach, lines);
    }
    
    // check if command is continue
    else if (strcmp(command, "continue") == 0 || strcmp(command, "c") == 0 || strcmp(command, "run") == 0) {
        lc3_run(mach, -1);
    }
    
    // check if want the registers
    else if (strcmp(command, "registers") == 0 || strcmp(command, "r") == 0) {
        cmd_registers(mach);
    }
    
    // check if command is to dump memory
    else if (strcmp(command, "dump") == 0) {
        
        // check if the command is valid
        if ((command = strtok(NULL, " ")) == NULL) {
            printf("ERR: Input invalid: Starting memory index required\n");
            return;
        }
        
        // get starting location
        char * pEnd;
        int dumpStart = (int) strtol(command, &pEnd, 16);
        
        // get ending locaiton
        command = strtok(NULL, " ");
        int dumpEnd = command == NULL ? dumpStart : ((int) strtol(command, &pEnd, 16));
        
        // dump the core
        cmd_dump(mach, dumpStart, dumpEnd);
    }
    
    // check if command is to set memory
    else if (strcmp(command, "setaddr") == 0) {
        
        // check if the command is valid
        if ((command = strtok(NULL, " ")) == NULL) {
            printf("ERR: Input invalid: Memory address required\n");
            return;
        }
        
        // get address
        char * pEnd;
        int address = (int) strtol(command, &pEnd, 16);
        
        // check if the command is valid
        if ((command = strtok(NULL, " ")) == NULL) {
            printf("ERR: Input invalid: Memory value required\n");
            return;
        }
        
        // set value
        cmd_setaddr(mach, address, atoi(command));
    }
    
    // check if command is to set memory
    else if (strcmp(command, "setreg") == 0) {
        
        // check if the command is valid
        if ((command = strtok(NULL, " ")) == NULL) {
            printf("ERR: Input invalid: Register number required\n");
            return;
        }
        
        // get register
        int reg = atoi(command + 1);
        
        // check if the command is valid
        if ((command = strtok(NULL, " ")) == NULL) {
            printf("ERR: Input invalid: Register value required\n");
            return;
        }
        
        // set value
        cmd_setreg(mach, reg, atoi(command));
    }
    
    // check if command is for help
    else if (strcmp(command, "help") == 0) {
        printf("step [n]\n\tExecutes n instructions (n defaults to 1)\nquit\n\tQuits the simulator\ncontinue\n\tRuns until the program halts\nregisters\n\tPrints all registers, pc, and cc values\ndump start [end]\n\tDumps the contents of memory from the starting address to the end (if not provided will just print one value)\nsetaddr addr value\n\tSets the value at the provided address\nsetreg Rn value\n\tSets register n with the provided value\nhelp\n\tDisplays this menu\n");
    }
    
    // command not found
    else
        printf("ERR: Input invalid. Type 'help' for a list of instructions\n");
}

/** cmd_registers
 *
 * Prints out the current register values, pc value and conditional code
 *
 * @param mach The LC3 Simulator
 */
void cmd_registers(lc3machine* mach)
{
    printf("CC: %s\n", mach->cc == 1 ? "p" : (mach->cc == 2 ? "z" : "n"));
    printf("PC: 0x%04X %d\n", mach->pc, mach->pc);
    for (int i = 0; i < 8; i++)
        printf("R%d: 0x%04X %d\n", i, (unsigned short) mach->regs[i], mach->regs[i]);
}

/* cmd_dump
 *
 * Prints out the memory from start to end
 *
 * @param mach The LC3 Simulator
 * @param start The integer value of the memory location
 * @param end The integer value of the ending locaiton
 *      if end = -1 all memory from start will be printed
 */
void cmd_dump(lc3machine* mach, int start, int end)
{
    // print out memory at each location
    printf("|--------------------------------------|\n");
    printf("|  ADDR  | VALUES (4)                  |\n");
    printf("|--------------------------------------|\n");
    end = end < 0 ? 65536 : end + 1;
    for (int i = start; i < end; i += 4) {
        
        // print out each line
        printf("| 0x%04X | ", (unsigned short) i);
        printf("0x%04X ", (unsigned short) mach->mem[i]);
        if (i + 1 < end)
            printf("0x%04X ", (unsigned short) mach->mem[i + 1]);
        else
            printf("       ");
        if (i + 2 < end)
            printf("0x%04X ", (unsigned short) mach->mem[i + 2]);
        else
            printf("       ");
        if (i + 3 < end)
            printf("0x%04X ", (unsigned short) mach->mem[i + 3]);
        else
            printf("       ");
        printf("|\n");
    }
    printf("|--------------------------------------|\n");
}

/** cmd_setaddr
 * 
 * Sets the value of memory at a given address
 *
 * @param mach The LC3 Simulator
 * @param address The memory locaiton to be set
 * @param value The value to be set 
 */
void cmd_setaddr(lc3machine* mach, int address, short value)
{
    mach->mem[address] = value;
}

/** cmd_setreg
 *
 * Sets the register to be the value
 *
 * @param mach The LC3 Simulator
 * @param reg The register number
 * @param value The value to be set
 */
void cmd_setreg(lc3machine* mach, int reg, short value)
{
    mach->regs[reg] = value;
}

