#!/usr/bin/env python3

import base
import itertools

memory = base.Memory()
GENERATE_MANUALLY = False

last_insert = 0
def nextfit(memory, process):
    global last_insert
    # last_insert = (last_insert + 1) % len(memory.blocks)
    for i in itertools.chain(
            range(last_insert, len(memory.blocks)),
            range(last_insert)):
        block = memory.blocks[i]
        if process.size <= block.free_space():
            block.contents.append(process)
            last_insert = i
            return True
    else:
        return False

print(memory)
if not GENERATE_MANUALLY:
    for process in base.generate_processes(7):
        print("*"*80)
        print("New process to insert", process)
        if not nextfit(memory, process):
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
        if not nextfit(memory, process):
            print("*** COULD NOT ALLOCATE ***")
        print("Memory contents")
        print(memory)
