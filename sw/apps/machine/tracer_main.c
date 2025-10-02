// ########################################################################
// Compiled using riscv-tests toolchain
// include it in a folder in riscv-tests/benchmarks (tracer)
// update riscv-tests/benchmarks/Makefile with the folder name
//
// C program that enable the tracer, run known instructions,
// Then disable the tracer and print the value of the ladst written byte
//
// Maxime Travaillard
// ########################################################################

// ToDo: Implement comparison between written traces and expected traces

#include "tracer.h"
#include <stdint.h>
#include "util.h"

void tracer_value(uint64_t value);

int main (int argc, char** argv) {
  enable_tracer(); // make sure that trace_activated_q reset to 0 in te_reg.sv (line 776)
  while(!tracer_activated());
  asm(
    "li t0, 6\n"
    "li t1, 1\n"
    "li t2, 2\n"
    "li t3, 20\n"
    "li t4, 20\n"
    "loop:\n"
    "sub t0, t0, t1\n"
    "sub t3, t3, t1\n"
    "sub t4, t4, t2\n"
    "sub t5, t3, t4\n"
    "bnez t0, loop\n"
    "loop_exit:\n"
    "nop\n"
    "nop\n"
  );
  disable_tracer();
  while(!cause_filter_activated());

  int beginning_address = tracer_starting_address();
  int current_address = tracer_current_address();
  int ending_address = tracer_ending_address();
  int last_address = tracer_current_address();
  while(last_address != current_address) {
    last_address = current_address;
    current_address = tracer_current_address();
  }

  //temporary
  printf("Written data in AXI bus from tracer at address :\n");
  printf("from  : 0x%08x\n", beginning_address);
  printf("to    : 0x%08x\n", last_address);
  printf("limit : 0x%08x\n", ending_address);

  printf("TRACES :\n");
  printf("address   -        value       -  type   -   information\n");

  uint64_t value;
  uint64_t value_l;
  uint64_t value_m;

  for(int i = 0; i <= (last_address-beginning_address); i += 8) {
    printf("%08x  -  ", beginning_address + i);
    if((i == 0x018) || (i == 0x078) || (i == 0x078)) value = 0; // problem with those values...
    else {
      value_l = (uint64_t)*reg32((void*)beginning_address, i);
      value_m = (uint64_t)*reg32((void*)beginning_address, i+4);
      value = (value_m << 32) | value_l;
    }
    printf("%016llx  ", value);
    tracer_value(value);
    printf("\n");
  }

  return 0;
};

  


void tracer_value(uint64_t value) {
  int f = (value & 0x3FF) >> 8; // format
  int sf_f = (value & 0xfff) >> 8; // subformat and format
    switch (f) {
      case 0:
        if((value >> 8) == 0) break;

        switch (sf_f) {
          case 0x00:
            printf("- F0SF0");
            break;
          case 0x04:
            printf("- F0SF1");
            break;
        }
        break;

      case 1:
          printf("- F1 - DELTA DIFF");
          break;

      case 2:
          printf("- F2");
          break;

      case 3:
          //scanf("%d", &subChoice);

          switch (sf_f) {
            case 0x03:
              printf("- F3SF0 - 1st branch");
              printf(" (%016llx)", value >> 15);
              break;
            case 0x07:
              printf("- F3SF1");
              break;
            case 0x0b:
              printf("- F3SF2");
              break;
            case 0xf:
              printf("- F3SF3");
              break;
          }
          break;
  }
}



/* ToDo: 
    - compare to expected value

  ###############################################

  // mask value is 0x03ff, because we are looking for a value of 0x300
  // This is the case for F1 trace => branch diff delta*/
  /*
  current_address = beginning_address;
  while(value != 0x00000100 || current_address <= last_address) {
    current_address += 8;
    value = *reg32((void*)current_address, 0) & 0x000003ff;  
  }
  printf("BRANCH DIFF DELTA FOUND\n");

  ################################################

  it's possible to do the same for other value :
  mask : 0x0fff (12 LSB)
  looking for value :
  0xf00 : SF3F3 (sync signals) => when tracer enable/disable (400f00/401f00)
  0x300 ; SF0F3 => sent by tracer when jumping to main
    * to get main address, mask 0x0000FFFFFFF00000 and do >> 20 
*/