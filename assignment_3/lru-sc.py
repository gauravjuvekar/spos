#!/usr/bin/env python3

import base
import pprint
import itertools


class LruScMem(base.Memory):
    def __init__(self, n_frames):
        base.Memory.__init__(self, n_frames)
        self.last_insert_index = -1 % n_frames
        self.referenced = [0] * n_frames

    def next(self, page, **kwargs):
        index = self.index(page)
        if index is not None:
            self.referenced[index] = 1
            return False, index

        self.last_insert_index += 1
        self.last_insert_index %= len(self.contents)
        while self.referenced[self.last_insert_index]:
            self.referenced[self.last_insert_index] = 0
            self.last_insert_index += 1
            self.last_insert_index %= len(self.contents)

        frame_index = self.last_insert_index
        frame = self.contents[frame_index]
        if frame.contents is not None:
            del self.page_table[frame.contents.num]
        frame.contents = page
        self.page_table[page.num] = frame_index
        self.referenced[frame_index] = 1

        return True, frame_index

    def __repr__(self):
        return (base.Memory.__repr__(self) + "\n" +
                "Ref history " + pprint.pformat(list(self.referenced)))

base.run_cls(LruScMem)
