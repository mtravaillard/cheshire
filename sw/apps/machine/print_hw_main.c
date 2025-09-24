// #############################################################
// Compiled using riscv-tests toolchain
// Include it in a folder in riscv-tests/benchmarks (print_hw)
// Update riscv-tests/benchmarks/Makefile with the folder name
//
// Basic printf using syscall with fesvr, defined in riscv-tets 
//
// Maxime Travaillard
// #############################################################

#pragma GCC optimize ("no-inline")

#include "util.h"

#include <alloca.h>

int main (int argc, char** argv) {
  printf("Hello World!\r\n");

  return 0;
}

