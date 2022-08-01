#pragma once

#include "FCDocument/FCDocument.h"

#include "ColladaImage.h"

class ColladaLibraryManager
{
public:
	ColladaLibraryManager(Model& model, FCDocument& document);

	Mesh* GetGeometry(FCDGeometry* geometry);

	Material* GetMaterial(FCDMaterial* fcdMaterial);

	Image* GetImage(ColladaImage colladaImage);

private:
	static Logger logger;

	Model* _model;
	FCDocument* _document;
};