#!/usr/bin/env python3

import base
import pprint


class FifoMem(base.Memory):
    def __init__(self, n_frames):
        base.Memory.__init__(self, n_frames)
        self.last_insert_index = -1 % n_frames

    def next(self, page, **kwargs):
        index = self.index(page)
        if index is not None:
            return False, index

        self.last_insert_index += 1
        self.last_insert_index %= len(self.contents)
        frame_index = self.last_insert_index
        frame = self.contents[frame_index]
        if frame.contents is not None:
            del self.page_table[frame.contents.num]
        frame.contents = page
        self.page_table[page.num] = frame_index
        return True, frame_index

base.run_cls(FifoMem)
