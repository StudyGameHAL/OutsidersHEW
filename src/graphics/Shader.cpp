#include "Shader.h"
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;
#include "Directx.h"
#include "../core/debug_ostream.h"
#include <fstream>

Shader::Shader(ID3D11Device* device, ID3D11DeviceContext* deviceContext) {
  _device = device;
  _deviceContext = deviceContext;

  std::ifstream ifs_vs("shaderVertex3D.cso", std::ios::binary);
  ifs_vs.seekg(0, std::ios::end); // ファイルポインタを末尾に移動
  std::streamsize filesize = ifs_vs.tellg(); // ファイルポインタの位置を取得（つまりファイルサイズ）
  ifs_vs.seekg(0, std::ios::beg); // ファイルポインタを先頭に戻す

  // バイナリデータを格納するためのバッファを確保
  unsigned char* vsbinary_pointer = new unsigned char[filesize];
  ifs_vs.read((char*)vsbinary_pointer, filesize); // バイナリデータを読み込む
  ifs_vs.close(); // ファイルを閉じる

  // 頂点シェーダーの作成
  _device->CreateVertexShader(vsbinary_pointer, filesize, nullptr, &_vertexShader);

  // 頂点レイアウトの定義
  D3D11_INPUT_ELEMENT_DESC layout[] = {
	  { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	  { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	  { "COLOR",	  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	  { "NORMAL",	  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  };
  UINT num_elements = ARRAYSIZE(layout); // 配列の要素数を取得
  _device->CreateInputLayout(layout, num_elements, vsbinary_pointer, filesize, &_inputLayout);
  delete[] vsbinary_pointer; // バイナリデータのバッファを解放

  // 頂点シェーダー用定数バッファの作成
  {
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(Matrix); // バッファのサイズ
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // バインドフラグ
    _device->CreateBuffer(&buffer_desc, nullptr, &_constantBuffer);
  }
  {
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(Light); // バッファのサイズ
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // バインドフラグ
    _device->CreateBuffer(&buffer_desc, nullptr, &_lightBuffer);
  }
  
  // 事前コンパイル済みピクセルシェーダーの読み込み
  std::ifstream ifs_ps("shaderPixel3D.cso", std::ios::binary);
  ifs_ps.seekg(0, std::ios::end);
  filesize = ifs_ps.tellg();
  ifs_ps.seekg(0, std::ios::beg);
  unsigned char* psbinary_pointer = new unsigned char[filesize];
  ifs_ps.read((char*)psbinary_pointer, filesize);
  ifs_ps.close();

  // ピクセルシェーダーの作成
  _device->CreatePixelShader(psbinary_pointer, filesize, nullptr, &_pixelShader);
  delete[] psbinary_pointer; // バイナリデータのバッファを解放

  D3D11_SAMPLER_DESC samplerDesc = {};
  samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
  samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.MipLODBias = 0;
  samplerDesc.MaxAnisotropy = 16;
  samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  samplerDesc.MinLOD = 0;
  samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

  _device->CreateSamplerState(&samplerDesc, &_samplerState);
  _deviceContext->PSSetSamplers(0, 1, &_samplerState);
}

Shader::~Shader() {
  SAFE_RELEASE(_pixelShader);
  SAFE_RELEASE(_constantBuffer);
  SAFE_RELEASE(_inputLayout);
  SAFE_RELEASE(_vertexShader);
  SAFE_RELEASE(_samplerState);
}

void Shader::begin() {
  _deviceContext->VSSetShader(_vertexShader, nullptr, 0);
  _deviceContext->PSSetShader(_pixelShader, nullptr, 0);
  _deviceContext->IASetInputLayout(_inputLayout);
  _deviceContext->VSSetConstantBuffers(0, 1, &_constantBuffer);
  _deviceContext->VSSetConstantBuffers(1, 1, &_lightBuffer);
}

void Shader::setMatrix(const Matrix& matrix) {
  XMFLOAT4X4 transpose[3];
  XMStoreFloat4x4(&transpose[0], XMMatrixTranspose(matrix.world));
  XMStoreFloat4x4(&transpose[1], XMMatrixTranspose(matrix.view));
  XMStoreFloat4x4(&transpose[2], XMMatrixTranspose(matrix.projection));
  _deviceContext->UpdateSubresource(_constantBuffer, 0, nullptr, &transpose, 0, 0);
}

void Shader::setLight(const Light light) {
  _deviceContext->UpdateSubresource(_lightBuffer, 0, nullptr, &light, 0, 0);
}