#include "stdafx.h"
#include "Image.h"

Image::Image(const std::string& filename) :
	_bindId(0),
	_filename(filename)
{
}

Image::~Image()
{
}

unsigned int& Image::GetBindId()
{
	return _bindId;
}

unsigned int Image::GetBindId() const
{
	return _bindId;
}

void Image::SetBindId(unsigned int id)
{
	_bindId = id;
}

const std::string& Image::GetFilename() const
{
	return _filename;
}

void Image::SetFilename(std::string& filename)
{
	_filename = filename;
}