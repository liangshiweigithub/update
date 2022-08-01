#pragma once

#include "FCDocument/FCDGeometry.h"
#include "FCDocument/FCDGeometryPolygons.h"

#include "ColladaPolygonGroup.h"

class ColladaGeometry
{
public:
	ColladaGeometry(FCDGeometry& geometry);

	std::string GetDaeId() const;

	bool IsMesh() const;

	unsigned int GetPolygonGroupCount() const;
	ColladaPolygonGroup GetPolygonGroup(unsigned int index) const;

	Mesh* Convert();

private:
	static Logger logger;

	FCDGeometry* _geometry;
};