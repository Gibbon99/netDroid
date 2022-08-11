#include "../../hdr/system/c_renderClientFrame.h"
#include "../../hdr/main.h"
#include "../../hdr/system/c_graphics.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Render a frame
void renderFrame()
//----------------------------------------------------------------------------------------------------------------------
{
    clientWindow.startFrame();

	float texCoords[] = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0};
	glm::vec2 renderPosition{0,0};
	glm::vec2 textureSize{};

	textureSize.x = clientTextures["testImage"].getWidth();
	textureSize.y = clientTextures["testImage"].getHeight();

	if (clientTextures["testImage"].getTextureState () == TEXTURE_LOADED)
		c_draw2DQuad (renderPosition, textureSize, "2DQuad", clientTextures["testImage"].getTextureID(), texCoords);

	clientTestFont.addText (glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, glm::vec2{clientWindow.getWidth() - clientTestFont.getTextWidth("Here is some new text."),50}, "Here is some new text.");
	clientTestFont.addText (glm::vec4{1.0f, 0.0f, 1.0f, 1.0f}, glm::vec2{50,150}, "and a second line from the same buffer !@#.");
	clientTestFont.addText (glm::vec4{1.0f, 0.0f, 1.0f, 1.0f}, glm::vec2{50,150 + clientTestFont.getHeight()}, "Special CHARS !@#$%^&*()_+  {}[]\\|;':\",./<>?)");

	clientTestFont.render (glm::ortho (0.0f, static_cast<float>(clientWindow.getWidth ()), 0.0f, static_cast<float>(clientWindow.getHeight ())) * (glm::mat4 (1.0f) * glm::mat4 (1.0f)));

/*
	static int windowHeightInLines = 0;

	if (0 == windowHeightInLines)
		windowHeightInLines = clientWindow.getHeight() / 16;

	clientWindow.startFrame();

	ImGui::Begin("Messages", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
	ImGui::SetWindowPos(ImVec2{0,0}, 0);
	ImGui::SetWindowSize(ImVec2{static_cast<float>(clientWindow.getWidth()), static_cast<float>(clientWindow.getHeight())});

	ImGui::SetCursorPosX(3);

	if (clientMessage.getNumberOfLines() < windowHeightInLines)
	{
		auto indexCount = 0;
		auto drawPosY = 0;
		auto numberConsoleLines = clientMessage.getNumberOfLines();

		drawPosY = numberConsoleLines ;
		for (auto lineCount = 0; lineCount != numberConsoleLines;  lineCount++ )
		{
			ImGui::SetCursorPosY (clientWindow.getHeight() - (drawPosY * 16));
			drawPosY--;
			ImGui::Text("%s", clientMessage.getConsoleLine (indexCount++).c_str());
		}
	}
	else
	{
		for (auto lineY = 0; lineY != windowHeightInLines; lineY++)
		{
			ImGui::SetCursorPosY (clientWindow.getHeight () - (lineY * 16));
			ImGui::Text ("%s", clientMessage.getConsoleLine (lineY).c_str ());
		}
	}
	//ImGui::ShowDemoWindow();
	ImGui::SetCursorPos (ImVec2{0, 0});
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

//	ImGui::Image(testTexture.getSurfacePointer(), ImVec2(300, 300));

	ImGui::End();
*/

    clientWindow.presentFrame();
}
