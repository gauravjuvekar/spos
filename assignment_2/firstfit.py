#!/usr/bin/env python3

import base
import itertools

memory = base.Memory()
GENERATE_MANUALLY = True

def first_fit(memory, process):
    for block in memory.blocks:
        if process.size <= block.free_space():
            block.contents.append(process)
            return True
    else:
        return False

print(memory)
if not GENERATE_MANUALLY:
    for process in base.generate_processes(5):
        print("*"*80)
        print("New process to insert", process)
        if not bestfit(memory, process):
            print("*** COULD NOT ALLOCATE ***")
        print("Memory contents")
        print(memory)
else:
    for name in itertools.count():
        print("*"*80)
        size = int(input("Enter process size (0 to exit) "))
        if not size:
            break
        process = base.Process(name, size)
        print("New process to insert", process)
        if not first_fit(memory, process):
            print("*** COULD NOT ALLOCATE ***")
        print("Memory contents")
        print(memory)
