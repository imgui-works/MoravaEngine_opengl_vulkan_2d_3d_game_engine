#define _CRT_SECURE_NO_WARNINGS

#pragma once

#include "DX11.h"
#include "DX11VertexShader.h"
#include "DX11PixelShader.h"
#include "DX11Material.h"

#include "Shader/MoravaShader.h"
#include "H2M/Renderer/TextureH2M.h"


class DX11Shader : public MoravaShader
{
public:
	struct UniformBuffer
	{
		uint32_t Size = 0;
		uint32_t BindingPoint = 0;
		std::string Name;
	};

	struct ImageSampler
	{
		uint32_t BindingPoint = 0;
		std::string Name;
	};

	struct PushConstantRange
	{
		uint32_t Offset = 0;
		uint32_t Size = 0;
	};

	enum class Type
	{
		None = 0,
		Vertex,
		Pixel,
	};

public:
	DX11Shader();
	DX11Shader(const std::string& path, bool forceCompile);
	// DirectX 11
	DX11Shader(const wchar_t* vertexShaderPath, const wchar_t* pixelShaderPath);
	virtual ~DX11Shader();
	static H2M::RefH2M<DX11Shader> CreateFromString(const std::string& source);

	virtual void Bind() override {};
	virtual uint32_t GetRendererID() const override { return (uint32_t)0; };

	virtual void Reload(bool forceCompile = false) override;

	virtual size_t GetHash() const override;

	virtual void SetUniformBuffer(const std::string& name, const void* data, uint32_t size) override;

	virtual void SetUniform(const std::string& fullname, uint32_t value) override;
	virtual void SetUniform(const std::string& fullname, float value) override;
	virtual void SetUniform(const std::string& fullname, int value) override;
	virtual void SetUniform(const std::string& fullname, const glm::vec2& value) override;
	virtual void SetUniform(const std::string& fullname, const glm::vec3& value) override;
	virtual void SetUniform(const std::string& fullname, const glm::vec4& value) override;
	virtual void SetUniform(const std::string& fullname, const glm::mat3& value) override;
	virtual void SetUniform(const std::string& fullname, const glm::mat4& value) override;

	virtual void SetFloat(const std::string& name, float value) override;
	virtual void SetUInt(const std::string& name, uint32_t value) override;
	virtual void SetInt(const std::string& name, int value) override;
	virtual void SetBool(const std::string& name, bool value) override;
	virtual void SetFloat2(const std::string& name, const glm::vec2& value) override;
	virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
	virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;
	virtual void SetMat2(const std::string& name, const glm::mat2& mat) override;
	virtual void SetMat3(const std::string& name, const glm::mat3& mat) override;
	virtual void SetMat4(const std::string& name, const glm::mat4& value) override;
	virtual void SetMat4FromRenderThread(const std::string& name, const glm::mat4& value, bool bind = true) override;
	virtual void SetIntArray(const std::string& name, int* values, uint32_t size) override;

	// generic setter methods for uniform location variables
	virtual void SetVec2(const std::string& name, const glm::vec2& value) override; // TODO: remove, use SetFloat2 instead
	virtual void SetVec2(const std::string& name, float x, float y) override;       // TODO: remove, use SetFloat2 instead

	virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& callback) override;

	virtual GLint GetUniformLocation(const std::string& name) override;
	virtual void UploadUniformMat4(const std::string& name, const glm::mat4& values) override;
	virtual void UploadUniformMat4(uint32_t location, const glm::mat4& values) override;
	virtual void setLightMat4(std::vector<glm::mat4> lightMatrices) override;
	// Omni shadow maps
	virtual void SetLightMatrices(std::vector<glm::mat4> lightMatrices) override;

	virtual void Unbind() override;

	virtual const std::unordered_map<std::string, H2M::ShaderBufferH2M>& GetShaderBuffers() const override;
	virtual const std::unordered_map<std::string, H2M::ShaderResourceDeclarationH2M>& GetResources() const override;

	virtual void CreateFromString(const char* vertexCode, const char* fragmentCode) override;
	virtual void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation) override;
	virtual void CreateFromFiles(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation) override;

	virtual void CreateFromFileVertex(const char* vertexLocation) override;
	virtual void CreateFromFileFragment(const char* fragmentLocation) override;
	virtual void CreateFromFileGeometry(const char* geometryLocation) override;
	virtual void CreateFromFileCompute(const char* computeLocation) override;

	virtual void Validate() override;
	virtual void ClearShader() override;

	static std::string ReadFile(const char* fileLocation);

	void* MapUniformBuffer(uint32_t bindingPoint);
	void UnmapUniformBuffer(uint32_t bindingPoint);

	UniformBuffer& GetUniformBuffer() { return m_UniformBuffers[0]; }
	const std::vector<PushConstantRange>& GetPushConstantRanges() const { return m_PushConstantRanges; }

	uint32_t GetUniformBufferCount(uint32_t set = 0)
	{
		if (m_ShaderDescriptorSets.find(set) == m_ShaderDescriptorSets.end())
			return 0;

		return (uint32_t)m_ShaderDescriptorSets.at(set).UniformBuffers.size();
	}

	std::unordered_map<uint32_t, std::string> PreProcess(const std::string& source);

	struct ShaderDescriptorSet
	{
		std::unordered_map<uint32_t, UniformBuffer*> UniformBuffers;
		std::unordered_map<uint32_t, ImageSampler> ImageSamplers;
		std::unordered_map<uint32_t, ImageSampler> StorageImages;
	};
	const std::unordered_map<uint32_t, ShaderDescriptorSet>& GetShaderDescriptorSets() const { return m_ShaderDescriptorSets; }
		
	struct ShaderMaterialDescriptorSet
	{
	};

	ShaderMaterialDescriptorSet CreateDescriptorSets(uint32_t set = 0);
	ShaderMaterialDescriptorSet CreateDescriptorSets(uint32_t set, uint32_t numberOfSets);

	static void ClearUniformBuffers();

	H2M::RefH2M<DX11VertexShader> GetVertexShader() { return m_VertexShader; }
	H2M::RefH2M<DX11PixelShader> GetPixelShader() { return m_PixelShader; }

private:
	void CompileOrGetDX11Binary(std::array<std::vector<uint32_t>, 2>& outputBinary, bool forceCompile);
	void LoadAndCreateVertexShader(const std::vector<uint32_t>& shaderData);
	void LoadAndCreatePixelShader(const std::vector<uint32_t>& shaderData);

	void Reflect(const std::string& shaderStage, const std::vector<uint32_t>& shaderData);
	void CreateDescriptors();

	void AllocateUniformBuffer(UniformBuffer& dst);

private:
	std::string m_AssetPath;
	std::string m_Name;

	std::unordered_map<uint32_t, ShaderDescriptorSet> m_ShaderDescriptorSets;

	std::unordered_map<uint32_t, UniformBuffer> m_UniformBuffers;
	std::unordered_map<uint32_t, ImageSampler> m_ImageSamplers;
	std::vector<PushConstantRange> m_PushConstantRanges;

	std::unordered_map<std::string, H2M::ShaderBufferH2M> m_Buffers;

	H2M::RefH2M<DX11VertexShader> m_VertexShader;
	H2M::RefH2M<DX11PixelShader> m_PixelShader;

	friend class DX11Material;

};
