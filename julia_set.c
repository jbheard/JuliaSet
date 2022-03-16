#include <Python.h>
#include <numpy/arrayobject.h>
#include "palettes.h"

const double MIN = -1.1;
const double MAX = 1.1;

Complex f(Complex z0, Complex c) {
    Complex tmp;
    tmp.real = z0.real*z0.real - z0.imag*z0.imag;
    tmp.imag = z0.imag*z0.real + z0.real*z0.imag;
    tmp.real += c.real;
    tmp.imag += c.imag;
    return tmp;
}

double norm(Complex c) {
    return c.real*c.real + c.imag*c.imag;
}

Color** getMap(Complex seed, int width, int height, int paletteID) {
	Color *palette = getPaletteByID( paletteID );
    Color **iters = malloc( height * sizeof(Color*) );
    for(int i = 0; i < height; i++) {
        iters[i] = malloc( width * sizeof(Color) );
	}
    Complex z;
    int num;
    for(int i = 0; i < width; i++) {
        for(int j = 0; j < height; j++) {
            z = (Complex){ MIN + i*(MAX-MIN)/width, MIN + j*(MAX-MIN)/height };
            num = 0;
            for(; num < 50; num++) {
                z = f(z, seed);
                if(norm(z) > 2.0) break;
            }
			iters[i][j] = palette[num];
        }
    }
    return iters;
}

PyObject* arrayToList2d(Color **arr, int w, int h) {
    PyObject *pylist2d = PyList_New( h );
    for (int i = 0; i < h; ++i) {
		PyObject *pylist2d_row = PyList_New( w );
		for(int j = 0; j < w; ++j) {
			PyObject *Color = Py_BuildValue("(BBB)", arr[i][j].r, arr[i][j].g, arr[i][j].b);
			PyList_SetItem(pylist2d_row, j, Color);
		}
		PyList_SetItem(pylist2d, i, pylist2d_row);
    }
    return pylist2d;
}

PyObject *arrayToNumpyArray2d(Color **arr, int w, int h) {
	npy_intp const dims[3] = {w, h, 3};
	PyObject *nparr = PyArray_Zeros( 3, dims, PyArray_DescrFromType(NPY_UINT8), 0 );
	for (int i = 0; i < h; ++i) {
		for(int j = 0; j < w; ++j) {
			void *ptr = PyArray_GETPTR3( nparr, i, j, 0 );
			PyObject *color = Py_BuildValue("B", arr[i][j].r);
			PyArray_SETITEM( nparr, ptr, color );
			
			ptr = PyArray_GETPTR3( nparr, i, j, 1 );
			color = Py_BuildValue("B", arr[i][j].g);
			PyArray_SETITEM( nparr, ptr, color );
			
			ptr = PyArray_GETPTR3( nparr, i, j, 2 );
			color = Py_BuildValue("B", arr[i][j].b);
			PyArray_SETITEM( nparr, ptr, color );
		}
    }
	return nparr;
}

void freeArray2d(Color **arr, int h) {
	for(int i = 0; i < h; i++) { 
		free(arr[i]);
	}
	free(arr);
}

static PyObject *method_make_julia_set(PyObject *self, PyObject *args) {
    int width, height, paletteID;
	double a, b;

    /* Parse arguments */
    if(!PyArg_ParseTuple(args, "ddiii", &a, &b, &width, &height, &paletteID)) {
        return NULL;
    }
		
	Color **map = getMap( (Complex){a, b}, width, height, paletteID );
	PyObject *result = arrayToNumpyArray2d( map, width, height );
	freeArray2d( map, height );

    return result;
}

static PyMethodDef JuliaSetMethods[] = {
    {"make_julia_set", method_make_julia_set, METH_VARARGS, "Generates a Julia Set fractal"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef juliasetmodule = {
    PyModuleDef_HEAD_INIT,
    "julia_set_c",
    "Python interface for generating Julia set fractals",
    -1,
    JuliaSetMethods
};

PyMODINIT_FUNC PyInit_julia_set_c(void) {
	import_array(); // init numpy arrays
    return PyModule_Create(&juliasetmodule);
}

