#include "app.h"

LRESULT CALLBACK WindowProcessor(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	if (App::GetSingleton())
		return App::GetSingleton()->MessageProcessor(window, message, w_param, l_param);
	else
		return DefWindowProc(window, message, w_param, l_param);
}


App::App() = default;

App* App::singleton_;

App* App::GetSingleton()
{
	if (singleton_ == 0)
		singleton_ = new App;

	return singleton_;
}

void App::Release()
{
	if (window_ != nullptr)
		DestroyWindow(window_);

	//safe_release(CConnection::GetSingleton());
	safe_release(scene_);
}

bool App::Init(HINSTANCE instance)
{
	time_ = new Time;

	directx_ = D3D::GetSingleton();

	InitSettings();

	if (!InitWindow(instance))
	{
		MessageBox(window_, "Failed to create window", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!directx_->Init(resolution_->x, resolution_->y, vsync_enabled_, fullscreen_enabled_, 1000.0f, 0.1f))
	{
		MessageBox(App::GetSingleton()->GetHwnd(), "DirectX initialization failed", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	//if (!CConnection::GetSingleton()->Establish("127.0.0.1", 1111))
	//{
	//	MessageBox(m_hwnd, "Failed to conenct to server", "Network Error", MB_OK | MB_ICONERROR);
	//	return false;
	//}
	//else
	//	if (CConnection::GetSingleton()->SendPacketType(PacketType::HelloMessage))
	//		CConnection::GetSingleton()->SendString(std::string("Hi Server!"));

	if (!LoadScene(new Scene))
		return false;

	return true;
}

void App::InitSettings()
{
	vsync_enabled_ = true;
	fullscreen_enabled_ = false;

	if (fullscreen_enabled_)
	{
		resolution_ = new IntVector2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	}
	else
	{
		resolution_ = new IntVector2(800, 600);
	}
}

bool App::InitWindow(HINSTANCE instance)
{
	WNDCLASSEX wc;
	LPCSTR class_name = "MainWindowClass";
	window_caption_ = "Nixie";
	DWORD style = WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hInstance = instance;
	wc.lpfnWndProc = WindowProcessor;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = class_name;

	if (!RegisterClassEx(&wc))
		return false;

	IntVector2* window_size;
	if (fullscreen_enabled_)
	{
		DEVMODE dmScreenSettings;

		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = static_cast<ULONG>(resolution_->x);
		dmScreenSettings.dmPelsHeight = static_cast<ULONG>(resolution_->y);
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		window_size = new IntVector2();
	}
	else
		window_size = new IntVector2(GetSystemMetrics(SM_CXSCREEN) / 2 - resolution_->x / 2, GetSystemMetrics(SM_CYSCREEN) / 2 - resolution_->y / 2);

	window_ = CreateWindowEx(WS_EX_APPWINDOW, class_name, window_caption_, style,
		window_size->x, window_size->y, resolution_->x, resolution_->y, NULL, NULL, instance, NULL);
	if (!window_)
	{
		std::cerr << "Failed to create main window" << std::endl;
		return false;
	}

	ShowWindow(window_, SW_SHOW);

	return true;
}

LRESULT App::MessageProcessor(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_ACTIVATE:
		if (LOWORD(w_param) == WA_INACTIVE)
		{
			is_paused_ = true;
			time_->Stop();
		}
		else
		{
			is_paused_ = false;
			time_->Start();
		}
		return 0;
	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)l_param)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)l_param)->ptMinTrackSize.y = 200;
		return 0;
	default:
		return DefWindowProc(window, message, w_param, l_param);
	}
}

int App::Run()
{
	time_->Reset();

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			time_->Tick();
			if (!is_paused_)
			{
#ifdef _DEBUG
				CalculateFrameStats();
#endif
				Update(0.0f);
			}
			else
				Sleep(100);
		}
	}

	return static_cast<int>(msg.wParam);
}

void App::Update(float delta_time)
{
	directx_->BeginScene(scene_->GetClearColor());
	scene_->Update();
	directx_->EndScene();
}

HWND App::GetHwnd()
{
	return window_;
}

D3D* App::GetDirectX()
{
	return directx_;
}

Scene* App::GetScene()
{
	return scene_;
}

void App::CalculateFrameStats()
{
	static int frame_count = 0;
	static float time_elapsed = 0;

	frame_count++;

	if ((time_->GetTime() - time_elapsed) >= 1)
	{
		float fps = static_cast<float>(frame_count);
		float mspf = 1000 / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << window_caption_ << " | FPS: " << fps << " Frame time: " << mspf << "(ms)";
		SetWindowTextW(window_, (LPCWSTR)outs.str().c_str());
		frame_count = 0;
		time_elapsed += 1;
	}
}

bool App::LoadScene(Scene* scene)
{
	if (!scene->Init())
		return false;

	scene_ = scene;

	return true;
}