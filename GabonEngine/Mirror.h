#pragma once
#include "d3dUtil.h"
#include "MathHeader.h"
#include "ModelObject.h"
class Mirror : public ModelObject
{
public:
	Mirror();
	~Mirror();
	virtual void Init(std::string name, TextureShader* shader, std::string meshName, std::vector<std::string> texNames);
	
	virtual void Render() override;
	// Change depth stencil state, cache / mirror
	void RenderToDepthStencilView();
	// cull state, draw to depthstencil, push model needed to reflection
	void RenderReflectModels();
	// Render mirror self, square model, alphablend
	void RenderMirror();

private:
	// render states
	ID3D11BlendState* NoRenderTargetWritesBS;
	ID3D11BlendState* TransparentBS;
	ID3D11DepthStencilState* MarkMirrorDSS;
	ID3D11DepthStencilState* DrawReflectionDSS;
	ID3D11RasterizerState* CullClockwiseRS;
	ID3D11RasterizerState* CullCounterClockwiseRS;
	class ModelObject* m_pReflectModel = NULL;
};

