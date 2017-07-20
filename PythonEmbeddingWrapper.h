#ifndef DPTK_SRC_SEDEEN_PYTHON_WRAPPER_H
#define DPTK_SRC_SEDEEN_PYTHON_WRAPPER_H

namespace sedeen {
namespace python {
	
	extern void initPython(  ) ;
	extern void releasePython() ;
	extern bool getInit();

} // namespace python
} //namespace sedeen

#endif