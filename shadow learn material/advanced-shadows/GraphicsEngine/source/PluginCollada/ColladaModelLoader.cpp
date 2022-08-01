#include "stdafx.h"
#include "ColladaModelLoader.h"
#include "ColladaSceneNode.h"
#include "ColladaLibraryManager.h"
#include "ColladaGeometry.h"

#include "FCDocument/FCDocument.h"
#include "FCDocument/FCDLibrary.h"
#include "FCDocument/FCDEntity.h"
#include "FCDocument/FCDSceneNode.h"
#include "FCDocument/FCDGeometryInstance.h"
#include "FCDocument/FCDMaterialInstance.h"

Logger ColladaModelLoader::logger("ColladaModelLoader");

Model* ColladaModelLoader::Load(const std::string& fileName)
{
	Model* model = 0;

	FCollada::Initialize();
	FCDocument* document = FCollada::NewTopDocument();
	
	if(FCollada::LoadDocumentFromFile(document, fileName.c_str()))
	{
		if (logger.IsEnabled(Logger::Info))
			Logger::Stream(logger, Logger::Info) << "Load file=" << fileName.c_str() 
			<< " (FCollada v" << (FCOLLADA_VERSION >> 16) << "." << (FCOLLADA_VERSION & 0xFFFF)
			<< ").";
	
		FCDSceneNode* sceneNode = document->GetVisualSceneInstance();

		if(sceneNode)
		{
			ColladaSceneNode node(*sceneNode);

			if (logger.IsEnabled(Logger::Debug))
				Logger::Stream(logger, Logger::Debug) << "Create model '" << node.GetName() << "'.";

			model = new Model();
			model->SetName(node.GetName());

			ModelNode* modelNode = new ModelNode(*model);
			modelNode->SetName(node.GetName());			

			if (logger.IsEnabled(Logger::Debug))
				Logger::Stream(logger, Logger::Debug) << "Add model '" << model->GetName() 
					<< "' to model node '" << modelNode->GetName() << "'.";

			model->SetModelNode(modelNode);

			ColladaLibraryManager manager(*model, *document);

			for (unsigned int i = 0; i < node.GetChildCount(); ++i)
			{
				GroupNode* groupNode = LoadSceneNode(node.GetChild(i), manager);

				if (groupNode)
				{
					if (logger.IsEnabled(Logger::Debug))
						Logger::Stream(logger, Logger::Debug) << "Add group node '" << groupNode->GetName() 
							<< "' to model node '" << modelNode->GetName() << "'.";

					modelNode->AddChild(groupNode);
				}
			}
		}
	}
	else
		Logger::Stream(logger, Logger::Error) << "Cannot load file: " << fileName << ".";

	SAFE_DELETE(document);

	FCollada::Release();
	
	return model;
}

Model* ColladaModelLoader::Load(const std::string& fileName, RenderSystem& renderSystem, float start, float end)
{
	Model* model = 0;

	FCollada::Initialize();
	FCDocument* document = FCollada::NewTopDocument();
	
	if(FCollada::LoadDocumentFromFile(document, fileName.c_str()))
	{
		if (logger.IsEnabled(Logger::Info))
			Logger::Stream(logger, Logger::Info) << "Load file=" << fileName.c_str() 
			<< " (FCollada v" << (FCOLLADA_VERSION >> 16) << "." << (FCOLLADA_VERSION & 0xFFFF)
			<< ").";
	
		FCDSceneNode* sceneNode = document->GetVisualSceneInstance();

		if(sceneNode)
		{
			ColladaSceneNode node(*sceneNode);

			if (logger.IsEnabled(Logger::Debug))
				Logger::Stream(logger, Logger::Debug) << "Create model '" << node.GetName() << "'.";

			model = new Model();
			model->SetName(node.GetName());

			ModelNode* modelNode = new ModelNode(*model);
			modelNode->SetName(node.GetName());			

			if (logger.IsEnabled(Logger::Debug))
				Logger::Stream(logger, Logger::Debug) << "Add model '" << model->GetName() 
					<< "' to model node '" << modelNode->GetName() << "'.";

			model->SetModelNode(modelNode);

			ColladaLibraryManager manager(*model, *document);

			float step = (end - start) / static_cast<float>(node.GetChildCount());

			for (unsigned int i = 0; i < node.GetChildCount(); ++i)
			{
				GroupNode* groupNode = LoadSceneNode(node.GetChild(i), manager);

				if (groupNode)
				{
					if (logger.IsEnabled(Logger::Debug))
						Logger::Stream(logger, Logger::Debug) << "Add group node '" << groupNode->GetName() 
							<< "' to model node '" << modelNode->GetName() << "'.";

					modelNode->AddChild(groupNode);
				}

				unsigned int percent = static_cast<unsigned int>(start + step * (i + 1));
				renderSystem.DrawStatus(percent);
			}
		}
	}
	else
		Logger::Stream(logger, Logger::Error) << "Cannot load file: " << fileName << ".";

	SAFE_DELETE(document);

	FCollada::Release();
	
	return model;
}

GroupNode* ColladaModelLoader::LoadSceneNode(ColladaSceneNode node, ColladaLibraryManager& manager)
{
	GroupNode* groupNode = new GroupNode();

	groupNode->SetName(node.GetName());
	groupNode->GetTransform().SetMatrix(node.GetPosition());

	for (unsigned int i = 0; i < node.GetInstanceCount(); i++)
	{
		FCDEntityInstance* entityInstance = node.GetInstance(i);

		switch (entityInstance->GetType())
		{
			case FCDEntityInstance::GEOMETRY:
				Node* node = LoadGeometryInstance(dynamic_cast<FCDGeometryInstance*>(entityInstance), manager);

				if (node)
				{
					if (logger.IsEnabled(Logger::Debug))
						Logger::Stream(logger, Logger::Debug) << "Add node '" << node->GetName() 
							<< "' to group node '" << groupNode->GetName() << "'.";

					groupNode->AddChild(node);
				}
				break;
		}
	}

	for(unsigned int i = 0; i < node.GetChildCount(); ++i)
	{
		GroupNode* childGroupNode = LoadSceneNode(node.GetChild(i), manager);
		
		if (childGroupNode)
		{
			if (logger.IsEnabled(Logger::Debug))
				Logger::Stream(logger, Logger::Debug) << "Add group node '" << childGroupNode->GetName() 
					<< "' to group node '" << groupNode->GetName() << "'.";

			groupNode->AddChild(childGroupNode);
		}
	}
	
	return groupNode;
}

Node* ColladaModelLoader::LoadGeometryInstance(FCDGeometryInstance* geometryInstance, ColladaLibraryManager& manager)
{
	if (!geometryInstance)
		return 0;

	FCDGeometry* fcdGeometry = dynamic_cast<FCDGeometry*>(geometryInstance->GetEntity());

	Mesh* mesh = manager.GetGeometry(fcdGeometry);

	if (!mesh)
		return 0;

	MeshNode* node = new MeshNode(*mesh);
	node->SetName(mesh->GetName());

	std::map<std::string, StateNode*> materialMap;
	for (unsigned int iMat = 0; iMat < geometryInstance->GetMaterialInstanceCount(); ++iMat)
	{
		FCDMaterialInstance* materialInstance = geometryInstance->GetMaterialInstance(iMat);					
		
		std::string semantic(materialInstance->GetSemantic());

		if (materialMap.find(semantic) == materialMap.end())
		{
			Material* material = manager.GetMaterial(materialInstance->GetMaterial());

			if (material)
			{
				StateNode* materialNode = new StateNode(*material);
				materialNode->SetName(material->GetName());

				materialMap.insert(std::pair<std::string, StateNode*>(semantic, materialNode));
				
				if (logger.IsEnabled(Logger::Debug))
					Logger::Stream(logger, Logger::Debug) << "Add state node '" << materialNode->GetName() 
						<< "' to node '" << node->GetName() << "'.";

				node->AddChild(materialNode);
			}
		}
	}

	for (std::map<std::string, StateNode*>::iterator iter = materialMap.begin();
		iter != materialMap.end(); iter++)
	{
		ColladaGeometry colladaGeometry(*fcdGeometry);
		
		for (unsigned int iPoly = 0; iPoly < colladaGeometry.GetPolygonGroupCount(); ++iPoly)
		{
			if (iter->first == colladaGeometry.GetPolygonGroup(iPoly).GetMaterialSemantic())
				if (Geometry* geometry = mesh->GetGeometry(iPoly))
				{
					DrawableNode* drawableNode = new DrawableNode(*geometry);
					drawableNode->SetName(geometry->GetName());

					if (logger.IsEnabled(Logger::Debug))
						Logger::Stream(logger, Logger::Debug) << "Add drawable node '" << drawableNode->GetName()
							<< "' to state node '" << iter->second->GetName() << "'.";

					iter->second->AddChild(drawableNode);
				}
		}
	}

	return node;
}
