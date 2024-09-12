
#ifndef HW_VADL_VIRT_H
#define HW_VADL_VIRT_H

#include "qemu/typedefs.h"
#include "qom/object.h"
#include "hw/boards.h"
#include "cpu-qom.h"
#include "target/vadl/cpu.h"

#define VADL_VIRT_FLASH_SIZE 1024 * KiB

#define TYPE_VADL_VIRT_MACHINE MACHINE_TYPE_NAME("virt")

/* No class required as it is only a state object without methods */
OBJECT_DECLARE_SIMPLE_TYPE(VADLVirtMachineState, VADL_VIRT_MACHINE)

struct VADLVirtMachineState {
    /*< private >*/
    MachineState parent;

    /*< public >*/
    Notifier machine_ready;
    VADLCPU cpu;
};

enum {
    VIRT_DEBUG,
    VIRT_MROM,
    VIRT_TEST,
    VIRT_RTC,
    VIRT_CLINT,
    VIRT_ACLINT_SSWI,
    VIRT_PLIC,
    VIRT_APLIC_M,
    VIRT_APLIC_S,
    VIRT_UART0,
    VIRT_VIRTIO,
    VIRT_FW_CFG,
    VIRT_IMSIC_M,
    VIRT_IMSIC_S,
    VIRT_FLASH,
    VIRT_DRAM,
    VIRT_PCIE_MMIO,
    VIRT_PCIE_PIO,
    VIRT_PLATFORM_BUS,
    VIRT_PCIE_ECAM
};

#endif // HW_VADL_VIRT_H
