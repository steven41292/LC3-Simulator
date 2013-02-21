/* LC3 function 
 * 
 * Name: Ryan Goodrich
 */

#include "lc3.h"

/** lc3_init
 *
 * Start the LC3 simulator and itialzie variables
 *
 * @param state The LC3 state machine
 */
void lc3_init(lc3machine* state) 
{
    state->pc = 0x3000;
    state->cc = 2;
    state->halted = 0;
}

/** lc3_load
 * 
 * Load the simulator with the contents of the program
 * 
 * NOTE: index 0 contains the starting location in memory
 *
 * @param state The LC3 state machine
 * @param program The file to be loaded
 */
void lc3_load(lc3machine* state, FILE* program)
{
    
    // start reading in the rest of the file
    int nextLetter;
    unsigned short memoryIndex = (fgetc(program) << 8) | fgetc(program);
    while ((nextLetter = fgetc(program)) != EOF) {
        
        // debuging
        //printf("%x \n", nextLetter << 8 | fgetc(program));
        
        // create instruction and add to list
        state->mem[memoryIndex ++] = nextLetter << 8 | fgetc(program);
    }
}

/** lc3_step_one
 *
 * Get the next instrution and execute it if not halted
 *
 * @param state LC3 simulator
 */
void lc3_step_one(lc3machine* state)
{
    // check if machine is running
    if (state->halted)
        return;
    
    // get the next instruciton
	unsigned short instruction = lc3_fetch(state);
    
    // run it
    lc3_execute(state, instruction);
}

void lc3_run(lc3machine* state, int num_steps)
{
    // set up an instruction counter
    int step = 0;
    
    // run until halted or reach number of steps
    while (!state->halted && (num_steps < 0 ? 1 : step < num_steps)) {
        lc3_step_one(state);
        step ++;
    }
}

/** lc3_fetch
 *
 * Get the next instruction to execute
 *
 * @param state The LC3 simulator
 */
unsigned short lc3_fetch(lc3machine* state)
{
    return state->mem[state->pc ++];
}

/** lc3_execute
 *
 * Execute the given instruction
 *
 * @param state LC3 simulator
 * @param instruction Command to execute
 */
void lc3_execute(lc3machine* state, unsigned short instruction)
{
    // create all the different variables that make up each command
    unsigned char strCommand = (instruction & 0xF000) >> 12;    
    unsigned char DR = (instruction & 0x0E00) >> 9;
    unsigned char SR1 = (instruction & 0x01C0) >> 6;
    unsigned char SR2 = instruction & 0x0007;
    char bit5 = (instruction & 0x20) >> 5;
    char bit9 = (instruction & 0x0200) >> 9;
    char bit10 = (instruction & 0x0400) >> 10;
    char bit11 = (instruction & 0x0800) >> 11;
    char imm5 = (instruction & 0x1F) | (instruction & 0x10 ? 0xF0 : 0);
    char offset6 = (instruction & 0x3F) | (instruction & 0x20 ? 0xE0 : 0);
    unsigned char offset8 = instruction & 0xFF;
    short offset9 = (instruction & 0x01FF) | (instruction & 0x100 ? 0xFF00 : 0);
    short offset11 = (instruction & 0x07FF) | (instruction & 0x400 ? 0xFC00 : 0);
    
    // figure out which command it is
    if (strCommand == 1) { // ADD
        state->regs[DR] = state->regs[SR1] + (bit5 ? imm5 : state->regs[SR2]);
        state->cc = state->regs[DR] > 0 ? 1 : (state->regs[DR] < 0 ? 4 : 2);
    }
    
    else if (strCommand == 5) { // AND
        state->regs[DR] = state->regs[SR1] & (bit5 ? imm5 : state->regs[SR2]);
        state->cc = state->regs[DR] > 0 ? 1 : (state->regs[DR] < 0 ? 4 : 2);
    }
    
    else if (strCommand == 0) { // BR
        if ((bit11 && state->cc == 4) || (bit10 && state->cc == 2) || (bit9 && state->cc == 1))
            state->pc += offset9;
    }
    
    else if (strCommand == 12) { // JMP
        state->pc = state->regs[SR1];
    }
    
    else if (strCommand == 4) { // JSR
        state->regs[7] = state->pc;
        state->pc = bit11 ? (state->pc + offset11) : state->regs[SR1];
    }
    
    else if (strCommand == 2) { // LD
        state->regs[DR] = state->mem[state->pc + offset9]; // take into account array starts at 1
        state->cc = state->regs[DR] > 0 ? 1 : (state->regs[DR] < 0 ? 4 : 2);
    }
    
    else if (strCommand == 10) { // LDI
        state->regs[DR] = state->mem[state->mem[state->pc + offset9]];
        state->cc = state->regs[DR] > 0 ? 1 : (state->regs[DR] < 0 ? 4 : 2);
    }
    
    else if (strCommand == 6) { // LDR
        state->regs[DR] = state->mem[state->regs[SR1] + offset6];
        state->cc = state->regs[DR] > 0 ? 1 : (state->regs[DR] < 0 ? 4 : 2);
    }
    
    else if (strCommand == 14) { // LEA
        state->regs[DR] = state->pc + offset9;
        state->cc = state->regs[DR] > 0 ? 1 : (state->regs[DR] < 0 ? 4 : 2);
    }
    
    else if (strCommand == 9) { // NOT
        state->regs[DR] = ~ state->regs[SR1];
        state->cc = state->regs[DR] > 0 ? 1 : (state->regs[DR] < 0 ? 4 : 2);
    }
    
    else if (strCommand == 3) { // ST
        state->mem[state->pc + offset9] = state->regs[DR];
    }
    
    else if (strCommand == 11) { // STI
        state->mem[state->mem[state->pc + offset9]] = state->regs[DR];
    }
    
    else if (strCommand == 7) { // STR
       state->mem[state->regs[SR1] + offset6] = state->regs[DR];
    }
    
    else if(strCommand == 15) { // TRAP
    	state->regs[7] = state->pc;
        lc3_trap(state, offset8);
    }
}

/** lc3_trap
 *
 * Execute the given trap
 *
 * @param state LC3 simulator
 * @param vector8 Trap to execute
 */
void lc3_trap(lc3machine* state, unsigned char vector8)
{
    if (vector8 == 0x20) { // GETC
        state->regs[0] = (short) getchar();
        getchar(); // absob \n
    }
    
    else if (vector8 == 0x21) { // OUT
        printf("%c", state->regs[0]);
    }
    
    else if (vector8 == 0x22) { // PUTS
        int index = state->regs[0];
        while (state->mem[index]) 
            printf("%c", state->mem[index ++]);
    }
    
    else if (vector8 == 0x23) { // IN
        printf(">> ");
        state->regs[0] = (short) getchar();
        getchar();
    }
    
    else if (vector8 == 0x24) { // PUTSP
        int index = state->regs[0];
        char first, second;
        do {
            first = state->mem[index] & 0xFF; // 7:0
            second = (state->mem[index] & 0xFF00) >> 8; // 15:8
            if (first)
                printf("%c", first);
            if (second)
                printf("%c", second);
            index ++;
        } while (second);
        
    }
    
    else if (vector8 == 0x25) { // HALT
        -- state->pc;
        state->halted = 1;
    }
}
