#ifndef DPTK_SRC_SEDEEN_PYTHON_EXCEPTION_H
#define DPTK_SRC_SEDEEN_PYTHON_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace sedeen {
namespace python {
    
// This class represents a Python error.
// If a Python API call returns an error, we translate it into a C++ exception.

class PythonException : public std::runtime_error
{
public:
    PythonException( const char* pExceptionMsg ) ;
protected:
    PythonException( const std::string& errorMsg , 
               const std::string& excType , const std::string& excValue ,
			   const std::string& excTraceback ) ;
public: 
    static void translateException() ;
private:
    std::string excType_ ;
    std::string excValue_ ;
    std::string excTraceback_ ;
} ;

} // namespace python
} //namespace sedeen

#endif