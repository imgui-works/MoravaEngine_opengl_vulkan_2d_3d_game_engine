#pragma once

#include "Scene.h"
#include "Hazel/Renderer/MeshAnimPBR.h"
#include "TextureCubemap.h"
#include "CubeSkybox.h"
#include "MaterialWorkflowPBR.h"
#include "AABB.h"
#include "Framebuffer.h"

#include <map>
#include <string>


const int MAX_LIGHTS = 4 + 4; // (4 x point lights) + (4 x spot lights)

class SceneAnimPBR : public Scene
{

public:
	SceneAnimPBR();
	virtual ~SceneAnimPBR() override;

	virtual void Update(float timestep, Window* mainWindow) override;
	virtual void UpdateImGui(float timestep, Window* mainWindow) override;
	void UpdateImGuizmo(Window* mainWindow);
	virtual void ShowExampleAppDockSpace(bool* p_open, Window* mainWindow) override;
	virtual void Render(Window* mainWindow, glm::mat4 projectionMatrix, std::string passType,
		std::map<std::string, Shader*> shaders, std::map<std::string, int> uniforms) override;
	void SetupUniforms();

private:
	virtual void SetLightManager() override;
	virtual void SetupTextures() override;
	virtual void SetupTextureSlots() override;
	virtual void SetupMaterials() override;
	virtual void SetupMeshes() override;
	virtual void SetupModels() override;
	virtual void SetupFramebuffers() override;

	void SetupShaders(); // Usually in Renderer* classes

	void SetupRenderFramebuffer();
	void ResizeViewport(glm::vec2 viewportPanelSize);
	void CheckIntersection(Window* mainWindow);

private:
	int m_ImGuiMainViewportX;
	int m_ImGuiMainViewportY;

	struct Viewport
	{
		int X;
		int Y;
		int Width;
		int Height;
		int MouseX;
		int MouseY;
	} m_ImGuiViewport;

	Shader* m_ShaderMain;
	Shader* m_ShaderBackground;
	Shader* m_ShaderHybridAnimPBR;
	Shader* m_ShaderHDR;
	Shader* m_ShaderEquirectangularConversion;
	Shader* m_ShaderEnvFiltering;
	Shader* m_ShaderEnvIrradiance;
	Shader* m_ShaderBasic;

	Hazel::MeshAnimPBR* m_MeshAnimPBR_M1911;
	Hazel::MeshAnimPBR* m_MeshAnimPBR_BobLamp;
	Hazel::MeshAnimPBR* m_MeshAnimPBR_AnimBoy;

	Material* m_BaseMaterial_M1911;
	Material* m_BaseMaterial_BobLamp;
	Material* m_BaseMaterial_AnimBoy;

	struct Entity {
		struct Transform {
			glm::vec3 Translation;
			glm::quat Rotation;
			glm::vec3 Scale;
			glm::mat4 Transform;
		} Transform;
		struct Init {
			struct Transform {
				glm::vec3 Scale;
			} Transform;
			struct AABBTransform {
				struct Transform {
					glm::vec3 Scale;
				} Transform;
			} AABB;
		} Init;
		struct AABBTransform {
			glm::vec3 Translation;
			glm::quat Rotation;
			glm::vec3 Scale;
			glm::mat4 Transform;
		} AABBTransform;

		AABB AABB;
		glm::vec3 OriginOffset;
		bool Intersecting;
		bool Enabled;
	};

	std::map<std::string, Entity> m_Entities;

	std::map<std::string, unsigned int> m_SamplerSlots;

	MaterialWorkflowPBR* m_MaterialWorkflowPBR;

	int m_HDRI_Edit;
	int m_HDRI_Edit_Prev;

	glm::vec3 m_LightPosition;
	glm::vec3 m_LightColor;

	float m_SkyboxLOD;

	// viewport
	bool m_IsViewportEnabled;
	bool m_ViewportFocused;
	bool m_ViewportHovered;
	glm::vec2 m_ViewportSize;
	Framebuffer* m_RenderFramebuffer;
	float m_CurrentTimestamp;
	EventCooldown m_ResizeViewport;

	bool m_VisibleAABBs;

	int m_ImGuizmoType = -1; // -1 = no gizmo

	glm::vec3 m_Translation_ImGuizmo;
	glm::mat4* m_Transform_ImGuizmo;

};