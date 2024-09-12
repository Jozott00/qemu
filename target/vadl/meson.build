
vadl_ss = ss.source_set()
vadl_system_ss = ss.source_set()

vadl_ss.add(files(
               'translate.c',
               'cpu.c',
               'disas.c'))

vadl_system_ss.add(files('machine.c'))

target_arch += {'vadl': vadl_ss}
target_system_arch += {'vadl': vadl_system_ss}
