#include "stdafx.h"
#include "ColladaImage.h"

#include "FCDocument/FCDObjectWithId.h"

ColladaImage::ColladaImage(FCDImage &image) :
	_image(&image)
{
}

std::string ColladaImage::GetDaeId() const
{
	return _image->GetDaeId().c_str();
}

Image* ColladaImage::Convert()
{
	Image* image = new Image(_image->GetFilename().c_str());
	image->SetName(_image->GetDaeId().c_str());
	
	return image;
}