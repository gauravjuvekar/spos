#!/usr/bin/env python3

import base


class SjfScheduler(base.Scheduler):
    def interrupt(self, time, process):
        if process is not None:
            self.ready_processes.append(process)
            self.ready_processes.sort(key=lambda x: x.burst_time)

        if self.currently_running is not None:
            if time == self.currently_running.end_time:
                self._add_to_history(self.currently_running,
                                     self.currently_running.start_time,
                                     self.currently_running.end_time)
                self.currently_running = None

        if self.currently_running is None:
            if len(self.ready_processes):
                self.currently_running = self.ready_processes[0]
                self.currently_running.start_time = time
                self.currently_running.end_time = (
                    time + self.currently_running.burst_time)
                del self.ready_processes[0]

        if self.currently_running is None:
            return None
        else:
            return self.currently_running.end_time

base.run(SjfScheduler)
