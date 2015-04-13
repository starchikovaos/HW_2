#include <vector>
#include <limits>

extern "C" {
#include <Python.h>
}

using namespace std;

namespace matrixops {	
	typedef vector<double>  row_t;
	typedef vector<row_t>   matrix_t;

	static matrix_t resistance(const matrix_t &input_matrix)
	{
		const double inf = numeric_limits<double>::infinity();
		matrix_t d = input_matrix;
		for (size_t k=0; k < input_matrix.size(); k++) {
			for (size_t i=0; i < input_matrix.size(); i++) {
				for (size_t j=0; j<input_matrix.size(); j++) {
					double v1 = d[i][j];
					double v2 = d[i][k] + d[k][j];
					if (v1*v2 == 0)
					{
						d[i][j] = 0;
					}
					else
					{
						if ((1/v1 + 1/v2)==0)
						{
							d[i][j] = inf;
						}
						else
							d[i][j] = 1/(1/v1 + 1/v2);
					}
				}
			}
		}
		return d;
	}
}

static matrixops::matrix_t pyobject_to_cxx(PyObject * py_matrix)
{
	matrixops::matrix_t result;
	result.resize(PyObject_Length(py_matrix));
	for (size_t i=0; i<result.size(); ++i) {
		PyObject * py_row = PyList_GetItem(py_matrix, i);
		matrixops::row_t & row = result[i];
		row.resize(PyObject_Length(py_row));
		for (size_t j=0; j<row.size(); ++j) {
			PyObject * py_elem = PyList_GetItem(py_row, j);
			const double elem = PyFloat_AsDouble(py_elem);
			row[j] = elem;
		}
	}
	return result;
}

static PyObject * cxx_to_pyobject(const matrixops::matrix_t &matrix)
{
	PyObject * result = PyList_New(matrix.size());
	for (size_t i=0; i<matrix.size(); ++i) {
		const matrixops::row_t & row = matrix[i];
		PyObject * py_row = PyList_New(row.size());
		PyList_SetItem(result, i, py_row);
		for (size_t j=0; j<row.size(); ++j) {
			const double elem = row[j];
			PyObject * py_elem = PyFloat_FromDouble(elem);
			PyList_SetItem(py_row, j, py_elem);
		}
	}
	return result;
}

static PyObject * matrixops_faster_resistance(PyObject * module, PyObject * args)
{
	PyObject * py_input_matrix = PyTuple_GetItem(args, 0);

	/* Convert to C++ structure */
	const matrixops::matrix_t input_matrix = pyobject_to_cxx(py_input_matrix);

	/* Perform calculations */
	const matrixops::matrix_t result = matrixops::resistance(input_matrix);

	/* Convert back to Python object */
	PyObject * py_result = cxx_to_pyobject(result);
	return py_result;
}

PyMODINIT_FUNC PyInit_matrixops()
{
	static PyMethodDef ModuleMethods[] = {
		{ "faster_resistance", matrixops_faster_resistance, METH_VARARGS, "Fater matrix production" },
		{ NULL, NULL, 0, NULL }
	};
	static PyModuleDef ModuleDef = {
		PyModuleDef_HEAD_INIT,
		"matrixops",
		"Matrix operations",
		-1, ModuleMethods, 
		NULL, NULL, NULL, NULL
	};
	PyObject * module = PyModule_Create(&ModuleDef);
	return module;
}