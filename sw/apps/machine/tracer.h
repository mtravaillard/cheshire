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
#define TVEC_ENABLE_MODE    0x01
#define TVAL_ENABLE_MODE    0x02
#define PRIV_ENABLE_MODE    0x03
#define IADDR_ENABLE_MODE   0x04
// cause
#define CAUSE_UPPER         0x05
#define CAUSE_LOWER         0x06
#define CAUSE_MATCH         0x07
// tvec
#define TVEC_UPPER_L        0x08
#define TVEC_UPPER_M        0x09
#define TVEC_LOWER_L        0x0A
#define TVEC_LOWER_M        0x0B
#define TVEC_MATCH_L        0x0C
#define TVEC_MATCH_M        0x0D
// tval
#define TVAL_UPPER_L        0x0E
#define TVAL_UPPER_M        0x0F
#define TVAL_LOWER_L        0x10
#define TVAL_LOWER_M        0x11
#define TVAL_MATCH_L        0x12
#define TVAL_MATCH_M        0x13
// priv
#define PRIV_RANGE          0x14
#define PRIV_MATCH          0x15
// iaddr
#define IADDR_UPPER_L       0x16
#define IADDR_UPPER_M       0x17
#define IADDR_LOWER_L       0x18
#define IADDR_LOWER_M       0x19
#define IADDR_MATCH_L       0x1A
#define IADDR_MATCH_M       0x1B
/* TRACE MANAGEMENT */
#define TRACE_STATE         0x1C
#define LOSSLESS_TRACE      0x1D
#define SHALLOW_TRACE       0x1E
/* PACKET EMITTER */
#define NO_TIME             0x1F
#define NO_CONTEXT          0x20
#define DELTA_ADDRESS       0x21
#define FULL_ADDRESS        0x22
#define IMPLICIT_EXCEPTION  0x23
#define SIJUMP              0x24
#define IMPLICIT_RETURN     0x25
#define BRANCH_PREDICTION   0x26
#define JUMP_TARGET_CACHE   0x27

// CVA6 REG
#define CHESHIRE_TRACER_ADDR_START_OFFSET 0x5c
#define CHESHIRE_TRACER_ADDR_END_OFFSET   0x60
#define CHESHIRE_TRACER_ADDR_CURR_OFFSET  0x64

static inline volatile uint8_t *reg8(void *base, int offs) {
    return (volatile uint8_t *)(base + offs);
}

static inline volatile uint32_t *reg32(void *base, int offs) {
    return (volatile uint32_t *)(base + offs);
}

void te_reg_write(uint8_t offset, uint8_t byte) {
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