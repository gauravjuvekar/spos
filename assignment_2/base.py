#!/usr/bin/env python3

import random
import pprint

def generate_processes(count):
    name = 0
    while name < count:
        name += 1
        yield Process(name=name, size=random.randint(1, 10))

def generate_blocks():
    return (Block(size=random.randint(1, 10))
            for block in range(random.randrange(10)))

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

