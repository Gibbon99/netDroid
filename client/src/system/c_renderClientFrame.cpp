#include "../../hdr/system/c_renderClientFrame.h"
#include "../../hdr/main.h"
#include "../../hdr/system/c_graphics.h"
#include "../../hdr/system/c_console.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Render a frame
void renderFrame()
//----------------------------------------------------------------------------------------------------------------------
{
    clientWindow.startFrame();

	float texCoords[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
	glm::vec2 renderPosition{0,0};
	glm::vec2 quadSize{};

	quadSize.x = clientTextures["splash"].getWidth();
	quadSize.y = clientTextures["splash"].getHeight();

	quadSize.x = 800;
	quadSize.y = 600;

	if (clientTextures["splash"].getTextureState () == TEXTURE_LOADED)
	{
		if (!c_draw2DQuad (renderPosition, quadSize, "2DQuad", clientTextures["splash"].getTextureID (), texCoords))
		{
			clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("Unable to render quad with shader [ %s ] and texture [ %s ].", "2DQuad", "splash"));
		}
	}

	c_renderConsole();

//	clientTestFont.addText (glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, glm::vec2{clientWindow.getWidth() - clientTestFont.getTextWidth("Here is some new text."),50}, "Here is some new text.");
//	clientTestFont.addText (glm::vec4{1.0f, 0.0f, 1.0f, 1.0f}, glm::vec2{0,0}, "and a second line from the same buffer !@#.");
//	clientTestFont.addText (glm::vec4{1.0f, 0.0f, 1.0f, 1.0f}, glm::vec2{50,150 + clientTestFont.getHeight()}, "Special CHARS !@#$%^&*()_+  {}[]\\|;':\",./<>?)");



    clientWindow.presentFrame();
}
