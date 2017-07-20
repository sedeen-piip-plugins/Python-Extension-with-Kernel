
#include <sstream>
#include <memory>
#include "python.h"

#include "PythonException.h"
#include "PythonObject.h"


namespace sedeen {
namespace python {

PythonException::PythonException( const char* pExceptionMsg )
    : runtime_error( pExceptionMsg )
{
}

PythonException::PythonException( const std::string& errorMsg , const std::string& excType , const std::string& excValue , const std::string& excTraceback )
    : runtime_error( errorMsg )
    , excType_(excType) , excValue_(excValue) , excTraceback_(excTraceback)
{
}

void
PythonException::translateException()
{
    // get the Python error details
    std::string excType , excValue , excTraceback ;
    PyObject *pExcType , *pExcValue , *pExcTraceback ;
    PyErr_Fetch( &pExcType , &pExcValue , &pExcTraceback ) ;
    if ( pExcType != NULL )
    {
        PythonObject obj( pExcType , true ) ;
        std::auto_ptr<PythonObject> attrObj( obj.py_GetAttr( "__name__" ) ) ;
        excType = attrObj->py_ReprVal() ;
    }
    if ( pExcValue != NULL )
    {
        PythonObject obj( pExcValue , true ) ;
        excValue = obj.py_ReprVal() ;
    }
    if ( pExcTraceback != NULL )
    {
        PythonObject obj( pExcTraceback , true ) ;
        excTraceback = obj.py_ReprVal() ;
    }

    // translate the error into a C++ exception
    std::stringstream buf ;
    buf << (excType.empty() ? "???" : excType) ;
    if ( ! excValue.empty() )
        buf << ": " << excValue ;
    throw PythonException( buf.str() , excType , excValue , excTraceback ) ;
}

} // namespace python
}

