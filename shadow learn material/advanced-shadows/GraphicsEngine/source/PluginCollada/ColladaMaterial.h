#pragma once

#include "FCDocument/FCDMaterial.h"
#include "FCDocument/FCDEffectStandard.h"

#include "ColladaImage.h"

class ColladaMaterial
{
public:
	ColladaMaterial(FCDMaterial& material);

	std::string GetDaeId() const;

	unsigned int GetTextureCount(FUDaeTextureChannel::Channel channel) const;
	ColladaImage GetTexture(unsigned int index, FUDaeTextureChannel::Channel channel) const;
	tVec2f GetTextureTiling(unsigned int index, FUDaeTextureChannel::Channel channel) const;
	tVec2f GetTextureOffset(unsigned int index, FUDaeTextureChannel::Channel channel) const;
	float GetTextureRotation(unsigned int index, FUDaeTextureChannel::Channel channel) const;
	
	Material* Convert();

private:
	FCDEffectStandard* GetEffectStandard() const;

private:
	FCDMaterial* _material;
};