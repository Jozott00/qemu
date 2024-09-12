#ifndef HW_VADL_BOOT_H
#define HW_VADL_BOOT_H

#include "hw/boards.h"


bool vadl_load_firmware(ArchCPU *cpu, MachineState *ms,
                        MemoryRegion *mr, const char *filename);

#endif // HW_VADL_BOOT_H
