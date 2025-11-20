#pragma once

#include "DirectX.h"

struct Float2 {
  float x;
  float y;
};

struct Int2 {
  int x;
  int y;
};

struct Float3 {
  float x;
  float y;
  float z;
};

struct Float4 {
  float r;
  float g;
  float b;
  float a;
};

struct Vertex {
  XMFLOAT3 position;
  XMFLOAT2 texCoord;
  XMFLOAT4 color;
  XMFLOAT3 normal;
};

struct Matrix {
  XMMATRIX world;
  XMMATRIX view;
  XMMATRIX projection;
};

struct Light {
  BOOL enable;
  XMFLOAT3 direction;
};