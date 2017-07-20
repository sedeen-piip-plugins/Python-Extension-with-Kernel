#include "PythonKernel.h"

#include "PythonEmbeddingWrapper.h"
#include "QtWidgets\qmessagebox.h"
#include "PythonException.h"
#include "PythonObject.h"

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include "numpy\core\include\numpy\arrayobject.h"

namespace sedeen {
namespace image {
namespace tile {

//std::ofstream PythonKernel::log_file= std::ofstream("C:\\Azadeh\\logFile.txt", std::ios_base::out | std::ios_base::app );

PythonKernel::PythonKernel( std::string path, std::string python_class_name, int block_size, float offset, std::vector<std::string>& params)
:python_class_name_(python_class_name),
		block_size_(block_size),
	    offset_(offset),
		method_(params[0]),
		mode_(params[1])
{
	pathToPythonScript_ = path.substr(0, path.find_last_of("/\\") + 1);
	auto p1 = path.find_last_of("/\\");
	auto p2 = path.find_last_of('.')-1;
	pyModuleName_ = path.substr( p1+1, (p2-p1));
}


PythonKernel::~PythonKernel(void)
{

}

			

RawImage PythonKernel::doProcessData(const RawImage &source)
{
	sedeen::image::RawImage OutputImage = nullptr;
	//Call the python code
	try
	{
		python::initPython();

		//set the python path
		std::auto_ptr<python::PythonObject> sys(python::PythonObject::py_ImportModule( "sys" ));
		assert(sys->pPyObject()!= NULL);
		std::auto_ptr<python::PythonObject> path( sys->py_GetAttr( "path" ) ) ;
		assert(path->pPyObject() != NULL);
		PyList_Append(path->py_Object(), PyString_FromString(pathToPythonScript_.c_str())) ;

		//load the module
		std::auto_ptr<python::PythonObject> pModule( python::PythonObject::py_ImportModule( pyModuleName_.c_str() ) ) ; //"PathologyAnalyser"
		assert(pModule->py_Object()!= NULL);

		// find the class
		std::auto_ptr<python::PythonObject> pClass( pModule->py_GetAttr( python_class_name_.c_str() ) ) ;
		assert(pClass->py_Object() != NULL);

		//convert RawImage to PythonObject
		auto pyImage = python::PythonObject::pyobject_from(source);
		assert(pyImage->py_Object()!=NULL);

		//buils the python variable to pass to function
		PyObject* pArgs = Py_BuildValue("(O, i, f, s, s)", pyImage->py_Object(), block_size_, 
														offset_, method_.c_str(), mode_.c_str() );
		assert(pArgs!=NULL);
	
		//create a new instance of the class		
		std::auto_ptr<python::PythonObject> pInstance(pClass->py_CallObject(python::PythonObject(pArgs, true)));
		assert(pInstance->py_Object()!=NULL);		
		Py_DECREF(pArgs);

		//get the function
		std::auto_ptr<python::PythonObject> pMethod(pInstance->py_GetAttr( "run")) ;
		assert(pMethod->py_Object() != NULL);

		pArgs = Py_BuildValue("()");
		assert(pArgs!=NULL);
		std::auto_ptr<python::PythonObject> pResult(pMethod->py_CallObject(python::PythonObject(pArgs, true)));
		assert(pResult->py_Object()!=NULL);

		//convert the returning python object to RawImage
		bool conversionIsDone = python::PythonObject::pyobject_to(pResult->py_Object(), OutputImage);
		assert(conversionIsDone != false);
		
		//python::releasePython();
	}
	catch( python::PythonException& xcptn )
	{
		QMessageBox msgBox;
		msgBox.setText(xcptn.what());
		msgBox.exec();
	}
	catch( std::exception& xcptn )
	{
		QMessageBox msgBox;
		msgBox.setText(xcptn.what());
		msgBox.exec();
	}

	return OutputImage;
}


Color PythonKernel::doGetColor() const
{
	Color color(ColorSpace::RGBA_8);
	return color;
}

} // namespace tile
} // namespace image
} // namespace sedeen