#!/usr/bin/env python3

import pprint


class Process(object):
    def __init__(self, pid, burst_time):
        self.pid = pid
        self.burst_time = burst_time

    def __repr__(self):
        return "Process(pid={pid})".format(
            **self.__dict__)


class Scheduler(object):
    def __init__(self):
        self.history = []
        self.ready_processes = []
        self.currently_running = None

    def _add_to_history(self, process, run_start, run_end):
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
        pprint.pprint(self.history)


def run(scheduler_class):
    processes = [
        (0, Process(pid=1, burst_time=4)),
        (1, Process(pid=2, burst_time=5)),
        (2, Process(pid=3, burst_time=2)),
        (3, Process(pid=4, burst_time=3)),
        (4, Process(pid=5, burst_time=6))]

    scheduler = scheduler_class()

    processes = iter(processes)
    try:
        next_process = processes.__next__()
    except StopIteration:
        next_process = None
        next_interrupt = None
    else:
        next_interrupt = next_process[0]
    while next_interrupt is not None:
        if next_process is None:
            try:
                next_process = processes.__next__()
            except StopIteration:
                next_process = None
        if next_process is not None:
            next_interrupt = min(next_interrupt, next_process[0])
            next_process = next_process[1]

        next_interrupt = scheduler.interrupt(time=next_interrupt,
                                             process=next_process)
        next_process = None

    scheduler.print()
