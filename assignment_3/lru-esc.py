#!/usr/bin/env python3

import base
import pprint
import itertools
import enum


class LruEscMem(base.Memory):
    def __init__(self, n_frames):
        base.Memory.__init__(self, n_frames)
        self.last_insert_index = -1 % n_frames
        self.referenced = [0] * n_frames
        self.dirty = [0] * n_frames

    def next(self, page, dirties, **kwargs):
        index = self.index(page)
        if index is not None:
            self.referenced[index] = 1
            if dirties:
                self.dirty[index] = 1
            return False, index

        def intify(index):
            ret = 0
            if self.referenced[index]:
                ret |= 1 << 1
            if self.dirty[index]:
                ret |= 1
            return ret
        self.last_insert_index += 1
        self.last_insert_index %= len(self.contents)
        victim = None
        victim_val = 3

        while victim != self.last_insert_index:
            if victim_val > intify(self.last_insert_index):
                victim = self.last_insert_index
                victim_val = intify(victim)
            if (not self.referenced[self.last_insert_index] and
                    not self.dirty[self.last_insert_index]):
                victim = self.last_insert_index
                break

            self.referenced[self.last_insert_index] = 0
            self.last_insert_index += 1
            self.last_insert_index %= len(self.contents)
        assert(victim is not None)
        frame_index = victim
        self.last_insert_index = victim
        frame = self.contents[frame_index]
        if frame.contents is not None:
            self.dirty[frame_index] = 0
            del self.page_table[frame.contents.num]
        frame.contents = page
        self.page_table[page.num] = frame_index
        self.referenced[frame_index] = 1

        return True, frame_index

    def __repr__(self):
        return (base.Memory.__repr__(self) + "\n" +
                "Ref history   " + pprint.pformat(list(self.referenced)) + "\n"+
                "Dirty history " + pprint.pformat(list(self.dirty)))


base.run_cls(LruEscMem)
