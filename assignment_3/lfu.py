#!/usr/bin/env python3

import base
import pprint


class LfuMem(base.Memory):
    def __init__(self, n_frames):
        base.Memory.__init__(self, n_frames)
        self.frequency = [0] * n_frames

    def next(self, page, **kwargs):

        frame_index = self.index(page)
        if frame_index is not None:
            self.frequency[frame_index] += 1
            return False, frame_index

        frame_index = self.frequency.index(min(self.frequency))
        frame = self.contents[frame_index]
        if frame.contents is not None:
            del self.page_table[frame.contents.num]

        self.page_table[page.num] = frame_index
        frame.contents = page
        self.frequency[frame_index] = 1
        return True, frame_index

    def __repr__(self):
        return (base.Memory.__repr__(self) + "\n" +
                "LFU access count " + pprint.pformat(list(self.frequency)))


base.run_cls(LfuMem)
