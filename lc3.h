// DO NOT CHANGE ANY PROTOTYPE NAMES
// OR ARGUMENTS PASSED IN TO ANY PROTOTYPE HERE.

// YOU MAY ADD ADDITIONAL FUNCTIONS BUT YOU MUST IMPLEMENT THE ONES BELOW

#include <stdio.h>

enum lc3_cc
{
	LC3_NEGATIVE = 4,
	LC3_ZERO = 2,
	LC3_POSITIVE = 1
};

/**
 * A structure that represents the state of the LC3 machine.
 */
typedef struct 
{
	short mem[65536]; /* Memory */
	short regs[8]; /* The eight registers in the LC-3 */
	unsigned short pc; /* The pc register */
	unsigned char cc; /* The condition code register the value will be one of the enum values above */
	int halted; /* 0 if the machine is running 1 if the machine is halted */
} lc3machine;

/* lc3_init
   This function initializes the lc3machine to its initial state
   The pc should be set to 3000 (hex)
   and the conditional code register should be set to indicate zero (LC3_ZERO)
*/
void lc3_init(lc3machine* state);

/* lc3_load
   This function is responsible for reading a .obj file
   produced from lc3as the format is explained in the
   assignment */
void lc3_load(lc3machine* state, FILE* program);

/* lc3_step_one
   This function just does one simulation step (goes through steps to fetch
   and execute one instruction */
void lc3_step_one(lc3machine* state);

/* lc3_run
   This function will run for the number of steps given or until you halt.
   @param num_steps number of steps to run (-1 for infinite)
*/
void lc3_run(lc3machine* state, int num_steps);

/* lc3_fetch
   Fetches an instruction from memory BE SURE to do everything that is needed
   to be done in the fetch stage
   @return the fetch'd instruction. */
unsigned short lc3_fetch(lc3machine* state);

/* lc3_execute
   Execution function. in charge of executing the instruction given
   @param instruction instruction to execute
*/
void lc3_execute(lc3machine* state, unsigned short instruction);


/* lc3_trap
   Trap execution function called from within lc3_execute.
*/
void lc3_trap(lc3machine* state, unsigned char vector8);
