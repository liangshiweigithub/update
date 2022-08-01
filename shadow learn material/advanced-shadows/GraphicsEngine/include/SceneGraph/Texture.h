#pragma once

#include "SceneGraphExports.h"
#include "Common/Object.h"
#include "Image.h"
#include "Common/VectorMathTypes.h"

class SCENEGRAPH_API Texture : public Object
{
public:
	enum Type
	{
		Diffuse = 0,
		Filter,
		Detail,
		Gloss,
		Bump,
		TypeCOUNT
	};

	Texture(Image& image, Type type, tVec2f tiling, tVec2f offset, float rotation);
	~Texture();

	const Image& GetImage() const;
	void SetImage(Image& image);

	Type GetTextureType() const;
	void SetTextureType(Type type);

	const tVec2f& GetTiling() const;
	void SetTiling(tVec2f& tiling);

	const tVec2f& GetOffset() const;
	void SetOffset(tVec2f& offset);

	const float GetRotation() const;
	void SetRotation(float roation);

private:
	Image* _image;
	Type _type;
	float _rotation;

#pragma warning(push)
#pragma warning(disable : 4251)
	tVec2f _tiling;
	tVec2f _offset;
#pragma warning(pop)

};