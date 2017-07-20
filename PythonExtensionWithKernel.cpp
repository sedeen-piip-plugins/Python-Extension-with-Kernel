// PythonExtension.cpp : Defines the exported functions for the DLL application.
//
// Primary header
#include "PythonExtensionWithKernel.h"

#include <algorithm>
#include <cassert>
#include <sstream>
#include <string>
#include <iostream>


#include "QtWidgets\qmessagebox.h"

// DPTK headers
#include "Algorithm.h"
#include "Geometry.h"
#include "Global.h"
#include "Image.h"
#include "image/io/Image.h"
#include "image/tile/Factory.h"

// Poco header needed for the macros below 
#include <Poco/ClassLibrary.h>

// Declare that this object has AlgorithmBase subclasses
//  and declare each of those sub-classes
POCO_BEGIN_MANIFEST(sedeen::algorithm::AlgorithmBase)
POCO_EXPORT_CLASS(sedeen::algorithm::PythonExtension)
POCO_END_MANIFEST

namespace sedeen {
namespace algorithm {

PythonExtension::PythonExtension()
	: display_area_(),
	result_(),
	python_class_name_(),
	block_size_(),
	method_(),
	offset_(),
	mode_(),
	pythonKernel_factory_(nullptr)
{
	//options_method_.push_back("generic");
	options_method_.push_back("gaussian");
	options_method_.push_back("mean");
	options_method_.push_back("median");

	options_mode_.push_back("reflect");
	options_mode_.push_back("constant");
	options_mode_.push_back("nearest");
	options_mode_.push_back("mirror");
	options_mode_.push_back("wrap");
}


PythonExtension::~PythonExtension()
{

}

void PythonExtension::run() {
// Has display area changed
auto display_changed = display_area_.isChanged();

// Build the segmentation pipeline
auto pipeline_changed = buildPipeline();

// Update results
if (pipeline_changed || display_changed ) {

	result_.update(pythonKernel_factory_, display_area_, *this);

}

// Ensure we run again after an abort
// 
// a small kludge that causes buildPipeline() to return TRUE
if (askedToStop()) {					
	pythonKernel_factory_.reset();
}

}

void PythonExtension::init(const image::ImageHandle& image) {
if (isNull(image)) return;
//
// bind algorithm members to UI and initialize their properties
//

// Bind system parameter for current view
display_area_ = createDisplayAreaParameter(*this);

python_class_name_ = createTextFieldParameter(*this,
                                    "python class name",
                                    "User defined python class name",
                                    "",
                                    0);

block_size_ = createIntegerParameter(*this,
                                    "Block_size",
                                    "Odd size of pixel neighborhood which is used to calculate the threshold value",
                                    35, 3, 105, 0);
method_ = createOptionParameter(*this,
                                    "Method",
                                    "Method used to determine adaptive threshold.",
                                    0, options_method_, 0);

offset_ = createDoubleParameter(*this,
                                    "Offset",
                                    " Constant subtracted from weighted mean of neighborhood.",
                                    0, -100, 100, 0);

mode_ = createOptionParameter(*this,
								"Mode",
								"The mode parameter determines how the array borders are handled.",
								0, options_mode_, 0);


// Bind result
result_ = createImageResult(*this, " PythonExtensionResult");

}

bool PythonExtension::buildPipeline() {
using namespace image::tile;
bool pipeline_changed = false;

// Get source image properties
auto source_factory = image()->getFactory();
auto source_color = source_factory->getColor();	

if((nullptr == pythonKernel_factory_))
{
	pathToPythonScript_ = openFile();
}

if ( display_area_.isChanged() || block_size_.isChanged() ||
		method_.isChanged()|| offset_.isChanged() ||
		mode_.isChanged() || (nullptr == pythonKernel_factory_) ) { //|| pipeline_changed
		// Build python Kernel	
		std::string python_class_name = python_class_name_;
		std::string method = options_method_.at(method_);
		std::string mode = options_mode_.at(mode_);
		std::vector<std::string> params;
		params.push_back(method);
		params.push_back(mode);
		int block_size = block_size_;
		if(block_size_%2 == 0){
			block_size++;
		}
		auto python_kernel = 
			std::make_shared<image::tile::PythonKernel>(pathToPythonScript_, python_class_name, 
															block_size , (float)offset_ , params);

		// Create a Factory for the composition of these Kernels
		auto non_cached_factory = 
			std::make_shared<FilterFactory>(source_factory,  python_kernel);

		// Wrap resulting Factory in a Cache for speedy results
		pythonKernel_factory_ = 
			std::make_shared<Cache>(non_cached_factory, RecentCachePolicy(30));

		pipeline_changed = true;
}

return pipeline_changed;
}

std::string PythonExtension::generateReport() const{

	std::string temp;
	return temp;
}

std::string PythonExtension::openFile(void)
{
	OPENFILENAME ofn;
	char szFileName[MAX_PATH]="";
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn); 
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "*.py";
	ofn.lpstrFile = (LPSTR)szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	GetOpenFileName(&ofn);

	return ofn.lpstrFile;
}


} // namespace algorithm
} // namespace sedeen


