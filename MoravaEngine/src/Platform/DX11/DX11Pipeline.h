#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "DX11.h"

#include "DX11Shader.h"
#include "Hazel/Renderer/Pipeline.h"


class DX11Pipeline : public Hazel::Pipeline
{
public:
	DX11Pipeline(const Hazel::PipelineSpecification& spec);
	virtual ~DX11Pipeline();

	virtual Hazel::PipelineSpecification& GetSpecification() { return m_Specification; }
	virtual const Hazel::PipelineSpecification& GetSpecification() const { return m_Specification; }

	virtual void Invalidate() override;

	virtual void Bind() override;

private:
	Hazel::PipelineSpecification m_Specification;

};