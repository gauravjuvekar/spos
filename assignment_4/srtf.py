#!/usr/bin/env python3

import base


class SrtfScheduler(base.Scheduler):
    def interrupt(self, time, process):
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

        self.ready_processes.sort(key=lambda x: x.remaining_time)
        self.currently_running = None
        if len(self.ready_processes):
            self.currently_running = self.ready_processes[0]
            self.currently_running.start_time = time
            self.currently_running.end_time = (
                time + self.currently_running.remaining_time)

        if self.currently_running is None:
            return None
        else:
            return self.currently_running.end_time

base.run(SrtfScheduler)
