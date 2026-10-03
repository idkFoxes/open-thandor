"""CPU load gate for the test tools: new game instances start only while the machine stays below CPU_LIMIT.

run_checks.py starts each check through it, run_all_maps.py each mission of a worker beyond --jobs. Starts are
serialized (one at a time, machine-wide within the process) and spaced by SETTLE seconds so the load of the
instance just started shows in the next measurement."""
import ctypes
import threading
import time
from ctypes import wintypes

CPU_LIMIT = 80   # percent; above this no further instance starts
SETTLE = 20      # seconds after a start before the next one may start

_lock = threading.Lock()
_last_start = [0.0]


def _system_times():
    idle, kernel, user = wintypes.FILETIME(), wintypes.FILETIME(), wintypes.FILETIME()
    ctypes.windll.kernel32.GetSystemTimes(ctypes.byref(idle), ctypes.byref(kernel), ctypes.byref(user))
    value = lambda t: t.dwHighDateTime << 32 | t.dwLowDateTime
    return value(idle), value(kernel), value(user)  # kernel time includes the idle time


def cpu_percent(seconds=3.0):
    """Average CPU load of the whole machine over the given time."""
    idle0, kernel0, user0 = _system_times()
    time.sleep(seconds)
    idle1, kernel1, user1 = _system_times()
    total = (kernel1 - kernel0) + (user1 - user0)
    return 100.0 * (total - (idle1 - idle0)) / total if total else 0.0


def try_start(settle=SETTLE):
    """True when a new instance may start now (and records the start); False when the last start is too
    recent or the machine is busy."""
    with _lock:
        if time.time() - _last_start[0] < settle:
            return False
        if cpu_percent() >= CPU_LIMIT:
            return False
        _last_start[0] = time.time()
        return True


def wait_to_start(settle=SETTLE, poll=2.0):
    """Blocks until try_start() allows a start; returns the seconds waited."""
    began = time.time()
    while not try_start(settle):
        time.sleep(poll)
    return time.time() - began
