
#include "qemu/osdep.h"
#include "cpu.h"
#include "qemu/error-report.h"
#include "sysemu/kvm.h"
#include "migration/cpu.h"
#include "sysemu/cpu-timers.h"


// we provide the status register to qemu
static int get_sreg(QEMUFile *f, void* opaque, size_t size,
    const VMStateField *field)
{
    uint32_t sreg;
    sreg = qemu_get_be32(f);
    return sreg;
}

// we set the status register
static int put_sreg(QEMUFile *f, void *opaque, size_t size,
    const VMStateField *field, JSONWriter *vmdesc)
{
    CPUVADLState *env = opaque;
    uint32_t sreg = env->sr;

    qemu_put_be32(f, sreg);
    return 0;
}

// the mapping
static const VMStateInfo vms_sreg = {
    .name = "sreg",
    .get = get_sreg,
    .put = put_sreg,
};

// The VMState definition
const VMStateDescription vms_vadl_cpu = {
    .name = "cpu",
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (VMStateField[]) {
        VMSTATE_UINT32_ARRAY(env.r, VADLCPU, VADL_REG_PAGE_SIZE),
        VMSTATE_SINGLE(env.sr, VADLCPU, 0, vms_sreg, uint32_t),
        VMSTATE_END_OF_LIST()
    }
};