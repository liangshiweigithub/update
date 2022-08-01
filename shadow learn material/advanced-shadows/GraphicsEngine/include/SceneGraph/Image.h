#pragma once

#include "SceneGraphExports.h"
#include "Common/Object.h"
#include <string>

class SCENEGRAPH_API Image : public Object
{
public:
	Image(const std::string& filename);
	~Image();

	unsigned int& GetBindId();
	unsigned int GetBindId() const;
	void SetBindId(unsigned int id);

	const std::string& GetFilename() const;
	void SetFilename(std::string& filename);

private:
	unsigned int _bindId;
	std::string _filename;

};