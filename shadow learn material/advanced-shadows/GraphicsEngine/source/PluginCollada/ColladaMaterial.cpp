#include "stdafx.h"
#include "ColladaMaterial.h"

#include "FCDocument/FCDEffect.h"
#include "FCDocument/FCDEffectProfile.h"
#include "FCDocument/FCDImage.h"
#include "FCDocument/FCDTexture.h"
#include "FCDocument/FCDExtra.h"

ColladaMaterial::ColladaMaterial(FCDMaterial &material) :
	_material(&material)
{
}

std::string ColladaMaterial::GetDaeId() const
{
	return _material->GetDaeId().c_str();
}

unsigned int ColladaMaterial::GetTextureCount(FUDaeTextureChannel::Channel channel) const
{
	FCDEffectStandard* effectStandard = GetEffectStandard();
	if (!effectStandard)
		return 0;

	return static_cast<unsigned int>(effectStandard->GetTextureCount(channel));
}

ColladaImage ColladaMaterial::GetTexture(unsigned int index, FUDaeTextureChannel::Channel channel) const
{
	FCDEffectStandard* effectStandard = GetEffectStandard();

	return ColladaImage(*effectStandard->GetTexture(channel, index)->GetImage());
}

tVec2f ColladaMaterial::GetTextureTiling(unsigned int index, FUDaeTextureChannel::Channel channel) const
{
	FCDEffectStandard* effectStandard = GetEffectStandard();
	FCDETechnique* eTechnique = effectStandard->GetTexture(channel, index)->GetExtra()->GetDefaultType()->FindTechnique("MAYA");

	if(eTechnique)
	{
		FCDENode* repeatU = eTechnique->FindChildNode("repeatU");
		FCDENode* repeatV = eTechnique->FindChildNode("repeatV");

		if(repeatU && repeatV)
			return tVec2f(FUStringConversion::ToFloat(repeatU->GetContent()),
						   FUStringConversion::ToFloat(repeatV->GetContent()));
	}

	return tVec2f(1.0f, 1.0f);
}

tVec2f ColladaMaterial::GetTextureOffset(unsigned int index, FUDaeTextureChannel::Channel channel) const
{
	FCDEffectStandard* effectStandard = GetEffectStandard();
	FCDETechnique* eTechnique = effectStandard->GetTexture(channel, index)->GetExtra()->GetDefaultType()->FindTechnique("MAYA");

	if(eTechnique)
	{
		FCDENode* offsetU = eTechnique->FindChildNode("offsetU");
		FCDENode* offsetV = eTechnique->FindChildNode("offsetV");

		if(offsetU && offsetV)
			return tVec2f(FUStringConversion::ToFloat(offsetU->GetContent()),
						   FUStringConversion::ToFloat(offsetV->GetContent()));
	}

	return tVec2f(.0f, .0f);
}

float ColladaMaterial::GetTextureRotation(unsigned int index, FUDaeTextureChannel::Channel channel) const
{
	FCDEffectStandard* effectStandard = GetEffectStandard();
	FCDETechnique* eTechnique = effectStandard->GetTexture(channel, index)->GetExtra()->GetDefaultType()->FindTechnique("MAYA");

	if(eTechnique)
	{
		FCDENode* rotateUV = eTechnique->FindChildNode("rotateUV");

		if(rotateUV)
			return FUStringConversion::ToFloat(rotateUV->GetContent());											
	}

	return .0f;
}

Material* ColladaMaterial::Convert()
{
	FCDEffectStandard* effectStandard = GetEffectStandard();

	if (!effectStandard)
		return 0;

	float transparency = effectStandard->GetTranslucencyFactor();
	
	Material* material = new Material();
	material->SetName(_material->GetDaeId().c_str());

	FMVector4 color = effectStandard->GetAmbientColor();
	material->SetAmbient(tVec4f(color.x, color.y, color.z, color.w * transparency));

	color = effectStandard->GetDiffuseColor();
	material->SetDiffuse(tVec4f(color.x, color.y, color.z, color.w * transparency));

	color = effectStandard->GetSpecularColor();
	material->SetSpecular(tVec4f(color.x, color.y, color.z, color.w * transparency ));

	color = effectStandard->GetEmissionColor();
	material->SetEmission(tVec4f(color.x, color.y, color.z, color.w * transparency));

	material->SetShininess(effectStandard->GetShininess());

	return material;

}

FCDEffectStandard* ColladaMaterial::GetEffectStandard() const
{
	FCDEffect* effect = _material->GetEffect();
	if(!effect)
		return 0;

	return (dynamic_cast<FCDEffectStandard*>(effect->FindProfile(FUDaeProfileType::COMMON)));
}
