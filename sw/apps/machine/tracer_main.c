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

int main (int argc, char** argv) {
  enable_tracer();
  while(!tracer_activated());
  asm(
    "li t0, 5\n"
    "li t1, 1\n"
    "loop:\n"
    "sub t0, t0, t1\n"
    "nop\n"
    "bnez t0, loop\n"
    "loop_exit:\n"
    "nop\n"
    "nop\n"
  );
  disable_tracer();

  int beginning_address = tracer_starting_address();
  int current_address = tracer_current_address();
  int ending_address = tracer_ending_address();
  //temporary
  printf("Written data in AXI bus from tracer at address :\n");
  printf("from  : 0x%08x\n", beginning_address);
  printf("to    : 0x%08x\n", current_address);
  printf("limit : 0x%08x\n", ending_address);
  
  //printf("Hello World!\r\n");
/* ToDo:
    - get last written addr value
    - read traces from base_traces_addr to last written addr 
    - compare to expected value
*/
  return 0;
};

