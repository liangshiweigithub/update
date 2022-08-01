#pragma once

#include "FCDocument/FCDImage.h"

class ColladaImage
{
public:
	ColladaImage(FCDImage& image);

	std::string GetDaeId() const;

	Image* Convert();
	
private:
	FCDImage* _image;
};