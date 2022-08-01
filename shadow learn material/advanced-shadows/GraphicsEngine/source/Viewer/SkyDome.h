#pragma once

class SkyDome : public Pass
{
private:
	class SkyDomeShader : public D3dEffect
	{
	friend class SkyDome;

	public:
		SkyDomeShader();

		void UniformCallBack(D3dEffectManager& effectManager, const Texture& texture, ID3D11ShaderResourceView& resource, float* texMtx);
		void UniformCallBack(D3dEffectManager& effectManager, const Transform& transform);

		bool Initialize(ID3DX11Effect& effect);

	private:
		static const std::string ShaderSource;
		ID3DX11EffectShaderResourceVariable* diffuseTex;
		ID3DX11EffectMatrixVariable* world;
		ID3DX11EffectMatrixVariable* view;
		ID3DX11EffectMatrixVariable* projection;
		ID3DX11EffectMatrixVariable* tex;
	};
public:
	SkyDome();
	~SkyDome();

	bool Initialize(RenderSystem& renderSystem);
	void Render();

private:
	D3dRenderSystem* _renderSystem;

	SkyDomeShader _skyDomeShader;

};