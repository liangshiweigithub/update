#include "stdafx.h"
#include "ColladaPolygonGroup.h"

#include "FCDocument/FCDGeometryPolygonsInput.h"

Logger ColladaPolygonGroup::logger("ColladaPolygonGroup");

ColladaPolygonGroup::ColladaPolygonGroup(FCDGeometryPolygons &geometryPolygons) :
	_geometryPolygons(&geometryPolygons)
{
}

std::string ColladaPolygonGroup::GetMaterialSemantic() const
{
	return _geometryPolygons->GetMaterialSemantic().c_str();
}

Geometry* ColladaPolygonGroup::Convert()
{
	Geometry* geometry = new Geometry();

	for (unsigned int iInput = 0; iInput < _geometryPolygons->GetInputCount(); ++iInput)
	{
		const FCDGeometryPolygonsInput* PolygonInput = _geometryPolygons->GetInput(iInput);

		switch (PolygonInput->GetSemantic())
		{
		case FUDaeGeometryInput::POSITION:
			for (unsigned int i = 0; i < PolygonInput->GetIndexCount(); ++i)
				geometry->AddVertexIndex(PolygonInput->GetIndices()[i]);
			break;
		case FUDaeGeometryInput::NORMAL:
			for (unsigned int i = 0; i < PolygonInput->GetIndexCount(); ++i)
				geometry->AddNormalIndex(PolygonInput->GetIndices()[i]);
			break;
		case FUDaeGeometryInput::TEXCOORD:
			for (unsigned int i = 0; i < PolygonInput->GetIndexCount(); ++i)
				geometry->AddTexCoordIndex(PolygonInput->GetIndices()[i]);
			break;
		default:
			Logger::Stream(logger, Logger::Warn) << "Unsupported Polygon semantic '" << PolygonInput->GetSemantic() << "'.";
			break;
		};
	}
	
	return geometry;
}