#!/usr/bin/env python3

import base
import pprint
from collections import deque


class LruMem(base.Memory):
    def __init__(self, n_frames):
        base.Memory.__init__(self, n_frames)
        self.history = deque(range(n_frames))

    def next(self, page, **kwargs):

        frame_index = self.index(page)
        if frame_index is not None:
            self.history.remove(frame_index)
            self.history.append(frame_index)
            return False, frame_index

        frame_index = self.history.popleft()
        self.history.append(frame_index)

        frame = self.contents[frame_index]
        if frame.contents is not None:
            del self.page_table[frame.contents.num]

        self.page_table[page.num] = frame_index
        frame.contents = page
        return True, frame_index

    def __repr__(self):
        return (base.Memory.__repr__(self) + "\n" +
                "LRU history " + pprint.pformat(list(self.history)))


base.run_cls(LruMem)
