#include "stdafx.h"
#include "float.h"
#include "Culler.h"
#include "Model.h"
#include "GroupNode.h"

Culler::Culler() :
	_isEnabled(false)
{
}

Culler::~Culler()
{
}

bool Culler::IsEnabled() const
{
	return _isEnabled;
}

void Culler::SetIsEnabled(bool enable)
{
	_isEnabled = enable;
}

const BoundingBox* Culler::GenerateBoundingVolumes(Model& model)
{
	return GenerateBoundingVolumes(*model.GetModelNode());
}

const BoundingBox* Culler::GenerateBoundingVolumes(Node& node)
{
	BoundingBox* bv = 0;

	GroupNode* groupNode = dynamic_cast<GroupNode*>(&node); 
	
	if (groupNode)
		bv = new BoundingBox();

	for (unsigned int i = 0; i < node.GetChildCount(); ++i)
	{
		const BoundingBox* childBV = GenerateBoundingVolumes(*node.GetChild(i));

		if (bv && childBV)
		{
			GroupNode* childNode = dynamic_cast<GroupNode*>(node.GetChild(i));

			if (childNode)
			{
				const tMat4f& localTransform = childNode->GetTransform().GetMatrix();

				//for (unsigned int i = 0; i < BoundingBox::CornerCOUNT; ++i)
				//{
				//	const tVec4f corner = tVec4f(childBV->GetCorner(static_cast<BoundingBox::Corner>(i)), 1.0f);
				//	bv->Expand(tVec3f(localTransform * corner));
				//}

				bv->Expand(tVec3f(localTransform * tVec4f(childBV->GetMinimum(), 1.0f)));
				bv->Expand(tVec3f(localTransform * tVec4f(childBV->GetMaximum(), 1.0f)));
			}
		}
		else if (bv)
		{
			const tMat4f& localTransform = groupNode->GetTransform().GetMatrix();
			
			//const BoundingBox* tempBV = groupNode->GetBoundingVolume();
			//for (unsigned int i = 0; i < BoundingBox::CornerCOUNT; ++i)
			//{
			//	const tVec4f corner = tVec4f(tempBV->GetCorner(static_cast<BoundingBox::Corner>(i)), 1.0f);
			//	bv->Expand(tVec3f(localTransform * corner));
			//}

			bv->Expand(tVec3f(localTransform * tVec4f(groupNode->GetBoundingVolume()->GetMinimum(), 1.0f)));
			bv->Expand(tVec3f(localTransform * tVec4f(groupNode->GetBoundingVolume()->GetMaximum(), 1.0f)));
		}
	}

	if (groupNode)
		groupNode->SetBoundingVolume(*bv);

	return bv;
}