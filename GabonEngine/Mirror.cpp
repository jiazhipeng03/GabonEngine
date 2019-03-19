#include "Mirror.h"
#include "MainApp.h"


Mirror::Mirror()
{
}


Mirror::~Mirror()
{
	SafeDelete(m_pReflectModel);
}

void Mirror::Init(std::string name, TextureShader* shader, std::string meshName, std::vector<std::string> texNames)
{
	ModelObject::Init(name, shader, meshName, texNames);
	Matrix3 rot; 
	rot.FromAngleAxis(Vector3(0, 1, 0), -Math::PI*0.5f);
	m_World.makeTransform(Vector3(-6,0,0), Vector3(5, 5, 5), Quaternion::FromRotationMatrix(rot));
	m_World.transpose(); 

	m_pReflectModel = g_App->GetModelMan()->CloneModelObj("cubeMain");
	Plane mirrorPlane(1, 0, 0, 6); // xy plane
	Matrix4 reflectionMatrix = Matrix4::buildReflectionMatrix(mirrorPlane);
	Matrix4 worldreflect = m_pReflectModel->GetWorldMatrix() * reflectionMatrix;
	m_pReflectModel->SetWorldMatrix(worldreflect);

	// Init blend state
	D3D11_BLEND_DESC noRenderTargetWritesDesc = { 0 };
	noRenderTargetWritesDesc.AlphaToCoverageEnable = false;
	noRenderTargetWritesDesc.IndependentBlendEnable = false;

	noRenderTargetWritesDesc.RenderTarget[0].BlendEnable = false;
	noRenderTargetWritesDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	noRenderTargetWritesDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	noRenderTargetWritesDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	noRenderTargetWritesDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	noRenderTargetWritesDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	noRenderTargetWritesDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	noRenderTargetWritesDesc.RenderTarget[0].RenderTargetWriteMask = 0;

	HR(g_App->GetDevice()->CreateBlendState(&noRenderTargetWritesDesc, &NoRenderTargetWritesBS));

	// mirror depth stencil state
	D3D11_DEPTH_STENCIL_DESC mirrorDesc;
	mirrorDesc.DepthEnable = true;
	mirrorDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	mirrorDesc.DepthFunc = D3D11_COMPARISON_LESS;
	mirrorDesc.StencilEnable = true;
	mirrorDesc.StencilReadMask = 0xff;
	mirrorDesc.StencilWriteMask = 0xff;

	mirrorDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE; // Ìæ»»
	mirrorDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS; // ×ÜÊÇÐ´Èë

	// We are not rendering backfacing polygons, so these settings do not matter.
	mirrorDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	mirrorDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	mirrorDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HR(g_App->GetDevice()->CreateDepthStencilState(&mirrorDesc, &MarkMirrorDSS));
		
	// cull state
	D3D11_RASTERIZER_DESC cullClockwiseDesc;
	ZeroMemory(&cullClockwiseDesc, sizeof(D3D11_RASTERIZER_DESC));
	cullClockwiseDesc.FillMode = D3D11_FILL_SOLID;
	cullClockwiseDesc.CullMode = D3D11_CULL_BACK;
	cullClockwiseDesc.FrontCounterClockwise = true;
	cullClockwiseDesc.DepthClipEnable = true;

	HR(g_App->GetDevice()->CreateRasterizerState(&cullClockwiseDesc, &CullClockwiseRS));

	// cull state
	D3D11_RASTERIZER_DESC cullCounterClockwiseDesc;
	ZeroMemory(&cullCounterClockwiseDesc, sizeof(D3D11_RASTERIZER_DESC));
	cullCounterClockwiseDesc.FillMode = D3D11_FILL_SOLID;
	cullCounterClockwiseDesc.CullMode = D3D11_CULL_FRONT;
	cullCounterClockwiseDesc.FrontCounterClockwise = true;
	cullCounterClockwiseDesc.DepthClipEnable = true;
	HR(g_App->GetDevice()->CreateRasterizerState(&cullCounterClockwiseDesc, &CullCounterClockwiseRS));

	// draw reflection state
	D3D11_DEPTH_STENCIL_DESC drawReflectionDesc;
	drawReflectionDesc.DepthEnable = true;
	drawReflectionDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	drawReflectionDesc.DepthFunc = D3D11_COMPARISON_LESS;
	drawReflectionDesc.StencilEnable = true;
	drawReflectionDesc.StencilReadMask = 0xff;
	drawReflectionDesc.StencilWriteMask = 0xff;

	drawReflectionDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	// We are not rendering backfacing polygons, so these settings do not matter.
	drawReflectionDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	drawReflectionDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	HR(g_App->GetDevice()->CreateDepthStencilState(&drawReflectionDesc, &DrawReflectionDSS));

	// transparent blend state
	D3D11_BLEND_DESC transparentDesc = { 0 };
	transparentDesc.AlphaToCoverageEnable = false;
	transparentDesc.IndependentBlendEnable = false;

	transparentDesc.RenderTarget[0].BlendEnable = true;
	transparentDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	transparentDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	transparentDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	transparentDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	transparentDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	transparentDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(g_App->GetDevice()->CreateBlendState(&transparentDesc, &TransparentBS));

}

void Mirror::Render()
{
	RenderToDepthStencilView();
	RenderReflectModels();
	RenderMirror();
}

void Mirror::RenderToDepthStencilView()
{
	ModelObject::Render();
	// Do not write to render target.
	float blendFactor[4] = { 0,0,0,0 };
	g_App->GetDeviceContext()->OMSetBlendState(NoRenderTargetWritesBS, blendFactor, 0xffffffff);

	// Render visible mirror pixels to stencil buffer.
	// Do not write mirror depth to depth buffer at this point, otherwise it will occlude the reflection.
	g_App->GetDeviceContext()->OMSetDepthStencilState(MarkMirrorDSS, 1);
	
	m_Shader->Render(g_App->GetDeviceContext(), m_VertexCount, 0, m_World, g_App->GetCamera()->Proj(), nullptr);
	// Restore states.
	g_App->GetDeviceContext()->OMSetDepthStencilState(0, 0);
	//g_App->GetDeviceContext()->OMSetBlendState(0, blendFactor, 0xffffffff);
}

void Mirror::RenderReflectModels()
{	
	g_App->EnableAlphaBlending(true);
	// Cull clockwise triangles for reflection.
	g_App->GetDeviceContext()->RSSetState(CullClockwiseRS);
	// Only draw reflection into visible mirror pixels as marked by the stencil buffer. 
	g_App->GetDeviceContext()->OMSetDepthStencilState(DrawReflectionDSS, 1);

	// TODO: reflect every direction
	
	
	//m_pReflectModel->SetWorldMatrix(worldreflect);
	m_pReflectModel->Render();
	m_pReflectModel->GetShader()->Render(g_App->GetDeviceContext(), m_pReflectModel->GetVertexCount(), m_pReflectModel->GetStartVertexIndex(),
		m_pReflectModel->GetWorldMatrix(), g_App->GetCamera()->Proj(), m_pReflectModel->GetTexArray());
	// light dir * reflect

	// restore
	g_App->GetDeviceContext()->RSSetState(CullCounterClockwiseRS);
	g_App->GetDeviceContext()->OMSetDepthStencilState(0, 0);
}

void Mirror::RenderMirror()
{
	float blendFactor[4] = { 0,0,0,0 };
	//g_App->GetDeviceContext()->OMSetBlendState(TransparentBS, blendFactor, 0xffffffff);
	g_App->EnableZBuffer(true);
	g_App->EnableAlphaBlending(true);
	ModelObject::Render();
	m_Shader->Render(g_App->GetDeviceContext(), m_VertexCount, 0, m_World, g_App->GetCamera()->Proj(), m_TexArray);
}
