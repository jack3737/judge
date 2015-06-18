#include <Python.h>

#define UNREFERENCED_PARAMETER(p)
#if defined(_MSC_VER)
#	define inline __declspec(inline)
#	pragma warning(disable: 4127)
#	undef UNREFERENCED_PARAMETER
#	define UNREFERENCED_PARAMETER(p) (p)
#elif !defined(__GNUC__)
#	define inline
#endif

static inline int isline(char ch) {
	switch (ch) {
	case '\n':
	case '\r':
		return 1;
	}
	return 0;
}

static inline int iswhite(char ch) {
	switch (ch) {
	case ' ':
	case '\t':
	case '\v':
	case '\f':
	case '\n':
	case '\r':
		return 1;
	}
	return 0;
}

static int check_standard(const char *judge, size_t jlen, const char *process, size_t plen) {
	size_t j = 0, p = 0;
	int nj, np;

	for (;;) {
		nj = np = 0;
		while (j < jlen && ((nj |= isline(judge[j])), iswhite(judge[j]))) ++j;
		while (p < plen && ((np |= isline(process[p])), iswhite(process[p]))) ++p;
		if (j == jlen || p == plen) return j == jlen && p == plen;
		if (nj != np) return 0;

		while (j < jlen && !iswhite(judge[j])) {
			if (p >= plen) return 0;
			if (judge[j++] != process[p++]) return 0;
		}
	}
}

static PyObject *checker_standard(PyObject *self, PyObject *args) {
	PyObject *expected, *actual, *result;

	UNREFERENCED_PARAMETER(self);
	if (!PyArg_ParseTuple(args, "OO:standard", &expected, &actual))
		return NULL;

	if (!PyString_Check(expected) || !PyString_Check(actual)) {
		PyErr_SetString(PyExc_ValueError, "expected strings");
		return NULL;
	}

	Py_INCREF(expected);
	Py_INCREF(actual);
	Py_BEGIN_ALLOW_THREADS
	result = check_standard(PyString_AS_STRING(expected), PyString_GET_SIZE(expected),
							PyString_AS_STRING(actual), PyString_GET_SIZE(actual)) ?
			Py_True : Py_False;
	Py_END_ALLOW_THREADS
	Py_DECREF(expected);
	Py_DECREF(actual);
	Py_INCREF(result);
	return result;
}

static PyMethodDef checker_methods[] = {
	{"standard", checker_standard, METH_VARARGS,
	 "Standard DMOJ checker."},
	{NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC init_checker(void) {
	(void) Py_InitModule("_checker", checker_methods);
}