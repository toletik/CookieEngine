#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__


#include "Core/Window.hpp"
#include "Light.hpp"
#include "Drawers/FBODrawer.hpp"
#include "Render/RendererRemote.hpp"
#include "Render/DrawDataHandler.hpp"
#include "Render/RenderPass/GeometryPass.hpp"
#include "Render/RenderPass/ShadowPass.hpp"
#include "Render/RenderPass/LightPass.hpp"
#include "Render/RenderPass/ComposePass.hpp"
#include "Render/RenderPass/GameplayPass.hpp"

namespace Cookie
{
	class Game;

	namespace Resources
	{
		class ResourcesManager;
	};

	namespace Render
	{
		class Camera;


		class Renderer
		{
			private:
				IDXGISwapChain*				swapchain		= nullptr;
				ID3D11RenderTargetView*		backbuffer		= nullptr;
					
			public:

				Core::Window	window;
				RendererRemote	remote;
				D3D11_VIEWPORT	viewport;
				D3D11_PRIMITIVE_TOPOLOGY topo{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
				DrawDataHandler drawData;

				GeometryPass	geomPass;
				ShadowPass		shadPass;
				LightPass		lightPass;
				ComposePass		compPass;
				GameplayPass	gamePass;

				LightsArray lights;

			private:
				Render::FBODrawer	fboDrawer;

			private:
				RendererRemote InitDevice(Core::Window& window);
				bool CreateDrawBuffer(int width, int height);

			public:
				/* CONSTRUCTORS/DESTRUCTORS */
				Renderer();
				~Renderer();
				
				void Draw(const Camera* cam, Game& game, FrameBuffer& fbo);
				void Render()const;
				void Clear();
				void SetBackBuffer();
				void DrawFrameBuffer(FrameBuffer& fbo);
				void ClearFrameBuffer(FrameBuffer& fbo);

				void ResizeBuffer(int width, int height);
		};
	}
}

#endif /*__RENDERER_HPP__*/