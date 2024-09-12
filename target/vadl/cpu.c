
#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/qemu-print.h"
#include "exec/exec-all.h"
#include "cpu.h"
#include "disas/dis-asm.h"
#include "tcg/debug-assert.h"

static VADLCPU* cpu_self;
static bool first_reset = true;
static void vadl_cpu_disas_set_info(CPUState *cpu, disassemble_info *info)
{
    printf("[VADL-DISAS] vadl_cpu_disas_set_info\n");
    info->mach = bfd_arch_vadl;
}

// An init function for the CPU
static void vadl_cpu_init(Object *obj)
{
    CPUState *cs = CPU(obj);
    VADLCPU *cpu = VADL_CPU(obj);
    CPUVADLState *env = &cpu->env;
}

// Realice function that sets up the CPU
static void vadl_cpu_realizefn(DeviceState *dev, Error **errp)
{
    CPUState *cs = CPU(dev);
    cpu_self = VADL_CPU(cs);
    VADLCPUClass* vcc = VADL_CPU_GET_CLASS(dev);
    Error *local_err = NULL;

    cpu_exec_realizefn(cs, &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        return;
    }

    qemu_init_vcpu(cs);
    cpu_reset(cs);
    vcc->parent_realize(dev, errp);
}

// CPU reset function sets the CPU state to the default state that is
// defined in the instruction set manual.
static void vadl_cpu_reset(DeviceState *dev)
{
    CPUState *cs = CPU(dev);
    VADLCPU *cpu = VADL_CPU(dev);
    VADLCPUClass *vcc = VADL_CPU_GET_CLASS(dev);
    CPUVADLState *env = &cpu->env;

    env->isIntInterrupt = 0;
    env->intlevel = 0;
    env->intsrc = -1;
    vcc->parent_reset(dev);

    if(first_reset) {
        memset(env->r, 0, sizeof(env->r));
        memset(env->sflags, 0, sizeof(env->sflags));
        first_reset = false;
    }

    env->sr = 0;

    for (int i = 0; i < 32; i++ ){
        env->r[i] = 0;
    }
    env->sflags[16] = 1;
    env->sflags[21] = 1;
    env->sflags[22] = 1;

    for(int i=0; i < VADL_REG_PAGE_SIZE; i++) {
        env->r[i] = 0;
    }
    printf("RESET 2\n");

    //Start address of the execution. Notice, that this is the start of the flash memory address
    //from the microcontroller implementation.
    env->r[VADL_PC_REG] = 0xd0000000;
    env->r[VADL_LR_REG] = 0;
    env->r[VADL_SP_REG] = 0;
}


static ObjectClass* vadl_cpu_class_by_name(const char *cpu_model)
{
    return object_class_by_name(cpu_model);
}

static bool vadl_cpu_has_work(CPUState *cs)
{
    // TODO: Could be true (RISCV)
    VADLCPU *cpu = VADL_CPU(cs);
    CPUVADLState *env = &cpu->env;
    return (cs->interrupt_request & CPU_INTERRUPT_HARD)
        && cpu_interrupts_enabled(env);
}

//When using the '-d cpu' paramater, QEMU executes this function after every translation block
//to give us the CPU state at the begining of the block.
static void vadl_cpu_dump_state(CPUState *cs, FILE *f, int flags)
{
    VADLCPU *cpu = VADL_CPU(cs);
    //The CPU environment is used to access the content of the emulated registers.
    CPUVADLState *env = &cpu->env;

    qemu_fprintf(f, "PC:    " TARGET_FMT_lx "\n", env->r[VADL_PC_REG]);
    qemu_fprintf(f, "SP:    " TARGET_FMT_lx "\n", env->r[VADL_SP_REG]);
    qemu_fprintf(f, "LR:    " TARGET_FMT_lx "\n", env->r[VADL_LR_REG]);

    int i;
    for(i = 0; i < VADL_REG_PAGE_SIZE-3; ++i) {
        qemu_fprintf(f, "r%d:    " TARGET_FMT_lx "\n", i, env->r[i]);
    }

    //Print status register
    for(i= 0; i< 32; i++){
        qemu_fprintf(f, "%s:    " TARGET_FMT_lx "\n", vadl_cpu_sr_flag_names[i], env->sflags[i]);
    }
    qemu_fprintf(f, "\n");
}

static void vadl_cpu_set_pc(CPUState *cs, vaddr value)
{
    VADLCPU *cpu = VADL_CPU(cs);

    printf("[VADL-CPU] vadl_cpu_set_pc, pc: %04lx\n", value);
    cpu->env.r[VADL_PC_REG] = value;
}

void vadl_cpu_do_interrupt(CPUState *cpu)
{
    qemu_printf("[VADL] vadl_do_interrupt");
}

hwaddr vadl_cpu_get_phys_page_debug(CPUState *cs, vaddr addr)
{
    return addr; /* I assume 1:1 address correspondence */
}

int vadl_cpu_memory_rw_debug(CPUState *cs, vaddr addr, uint8_t *buf, int len, bool is_write)
{
    // TODO: Later
    return -1;
}

static bool vadl_cpu_exec_interrupt(CPUState *cs, int interrupt_request)
{
    // TODO: Later
    return false;
}

void vadl_tcg_init(void)
{
    // TODO: Later
}

bool vadl_cpu_tlb_fill(CPUState *cs, vaddr address, int size,
                       MMUAccessType access_type, int mmu_idx,
                       bool probe, uintptr_t retaddr)
{
    // TODO: Later
    return false;
}

#include "hw/core/sysemu-cpu-ops.h"

static const struct SysemuCPUOps vadl_sysemu_ops = {
    .get_phys_page_debug = vadl_cpu_get_phys_page_debug,
};

#include "hw/core/tcg-cpu-ops.h"

// Here we map the above function to their pupose,
// so that QEMU knows when to execute them.
static const struct TCGCPUOps vadl_tcg_ops = {
    .initialize = vadl_tcg_init,
    .synchronize_from_tb = vadl_cpu_synchronize_from_tb,
    .cpu_exec_interrupt = vadl_cpu_exec_interrupt,
    .tlb_fill = vadl_cpu_tlb_fill,
    .do_interrupt = vadl_cpu_do_interrupt,
};

void vadl_cpu_synchronize_from_tb(CPUState *cs, const TranslationBlock *tb)
{
    VADLCPU *cpu = VADL_CPU(cs);
    cpu->env.r[VADL_PC_REG] = tb->pc;
}

static void vadl_cpu_class_init(ObjectClass *oc, void *data)
{
    printf("CPU-INIT!\n");
    VADLCPUClass *vcc = VADL_CPU_CLASS(oc);
    CPUClass *cc = CPU_CLASS(oc);
    DeviceClass *dc = DEVICE_CLASS(oc);

    device_class_set_parent_realize(dc, vadl_cpu_realizefn, &vcc->parent_realize);
    device_class_set_parent_reset(dc, vadl_cpu_reset, &vcc->parent_reset);

    cc->class_by_name = vadl_cpu_class_by_name;

    cc->has_work = vadl_cpu_has_work;
    cc->dump_state = vadl_cpu_dump_state;
    cc->set_pc = vadl_cpu_set_pc;
    cc->memory_rw_debug = vadl_cpu_memory_rw_debug;
    cc->sysemu_ops = &vadl_sysemu_ops;
    cc->disas_set_info = vadl_cpu_disas_set_info;
    cc->tcg_ops = &vadl_tcg_ops;
}

static const VADLCPUDef vadl_cpu_defs[] = {
    {
        //This is the TYPE that was defined in avr32emp.h in the /hw folder
        //Here we tell QEMU that this CPU logic should be used with the example board/microncontroller
        .name = "VADLVirtMcu",
        .clock_speed = 66 * 1000 * 1000,
    }
};

static void vadl_cpu_cpudef_class_init(ObjectClass *oc, void *data)
{
    VADLCPUClass *vcc = VADL_CPU_CLASS(oc);
    vcc->cpu_def = data;
}

static char* vadl_cpu_type_name(const char* mode_name) {
    return g_strdup_printf(VADL_CPU_TYPE_NAME("%s"), mode_name);
}

static void vadl_register_cpudef_type(const struct VADLCPUDef *def)
{
    char *cpu_model_name = vadl_cpu_type_name(def->name);
    TypeInfo ti = {
        .name = cpu_model_name,
        .parent = TYPE_VADL_CPU,
        .class_init = vadl_cpu_cpudef_class_init,
        .class_data = (void *)def,
    };

    type_register(&ti);
    g_free(cpu_model_name);
}

static const TypeInfo vadl_cpu_arch_types[] = {
    {
        .name = TYPE_VADL_CPU,
        .parent = TYPE_CPU,
        .instance_size = sizeof(VADLCPU),
        .instance_init = vadl_cpu_init,
        .abstract = true,
        .class_size = sizeof(VADLCPUClass),
        .class_init = vadl_cpu_class_init,
    }
};

static void vadl_cpu_register_types(void)
{
    int i;

    type_register_static_array(vadl_cpu_arch_types, 1);
    for(i = 0; i < ARRAY_SIZE(vadl_cpu_defs); ++i) {
        vadl_register_cpudef_type(&vadl_cpu_defs[i]);
    }
}

type_init(vadl_cpu_register_types)
