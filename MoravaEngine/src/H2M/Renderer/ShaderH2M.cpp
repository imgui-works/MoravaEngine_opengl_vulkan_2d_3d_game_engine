#include "ShaderH2M.h"

#include "H2M/Core/Assert.h"
#include "H2M/Platform/OpenGL/OpenGLShader.h"
#include "H2M/Platform/Vulkan/VulkanShader.h"
#include "H2M/Renderer/RendererAPI.h"

#include "Platform/DX11/DX11Shader.h"


namespace H2M
{

	std::vector<H2M::Ref<ShaderH2M>> ShaderH2M::s_AllShaders;

	H2M::Ref<ShaderH2M> ShaderH2M::Create(const std::string& filepath, bool forceCompile)
	{
		Log::GetLogger()->info("ShaderH2M::Create('{0}')", filepath.c_str());

		H2M::Ref<ShaderH2M> result = H2M::Ref<ShaderH2M>();

		switch (H2M::RendererAPIH2M::Current())
		{
			case H2M::RendererAPIH2MType::None: return H2M::Ref<ShaderH2M>();
			case H2M::RendererAPIH2MType::OpenGL:
				result = H2M::Ref<H2M::OpenGLShader>::Create(filepath, forceCompile);
				break;
			case H2M::RendererAPIH2MType::Vulkan:
				result = H2M::Ref<H2M::VulkanShader>::Create(filepath, forceCompile);
				break;
			case H2M::RendererAPIH2MType::DX11:
				result = H2M::Ref<DX11Shader>::Create(filepath, forceCompile);
				break;
		}
		s_AllShaders.push_back(result);
		return result;
	}

	H2M::Ref<ShaderH2M> ShaderH2M::CreateFromString(const std::string& source)
	{
		Log::GetLogger()->info("ShaderH2M::CreateFromString('{0}')", source.c_str());

		H2M::Ref<ShaderH2M> result = H2M::Ref<ShaderH2M>();

		switch (H2M::RendererAPIH2M::Current())
		{
			case H2M::RendererAPIH2MType::None:   return H2M::Ref<ShaderH2M>();
			case H2M::RendererAPIH2MType::OpenGL: result = H2M::OpenGLShader::CreateFromString(source);
			case H2M::RendererAPIH2MType::Vulkan: result = H2M::VulkanShader::CreateFromString(source);
			case H2M::RendererAPIH2MType::DX11:   result = DX11Shader::CreateFromString(source);
		}
		s_AllShaders.push_back(result);
		return result;
	}

	ShaderLibraryH2M::ShaderLibraryH2M()
	{
	}

	ShaderLibraryH2M::~ShaderLibraryH2M()
	{
	}

	void ShaderLibraryH2M::Add(const H2M::Ref<ShaderH2M>& shader)
	{
		auto& name = shader->GetName();
		HZ_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
		m_Shaders[name] = shader;

		// m_Shaders.insert(std::make_pair(name, shader));
	}

	void ShaderLibraryH2M::Load(const std::string& path, bool forceCompile)
	{
		Log::GetLogger()->info("ShaderLibraryH2M::Load(path: '{0}')", path);

		auto shader = ShaderH2M::Create(path, forceCompile);
		auto& name = shader->GetName();
		// HZ_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());

		if (m_Shaders.find(name) != m_Shaders.end())
		{
			Log::GetLogger()->warn("ShaderLibraryH2M::Load: the shader (path: '{0}', name: '{1}') already exists in ShaderLibraryH2M!", path, name);
			return;
		}

		m_Shaders[name] = shader;

		// m_Shaders.insert(std::make_pair(name, shader));
	}

	void ShaderLibraryH2M::Load(const std::string& name, const std::string& path)
	{
		Log::GetLogger()->info("ShaderLibraryH2M::Load(name: '{0}', path: '{1}')", name, path);

		HZ_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
		m_Shaders[name] = ShaderH2M::Create(path, true);

		// m_Shaders.insert(std::make_pair(name, shader));
	}

	H2M::Ref<ShaderH2M> ShaderLibraryH2M::Get(const std::string& name)
	{
		// HZ_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end());
		if (m_Shaders.find(name) == m_Shaders.end())
		{
			Log::GetLogger()->error("ShaderLibraryH2M::Get - shader '{0}' not found in ShaderLibrary!", name);
			return H2M::Ref<ShaderH2M>();
		}
		return m_Shaders.at(name);
	}

	// ---------------------------------------------------------------

	bool ShaderH2M::HasVSMaterialUniformBuffer()
	{
		Log::GetLogger()->warn("ShaderH2M::HasVSMaterialUniformBuffer - Method not implemented!");
		return false;
	}

	bool ShaderH2M::HasPSMaterialUniformBuffer()
	{
		Log::GetLogger()->warn("ShaderH2M::HasPSMaterialUniformBuffer - Method not implemented!");
		return false;
	}

	H2M::Buffer ShaderH2M::GetVSMaterialUniformBuffer()
	{
		Log::GetLogger()->warn("ShaderH2M::GetVSMaterialUniformBuffer - Method not implemented!");
		return H2M::Buffer();
	}

	H2M::Buffer ShaderH2M::GetPSMaterialUniformBuffer()
	{
		Log::GetLogger()->warn("ShaderH2M::GetPSMaterialUniformBuffer - Method not implemented!");
		return H2M::Buffer();
	}

	ShaderUniform::ShaderUniform(const std::string& name, ShaderUniformType type, uint32_t size, uint32_t offset)
		: m_Name(name), m_Type(type), m_Size(size), m_Offset(offset)
	{
	}

	const std::string& ShaderUniform::UniformTypeToString(ShaderUniformType type)
	{
		if (type == ShaderUniformType::Bool)
		{
			return "Boolean";
		}
		else if (type == ShaderUniformType::Int)
		{
			return "Int";
		}
		else if (type == ShaderUniformType::Float)
		{
			return "Float";
		}

		return "None";
	}

}