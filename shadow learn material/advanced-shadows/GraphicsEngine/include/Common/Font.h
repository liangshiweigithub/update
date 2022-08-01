#pragma once

#include "CommonExports.h"
#include "Object.h"

class COMMON_API Font : public Object
{
public:
	Font(const std::string& name, int size, bool bold, bool italic);

	int GetSize() const;
	bool GetBoldFlag() const;
	bool GetItalicFlag() const;

	unsigned int GetId() const;
	void SetId(unsigned int id);

private:
	unsigned int _id;
	int _size;
	bool _bold;
	bool _italic;

};