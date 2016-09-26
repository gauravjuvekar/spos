#!/usr/bin/env python3

import base
import itertools

memory = base.Memory()

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
for process in base.generate_processes(5):
    print("New process to insert", process)
    if not nextfit(memory, process):
        print("*** COULD NOT ALLOCATE ***")
    print("Memory contents")
    print(memory)


