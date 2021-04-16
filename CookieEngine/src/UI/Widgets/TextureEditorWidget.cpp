#include "Resources/ResourcesManager.hpp"
#include <string>
#include "TextureEditorWidget.hpp"
#include "Serialization.hpp"

#include <imgui.h>
#include <imgui_stdlib.h>

#include "CustomImWidget.hpp"

using namespace ImGui;
using namespace Cookie::UIwidget;
using namespace Cookie::Resources;


void TextureEditor::WindowDisplay()
{
	TryBeginWindow()
	{
		Text("Currently loaded and available textures:");
		
		
		BeginGroup();

        for (std::unordered_map<std::string, std::shared_ptr<Texture>>::iterator textPtr = resources.textures.begin(); textPtr != resources.textures.end(); textPtr++)
        {
			BeginGroup();

			Custom::Zoomimage(static_cast<ImTextureID>(textPtr->second->GetResourceView()), 100, 100, 10, true);
			
			if (newTexture.name == textPtr->second->name)
			{
				TextColored({0.75, 0.25, 0.25, 1}, "%s (<- Already in use!)", textPtr->second->name.c_str());
				newTexture.invalidName = true;
			}
			else Custom::TextSnip(textPtr->second->name.c_str(), 15);

			EndGroup();

			SameLine();
			if (GetContentRegionAvail().x < 100.f) NewLine();
        }

		EndGroup();


		Separator();

		if (!newTexture.creating)
		{
			if (Button("Create new Texture")) 
				newTexture.creating = true;
		}
		else
		{
			InputText("Texture name", &newTexture.name);
			ColorPicker4("##NEW_TEXTURE_COLOR", newTexture.color.e, ImGuiColorEditFlags_AlphaPreviewHalf | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_DisplayRGB);

			NewLine();
			
			
			if (Button("Cancel"))
			{
				newTexture.creating = false;
				newTexture.name.clear();
				newTexture.invalidName = false;
				newTexture.color = { 0, 0, 0, 1 };
			}

			SameLine();
			
			if (newTexture.name.empty())
			{
				Text("Your texture has no name!");
			}
			else if (newTexture.invalidName)
			{
				Text("Your texture's name is invalid. (already in use)");
				newTexture.invalidName = false;
			}
			else if (Button("Confirm and save"))
			{				
				resources.textures[newTexture.name] = (std::make_shared<Texture>(newTexture.name, newTexture.color));
				Cookie::Resources::Serialization::Save::SaveTexture(newTexture.name, newTexture.color);

				newTexture.creating = false;
				newTexture.name.clear();
				newTexture.color = { 0, 0, 0, 1 };
			}
		}
	}

	ImGui::End();
}
