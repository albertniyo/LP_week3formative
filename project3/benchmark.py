"""Compare normal Python vs C module analysis"""

import time
import random
import normal
import analysis

def time_it(fn, data, reps=3):
    start = time.perf_counter()
    for _ in range(reps):
        fn(data)
    return (time.perf_counter() - start) / reps

def compare(label, py_fn, c_fn):
    print()
    print("---", label, "---")
    print("%10s %14s %14s %10s" % ("size", "python (s)", "C (s)", "speedup"))
    for size in (1000, 100000, 1000000):
        data = [random.uniform(-100, 100) for _ in range(size)]
        py_t = time_it(py_fn, data)
        c_t = time_it(c_fn, data)
        speed = py_t / c_t if c_t > 0 else 0
        print("%10d %14.6f %14.6f %9.2fx" % (size, py_t, c_t, speed))

compare("mean", normal.mean, analysis.mean)
compare("variance", normal.variance, analysis.variance)
compare("min_max", normal.min_max, analysis.min_max)
compare("count_outliers",
        lambda d: normal.count_outliers(d, 1.0),
        lambda d: analysis.count_outliers(d, 1.0))
compare("normalize", normal.normalize, analysis.normalize)
