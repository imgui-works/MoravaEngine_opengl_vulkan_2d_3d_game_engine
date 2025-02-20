#pragma once

#include "H2M/Renderer/MeshH2M.h"

#include "Framebuffer/MoravaFramebuffer.h"
#include "Material/MaterialWorkflowPBR.h"
#include "Mesh/CubeSkybox.h"
#include "Scene/Scene.h"
#include "Texture/TextureCubemap.h"

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
		std::map<std::string, H2M::RefH2M<MoravaShader>> shaders, std::map<std::string, int> uniforms) override;
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

	H2M::RefH2M<MoravaShader> m_ShaderMain;
	H2M::RefH2M<MoravaShader> m_ShaderBackground;
	H2M::RefH2M<MoravaShader> m_ShaderHybridAnimPBR;
	H2M::RefH2M<MoravaShader> m_ShaderHDR;
	H2M::RefH2M<MoravaShader> m_ShaderEquirectangularConversion;
	H2M::RefH2M<MoravaShader> m_ShaderEnvFiltering;
	H2M::RefH2M<MoravaShader> m_ShaderEnvIrradiance;
	H2M::RefH2M<MoravaShader> m_ShaderBasic;

	H2M::RefH2M<H2M::MeshH2M> m_MeshAnimPBR_M1911;
	H2M::RefH2M<H2M::MeshH2M> m_MeshAnimPBR_BobLamp;
	H2M::RefH2M<H2M::MeshH2M> m_MeshAnimPBR_AnimBoy;

	H2M::RefH2M<Material> m_BaseMaterial_M1911;
	H2M::RefH2M<Material> m_BaseMaterial_BobLamp;
	H2M::RefH2M<Material> m_BaseMaterial_AnimBoy;

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

		AABB AABBox;
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
	H2M::RefH2M<MoravaFramebuffer> m_RenderFramebuffer;
	float m_CurrentTimestamp;
	EventCooldown m_ResizeViewport;

	bool m_VisibleAABBs;

	int m_ImGuizmoType = -1; // -1 = no gizmo

	glm::vec3 m_Translation_ImGuizmo;
	glm::mat4* m_Transform_ImGuizmo;

};
