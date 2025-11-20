#include "DirectX.h"

#pragma comment(lib, "d3d11.lib")

DirectX11::DirectX11(HWND hWnd) {
  DXGI_SWAP_CHAIN_DESC scDesc = {};
  scDesc.BufferCount = 1;
  scDesc.BufferDesc.Width = SCREEN_WIDTH;
  scDesc.BufferDesc.Height = SCREEN_HEIGHT;
  scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  scDesc.BufferDesc.RefreshRate.Numerator = 60;
  scDesc.BufferDesc.RefreshRate.Denominator = 1;
  scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  scDesc.OutputWindow = hWnd;
  scDesc.SampleDesc.Count = 1;
  scDesc.SampleDesc.Quality = 0;
  scDesc.Windowed = true;

  D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

  if (FAILED(D3D11CreateDeviceAndSwapChain(
    NULL,
    D3D_DRIVER_TYPE_HARDWARE,
    NULL,
    0,
    NULL,
    0,
    D3D11_SDK_VERSION,
    &scDesc,
    &_swapChain,
    &_device,
    &featureLevel,
    &_deviceContext
  ))) {
    return;
  }

  ID3D11Texture2D* backBuffer = NULL;
  _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
  _device->CreateRenderTargetView(backBuffer, NULL, &_rtv);
  backBuffer->Release();

  D3D11_TEXTURE2D_DESC txDesc = {};
  txDesc.Width = scDesc.BufferDesc.Width;
  txDesc.Height = scDesc.BufferDesc.Height;
  txDesc.MipLevels = 1;
  txDesc.ArraySize = 1;
  txDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  txDesc.SampleDesc = scDesc.SampleDesc;
  txDesc.Usage = D3D11_USAGE_DEFAULT;
  txDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  txDesc.CPUAccessFlags = 0;
  txDesc.MiscFlags = 0;
  _device->CreateTexture2D(&txDesc, NULL, &_depthStencilTex);

  D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
  dsvDesc.Format = txDesc.Format;
  dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  dsvDesc.Flags = 0;
  _device->CreateDepthStencilView(_depthStencilTex, &dsvDesc, &_depthStencilView);

  _deviceContext->OMSetRenderTargets(1, &_rtv, _depthStencilView);

  D3D11_VIEWPORT vp;
  vp.Width = (FLOAT)SCREEN_WIDTH;
  vp.Height = (FLOAT)SCREEN_HEIGHT;
  vp.MinDepth = 0.0f;
  vp.MaxDepth = 1.0f;
  vp.TopLeftX = 0;
  vp.TopLeftY = 0;
  _deviceContext->RSSetViewports(1, &vp);

  D3D11_RASTERIZER_DESC rDesc = {};
  rDesc.FillMode = D3D11_FILL_SOLID;
  rDesc.CullMode = D3D11_CULL_BACK;
  rDesc.DepthClipEnable = TRUE;
  rDesc.MultisampleEnable = FALSE;
  _device->CreateRasterizerState(&rDesc, &_rasterizerState);
  _deviceContext->RSSetState(_rasterizerState);

  D3D11_BLEND_DESC blendDesc = {};
  blendDesc.AlphaToCoverageEnable = FALSE;
  blendDesc.IndependentBlendEnable = FALSE;
  blendDesc.RenderTarget[0].BlendEnable = TRUE;

  blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

  blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

  blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

  float blend_factor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
  _device->CreateBlendState(&blendDesc, &_blendState);
  _deviceContext->OMSetBlendState(_blendState, blend_factor, 0xffffffff);

  D3D11_DEPTH_STENCIL_DESC dsDesc = {};
  dsDesc.DepthEnable = FALSE;
  dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
  dsDesc.StencilEnable = FALSE;
  _device->CreateDepthStencilState(&dsDesc, &_depthStencilStateDepthDisable);
  
  dsDesc.DepthEnable = TRUE;
  _device->CreateDepthStencilState(&dsDesc, &_depthStencilStateDepthEnable);
  
  _deviceContext->OMSetDepthStencilState(_depthStencilStateDepthDisable, NULL);
}

DirectX11::~DirectX11() {
  SAFE_RELEASE(_depthStencilStateDepthDisable);
  SAFE_RELEASE(_depthStencilStateDepthEnable);
  SAFE_RELEASE(_blendState);
  SAFE_RELEASE(_rasterizerState);
  SAFE_RELEASE(_depthStencilView);
  SAFE_RELEASE(_depthStencilTex);
  SAFE_RELEASE(_rtv);
  SAFE_RELEASE(_swapChain);
  SAFE_RELEASE(_deviceContext);
  SAFE_RELEASE(_device);
}

ID3D11Device* DirectX11::getDevice() {
  return _device;
}

ID3D11DeviceContext* DirectX11::getDeviceContext() {
  return _deviceContext;
}

void DirectX11::clear() {
  float clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
  _deviceContext->ClearRenderTargetView(_rtv, clearColor);
  _deviceContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void DirectX11::present() {
  _swapChain->Present(0, 0);
}

void DirectX11::setDepthEnable(bool depthEnable) {
  if (depthEnable) {
    _deviceContext->OMSetDepthStencilState(_depthStencilStateDepthEnable, NULL);
  }
  else {
    _deviceContext->OMSetDepthStencilState(_depthStencilStateDepthDisable, NULL);
  }
}