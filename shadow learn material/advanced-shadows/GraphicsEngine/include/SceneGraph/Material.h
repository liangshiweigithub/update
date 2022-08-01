#pragma once

#include <vector>

#include "SceneGraphExports.h"
#include "Common/VectorMathTypes.h"
#include "State.h"
#include "Texture.h"

class SCENEGRAPH_API Material : public State
{
public:
	typedef std::vector<Texture*> tTextureArray;

	/* c'tors */
	Material();
	
	/* d'tors */
	~Material();

	/* properties */
	const tVec4f& GetAmbient() const;
	void SetAmbient(const tVec4f& ambient);

	const tVec4f& GetDiffuse() const;
	void SetDiffuse(const tVec4f& diffuse);

	const tVec4f& GetSpecular() const;
	void SetSpecular(const tVec4f& specilar);

	const tVec4f& GetEmission() const;
	void SetEmission(const tVec4f& ambient);

	float GetShininess() const;
	void SetShininess(float shininess);

	bool IsTranslucent() const;

	unsigned int GetTextureCount() const;
	Texture* GetTexture(unsigned int index) const; 
	void AddTexture(Texture* texture);

	void Push(RenderSystem& renderSystem) const;
	void Pop(RenderSystem& renderSystem) const;

private:
	/* fields */
	float _shininess;

#pragma warning(push)
#pragma warning(disable : 4251)
	tVec4f _ambient;
	tVec4f _diffuse;
	tVec4f _specular;
	tVec4f _emission;

	tTextureArray _textureArray;
#pragma warning(pop)
};
