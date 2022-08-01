#include "stdafx.h"
#include "ColladaLibraryManager.h"
#include "ColladaGeometry.h"
#include "ColladaMaterial.h"

Logger ColladaLibraryManager::logger("ColladaLibraryManager");

ColladaLibraryManager::ColladaLibraryManager(Model& model, FCDocument& document) :
	_model(&model),
	_document(&document)
{
}

Mesh* ColladaLibraryManager::GetGeometry(FCDGeometry* geometry)
{
	if (!geometry)
		return 0;

	ColladaGeometry colladaGeometry(*geometry);

	Mesh* mesh = _model->GetMesh(colladaGeometry.GetDaeId());
	
	if (!mesh)
	{
		mesh = colladaGeometry.Convert();

		if (!mesh)
		{
			Logger::Stream(logger, Logger::Warn) << "Can not convert '" << colladaGeometry.GetDaeId() 
				<< "' to mesh in model '" << _model->GetName() << "'.";

			return mesh;
		}

		for (unsigned int i = 0; i < colladaGeometry.GetPolygonGroupCount(); ++i)
		{
			Geometry* geometry = colladaGeometry.GetPolygonGroup(i).Convert();
			
			if (geometry)
			{
				std::stringstream name;
				name << mesh->GetName() << "_" << i;

				geometry->SetName(name.str());
				
				if (logger.IsEnabled(Logger::Debug))
					Logger::Stream(logger, Logger::Debug) << "Add geometry '" << geometry->GetName() 
						<< "' to mesh '" << mesh->GetName() << "'.";

				mesh->AddGeometry(geometry);
				geometry->SetMesh(mesh);
			}
		}

		if (logger.IsEnabled(Logger::Debug))
			Logger::Stream(logger, Logger::Debug) << "Add mesh '" << mesh->GetName() 
				<< "' to model '" << _model->GetName() << "'.";

		_model->AddMesh(mesh);
	}

	return mesh;
}

Material* ColladaLibraryManager::GetMaterial(FCDMaterial* fcdMaterial)
{
	if (!fcdMaterial)
		return 0;

	ColladaMaterial colladaMaterial(*fcdMaterial);

	Material* material = _model->GetMaterial(colladaMaterial.GetDaeId());
	
	if(!material)
	{
		material = colladaMaterial.Convert();

		for (unsigned int i = 0; i < colladaMaterial.GetTextureCount(FUDaeTextureChannel::DIFFUSE); ++i)
		{
			Image* image = GetImage(colladaMaterial.GetTexture(i, FUDaeTextureChannel::DIFFUSE));
			
			if(image)
			{	
				material->AddTexture(new Texture(*image, Texture::Diffuse,
					colladaMaterial.GetTextureTiling(i, FUDaeTextureChannel::DIFFUSE),
					colladaMaterial.GetTextureOffset(i, FUDaeTextureChannel::DIFFUSE),
					colladaMaterial.GetTextureRotation(i, FUDaeTextureChannel::DIFFUSE)));
			}
		}

		// TODO: do the same for other texture channels

		if (logger.IsEnabled(Logger::Debug))
			Logger::Stream(logger, Logger::Debug) << "Add material '" << material->GetName() << "' to model '" << _model->GetName() << "'.";

		_model->AddMaterial(material);
	}
	
	return material;
}

Image* ColladaLibraryManager::GetImage(ColladaImage colladaImage)
{
	Image* image = _model->GetImage(colladaImage.GetDaeId());

	if(!image)
	{
		image = colladaImage.Convert();

		if (logger.IsEnabled(Logger::Debug))
			Logger::Stream(logger, Logger::Debug) << "Add image '" << image->GetName() << "' to model '" << _model->GetName() << "'.";

		_model->AddImage(image);
	}
	
	return image;
}