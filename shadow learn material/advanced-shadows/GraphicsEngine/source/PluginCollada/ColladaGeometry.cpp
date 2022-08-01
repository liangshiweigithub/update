#include "stdafx.h"
#include "ColladaGeometry.h"

#include "FCDocument/FCDGeometryMesh.h"
#include "FCDocument/FCDGeometryPolygonsTools.h"
#include "FCDocument/FCDGeometrySource.h"

Logger ColladaGeometry::logger("ColladaGeometry");

ColladaGeometry::ColladaGeometry(FCDGeometry& geometry) :
	_geometry(&geometry)
{
}

std::string ColladaGeometry::GetDaeId() const
{
	return _geometry->GetDaeId().c_str();
}

bool ColladaGeometry::IsMesh() const
{
	return _geometry->IsMesh();
}

unsigned int ColladaGeometry::GetPolygonGroupCount() const
{
	if (IsMesh())
		return static_cast<unsigned int>(_geometry->GetMesh()->GetPolygonsCount());

	return 0;
}

ColladaPolygonGroup ColladaGeometry::GetPolygonGroup(unsigned int index) const
{
	return ColladaPolygonGroup(*_geometry->GetMesh()->GetPolygons(index));
}

Mesh* ColladaGeometry::Convert()
{
	if (!IsMesh())
		return 0;

	Mesh* mesh = new Mesh();
	mesh->SetName(_geometry->GetDaeId().c_str());

	FCDGeometryMesh* geometryMesh = _geometry->GetMesh();
	if (!geometryMesh->IsTriangles())
		FCDGeometryPolygonsTools::Triangulate(geometryMesh);

	for (unsigned int iSource = 0; iSource < geometryMesh->GetSourceCount(); ++iSource)
	{
		const FCDGeometrySource* geometrySource = geometryMesh->GetSource(iSource);
		
		switch(geometrySource->GetType())
		{
		case FUDaeGeometryInput::POSITION:
			if (geometrySource->GetStride() != 3)
				Logger::Stream(logger, Logger::Warn) << "Vertex data of mesh '" << mesh->GetName() 
					<< "' has a stride of '" << geometrySource->GetStride() << "'.";

			for (unsigned int iData = 0; iData < geometrySource->GetDataCount(); iData += geometrySource->GetStride())
			{
				Mesh::tVertex vertex(geometrySource->GetData()[iData],
					geometrySource->GetData()[iData + 1],
					geometrySource->GetData()[iData + 2]);

				mesh->AddVertex(vertex);
				mesh->GetBoundingVolume().Expand(static_cast<tVec3f>(vertex));
			}
			break;
		case FUDaeGeometryInput::NORMAL:
			if (geometrySource->GetStride() != 3)
				Logger::Stream(logger, Logger::Warn) << "Normal data of mesh '" << mesh->GetName() 
					<< "' has a stride of '" << geometrySource->GetStride() << "'.";

			for (unsigned int iData = 0; iData < geometrySource->GetDataCount(); iData += geometrySource->GetStride())
			{
				mesh->AddNormal(Mesh::tNormal(geometrySource->GetData()[iData],
					geometrySource->GetData()[iData + 1],
					geometrySource->GetData()[iData + 2]));
			}
			break;
		case FUDaeGeometryInput::TEXCOORD:
			if (geometrySource->GetStride() != 3)
				Logger::Stream(logger, Logger::Warn) << "Texcoord data of mesh '" << mesh->GetName() 
					<< "' has a stride of '" << geometrySource->GetStride() << "'.";

			for (unsigned int iData = 0; iData < geometrySource->GetDataCount(); iData += geometrySource->GetStride())
			{
				mesh->AddTexCoord(Mesh::tTexCoord(geometrySource->GetData()[iData],
					geometrySource->GetData()[iData + 1]));
			}
			break;
		default:
			Logger::Stream(logger, Logger::Warn) << "Unsupported geometry source type '" << geometrySource->GetType() 
				<< "' in mesh '" << mesh->GetName() << "'.";
			break;
		};
	}

	return mesh;
}
