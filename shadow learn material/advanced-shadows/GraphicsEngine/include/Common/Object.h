#pragma once

#include <string>

#include "CommonExports.h"

class COMMON_API Object
{
public:
	Object();
	Object(const std::string& name);

	virtual ~Object();

	const std::string& GetName() const;
	void SetName(const std::string& name);
	
private:
	std::string _name;
};