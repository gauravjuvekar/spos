#!/usr/bin/env python3

import base
import pprint


class FifoMem(base.Memory):
    def __init__(self, n_frames):
        base.Memory.__init__(self, n_frames)
        self.last_insert_index = -1 % n_frames

    def next(self, page):
        index = self.index(page)
        if index is not None:
            return False, index

        self.last_insert_index += 1
        self.last_insert_index %= len(self.contents)
        self.contents[self.last_insert_index].contents = page
        return True, self.last_insert_index

base.run_cls(FifoMem)
