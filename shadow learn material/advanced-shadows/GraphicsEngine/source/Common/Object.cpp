#include "stdafx.h"
#include "Object.h"

Object::Object() :
	_name("")
{
}

Object::Object(const std::string& name) :
	_name(name)
{
}

Object::~Object()
{
}

const std::string& Object::GetName() const
{
	return _name;
}

void Object::SetName(const std::string& name)
{
	_name = name;
}