#!/usr/bin/env python3

import base

memory = base.Memory()

def first_fit(memory, process):
    for block in memory.blocks:
        if process.size <= block.free_space():
            block.contents.append(process)
            return True
    else:
        return False

print(memory)
for process in base.generate_processes(5):
    print("New process to insert", process)
    if not first_fit(memory, process):
        print("*** COULD NOT ALLOCATE **")

    print("Memory contents")
    print(memory)


