#include "MaterialEditorPanel.h"

#include "Material/MaterialLibrary.h"

#include "imgui.h"


MaterialEditorPanel::MaterialEditorPanel()
{
	m_CheckerboardTexture = ResourceManager::LoadHazelTexture2D("Textures/Hazel/Checkerboard.png");
}

MaterialEditorPanel::~MaterialEditorPanel()
{
}

void MaterialEditorPanel::OnImGuiRender(bool* p_open)
{
	ImGui::Begin("Material Editor", p_open);

	unsigned int materialIndex = 0;
	for (auto material_it = MaterialLibrary::s_EnvMapMaterials.begin(); material_it != MaterialLibrary::s_EnvMapMaterials.end();)
	{
		Hazel::Ref<EnvMapMaterial> material = material_it->second;
		std::string materialName = material->GetName();
		MaterialUUID materialUUID = material->GetUUID();

		// Material section
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)materialIndex++, flags, materialName.c_str());

		bool materialDelete = false;
		bool materialClone = false;

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Material"))
			{
				materialDelete = true;
			}

			if (ImGui::MenuItem("Clone Material"))
			{
				materialClone = true;
			}

			ImGui::EndPopup();
		}

		if (opened)
		{
			ImGuiWrapper::DrawMaterialUI(material, m_CheckerboardTexture);

			ImGui::TreePop();
		}

		if (materialClone) {
			auto envMapMaterialSrc = MaterialLibrary::s_EnvMapMaterials.at(materialUUID);
			Hazel::Ref<EnvMapMaterial> envMapMaterialDst = Hazel::Ref<EnvMapMaterial>::Create(MaterialLibrary::NewMaterialName(), envMapMaterialSrc);
			MaterialLibrary::AddEnvMapMaterial(envMapMaterialDst->GetUUID(), envMapMaterialDst);
		}

		if (materialDelete) {
			material_it = MaterialLibrary::s_EnvMapMaterials.erase(material_it++);
		}
		else {
			++material_it;
		}
	}

	// Right-click on blank space
	if (ImGui::BeginPopupContextWindow(0, 1, false))
	{
		if (ImGui::MenuItem("Create a Material"))
		{
			MaterialLibrary::AddNewMaterial("");
		}
		ImGui::EndPopup();
	}

	ImGui::End();
}