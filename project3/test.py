"""check if normal Python and the C module analysis give the same answers"""

import random
import normal
import analysis  # C extension

def almost_equal(a, b):
    if isinstance(a, (list, tuple)):
        if len(a) != len(b):
            return False
        for x, y in zip(a, b):
            if not almost_equal(x, y):
                return False
        return True
    return abs(a - b) < 1e-9

def check(name, data, thr=1.0):
    assert almost_equal(normal.mean(data), analysis.mean(data)), name + " mean"
    assert almost_equal(normal.variance(data), analysis.variance(data)), name + " var"
    assert almost_equal(normal.min_max(data), analysis.min_max(data)), name + " min_max"
    assert normal.count_outliers(data, thr) == analysis.count_outliers(data, thr), name + " outliers"
    assert almost_equal(normal.normalize(data), analysis.normalize(data)), name + " normalize"
    print("pass:", name)

check("empty", [])
check("single", [42.0])
check("all equal", [5.0, 5.0, 5.0])
check("small", [1.0, 2.0, 3.0, 4.0, 5.0])
check("random 1k", [random.uniform(-100, 100) for _ in range(1000)])
check("random 100k", [random.uniform(-100, 100) for _ in range(100000)])

print("\nall equivalence tests passed.")
