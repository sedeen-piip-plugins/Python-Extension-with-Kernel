#ifndef DPTK_SRC_SEDEEN_PYTHON_OBJECT_H
#define DPTK_SRC_SEDEEN_PYTHON_OBJECT_H

#include "Global.h"
#include "Image.h"


#include <string>
#include <fstream>

#include "PythonException.h"

namespace sedeen {
namespace python {

typedef struct _object PyObject ; // nb: from python.h 

// This class wraps a PyObject* returned to us by Python.
// NOTE: It is not thread-safe!

class PythonObject
{
public:
    PythonObject( _object* pPyObject , bool decRef ) ;
    explicit PythonObject( int val ) ;
    explicit PythonObject( const char* pVal ) ;
    virtual ~PythonObject() ;
public:
    PythonObject* py_CallObject() ;
    PythonObject* py_CallObject( PythonObject& args ) ;
    PythonObject* py_CallObject( PythonObject& args , PythonObject& kywdArgs ) ;
    PythonObject* py_GetAttr( const char* pAttrName ) const ;
    void py_SetTupleItem( size_t pos , PythonObject& obj ) ;
    std::string py_ReprVal() const ;
public:
	static bool pyobject_to(PyObject*, sedeen::image::RawImage& );
	static bool pyobject_to(PyObject*, bool& value );
	static bool pyobject_to(PyObject*, int& value );
	static bool pyobject_to(PyObject*, double& value );
	static bool pyobject_to(PyObject*, float& value );
	static bool pyobject_to(PyObject*, std::string& value );
	static bool pyobject_to(PyObject*, sedeen::Color& value ); 
	static bool pyobject_to(PyObject*, sedeen::Size& value );

	static PythonObject* pyobject_from(const sedeen::image::RawImage& );
	static PythonObject* pyobject_from(const bool& value );
	static PythonObject* pyobject_from(const int& value );
	static PythonObject* pyobject_from(const double& value );
	static PythonObject* pyobject_from(const float& value );
	static PythonObject* pyobject_from(const std::string& value );
	static PythonObject* pyobject_from(const sedeen::Color& value );
	static PythonObject* pyobject_from(const sedeen::Size& value );

public: 
    static PythonObject* py_ImportModule( const char* pModuleName ) ;
    static PythonObject* py_NewTuple( size_t nItems ) ;
    static PythonObject* py_NewDict() ;
public:
    PyObject* py_Object() const ;
private:
    PyObject* PyObject_ ; // Python object being wrapped
    bool decRef_ ; // flags if we should decref 

	//static std::ofstream log_file;
} ;

} // namespace python
} //namespace sedeen

#endif


