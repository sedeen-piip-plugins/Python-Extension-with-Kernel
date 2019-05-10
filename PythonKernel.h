#ifndef DPTK_SRC_IMAGE_FILTER_KERNELS_PYTHONFILTER_H
#define DPTK_SRC_IMAGE_FILTER_KERNELS_PYTHONFILTER_H


#include <stdio.h>
#include <fstream>

#include "Global.h"
#include "Image.h"

#include "Python.h"

namespace sedeen {

namespace image {
namespace tile {	

class PATHCORE_IMAGE_API PythonKernel : public Kernel {
public:
	///  Method used to determine adaptive threshold for local neighbourhood in
    /// weighted mean image.
	enum Method {
		///User selected ROI
		generic,
		///rgb_from_hex: Hematoxylin + Eosin
		gaussian,
		///rgb_from_hdx: Hematoxylin + DAB
		mean,
		///rgb_from_hed: Hematoxylin + Eosin + DAB
		median,
	};

	/// The mode parameter determines how the array borders are handled, where
    /// cval is the value when mode is equal to 'constant'.
	enum Mode {
		reflect, constant, nearest, mirror, wrap,
	};
	/// Creates a colour deconvolution Kernel with selected 
	// color-deconvolution matrix
	//
	/// \param 
	/// 
	explicit PythonKernel( std::string, std::string, int, float, std::vector<std::string>& );

	virtual ~PythonKernel();

private:
	virtual RawImage doProcessData(const RawImage &source );
	virtual ColorSpace const &doGetColorSpace() const;

	//static std::ofstream log_file;

private:
	std::string pathToPythonScript_;
	std::string python_class_name_;
	std::string pyModuleName_;
	int block_size_;
	float offset_;
	std::string method_;
	std::string mode_;

  ColorSpace color_ = ColorSpace(ColorModel::RGB, 8);
};

} // namespace tile

} // namespace image
} // namespace sedeen
#endif

