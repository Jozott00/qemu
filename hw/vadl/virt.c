
#include "qemu/osdep.h"
#include "qom/object.h"
#include "virt.h"
#include "boot.h"
#include "qapi/error.h"
#include "qemu/qemu-print.h"
#include "exec/address-spaces.h"
#include "sysemu/sysemu.h"
#include <stdlib.h>


static const MemMapEntry virt_memmap[] = {
        [VIRT_DRAM] =         { 0x80000000,           0x0 },
};

static void virt_machine_ready(Notifier *notifier, void *data)
{
    // load the firmware
    qemu_printf("[VADL] virt_machine_ready\n");
}

static void virt_machine_init(MachineState *machine)
{
    qemu_printf("[VADL] virt_machine_init\n");
    const MemMapEntry *memmap = virt_memmap;
    VADLVirtMachineState *s = VADL_VIRT_MACHINE(machine);

    qemu_printf("[VADL] ram-size: %x\n", machine->ram_size);
    MemoryRegion *system_memory = get_system_memory();
    qemu_printf("[VADL] sys mem size: %x\n", system_memory->size);

    memory_region_add_subregion(system_memory, memmap[VIRT_DRAM].base, machine->ram);

    s->machine_ready.notify = virt_machine_ready;
    qemu_add_machine_init_done_notifier(&s->machine_ready);
}




static void virt_machine_class_init(ObjectClass *oc, void *data) {
    MachineClass *mc = MACHINE_CLASS(oc);

    mc->desc = "VADL VirtIO board";

    mc->init = virt_machine_init;
    mc->default_cpus = 1;
    mc->default_cpu_type = TYPE_VADL_CPU,
    mc->min_cpus = mc->default_cpus;
    mc->max_cpus = mc->default_cpus;
    mc->no_floppy = 1;
    mc->no_cdrom = 1;
    mc->no_parallel = 1;
    mc->default_ram_id = "ram";
}

static void virt_machine_instance_init(Object *obj) {
    VADLVirtMachineState *m_state = VADL_VIRT_MACHINE(obj);

    // nothing to do
}

static const TypeInfo virt_machine_typeinfo = {
    .name = TYPE_VADL_VIRT_MACHINE,
    .parent = TYPE_MACHINE,
    .class_init = virt_machine_class_init,
    .instance_init = virt_machine_instance_init,
    .instance_size = sizeof(VADLVirtMachineState),
};

static void virt_machine_init_register_types(void)
{
    type_register_static(&virt_machine_typeinfo);
}

type_init(virt_machine_init_register_types)