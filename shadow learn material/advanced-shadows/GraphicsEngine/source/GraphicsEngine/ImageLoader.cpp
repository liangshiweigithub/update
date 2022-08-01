#include "stdafx.h"
#include "ImageLoader.h"
#include "FreeImage/FreeImage.h"

/* ImageLoader::Imagefile ****************************************************/
ImageLoader::ImageFile::ImageFile(FIBITMAP* fiBitmap) :
	_fiBitmap(fiBitmap)
{
}

ImageLoader::ImageFile::~ImageFile()
{

}

unsigned int ImageLoader::ImageFile::GetWidth() const
{
	return FreeImage_GetWidth(_fiBitmap);
}


unsigned int ImageLoader::ImageFile::GetHeight() const
{
	return FreeImage_GetHeight(_fiBitmap);
}

tVec4b ImageLoader::ImageFile::GetPixelColor(int x, int y)
{
	RGBQUAD value;
	FreeImage_GetPixelColor(_fiBitmap, x, y, &value);
	
	return tVec4b(value.rgbRed, value.rgbGreen, value.rgbBlue, value.rgbReserved);
}

const void* ImageLoader::ImageFile::GetPixelData() const
{
	return FreeImage_GetBits(_fiBitmap);
}

FIBITMAP* ImageLoader::ImageFile::GetFiBitmap()
{
	return _fiBitmap;
}

/* ImageLoader ***************************************************************/
ImageLoader::ImageLoader()
{
}

ImageLoader::~ImageLoader()
{
}

ImageLoader::ImageFile* ImageLoader::LoadImageFile(const std::string& filename)
{
	FREE_IMAGE_FORMAT fiFormat = FreeImage_GetFileType(filename.c_str(), 0);
	
	if(fiFormat == FIF_UNKNOWN)
		fiFormat = FreeImage_GetFIFFromFilename(filename.c_str());
	
	if(fiFormat == FIF_UNKNOWN)
	{
		std::cout << "[ERROR] ImageLoader::ImageFile: Unsupported imagefile: "
			<< filename.c_str() << std::endl;
		return 0;
	}

	if(!FreeImage_FIFSupportsReading(fiFormat))
	{
		std::cout << "[ERROR] ImageLoader::ImageFile: Cannot read from file: "
			<< filename.c_str() << std::endl;
		return 0;
	}
	
	FIBITMAP* bitmap = FreeImage_Load(fiFormat, filename.c_str());

	if(FreeImage_GetColorType(bitmap) == FIC_RGB)
	{
		FreeImage_AdjustGamma(bitmap,2.2);
		bitmap = FreeImage_ConvertTo32Bits(bitmap);
	}

	return new ImageFile(bitmap);
}

void ImageLoader::UnLoadImageFile(ImageLoader::ImageFile* imageFile)
{
	if(imageFile)
	{
		FreeImage_Unload(imageFile->GetFiBitmap());

		delete imageFile;
	}
}