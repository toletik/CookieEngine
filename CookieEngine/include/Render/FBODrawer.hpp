#ifndef __FBO_DRAWER_HPP__
#define __FBO_DRAWER_HPP__

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11SamplerState;

namespace Cookie
{
	namespace Render
	{
		class FrameBuffer;

		class FBODrawer
		{
			private:
				ID3D11VertexShader* VShader{ nullptr };
				ID3D11PixelShader*	PShader{ nullptr };
				ID3D11SamplerState* PSampler{ nullptr };

			private:
				void InitShader();

			public:
				FBODrawer();
				~FBODrawer();

				void Set();
				void Draw(FrameBuffer& fbo);

		};
	}
}

#endif // !__FBO_DRAWER_HPP__
