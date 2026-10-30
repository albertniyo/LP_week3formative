"""analysis with pure Python or the C module named analysis"""

import random
import sys
import normal

def run(backend, size, threshold):
    data = [random.uniform(-100, 100) for _ in range(size)]

    if backend == "python":
        api = normal
    else:
        import analysis as api  # C extension module

    lo, hi = api.min_max(data)
    print("backend:", backend)
    print("n:", len(data))
    print("mean:", api.mean(data))
    print("variance:", api.variance(data))
    print("min:", lo, "max:", hi)
    print("outliers:", api.count_outliers(data, threshold))
    
    if data:
        print("normalize[0]:", api.normalize(data)[0])

if __name__ == "__main__":
    # usage: python main.py python|c [size]
    backend = sys.argv[1] if len(sys.argv) > 1 else "python"
    size = int(sys.argv[2]) if len(sys.argv) > 2 else 1000
    run(backend, size, 1.0)
