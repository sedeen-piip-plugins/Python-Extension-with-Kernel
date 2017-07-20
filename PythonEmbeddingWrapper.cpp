#include <cassert>
#include "python.h"

#include "PythonEmbeddingWrapper.h"
namespace sedeen {
namespace python {

static bool gIsInit = false ;

void 
initPython( )
{
    assert( !gIsInit ) ;

    // initialize Python
	Py_SetPythonHome("C:\\Anaconda2");
    Py_Initialize() ;
    gIsInit = true ;
}

void 
releasePython()
{
    assert( gIsInit ) ;

    // clean up Python
    Py_Finalize() ;
    gIsInit = false ;
}

bool getInit()
{
	return gIsInit;
}

} // namespace python
} //namespace sedeen