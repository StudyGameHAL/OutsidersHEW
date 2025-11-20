#pragma once

#define NOMINMAX
#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#define SAFE_RELEASE(o)	  if(o) { (o)->Release(); o = NULL; } 

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

class DirectX11 {
  public:
		ID3D11Device* getDevice();
		ID3D11DeviceContext* getDeviceContext();
		void clear();
		void present();
		void setDepthEnable(bool depthEnable);

  private:
		ID3D11Device* _device = NULL;
		ID3D11DeviceContext* _deviceContext = NULL;
		IDXGISwapChain* _swapChain = NULL;
		ID3D11Texture2D* _depthStencilTex = NULL;
		ID3D11RenderTargetView* _rtv = NULL;
		ID3D11DepthStencilView* _depthStencilView = NULL;
		ID3D11RasterizerState* _rasterizerState;
		ID3D11BlendState* _blendState;
		ID3D11DepthStencilState* _depthStencilStateDepthDisable;
		ID3D11DepthStencilState* _depthStencilStateDepthEnable;

  public:
		static void CreateInstance(HWND hWnd) {
			DeleteInstance();
			s_instance = new DirectX11(hWnd);
		}
		static void DeleteInstance() {
			delete s_instance;
			s_instance = nullptr;
		}
		static DirectX11& GetInstance() {
			return *s_instance;
		}
  private:
		DirectX11(HWND hWnd);
		~DirectX11();
		static inline DirectX11* s_instance;
};

#define DX DirectX11::GetInstance()
