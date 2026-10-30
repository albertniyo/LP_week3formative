# Project 3 — CPython extension
Our Scenario was about engineering team which wants to optimize the computationally intensive data analysis operations using the CPython C API while retaining Python as the main interface.

## Files

| File | What it is |
|---|---|
| `analysis.c` | C extension (`analysis`) |
| `normal.py` | same functions in plain Python |
| `main.py` | demo: `python main.py python` or `python main.py c` |
| `setup.py` | builds the `.so` |
| `test.py` | checks Python and C match |
| `benchmark.py` | benchmark timing or speedup of both implementations|

## Build

```sh
source .venv/bin/activate   # venv activation
python setup.py build_ext --inplace
```
Which creates `analysis.cpython-*.so` and then run

```sh
python test.py # testing
python benchmark.py > benchmark.txt # benchmarking
python main.py c 10000 # module demo
```

**As conclusion**, Python calls C through the CPython API and this not only saves resources for computational intensive operations but also works like normal Python functions call (as our module is **`analysis`**, we simply import it by  `import analysis` then use it as normal function).
