#include "VertexArrayH2M.h"

#include "H2M/Platform/OpenGL/OpenGLVertexArrayH2M.h"
#include "H2M/Renderer/RendererAPI_H2M.h"


namespace H2M
{

	RefH2M<VertexArrayH2M> VertexArrayH2M::Create()
	{
		switch (RendererAPI_H2M::Current())
		{
		case RendererAPITypeH2M::None:    H2M_CORE_ASSERT(false, "RendererAPI_H2M::None is currently not supported!"); return RefH2M<VertexArrayH2M>();
		case RendererAPITypeH2M::OpenGL:  return RefH2M<OpenGLVertexArrayH2M>::Create();
		}

		H2M_CORE_ASSERT(false, "Unknown RendererAPI");
		return RefH2M<VertexArrayH2M>();
	}

}