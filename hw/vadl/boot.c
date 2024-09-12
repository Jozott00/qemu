#include "qemu/osdep.h"
#include "qemu/datadir.h"
#include "hw/loader.h"
#include "boot.h"
#include "qemu/error-report.h"

bool vadl_load_firmware(ArchCPU *cpu, MachineState *ms,
                        MemoryRegion *program_mr, const char *firmware)
{
    g_autofree char *filename = NULL;
    int bytes_loaded;

    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, firmware);
    if (filename == NULL) {
        error_report("Could not find firmware '%s'", firmware);
        return false;
    }

    bytes_loaded = load_image_mr(filename, program_mr);

    if (bytes_loaded < 0) {
        error_report("Unable to load firmware image %s as ELF or raw binary", firmware);
        return false;
    }
    return true;
}
