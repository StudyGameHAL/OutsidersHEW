#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <mmsystem.h>
#include "core/debug_ostream.h"
#include "render/DirectX.h"
#include "render/Shader.h"
#include "core/Keyboard.h"
#include "Game.h"

#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
int g_countFPS;
char g_debugStr[2048] = "ウインドウ表示";
#endif

static constexpr char WINDOW_CLASS[] = "GameWindow";
static constexpr char TITLE[] = "ウインドウ表示";
static Game* game;

void Initialize(HWND hwnd);
void Update();
void Draw();
void Finalize();

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hPrevInst, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
  WNDCLASSEX wcex{};
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.lpfnWndProc = WndProc;
  wcex.hInstance = hInst;
  wcex.hIcon = LoadIcon(hInst, IDI_APPLICATION);
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.hbrBackground = NULL;
  wcex.lpszMenuName = nullptr;
  wcex.lpszClassName = WINDOW_CLASS;
  wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);
  RegisterClassEx(&wcex);

  RECT windowRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
  DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
  AdjustWindowRect(&windowRect, windowStyle, false);
  int windowWidth = windowRect.right - windowRect.left;
  int windowHeight = windowRect.bottom - windowRect.top;

  HWND hWnd = CreateWindow(
    WINDOW_CLASS,
    TITLE,
    windowStyle,
    CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,
    nullptr,
    nullptr,
    hInst,
    nullptr
  );

  Initialize(hWnd);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  DWORD dwExecLastTime;
  DWORD dwFPSLastTime;
  DWORD dwCurrentTime;
  DWORD dwFrameCount;

  timeBeginPeriod(1);
  dwExecLastTime = dwFPSLastTime = timeGetTime();
  dwCurrentTime = dwFrameCount = 0;

  MSG msg;
  do {
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else {
      dwCurrentTime = timeGetTime();
      if (dwCurrentTime - dwFPSLastTime >= 1000) {
#ifdef _DEBUG
        g_countFPS = dwFrameCount;
#endif
        dwFPSLastTime = dwCurrentTime;
        dwFrameCount = 0;
      }

      if (dwCurrentTime - dwExecLastTime >= (1000 / 60)) {
        dwExecLastTime = dwCurrentTime;

#ifdef _DEBUG
        wsprintf(g_debugStr, TITLE);
        wsprintf(&g_debugStr[strlen(g_debugStr)], "FPS: %d", g_countFPS);
        SetWindowText(hWnd, g_debugStr);
#endif
        Update();
        Draw();

        dwFrameCount++;
      }
    }
  } while (msg.message != WM_QUIT);

  Finalize();

  return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  case WM_KEYDOWN:
    if (wParam == VK_ESCAPE) {
      SendMessage(hWnd, WM_CLOSE, 0, 0);
    }
  case WM_SYSKEYDOWN:
  case WM_KEYUP:
  case WM_SYSKEYUP:
    Keyboard_ProcessMessage(msg, wParam, lParam);
    break;

  case WM_CLOSE:
    if (IDOK == MessageBox(hWnd, "ゲームを終了しますが？", "ゲーム終了", MB_OKCANCEL | MB_ICONQUESTION)) {
      DestroyWindow(hWnd);
    }
    break;

  default:
    return DefWindowProc(hWnd, msg, wParam, lParam);
  }
  return 0;
}

void Initialize(HWND hWnd) {
  CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
  Keyboard_Initialize();
  DirectX11::CreateInstance(hWnd);
  Shader::CreateInstance(DX.getDevice(), DX.getDeviceContext());
  //Texture::CreateInstance(DX.getDevice());
  srand(time(NULL));
  game = new Game();
}

void Update() {
  game->Update();
  Keyboard_Update();
}

void Draw() {
  DX.clear();
  game->Draw();
  DX.present();
}

void Finalize() {
  delete game;
  //Texture::DeleteInstance();
  Shader::DeleteInstance();
  DirectX11::DeleteInstance();
}