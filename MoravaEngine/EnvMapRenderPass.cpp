#include "EnvMapRenderPass.h"

#include "Hazel/Core/Assert.h"
#include "Hazel/Platform/OpenGL/OpenGLRenderPass.h"
#include "Hazel/Renderer/RendererAPI.h"
#include "Hazel/Renderer/RenderPass.h"

#include "Log.h"


EnvMapRenderPass::EnvMapRenderPass(const EnvMapRenderPassSpecification& spec)
	: m_Specification(spec)
{
}