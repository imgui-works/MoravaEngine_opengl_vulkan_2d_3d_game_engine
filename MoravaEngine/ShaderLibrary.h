#pragma once

#include "Shader.h"
#include "Hazel/Core/Base.h"

#include <unordered_map>


class ShaderLibrary
{
public:
	static void Add(Ref<Shader>& shader);
	static void Load(const std::string& name, const std::string& vertexLocation, const std::string& fragmentLocation);
	static void Load(const std::string& name, const std::string& vertexLocation, const std::string& geometryLocation, const std::string& fragmentLocation);
	static void Load(const std::string& name, const std::string& computeLocation);
	static const Ref<Shader>& Get(const std::string& name);

private:
	static std::unordered_map<std::string, Ref<Shader>> s_Shaders;

};