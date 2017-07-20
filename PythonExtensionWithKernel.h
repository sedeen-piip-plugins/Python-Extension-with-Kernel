#ifndef SEDEEN_SRC_PLUGINS_PYTHONEXTENSION_PYTHONEXTENSION_H
#define SEDEEN_SRC_PLUGINS_PYTHONEXTENSION_PYTHONEXTENSION_H

// DPTK headers - a minimal set 
#include "algorithm/AlgorithmBase.h"
#include "algorithm/Parameters.h"
#include "algorithm/Results.h"
#include "PythonKernel.h"

#include <Windows.h>

namespace sedeen {
namespace tile {

} // namespace tile

namespace algorithm {


class PythonExtension : public algorithm::AlgorithmBase {
public:
	PythonExtension();
	~PythonExtension();

private:
	// virtual function
	virtual void run();
	virtual void init(const image::ImageHandle& image);

	/// Creates the Color Deconvolution pipeline with a cache
	//
	/// \return 
	/// TRUE if the pipeline has changed since the call to this function, FALSE
	/// otherwise
	bool buildPipeline();

	/// Generates a report of the stain matrix calculated from ROI
	//
	/// Report is formatted as a table containing the cofficients
	/// of each stain
	//
	/// \return
	/// a string containing the cofficients of each stain calculated from ROI
	std::string generateReport(void) const;
	std::string openFile(void);

private:
	algorithm::DisplayAreaParameter display_area_;
	algorithm::TextFieldParameter python_class_name_; 
	algorithm::IntegerParameter block_size_;
	algorithm::OptionParameter method_;
	algorithm::DoubleParameter offset_;
	algorithm::OptionParameter mode_;
	std::vector<std::string> options_method_;
	std::vector<std::string> options_mode_;
	std::string pathToPythonScript_;
	/// The output result
	algorithm::ImageResult result_;		
	/// The intermediate image factory after color deconvolution
	std::shared_ptr<image::tile::Factory> pythonKernel_factory_;

	
};

} // namespace algorithm
} // namespace sedeen

#endif

