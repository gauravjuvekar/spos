#!/usr/bin/env python3

import base


class HrrnScheduler(base.Scheduler):
    def interrupt(self, time, process):
        sort_key = lambda x: (time - x.arrival_time) / x.burst_time
        if process is not None:
            process.arrival_time = time
            self.ready_processes.append(process)

        if self.currently_running is not None:
            if time == self.currently_running.end_time:
                self._add_to_history(self.currently_running,
                                     self.currently_running.start_time,
                                     self.currently_running.end_time)
                self.currently_running = None

        if self.currently_running is None:
            if len(self.ready_processes):
                self.ready_processes.sort(key=sort_key, reverse=True)
                self.currently_running = self.ready_processes[0]
                self.currently_running.start_time = time
                self.currently_running.end_time = (
                    time + self.currently_running.burst_time)
                del self.ready_processes[0]

        if self.currently_running is None:
            return None
        else:
            return self.currently_running.end_time

base.run(HrrnScheduler)
