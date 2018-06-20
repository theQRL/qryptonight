// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

%include "stdint.i"
%include "carrays.i"
%include "cdata.i"
%include "std_vector.i"
%include "std_string.i"
%include "stl.i"
%include "std_except.i"
%include "std_shared_ptr.i"
%include "exception.i"

%{
SWIGEXPORT void HandleAllExceptions()
{
    try
    {
        throw;
    }
    catch(const std::invalid_argument& e)
    {
        SWIG_Error(SWIG_ValueError, e.what());
    }
    catch(const std::exception& e)
    {
        SWIG_Error(SWIG_RuntimeError, e.what());
    }
    catch (...)
    {
        SWIG_Error(SWIG_UnknownError, "unknown error");
    }
}
%}

%exception
{
    try {   $action }
    catch (...) {
        HandleAllExceptions();
        SWIG_fail;
    }
}

%array_class(unsigned char, ucharCArray)
%array_class(unsigned int, uintCArray)
%array_class(uint32_t, uint32CArray)

namespace std {
  %template(intVector) vector<int>;
  %template(uintVector) vector<unsigned int>;
  %template(ucharVector) vector<unsigned char>;
  %template(charVector) vector<char>;
  %template(doubleVector) vector<double>;

  %template(_string_list) vector<string>;
  %template(_string_list_list) vector<vector<unsigned char>>;
}

%feature("director:except") {
    if ($error != NULL)
    {
        PyObject *exc, *val, *tb;
        PyErr_Fetch(&exc, &val, &tb);
        PyErr_NormalizeException(&exc, &val, &tb);

        PyObject* exc_py = PyObject_GetAttrString(exc, "__name__");
        std::string exc_str = PyUnicode_AsUTF8(exc_py);

        std::string val_str;
        if (val != NULL) {
            PyObject* val_py = PyObject_Str(val);
            val_str += PyUnicode_AsUTF8(val_py);
            Py_XDECREF(val_py);
        }

        Py_XDECREF(exc_py);
        Py_XDECREF(exc);
        Py_XDECREF(val);
        Py_XDECREF(tb);

        std::string err_msg("'$symname' | " + exc_str + val_str);
        Swig::DirectorMethodException::raise(err_msg.c_str());
    }
}

%module(directors="1") pyqryptonight
%{
    #include "pow/powhelper.h"
    #include "misc/strbignum.h"
    #include "qryptonight/qryptonight.h"
    #include "qryptonight/qryptominer.h"
%}

%feature("director") Qryptominer;

%include "pow/powhelper.h"
%include "misc/strbignum.h"
%include "qryptonight/qryptonight.h"
%include "qryptonight/qryptominer.h"

