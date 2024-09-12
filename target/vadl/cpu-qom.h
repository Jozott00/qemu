#ifndef VADL_CPU_QOM_H
#define VADL_CPU_QOM_H

#include "hw/core/cpu.h"
#include "qom/object.h"

#define TYPE_VADL_CPU "vadl-cpu"

OBJECT_DECLARE_CPU_TYPE(VADLCPU, VADLCPUClass, VADL_CPU)

typedef struct VADLCPUDef VADLCPUDef;
struct VADLCPUClass {
    /*< private >*/
    CPUClass parent_class;

    /*< public >*/
    DeviceRealize parent_realize;
    DeviceReset parent_reset;

    VADLCPUDef* cpu_def;
};

#endif // AVR32A_CPU_QOM_H
