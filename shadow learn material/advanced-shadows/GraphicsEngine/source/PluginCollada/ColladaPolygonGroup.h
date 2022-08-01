#pragma once

#include <string>

#include "FCDocument/FCDGeometryPolygons.h"

class ColladaPolygonGroup
{
public:
	ColladaPolygonGroup(FCDGeometryPolygons& geometryPolygons);

	std::string GetMaterialSemantic() const;

	Geometry* Convert();

private:
	static Logger logger;

	FCDGeometryPolygons* _geometryPolygons;
};