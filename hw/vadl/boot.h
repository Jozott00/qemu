#ifndef HW_VADL_BOOT_H
#define HW_VADL_BOOT_H

#include "hw/boards.h"
#include "hw/loader.h"


target_ulong vadl_find_and_load_firmware(MachineState *ms, hwaddr firmware_load_addr, symbol_fn_t symbol_fn);

target_ulong vadl_load_firmware(const char *firmware_filename,
                                 hwaddr firmware_load_addr,
                                 symbol_fn_t symbol_fn);

#endif // HW_VADL_BOOT_H
