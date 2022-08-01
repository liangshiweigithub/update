#include "stdafx.h"

#include "Font.h"

Font::Font(const std::string& name, int size, bool bold, bool italic) :
	_size(size),
	_bold(bold),
	_italic(italic),
	_id(0)
{
	SetName(name);
}

int Font::GetSize() const
{
	return _size;
}

bool Font::GetBoldFlag() const
{
	return _bold;
}

bool Font::GetItalicFlag() const
{
	return _italic;
}

unsigned int Font::GetId() const
{
	return _id;
}

void Font::SetId(unsigned int id)
{
	_id = id;
}