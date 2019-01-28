////////////////////////////////////////////////////////////////////////////////
// Filename: TextureShader.cpp
////////////////////////////////////////////////////////////////////////////////
#include "TextureShader.h"
#include <atlconv.h>
#include "MainApp.h"
TextureShader::TextureShader()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;
}

TextureShader::~TextureShader()
{
	SafeRelease(m_vertexShader);
	SafeRelease(m_pixelShader);
	SafeRelease(m_layout);
	SafeRelease(m_matrixBuffer);
	SafeRelease(m_sampleState);
}


bool TextureShader::Initialize(/*ID3D11Device* device, HWND hwnd,*/ std::string vsFileName, std::string psFileName)
{
	bool result;
	wchar_t vsBuf[256], psBuf[256];
	size_t outNum;
	mbstowcs_s(&outNum, vsBuf, vsFileName.c_str(), 256);
	mbstowcs_s(&outNum, psBuf, psFileName.c_str(), 256);
	// Initialize the vertex and pixel shaders.
	auto device = g_App->GetDevice();
	auto hwnd = g_App->GetWindowHandle();
	result = InitializeShader(device, hwnd, vsBuf, psBuf);// L"../Engine/texture.vs", L"../Engine/texture.ps");
	if (!result)
	{
		return false;
	}

	return true;
}


void TextureShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}


bool TextureShader::Render(ID3D11DeviceContext* deviceContext, int vertexCount, int startVertexIndex, 
	Ogre::Matrix4& worldMatrix, Ogre::Matrix4& projectionMatrix, std::vector<ID3D11ShaderResourceView*> texture)
{
	bool result;

	Ogre::Matrix4 viewMatrix = g_App->GetCamera()->View();
	//Ogre::Matrix4 projectionMatrix = g_App->GetCamera()->Proj();
	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(deviceContext, vertexCount, startVertexIndex);

	return true;
}


bool TextureShader::Render(ID3D11DeviceContext* deviceContext, int vertexCount, int startVertexIndex, Ogre::Matrix4& worldMatrix, Ogre::Matrix4& projectionMatrix, ID3D11ShaderResourceView* texture)
{
	std::vector<ID3D11ShaderResourceView*> srv;
	srv.push_back(texture);
	return Render(deviceContext, vertexCount, startVertexIndex, worldMatrix, projectionMatrix, srv);
}

bool TextureShader::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputlayout;
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;


	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader code.
	result = D3DCompileFromFile(vsFilename, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0",
		D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		&vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Compile the pixel shader code.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "main", "ps_5_0", 
		D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		&pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was  nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	// reflect
	ID3D11ShaderReflection* VSReflector = NULL;
	D3DReflect(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&VSReflector);
	D3D11_SHADER_DESC VSShaderDesc;
	VSReflector->GetDesc(&VSShaderDesc);
	D3D11_SHADER_BUFFER_DESC desc;
	D3D11_SHADER_VARIABLE_DESC vDesc;
	D3D11_SIGNATURE_PARAMETER_DESC iDesc;
	int offset = 0, stride = 0;
	for (unsigned int i = 0; i < VSShaderDesc.InputParameters; ++i)
	{
		VSReflector->GetInputParameterDesc(i, &iDesc);
		D3D11_INPUT_ELEMENT_DESC inputDesc;
		inputDesc.SemanticName = iDesc.SemanticName;
		inputDesc.SemanticIndex = iDesc.SemanticIndex;
		// get format from Mask(Count) & ComponentType
		DXGI_FORMAT fmt = GetFormatFromDesc(iDesc, stride);
		inputDesc.Format = fmt;
		inputDesc.InputSlot = iDesc.Stream;
		inputDesc.AlignedByteOffset = offset;
		inputDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputDesc.InstanceDataStepRate = 0;
		inputlayout.push_back(inputDesc);
		offset += stride;
	}
	for (unsigned int i = 0; i < VSShaderDesc.ConstantBuffers; ++i)
	{
		ID3D11ShaderReflectionConstantBuffer* pReflectCB = VSReflector->GetConstantBufferByIndex(i);
		pReflectCB->GetDesc(&desc);
		if(desc.Type != D3D_CT_CBUFFER)
			continue;
		for (unsigned int j = 0; j < desc.Variables; ++j)
		{
			ID3D11ShaderReflectionVariable* pVar = pReflectCB->GetVariableByIndex(j);
			pVar->GetDesc(&vDesc);
		}
	}
	// Create the vertex input layout description.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
// 	inputlayout[0].SemanticName = "POSITION";
// 	inputlayout[0].SemanticIndex = 0;
// 	inputlayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
// 	inputlayout[0].InputSlot = 0;
// 	inputlayout[0].AlignedByteOffset = 0;
// 	inputlayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
// 	inputlayout[0].InstanceDataStepRate = 0;
// 
// 	inputlayout[1].SemanticName = "NORMAL";
// 	inputlayout[1].SemanticIndex = 0;
// 	inputlayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
// 	inputlayout[1].InputSlot = 0;
// 	inputlayout[1].AlignedByteOffset = 12;
// 	inputlayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
// 	inputlayout[1].InstanceDataStepRate = 0;
// 
// 	inputlayout[2].SemanticName = "TEXCOORD";
// 	inputlayout[2].SemanticIndex = 0;
// 	inputlayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
// 	inputlayout[2].InputSlot = 0;
// 	inputlayout[2].AlignedByteOffset = 24;
// 	inputlayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
// 	inputlayout[2].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = (int)inputlayout.size();

	// Create the vertex input layout.
	result = device->CreateInputLayout(&inputlayout[0], numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
		&m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic DirectX::SimpleMath::Matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}


void TextureShader::ShutdownShader()
{
	// Release the sampler state.
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// Release the DirectX::SimpleMath::Matrix constant buffer.
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}


void TextureShader::OutputShaderErrorMessage(ID3DBlob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}


bool TextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, Ogre::Matrix4& worldMatrix, Ogre::Matrix4& viewMatrix,
	Ogre::Matrix4& projectionMatrix, std::vector<ID3D11ShaderResourceView*> texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;


	// Transpose the matrices to prepare them for the shader.
	worldMatrix = worldMatrix.transpose();
	viewMatrix = viewMatrix.transpose();
	projectionMatrix = projectionMatrix.transpose();
// 	worldMatrix.Transpose();
// 	viewMatrix.Transpose();
// 	projectionMatrix.Transpose();
	//MatrixTranspose(&worldMatrix, &worldMatrix);
	//MatrixTranspose(&viewMatrix, &viewMatrix);
	//MatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);
	
	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, texture.size(), &texture[0]);

	return true;
}


void TextureShader::RenderShader(ID3D11DeviceContext* deviceContext, int VertexCount, int StartVertexLocation)
{
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle.
	//deviceContext->Draw(VertexCount, StartVertexLocation);
	deviceContext->DrawIndexed(VertexCount, 0, 0);
	return;
}

DXGI_FORMAT TextureShader::GetFormatFromDesc(D3D11_SIGNATURE_PARAMETER_DESC inputDesc, int &OutStride)
{
	struct FormatPair
	{
		int ComponentNum;
		D3D_REGISTER_COMPONENT_TYPE ComponentType;
		FormatPair():ComponentNum(0), ComponentType(D3D_REGISTER_COMPONENT_FLOAT32) {};
		FormatPair(int cNum, D3D_REGISTER_COMPONENT_TYPE type) :ComponentNum(cNum), ComponentType(type) {};
		bool operator < (const FormatPair& other) const
		{
			if (ComponentNum < other.ComponentNum)
				return true;
			else if (ComponentNum == other.ComponentNum)
			{
				if (ComponentType < other.ComponentType)
					return true;
			}
			return false;
		}
	};
	
	std::map<FormatPair, DXGI_FORMAT> mapFormat = {
		{ FormatPair(2, D3D_REGISTER_COMPONENT_FLOAT32), DXGI_FORMAT_R32G32_FLOAT},
		{ FormatPair(2, D3D_REGISTER_COMPONENT_UINT32), DXGI_FORMAT_R32G32_UINT },
		{ FormatPair(3, D3D_REGISTER_COMPONENT_FLOAT32), DXGI_FORMAT_R32G32B32_FLOAT },
		{ FormatPair(3, D3D_REGISTER_COMPONENT_UINT32), DXGI_FORMAT_R32G32B32_UINT },
	};
	int ComponentNum = 0;
	BYTE Mask = inputDesc.Mask;
	while (Mask)
	{
		ComponentNum++;
		Mask >>= 1;
	}
	// 32bit
	// to-do : 16bit component
	OutStride = 4 * ComponentNum;
	return mapFormat[FormatPair(ComponentNum, inputDesc.ComponentType)];
}
