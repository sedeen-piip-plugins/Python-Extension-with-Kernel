#include "python.h"

#include "PythonObject.h"

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include "numpy\core\include\numpy\arrayobject.h"

using namespace std ;
namespace sedeen {
namespace python {
	
//std::ofstream PythonObject::log_file= std::ofstream("C:\\Azadeh\\logFile_.txt", std::ios_base::out | std::ios_base::app );

PythonObject::PythonObject( PyObject* pPyObject , bool decRef )
    : PyObject_(pPyObject) , decRef_(decRef)
{
	
}

PythonObject::PythonObject( int val )
{
    // create a Python int object 
    PyObject_ = PyInt_FromLong( val ) ;
    assert( PyObject_ != NULL ) ;
    decRef_ = true ;
}

PythonObject::PythonObject( const char* pVal )
{
    // create a Python string object 
    PyObject_ = PyString_FromString( pVal ) ;
    assert( PyObject_ != NULL ) ;
    decRef_ = true ;
}

PythonObject::~PythonObject()
{
    // clean up
    if ( decRef_ && PyObject_ != NULL )
        Py_DecRef( PyObject_ ) ;
}

void 
PythonObject::py_SetTupleItem( size_t pos , PythonObject& obj )
{
    // set the tuple item
    int rc = PyTuple_SetItem( py_Object() , pos , obj.py_Object() ) ;
    if ( rc != 0 )
        PythonException::translateException() ;
    // NOTE: The tuple takes ownership of the object, so it doesn't need to decref itself.
    obj.decRef_ = false ;
}

PythonObject*
PythonObject::py_CallObject( PythonObject& args , PythonObject& kywdArgs ) 
{
    // call the object
    PyObject* pResultObj = PyEval_CallObject( py_Object() , args.py_Object() ) ;
    if ( pResultObj == NULL )
        PythonException::translateException() ;
    return new PythonObject( pResultObj , true ) ; 
}

PythonObject* PythonObject::py_CallObject() { return py_CallObject( *py_NewTuple(0) , *py_NewDict() ) ; }
PythonObject* PythonObject::py_CallObject( PythonObject& a ) { return py_CallObject( a , *py_NewDict() ) ; }

PythonObject*
PythonObject::py_GetAttr( const char* pAttrName ) const
{ 
    // return the specified attribute
    PyObject* pAttrObj = PyObject_GetAttrString( py_Object() , pAttrName ) ;
    if ( pAttrObj == NULL )
        PythonException::translateException() ;
    return new PythonObject( pAttrObj , true ) ; 
}

PythonObject*
PythonObject::py_ImportModule( const char* pModuleName ) 
{
    // execute the code module 
    PyObject* pModuleObj = PyImport_ImportModule( (char*)pModuleName  ) ;
    if ( pModuleObj == NULL )
        PythonException::translateException() ;
    return new PythonObject( pModuleObj , true ) ; 
}

PythonObject* PythonObject::py_NewTuple( size_t n ) { PyObject* p=PyTuple_New(n) ; assert(p != NULL) ; return new PythonObject(p,true) ; }
PythonObject* PythonObject::py_NewDict() { PyObject* p=PyDict_New() ; assert(p != NULL) ; return new PythonObject(p,true) ; }

string
PythonObject::py_ReprVal() const 
{
    // return the "repr" value for the Python object
    // NOTE: We have to return a std::string since PyString_AsString() returns
    //  a char* for the "repr" object, which will be destroyed when we return.
    PythonObject repr( PyObject_Repr(PyObject_) , true ) ;
    return PyString_AsString( repr.py_Object() ) ; 
}

PyObject* PythonObject::py_Object() const { return PyObject_ ; }



bool PythonObject::pyobject_to(PyObject* obj, sedeen::image::RawImage& image )
{
	int rc=0;
	if(PyArray_API == NULL){
		rc = _import_array();	}	
	assert( rc== 0);

	if( !PyArray_Check(obj) )
	{
		python::PythonException::translateException() ;
		return false;
	}

	PyArrayObject* oarr = (PyArrayObject*) obj;
	bool needcopy = false, needcast = false;
	int typenum = PyArray_TYPE(oarr), new_typenum = typenum;
	int ndims = PyArray_NDIM(oarr);
#ifndef MAX_DIM
	const int MAX_DIM = 4;
#endif

	if(ndims >= MAX_DIM)
	{
		python::PythonException::translateException() ;
		return false;
	}

	sedeen::Color color(0,1);
	bool isSigned = false;
	int depth =0;
	switch(typenum)
	{
	case NPY_UBYTE:
		color= ndims==2 ?sedeen::Color(sedeen::ColorSpace::Gray_8) :
			ndims==3	?sedeen::Color(sedeen::ColorSpace::RGB_8) :
			ndims==4	?sedeen::Color(sedeen::ColorSpace::RGBA_8) : sedeen::Color(0, 1);
		isSigned = false;
		depth = 8;
		break;
	case NPY_BYTE:
		color= ndims==2 ?sedeen::Color(sedeen::ColorSpace::Gray_8) :
			ndims==3	?sedeen::Color(sedeen::ColorSpace::RGB_8) :
			ndims==4	?sedeen::Color(sedeen::ColorSpace::RGBA_8) : sedeen::Color(0, 1);
		isSigned = true;
		depth = 8;
		break;
	case NPY_USHORT:
		color= ndims==2 ?sedeen::Color(sedeen::ColorSpace::Gray_16) :
			ndims==3	?sedeen::Color(sedeen::ColorSpace::RGB_16) :
			ndims==4	?sedeen::Color(sedeen::ColorSpace::RGBA_16) : sedeen::Color(0, 1);
		isSigned = false;
		depth = 16;
		break;
	case NPY_SHORT:
		color= ndims==2 ?sedeen::Color(sedeen::ColorSpace::Gray_16) :
			ndims==3	?sedeen::Color(sedeen::ColorSpace::RGB_16) :
			ndims==4	?sedeen::Color(sedeen::ColorSpace::RGBA_16) : sedeen::Color(0, 1);
		isSigned = true;
		depth = 16;
		break;
	case NPY_INT:
		assert(false && "Unknown or unsupported Color!");
		break;
	case NPY_INT32:
		assert(false && "Unknown or unsupported Color!");
		break;
	case NPY_FLOAT:
		assert(false && "Unknown or unsupported Color!");
		break;
	case NPY_DOUBLE:
		assert(false && "Unknown or unsupported Color!");
		break;
	}

	auto dtype = PyArray_DescrFromType(typenum);
	NpyIter* iter = NpyIter_New(oarr, NPY_ITER_READONLY|
		NPY_ITER_EXTERNAL_LOOP|
		NPY_ITER_REFS_OK,
		NPY_KEEPORDER, NPY_NO_CASTING,
		dtype);
	Py_DECREF(dtype);
	if (iter == NULL) {
		return nullptr;
	}

	NpyIter_IterNextFunc* iternext = NpyIter_GetIterNext(iter, NULL);
	if (iternext == NULL) {
		NpyIter_Deallocate(iter);
		return nullptr;
	}
	/* The location of the data pointer which the iterator may update */
	char** dataptr = NpyIter_GetDataPtrArray(iter);
	/* The location of the stride which the iterator may update */
	npy_intp* strideptr = NpyIter_GetInnerStrideArray(iter);
	/* The location of the inner loop size which the iterator may update */
	npy_intp* innersizeptr = NpyIter_GetInnerLoopSizePtr(iter);

	const npy_intp* _sizes = PyArray_SHAPE(oarr);
	sedeen::Size sz(_sizes[1], _sizes[0]);
	sedeen::image::RawImage image_out = sedeen::image::RawImage(sz, color);
	unsigned int idx =0;
	do {
		/* Get the inner loop data/stride/count values */
		char* data = *dataptr;
		npy_intp stride = *strideptr;
		npy_intp count = *innersizeptr;

		/* This is a typical inner loop for NPY_ITER_EXTERNAL_LOOP */
		while (count--) {
			image_out.setValue(idx, *data);
			data += stride;			
			idx++;
		}

		/* Increment the iterator to the next inner loop */
	} while(iternext(iter));

	NpyIter_Deallocate(iter);

	image = image_out;
	return true;
}

bool PythonObject::pyobject_to(PyObject* obj, bool& value )
{
	if(!obj || obj == Py_None)
		return true;
	int _val = PyObject_IsTrue(obj);
	if(_val < 0)
		return false;
	value = _val > 0;
	return true;
}

bool PythonObject::pyobject_to(PyObject* obj, int& value )
{
	if(!obj || obj == Py_None)
		return true;
	if(PyInt_Check(obj))
		value = (int)PyInt_AsLong(obj);
	else if(PyLong_Check(obj))
		value = (int)PyLong_AsLong(obj);
	else
		return false;
	return value != -1 || !PyErr_Occurred();
}

bool PythonObject::pyobject_to(PyObject* obj, double& value )
{
	if(!obj || obj == Py_None)
        return true;
    if(!!PyInt_CheckExact(obj))
        value = (double)PyInt_AS_LONG(obj);
    else
        value = PyFloat_AsDouble(obj);
    return !PyErr_Occurred();
}

bool PythonObject::pyobject_to(PyObject* obj, float& value )
{
	if(!obj || obj == Py_None)
        return true;
    if(!!PyInt_CheckExact(obj))
        value = (float)PyInt_AS_LONG(obj);
    else
        value = (float)PyFloat_AsDouble(obj);
    return !PyErr_Occurred();
}

bool PythonObject::pyobject_to(PyObject* obj, std::string& value )
{
	if(!obj || obj == Py_None)
        return true;
    char* str = PyString_AsString(obj);
    if(!str)
        return false;
    value = std::string(str);
    return true;
}

bool PythonObject::pyobject_to(PyObject* obj, sedeen::Color& value )
{
	//Not implemented
	return true;
}

bool PythonObject::pyobject_to(PyObject* obj, sedeen::Size& value )
{
	//Not implemented
	return true;
}

PythonObject* PythonObject::pyobject_from(const sedeen::image::RawImage& image)
{
	if( image.isNull() )
		new PythonObject(Py_None, false);
		//Py_RETURN_NONE;

	int rc=0;
	if(PyArray_API == NULL){
		rc = _import_array();	}	
	assert( rc== 0);

	 npy_intp* shape = new npy_intp[3];
     shape[0] = image.height();
     shape[1] = image.width();
     shape[2] = image.bytesPerPixel();

	auto image_type = image.color().colorSpace();
	bool isSigned = image.isSigned();
	char typenum; // = NPY_UBYTE;
	if(!isSigned) {
		switch(image_type) {
		case sedeen::ColorSpace::Gray_8:
		case sedeen::ColorSpace::RGB_8:
		case sedeen::ColorSpace::RGBA_8:
		case sedeen::ColorSpace::RGBA_8_Premultiplied:
			typenum = NPY_UBYTE;
			break;
		case sedeen::ColorSpace::Gray_12:
			assert("No such convertion" && false);
			break;
		case sedeen::ColorSpace::RGB_16:
		case sedeen::ColorSpace::Gray_16:
		case sedeen::ColorSpace::RGBA_16:
			typenum = NPY_USHORT;
			break;
		default:
			assert(false && "Unknown colorspace");
		}
	}
	else {
		switch(image_type) {
		case sedeen::ColorSpace::Gray_8:
		case sedeen::ColorSpace::RGB_8:
		case sedeen::ColorSpace::RGBA_8:
		case sedeen::ColorSpace::RGBA_8_Premultiplied:
			typenum = NPY_BYTE;
			break;
		case sedeen::ColorSpace::Gray_12:
			assert("No such convertion" && false);
			break;
		case sedeen::ColorSpace::RGB_16:
		case sedeen::ColorSpace::Gray_16:
		case sedeen::ColorSpace::RGBA_16:
			typenum = NPY_SHORT;
			break;
		default:
			assert(false && "Unknown colorspace");
		}
	}

	/*if(typenum)
		python::PythonException::translateException() ;*/
	void* buffer = const_cast<void*>(static_cast<const void*>(image.data().get()));
 	//PyObject* obj = PyArray_SimpleNew(dims, _sizes, typenum);
	PyObject* obj = PyArray_SimpleNewFromData(3, shape, typenum, buffer);
	assert(obj != NULL);
	PyArrayObject* oarr = (PyArrayObject*)obj;
	Py_INCREF(obj);
	return new PythonObject((PyObject*)oarr, true);
}

PythonObject* PythonObject::pyobject_from(const bool& value )
{
	return new PythonObject(PyBool_FromLong(value), true);
}

PythonObject* PythonObject::pyobject_from(const int& value )
{
	return new PythonObject(PyInt_FromLong(int(value)), true);
}

PythonObject* PythonObject::pyobject_from(const double& value )
{
	return new PythonObject(PyFloat_FromDouble(value), true);
}

PythonObject* PythonObject::pyobject_from(const float& value )
{
	return new PythonObject(PyFloat_FromDouble(value), true);
}

PythonObject* PythonObject::pyobject_from(const std::string& value )
{
	return new PythonObject(PyString_FromString(value.empty() ? "" : value.c_str()), true);
}

PythonObject* PythonObject::pyobject_from(const sedeen::Color& value )
{
	//Not implemented
	return new PythonObject(PyInt_FromLong(0), true);
}

PythonObject* PythonObject::pyobject_from(const sedeen::Size& value )
{
	//Not implemented
	return new PythonObject(PyInt_FromLong(0), true);
}

} // namespace python
} //namespace sedeen
