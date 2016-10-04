#!/usr/bin/env python3

import base
import pprint


class OptimalMem(base.Memory):
    def next(self, page, **kwargs):
        assert('future_pages' in kwargs)
        future_pages = kwargs['future_pages']

        index = self.index(page)
        if index is not None:
            return False, index

        for i, frame in enumerate(self.contents):
            if frame.contents is None:
                self.page_table[page.num] = i
                frame.contents = page
                return True, i

        possible_frames = set(range(len(self.contents)))
        for future_page in future_pages:
            if len(possible_frames) > 1:
                if future_page.num in self.page_table:
                    possible_frames.remove(self.page_table[future_page.num])
            else:
                break
        assert(len(possible_frames) >= 1)
        frame_index = possible_frames.pop()
        frame = self.contents[frame_index]
        del self.page_table[frame.contents.num]
        self.page_table[page.num] = frame_index
        frame.contents = page
        return True, frame_index

base.run_cls(OptimalMem)
