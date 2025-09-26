// Compiled using riscv-tests toolchain
// include it in a folder in riscv-tests/benchmarks (tracer)
// update riscv-tests/benchmarks/Makefile with the folder name

#include <stdint.h>
#include <stdbool.h>

static void *const __base_tracer = (void *)0x0300a000;
static void *const __base_regs   = (void *)0x03000000;

//#define __base_tracer *0x0300a000 // change this to grab it from the symbols

// !!! 64-bit Architecture Specific Register Offsets
// 32 bits xlen makes registers offset changes.
/* FILTER */
// input and mode
#define CAUSE_ENABLE_MODE   0x00
#define TVEC_ENABLE_MODE    0x04
#define TVAL_ENABLE_MODE    0x08
#define PRIV_ENABLE_MODE    0x0c
#define IADDR_ENABLE_MODE   0x10
// cause
#define CAUSE_UPPER         0x14
#define CAUSE_LOWER         0x18
#define CAUSE_MATCH         0x1c
// tvec
#define TVEC_UPPER_L        0x20
#define TVEC_UPPER_M        0x24
#define TVEC_LOWER_L        0x28
#define TVEC_LOWER_M        0x2c
#define TVEC_MATCH_L        0x30
#define TVEC_MATCH_M        0x34
// tval
#define TVAL_UPPER_L        0x38
#define TVAL_UPPER_M        0x3c
#define TVAL_LOWER_L        0x40
#define TVAL_LOWER_M        0x44
#define TVAL_MATCH_L        0x48
#define TVAL_MATCH_M        0x4c
// priv
#define PRIV_RANGE          0x50
#define PRIV_MATCH          0x54
// iaddr
#define IADDR_UPPER_L       0x58
#define IADDR_UPPER_M       0x5c
#define IADDR_LOWER_L       0x60
#define IADDR_LOWER_M       0x64
#define IADDR_MATCH_L       0x68
#define IADDR_MATCH_M       0x6c

// TRACE MANAGEMENT
#define TRACE_STATE         0x70
#define LOSSLESS_TRACE      0x74
#define SHALLOW_TRACE       0x78

// PACKET EMITTER
#define NO_TIME             0x7c
#define NO_CONTEXT          0x80
#define DELTA_ADDRESS       0x84
#define FULL_ADDRESS        0x88
#define IMPLICIT_EXCEPTION  0x8c
#define SIJUMP              0x90
#define IMPLICIT_RETURN     0x94
#define BRANCH_PREDICTION   0x98
#define JUMP_TARGET_CACHE   0x9c
// CVA6 REG
#define CHESHIRE_TRACER_ADDR_START_OFFSET 0x5c
#define CHESHIRE_TRACER_ADDR_END_OFFSET   0x60
#define CHESHIRE_TRACER_ADDR_CURR_OFFSET  0x64

static inline volatile uint8_t *reg8(void *base, int offs) {
    return (volatile uint8_t *)((uint8_t *)base + offs);
}

static inline volatile uint32_t *reg32(void *base, int offs) {
    return (volatile uint32_t *)((uint8_t *)base + offs);
}

void te_reg_write(int offset, uint32_t byte) {
    *reg8(__base_tracer, offset) = byte;
}

// raising activation signal
void  enable_tracer() { te_reg_write(TRACE_STATE, 1); }

// add wait tracer_activated -> read te_reg 
bool tracer_activated() {
    uint8_t value = *reg8(__base_tracer, TRACE_STATE);
    return (value == 1);
}

//creating a filter in range mode between 0 and 0xff => should filter everything. 
void disable_tracer() { 
    te_reg_write(TRACE_STATE,       0x00); // desactivate tracing
    te_reg_write(LOSSLESS_TRACE,    0x00); // desactivate tracing
    te_reg_write(SHALLOW_TRACE,     0x00); // desactivate tracing
    // create filters
    te_reg_write(CAUSE_LOWER,       0xff);
    te_reg_write(CAUSE_ENABLE_MODE, 0x03); // activate matching
    // This should "disable" the tracer, by filtering everything with unpossible condition
}

// create wait_filter activated -> read te_reg -> tracer_enable -> usefull ?

uint32_t tracer_address(uint8_t offset) {
    uint32_t address = *reg32(__base_regs, offset);
    return address;
}

uint32_t tracer_starting_address() {
    return (tracer_address(CHESHIRE_TRACER_ADDR_START_OFFSET));
}

uint32_t tracer_ending_address() {
    return (tracer_address(CHESHIRE_TRACER_ADDR_END_OFFSET));
}

uint32_t tracer_current_address() {
    return (tracer_address(CHESHIRE_TRACER_ADDR_CURR_OFFSET));
}