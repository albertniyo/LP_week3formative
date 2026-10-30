#include <Python.h>
#include <math.h>

static PyObject *as_list(PyObject *obj, Py_ssize_t *n)
{
    PyObject *seq = PySequence_Fast(obj, "expected a sequence");
    if (!seq)
        return NULL;
    *n = PySequence_Fast_GET_SIZE(seq);
    return seq;
}

static int get_double(PyObject *item, double *out)
{
    *out = PyFloat_AsDouble(item);
    return PyErr_Occurred() ? -1 : 0;
}

/* --- mean --- */
static PyObject *py_mean(PyObject *self, PyObject *args)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;

    Py_ssize_t n;
    PyObject *seq = as_list(obj, &n);
    if (!seq)
        return NULL;
    if (n == 0) {
        Py_DECREF(seq);
        return PyFloat_FromDouble(0.0);
    }

    PyObject **items = PySequence_Fast_ITEMS(seq);
    double sum = 0.0;
    for (Py_ssize_t i = 0; i < n; i++) {
        double v;
        if (get_double(items[i], &v) < 0) {
            Py_DECREF(seq);
            return NULL;
        }
        sum += v;
    }
    Py_DECREF(seq);
    return PyFloat_FromDouble(sum / (double)n);
}

/* --- variance (population) --- */
static PyObject *py_variance(PyObject *self, PyObject *args)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;

    Py_ssize_t n;
    PyObject *seq = as_list(obj, &n);
    if (!seq)
        return NULL;
    if (n == 0) {
        Py_DECREF(seq);
        return PyFloat_FromDouble(0.0);
    }

    PyObject **items = PySequence_Fast_ITEMS(seq);
    double sum = 0.0;
    for (Py_ssize_t i = 0; i < n; i++) {
        double v;
        if (get_double(items[i], &v) < 0) {
            Py_DECREF(seq);
            return NULL;
        }
        sum += v;
    }

    double mean = sum / (double)n;
    double var = 0.0;
    for (Py_ssize_t i = 0; i < n; i++) {
        double v;
        get_double(items[i], &v);
        double d = v - mean;
        var += d * d;
    }
    Py_DECREF(seq);
    return PyFloat_FromDouble(var / (double)n);
}

/* --- min_max --- */
static PyObject *py_min_max(PyObject *self, PyObject *args)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;

    Py_ssize_t n;
    PyObject *seq = as_list(obj, &n);
    if (!seq)
        return NULL;
    if (n == 0) {
        Py_DECREF(seq);
        return Py_BuildValue("(dd)", 0.0, 0.0);
    }

    PyObject **items = PySequence_Fast_ITEMS(seq);
    double lo, hi;
    if (get_double(items[0], &lo) < 0) {
        Py_DECREF(seq);
        return NULL;
    }
    hi = lo;

    for (Py_ssize_t i = 1; i < n; i++) {
        double v;
        if (get_double(items[i], &v) < 0) {
            Py_DECREF(seq);
            return NULL;
        }
        if (v < lo) lo = v;
        if (v > hi) hi = v;
    }
    Py_DECREF(seq);
    return Py_BuildValue("(dd)", lo, hi);
}

/* --- count_outliers --- */
static PyObject *py_count_outliers(PyObject *self, PyObject *args)
{
    PyObject *obj;
    double thr;
    if (!PyArg_ParseTuple(args, "Od", &obj, &thr))
        return NULL;

    Py_ssize_t n;
    PyObject *seq = as_list(obj, &n);
    if (!seq)
        return NULL;
    if (n == 0) {
        Py_DECREF(seq);
        return PyLong_FromLong(0);
    }

    PyObject **items = PySequence_Fast_ITEMS(seq);
    double sum = 0.0;
    for (Py_ssize_t i = 0; i < n; i++) {
        double v;
        if (get_double(items[i], &v) < 0) {
            Py_DECREF(seq);
            return NULL;
        }
        sum += v;
    }

    double mean = sum / (double)n;
    long count = 0;
    for (Py_ssize_t i = 0; i < n; i++) {
        double v;
        get_double(items[i], &v);
        if (fabs(v - mean) > thr)
            count++;
    }
    Py_DECREF(seq);
    return PyLong_FromLong(count);
}

/* --- normalize --- */
static PyObject *py_normalize(PyObject *self, PyObject *args)
{
    PyObject *obj;
    if (!PyArg_ParseTuple(args, "O", &obj))
        return NULL;

    Py_ssize_t n;
    PyObject *seq = as_list(obj, &n);
    if (!seq)
        return NULL;

    PyObject *out = PyList_New(n);
    if (!out) {
        Py_DECREF(seq);
        return NULL;
    }
    if (n == 0) {
        Py_DECREF(seq);
        return out;
    }

    PyObject **items = PySequence_Fast_ITEMS(seq);
    double lo, hi;
    if (get_double(items[0], &lo) < 0) {
        Py_DECREF(seq);
        Py_DECREF(out);
        return NULL;
    }
    hi = lo;

    for (Py_ssize_t i = 1; i < n; i++) {
        double v;
        if (get_double(items[i], &v) < 0) {
            Py_DECREF(seq);
            Py_DECREF(out);
            return NULL;
        }
        if (v < lo) lo = v;
        if (v > hi) hi = v;
    }

    double range = hi - lo;
    for (Py_ssize_t i = 0; i < n; i++) {
        double v;
        get_double(items[i], &v);
        double nv = (range == 0.0) ? 0.0 : (v - lo) / range;
        PyObject *f = PyFloat_FromDouble(nv);
        if (!f) {
            Py_DECREF(seq);
            Py_DECREF(out);
            return NULL;
        }
        PyList_SET_ITEM(out, i, f); /* steals reference */
    }
    Py_DECREF(seq);
    return out;
}

/* Module table — import analysis; analysis.mean(...) */
static PyMethodDef methods[] = {
    {"mean",           py_mean,           METH_VARARGS, "Mean"},
    {"variance",       py_variance,       METH_VARARGS, "Population variance"},
    {"min_max",        py_min_max,        METH_VARARGS, "Return (min, max)"},
    {"count_outliers", py_count_outliers, METH_VARARGS, "Count outliers"},
    {"normalize",      py_normalize,      METH_VARARGS, "Min-max normalize"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "analysis",
    "C extension for simple dataset stats",
    -1,
    methods
};

PyMODINIT_FUNC PyInit_analysis(void)
{
    return PyModule_Create(&moduledef);
}
