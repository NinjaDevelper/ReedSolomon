/*
 * Copyright (c) 2015, utamaro
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * */


#include <vector>
#include "ReedSolomon.h"
#include <Python.h>

using namespace std;

static PyObject *ReedSolomon_encodeFile(PyObject *self, PyObject *args){
    const char *inFile=NULL;
    const char *outFile=NULL;
    int K=0;
    int N=0;
    if (!PyArg_ParseTuple(args, "ssii", &inFile,&outFile,&K,&N)){
            return NULL;
    }
    ReedSolomon rs;
    int result=rs.encodeFile(inFile,outFile,K,N);
    return Py_BuildValue("i", result);
}
static PyObject *ReedSolomon_decodeFile(PyObject *self, PyObject *args){
    const char *inFile=NULL;
    const char *outFile=NULL;
    int K=0;
    int N=0;
    long size=0;
    PyObject *errLoc_=NULL;
    vector<long> errLoc;
    if (!PyArg_ParseTuple(args, "ssiilO", &inFile,&outFile,&K,&N,&size,&errLoc_)){
            return NULL;
    }
    for(int i=0;i<PyList_Size(errLoc_);i++){
        long l=PyLong_AsLong(PyList_GetItem(errLoc_,i));
        errLoc.push_back(l);
    }
    ReedSolomon rs;
    int result=rs.decodeFile(inFile,outFile,K,N,size,errLoc);
    return Py_BuildValue("i", result);
}
static PyMethodDef ReedSolomonMethods[] = {
    {"encodeFile", ReedSolomon_encodeFile, METH_VARARGS, "Encode file by Reed-Solomon codes with interleaving."},
    {"decodeFile", ReedSolomon_decodeFile, METH_VARARGS, "Decode file that coded by Reed-Solomon codes."},
    {NULL, NULL, 0, NULL}
};
#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef ReedSolomonModule = {
    PyModuleDef_HEAD_INIT,
    "reedSolomon", /* name of module */
    "Reed-Solomon coding including interleaving", /* module documentation, may be NULL */
    -1, /* size of per-interpreter state of the module,
           or -1 if the module keeps state in global variables. */
    ReedSolomonMethods
};
PyMODINIT_FUNC PyInit_reedSolomon(void){
    return PyModule_Create(&ReedSolomonModule);
}
#else
#ifdef __cplusplus
extern "C" {
#endif
void initreedSolomon(void) {
    Py_InitModule("reedSolomon", ReedSolomonMethods);
}
#ifdef __cplusplus
}  // extern "C"
#endif
#endif


