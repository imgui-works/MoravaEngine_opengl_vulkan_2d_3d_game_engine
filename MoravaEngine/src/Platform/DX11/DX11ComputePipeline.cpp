#include "DX11ComputePipeline.h"

#include "H2M/Core/TimerH2M.h"
#include "H2M/Renderer/RendererH2M.h"

#include "Platform/DX11/DX11Context.h"


DX11ComputePipeline::DX11ComputePipeline(H2M::RefH2M<H2M::ShaderH2M> computeShader)
	: m_Shader(computeShader)
{
	// H2M::RefH2M<DX11ComputePipeline> instance = this;
	// H2M::HazelRenderer::Submit([instance]() mutable
	// {
	// });
	{
		CreatePipeline();
	}
}

DX11ComputePipeline::~DX11ComputePipeline()
{
}

void DX11ComputePipeline::CreatePipeline()
{
	// DX11Device device = DX11Context::GetCurrentDevice()->GetDX11Device();

	// TODO: Abstract into some sort of compute pipeline

	// TODO: CreatePipelineCache
	// TODO: CreateComputePipelines
}

void DX11ComputePipeline::Execute()
{
	// DX11Device device = DX11Context::GetCurrentDevice()->GetDX11Device();

	// TODO: QueueSubmit
}

void DX11ComputePipeline::Begin()
{
	// TODO CmdBindPipeline
}

void DX11ComputePipeline::Dispatch()
{
	// TODO
}

void DX11ComputePipeline::End()
{
	// TODO
}

void DX11ComputePipeline::SetPushConstants(const void* data, uint32_t size)
{
	// TODO CmdPushConstants
}
