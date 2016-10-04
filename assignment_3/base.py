#!/usr/bin/env python3

import pprint


class Page(object):
    def __init__(self, num):
        self.num = num

    def __repr__(self):
        return "Page(num={num})".format(num=self.num)

    def __hash__(self):
        return hash(self.num)

    def __eq__(self, other):
        return self.num == other.num


class Frame(object):
    def __init__(self, contents=None):
        self.contents = contents

    def __repr__(self):
        return "Frame(contents={contents})".format(contents=self.contents)


class Memory(object):
    def __init__(self, n_frames):
        self.contents = [Frame() for _ in range(n_frames)]

    def index(self, page):
        for i, frame in enumerate(self.contents):
            if frame.contents is not None and frame.contents == page:
                return i
        else:
            return None

    def __repr__(self):
        return pprint.pformat(self.contents)


def run(memory):
    pages = [0, 1, 3, 6, 2, 4, 5, 2, 5, 0, 3, 1, 2, 5, 4, 1, 0]

    print(memory)
    page_fault_count = 0
    for page in pages:
        print("*"*80)
        page = Page(page)
        print("Looking up", page)
        fault, index = memory.next(page)
        if fault:
            page_fault_count += 1
            print("*** Page fault occurred")
            print("New page at index", index)
        else:
            print("Found at index", index)
        print(memory)
    print("="*80)
    print("Total page faults", page_fault_count)

def run_cls(memory_cls):
    memory = memory_cls(4)
    run(memory)
