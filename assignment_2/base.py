#!/usr/bin/env python3

import random
import pprint

process_sizes = [200, 300, 55, 20, 85, 25, 70]
def generate_processes(count=None):
    return (Process(name=i, size=s) for i, s in enumerate(process_sizes))
    # name = 0
    # while name < count:
        # name += 1
        # yield Process(name=name, size=random.randint(1, 10))

block_sizes = [250, 50, 400, 75, 100, 60]
def generate_blocks():
    return (Block(size=s) for s in block_sizes)
    # return (Block(size=random.randint(1, 10))
            # for block in range(random.randrange(2, 10)))

class Block(object):
    def __init__(self, size, contents=None):
        self.size = size
        if contents is None:
            contents = []
        self.contents = contents

    def free_space(self):
        return self.size - sum(x.size for x in self.contents)

    def __repr__(self):
        return ('Block(size={size}, contents={contents}, free={freespace})'
                .format(
                    size=self.size,
                    contents=repr(self.contents),
                    freespace=self.free_space()))

class Process(object):
    def __init__(self, name, size):
        self.name = name
        self.size = size

    def __repr__(self):
        return 'Process(name={name}, size={size})'.format(
            name=self.name, size=self.size)

class Memory(object):
    def __init__(self, blocks=None):
        if blocks is None:
            blocks = list(generate_blocks())
        self.blocks = blocks

    def __repr__(self):
        return pprint.pformat(self.blocks)

