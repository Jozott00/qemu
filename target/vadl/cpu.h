
#ifndef QEMU_VADL_CPU_H
#define QEMU_VADL_CPU_H

#include "cpu-qom.h"
#include "exec/cpu-defs.h"
#include "exec/hwaddr.h"
#include "exec/vaddr.h"
#include "qemu/typedefs.h"

#define VADL_REG_PAGE_SIZE 16

// TODO: CHANGE THIS
#define VADL_REG_PAGE_SIZE 16 // r0 - r12 + LR + SP + PC
#define VADL_PC_REG 15
#define VADL_LR_REG 14
#define VADL_SP_REG 13

// TODO: CHANGE THIS
//There are also 256 system registers.
#define VADL_SYS_REG 256

struct VADLCPUDef {
    const char* name;
    size_t clock_speed;
};

#define VADL_CPU_TYPE_SUFFIX "-" TYPE_VADL_CPU
#define VADL_CPU_TYPE_NAME(name) (name VADL_CPU_TYPE_SUFFIX)
#define CPU_RESOLVING_TYPE TYPE_VADL_CPU
#define VADL_PC_REG 15
#define VADL_LR_REG 14
#define VADL_SP_REG 13
// Global Interrupt Mask
#define VADL_GM_FLAG(sr)   (sr & 0x10000) >> 16


static const char vadl_cpu_r_names[VADL_REG_PAGE_SIZE][8] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "r8", "r9", "r10", "r11", "r12", "SP", "LR", "PC"
};

static const char vadl_cpu_sr_flag_names[32][8] = {
        "sregC", "sregZ", "sregN", "sregV", "sregQ", "sregL",
        "sreg6", "sreg7", "sreg8", "sreg9", "sreg10", "sreg11","sreg12","sreg13",
        "sregT", "sregR",
        "sregGM","sregI0M","sregI1M", "sregI2M", "sregI3M","sregEM","sregM0",
        "sregM1",
        "sregM2", "sreg25","sregD","sregDM","sregJ","sregH", "sreg30", "sregSS"
};


typedef struct CPUArchState {
    uint sr;
    uint pc_w;

    uint32_t sflags[32];

    // Register File Registers
    uint32_t r[VADL_REG_PAGE_SIZE];

    // interrupt source
    int intsrc;
    int intlevel;
    uint64_t autovector;
    int isIntInterrupt;
} CPUVADLState;

struct ArchCPU {
    /*< private >*/
    CPUState parent_obj;

    /*< public >*/
    CPUVADLState env;
};


int vadl_print_insn(bfd_vma addr, disassemble_info *info);

static inline int cpu_interrupts_enabled(CPUVADLState* env)
{
    // TODO : CHANGE THIS
    return VADL_GM_FLAG(env->sr);
}

static inline void cpu_get_tb_cpu_state(CPUVADLState *env, target_long *pc,
                                        target_ulong *cs_base, uint32_t *pflags)
{
    *pc = env->r[VADL_PC_REG];
    *cs_base = 0;
    *pflags = 0;
}

void vadl_tcg_init(void);
bool vadl_cpu_tlb_fill(CPUState *cs, vaddr address, int size,
                        MMUAccessType access_type, int mmu_idx,
                        bool probe, uintptr_t retaddr);
void vadl_cpu_do_interrupt(CPUState *cpu);
void vadl_cpu_set_int(void *apaque, int irq, int level);
hwaddr vadl_cpu_get_phys_page_debug(CPUState *cs, vaddr addr);
int vadl_cpu_memory_rw_debug(CPUState *cs, vaddr addr, uint8_t *buf, int len, bool is_write);

void vadl_cpu_synchronize_from_tb(CPUState *cs, const TranslationBlock *tb);

#include "exec/cpu-all.h"

#endif
