#ifndef UI_H
#define UI_H

#include "rlImGui.h"
#include "imgui.h"
#include "utils.h"

#if defined(PLATFORM_DESKTOP)
    #define GLSL_VERSION            330
#else   // PLATFORM_ANDROID, PLATFORM_WEB
    #define GLSL_VERSION            100
#endif


class DocumentWindow
{
protected:
    virtual void Show() = 0;
public:
	bool open = false;

	RenderTexture viewTexture;

	virtual void Setup() = 0;
	virtual void Shutdown() = 0;
	virtual void ShowAsWindow() = 0;
    virtual void ShowAsChild() = 0;
    // virtual void ShowAsTabItem() = 0;
	virtual void Update() = 0;

	bool focused = false;

	bool resized = false;

	Rectangle contentRect = { 0 };
};

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

#endif