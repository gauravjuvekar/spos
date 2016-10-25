#!/usr/bin/env python3

import pprint
import matplotlib.pyplot as plot
import matplotlib
import numpy
import itertools
from collections import deque


class Process(object):
    def __init__(self, pid, burst_time, priority=0):
        self.pid = pid
        self.burst_time = burst_time
        self.priority = priority

    def __repr__(self):
        return "Process(pid={pid}, priority={priority})".format(
            **self.__dict__)


class Scheduler(object):
    def __init__(self):
        self.history = []
        self.ready_processes = []
        self.currently_running = None

    def _add_to_history(self, process, run_start, run_end):
        if run_start == run_end:
            return
        if (len(self.history) and
                self.history[-1][1] == run_start and
                self.history[-1][2] == process):
            orig = self.history[-1]
            self.history[-1] = orig[0], run_end, orig[2]
        else:
            self.history.append((run_start, run_end, process))

    def interrupt(self, time):
        raise NotImplementedError()

    def print(self):
        counter = itertools.count()
        uniq_processes = dict()
        for proc, count in zip(
                sorted(list(set((hist[2] for hist in self.history))),
                       key=lambda p:p.pid),
                counter):
            uniq_processes[proc] = count
        n_colors = counter.__next__()
        colors = list(matplotlib.cm.rainbow(numpy.linspace(0, 1, n_colors)))

        figure = matplotlib.pyplot.figure()
        subplot = figure.add_subplot(111)
        subplot.xaxis.set_major_locator(
            matplotlib.ticker.MultipleLocator(base=1))
        for start, end, proc in self.history:
            subplot.barh(
                bottom=uniq_processes[proc],
                width=end - start,
                height=1,
                left=start,
                color=colors[uniq_processes[proc]],
                align='center')
        subplot.set_autoscalex_on(False)
        subplot.set_xlim([0, self.history[-1][1]])
        matplotlib.pyplot.grid()
        ytics = list(uniq_processes.items())
        matplotlib.pyplot.yticks(
            [_[1] for _ in ytics], [pprint.pformat(_[0]) for _ in ytics])
        matplotlib.pyplot.show()
        pprint.pprint(self.history)


def run(scheduler_class):
    # Tuples of (arrival_time, process)
    processes = deque([
        (0, Process(pid=1, burst_time=5, priority=3)),
        (1, Process(pid=2, burst_time=1, priority=0)),
        (2, Process(pid=3, burst_time=3, priority=1)),
        (3, Process(pid=4, burst_time=5, priority=2)),
        (20, Process(pid=5, burst_time=7, priority=0)),
        (25, Process(pid=6, burst_time=1, priority=2)),
        (30, Process(pid=7, burst_time=3, priority=5)),
        (30, Process(pid=8, burst_time=5, priority=3)),
        (31, Process(pid=9, burst_time=2, priority=5)),
        (40, Process(pid=10, burst_time=8, priority=2))])

    scheduler = scheduler_class()

    try:
        next_process = processes.popleft()
    except IndexError:
        next_process = None
        next_interrupt = None
    else:
        next_interrupt = next_process[0]
    while next_interrupt is not None or next_process is not None:
        if next_process is None:
            try:
                next_process = processes.popleft()
            except IndexError:
                next_process = None
        if next_process is not None:
            if next_interrupt is None:
                next_interrupt = next_process[0]
            else:
                next_interrupt = min(next_interrupt, next_process[0])
        if next_process is not None:
            if next_interrupt == next_process[0]:
                next_interrupt = scheduler.interrupt(time=next_interrupt,
                                                     process=next_process[1])
                next_process = None
            else:
                next_interrupt = scheduler.interrupt(time=next_interrupt,
                                                     process=None)
        else:
            next_interrupt = scheduler.interrupt(time=next_interrupt,
                                                 process=None)

    scheduler.print()
