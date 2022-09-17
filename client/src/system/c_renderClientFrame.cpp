#include "../../hdr/system/c_renderClientFrame.h"
#include "../../hdr/main.h"
#include "../../hdr/system/c_requests.h"

//----------------------------------------------------------------------------------------------------------------------
//
// Render a frame
void renderFrame ()
//----------------------------------------------------------------------------------------------------------------------
{
	static bool sentInitRequest{false};

	clientWindow.startFrame ();
	switch (currentMode)
	{
		case MODE_TYPES::MODE_REQUEST_WAIT:
			break;

		case MODE_TYPES::MODE_REQUEST_INIT_SCRIPT:
			if (!sentInitRequest)
			{
				clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_CONSOLE | MESSAGE_TARGET_LOGFILE, sys_getString ("Sent request to server for init script."));
				sentInitRequest = true;
			}
			break;

		case MODE_TYPES::MODE_WAIT_FOR_INIT_SCRIPT:
			break;

		case MODE_TYPES::MODE_LOAD_RESOURCES:
			break;

		case MODE_TYPES::MODE_SPLASH_SCREEN:
			float     texCoords[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f};
			glm::vec2 renderPosition{0, 0};
			glm::vec2 quadSize{};

			quadSize.x = clientTextures["splash"].getWidth ();
			quadSize.y = clientTextures["splash"].getHeight ();

			quadSize.x = 800;
			quadSize.y = 600;

			if (clientTextures["splash"].getTextureState () == TEXTURE_LOADED)
			{
				if (!c_draw2DQuad (renderPosition, quadSize, "2DQuad", clientTextures["splash"].getTextureID (), texCoords))
				{
					clientMessage.message (MESSAGE_TARGET_STD_OUT | MESSAGE_TARGET_LOGFILE, sys_getString ("Unable to render quad with shader [ %s ] and texture [ %s ].", "2DQuad", "splash"));
				}
			}
			break;
	}

	c_renderConsole ();

	clientTestFont.addText (glm::vec4{0.5f, 0.5f, 0.6f, 1.0f}, glm::vec2{5, clientWindow.getHeight () - (clientTestFont.getHeight () * 2)},
							sys_getString("Connection Done : %s  --- peer.state : %s",
										  clientNetworkObject.getConnectionComplete() ? "true" : "false",
										  getPeerState(clientPeer.peerInfo.state).c_str()));

	if (clientNetworkObject.getConnectionComplete())
	clientTestFont.addText (glm::vec4{0.5f, 0.5f, 0.6f, 1.0f}, glm::vec2{5, clientWindow.getHeight () - (clientTestFont.getHeight () * 3)},
	                        sys_getString("Host : %s Request Count : %i", getHostnameFromAddress (clientNetworkObject.getPeerPointer()->address).c_str(), c_getNumWaitingRequests()));

//	clientTestFont.addText (glm::vec4{1.0f, 1.0f, 1.0f, 1.0f}, glm::vec2{clientWindow.getWidth() - clientTestFont.getTextWidth("Here is some new text."),50}, "Here is some new text.");
//	clientTestFont.addText (glm::vec4{1.0f, 0.0f, 1.0f, 1.0f}, glm::vec2{0,0}, "and a second line from the same buffer !@#.");
//	clientTestFont.addText (glm::vec4{1.0f, 0.0f, 1.0f, 1.0f}, glm::vec2{50,150 + clientTestFont.getHeight()}, "Special CHARS !@#$%^&*()_+  {}[]\\|;':\",./<>?)");

	clientWindow.presentFrame ();
}
