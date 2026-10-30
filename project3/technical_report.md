# CPython Technical Analysis

## Goal
Our goal was to speed up basic data analysis concepts such as mean, variance, min/max, outlier count, and normalize by moving the loops to C, while keeping a normal Python import as `import analysis`.

## Files

- `normal.py` — clear Python baseline
- `analysis.c` — same operations via the CPython C API
- `setup.py` — compiles to `analysis.cpython-*.so`
- `test.py` and `benchmark.py` — correctness, time and speed benchmarking

Builds by:
```sh
python setup.py build_ext --inplace
```

## How Python talks to C

1. `PyInit_analysis` creates the module named `analysis`.
2. Each function is listed in `PyMethodDef` (e.g. `mean`).
3. `PyArg_ParseTuple` reads arguments.
4. `PySequence_Fast` turns the Python list into something C can loop over.
5. Results go back as Python objects (`PyFloat_FromDouble`, etc.).

## Correctness

`test.py` compares normal Python and C on empty, tiny, and large random data. Results match within `1e-9`.

## Speed

Heavy loops (like variance, outliers) are much faster in C. Simple `mean` gains less because Python’s `sum` is already decent. `normalize` must build a Python list of floats, so at 1M values the conversion cost eats some of the win.

For full benchmark, check [benchmark.txt](benchmark.txt).

## Trade-offs

C is faster for number crunching, but you must build a matching `.so` for your Python version and various building tools make it easy to build it up, however debugging is still kinda hard. Static evidence of speed comes from the benchmarks and sometimes results are not accurate on both implementations as it can be expected (where you can check this by demo on [main.py](main.py)), however both implementations stay the same.
