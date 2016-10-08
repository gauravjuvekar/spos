#!/usr/bin/env python3

import pprint
from termcolor import colored


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
        self.page_table = dict()

    def index(self, page):
        if page.num in self.page_table:
            return self.page_table[page.num]
        else:
            return None

    def __repr__(self):
        return (pprint.pformat(self.contents) + "\n" +
                "Page table: " + pprint.pformat(self.page_table))


def run(memory):
    pages = [7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1]
    dirties = [1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1]
    pages = [Page(x) for x in pages]

    print(memory)
    page_fault_count = 0
    for i, page in enumerate(pages):
        print("*"*80)
        print("Looking up", page)
        fault, index = memory.next(page,
                                   future_pages=pages[i + 1:],
                                   dirties=dirties[i])
        if fault:
            page_fault_count += 1
            print(colored("*** Page fault occurred", 'red'))
            print("New page at index", index)
        else:
            print(colored("Found at index {index}".format(index=index),
                          'green'))
        print(memory)
    print("="*80)
    print("Total page faults", page_fault_count)

def run_cls(memory_cls):
    memory = memory_cls(3)
    run(memory)
