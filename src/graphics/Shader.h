#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "MathStruct.h"

class Shader {
  public:
	void begin();
	void setMatrix(const Matrix& matrix);
	void setLight(Light light);

  private:
	ID3D11Device* _device = nullptr;
	ID3D11DeviceContext* _deviceContext = nullptr;
	ID3D11VertexShader* _vertexShader = nullptr;
	ID3D11InputLayout* _inputLayout = nullptr;
	ID3D11Buffer* _constantBuffer = nullptr;
	ID3D11Buffer* _lightBuffer = nullptr;
	ID3D11PixelShader* _pixelShader = nullptr;
	ID3D11SamplerState* _samplerState = nullptr;

  public:
		static void CreateInstance(ID3D11Device* device, ID3D11DeviceContext* deviceContext) {
			DeleteInstance();
			s_instance = new Shader(device, deviceContext);
		}
		static void DeleteInstance() {
			delete s_instance;
			s_instance = nullptr;
		}
		static Shader& GetInstance() {
			return *s_instance;
		}
  private:
		Shader(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		~Shader();
		static inline Shader* s_instance;
};

#define SHADER Shader::GetInstance()