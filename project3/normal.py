"""Pure Python version of the same analysis"""

def mean(data):
    if not data:
        return 0.0
    return sum(data) / len(data)

def variance(data):
    if not data:
        return 0.0
    m = mean(data)
    total = 0.0
    for x in data:
        total += (x - m) ** 2
    return total / len(data)

def min_max(data):
    if not data:
        return (0.0, 0.0)
    return (min(data), max(data))

def count_outliers(data, threshold):
    if not data:
        return 0
    m = mean(data)
    count = 0
    for x in data:
        if abs(x - m) > threshold:
            count += 1
    return count

def normalize(data):
    if not data:
        return []
    lo, hi = min_max(data)
    span = hi - lo
    if span == 0.0:
        return [0.0 for _ in data]
    return [(x - lo) / span for x in data]
