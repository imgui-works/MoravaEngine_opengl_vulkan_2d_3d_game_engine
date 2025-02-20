#include "DX11VertexBuffer.h"

#include "DX11Context.h"

#include "H2M/Renderer/RendererH2M.h"


DX11VertexBuffer::DX11VertexBuffer(void* data, uint32_t stride, uint32_t count)
{
	// m_LocalData = H2M::Buffer::Copy(data, size);
	ID3D11Device* dx11Device = DX11Context::Get()->GetDX11Device();

	D3D11_BUFFER_DESC buff_desc = {};
	buff_desc.Usage = D3D11_USAGE_DEFAULT;
	buff_desc.ByteWidth = stride * count;
	buff_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buff_desc.CPUAccessFlags = 0;
	buff_desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = data;

	m_Stride = stride;
	m_Count = count;

	HRESULT hr = dx11Device->CreateBuffer(&buff_desc, &init_data, &m_Buffer);
	if (FAILED(hr))
	{
		throw std::exception("DX11VertexBuffer initialization failed.");
	}

	Log::GetLogger()->info("DX11VertexBuffer successfully created!");
}

DX11VertexBuffer::DX11VertexBuffer(void* data, uint32_t size, H2M::VertexBufferUsageH2M usage)
{
	Log::GetLogger()->error("DX11VertexBuffer::DX11VertexBuffer not implemented yet!");
}

DX11VertexBuffer::DX11VertexBuffer(uint32_t size, H2M::VertexBufferUsageH2M usage)
{
	Log::GetLogger()->error("DX11VertexBuffer::DX11VertexBuffer not implemented yet!");
}

DX11VertexBuffer::~DX11VertexBuffer()
{
	if (m_Buffer) m_Buffer->Release();

	Log::GetLogger()->info("DX11VertexBuffer destroyed!");
}

void DX11VertexBuffer::Bind() const
{
	uint32_t offset = 0;
	DX11Context::Get()->GetDX11DeviceContext()->IASetVertexBuffers(0, 1, &m_Buffer, &m_Stride, &offset);
}
