#!/usr/bin/env python3

import base
import itertools

memory = base.Memory()

def worstfit(memory, process):
    finder = iter(enumerate(memory.blocks))
    max_size_block_i = None
    for i, block in finder:
        if (block.free_space() >= process.size):
            max_size_block_i = i
            break
    for i, block in finder:
        if ((block.free_space() >
             memory.blocks[max_size_block_i].free_space()) and
            (process.size <= block.free_space())):
            max_size_block_i = i
    if max_size_block_i is not None:
        memory.blocks[max_size_block_i].contents.append(process)
        return True
    else:
        return False



print(memory)
for process in base.generate_processes(5):
    print("New process to insert", process)
    if not worstfit(memory, process):
        print("*** COULD NOT ALLOCATE ***")
    print("Memory contents")
    print(memory)


