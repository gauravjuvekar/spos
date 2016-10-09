#!/usr/bin/env python3

import base


class RoundRobinScheduler(base.Scheduler):
    def __init__(self):
        base.Scheduler.__init__(self)
        self.last_run = -1

    def interrupt(self, time, process):
        TIME_SLICE = 1
        if process is not None:
            process.remaining_time = process.burst_time
            self.ready_processes.append(process)

        if self.currently_running is not None:
            self.currently_running.remaining_time -= (
                time - self.currently_running.start_time)
            self._add_to_history(self.currently_running,
                                 self.currently_running.start_time,
                                 time)
            if self.currently_running.remaining_time == 0:
                self.ready_processes.remove(self.currently_running)
                self.last_run -= 1

        self.currently_running = None
        if len(self.ready_processes):
            self.last_run = (self.last_run + 1) % len(self.ready_processes)
            self.currently_running = self.ready_processes[self.last_run]
            self.currently_running.start_time = time
            self.currently_running.end_time = (
                time + TIME_SLICE)

        if self.currently_running is None:
            return None
        else:
            return self.currently_running.end_time

base.run(RoundRobinScheduler)
