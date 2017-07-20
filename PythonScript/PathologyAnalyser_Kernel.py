from skimage import color, filters
from skimage.exposure import rescale_intensity
import numpy as np

class FilterImage():
    """
    class Filter image.
    
    Appling different filters.
    
    """

    def __init__(self, image, block_size, offset, method, mode):
        """
        Generate filter object
        """
        self._image = np.asanyarray(image)
        self._block_size = block_size
        self._offset = offset
        self._method = method
        self._mode = mode

    def soble_edge(self):
        
        each_channel = self._image[:,:,0]
        soble_image = filters.sobel(each_channel)
        soble_image = rescale_intensity(1- soble_image, out_range=(0.0, 255.0))
        
        return (soble_image.astype(np.uint8))
    
    def rgb2gray(self):
        image_gray = color.rgb2gray(self._image)
        image_gray *= 255.0/image_gray.max()
        #image_gray = rescale_intensity(image_gray, out_range=(0.0, 255.0))
        
        return (image_gray.astype(np.uint8))
    
    def rgb2lab(self):
        
        img_lab = color.rgb2lab(self._image)
        #image_gray *= 255.0/image_gray.max()
        img_lab = rescale_intensity(img_lab, out_range=(0.0, 255.0))
        
        return (img_lab.astype(np.uint8))
    
    def global_thresholding(self):
        
        image_gray = color.rgb2gray(self._image)
        global_thresh = filters.threshold_otsu(image_gray)
        binary_global = image_gray > global_thresh
        
        return (255*binary_global.astype(np.uint8))
        
    def adaptive_thresholding(self):
        
        #block_size = 55            
        image_gray = color.rgb2gray(self._image)
        binary_adaptive = filters.threshold_adaptive(image_gray, block_size =self._block_size ,
                                                     method=self._method, offset=self._offset)
        
        return (255*binary_adaptive.astype(np.uint8))                               

    def run(self):
                
        """
        Apply adaptive thresholding
        
        Parameters
        ----------
        image : ndarray
		The image in RGB format
    
        Returns : ndarray
		The image in binary format
        -------
    
    
        Examples
        --------
		"""
        return self.adaptive_thresholding()
        
