// EngineSample.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "EngineSample.h"
#include "WiDemo.h"

using namespace std;

#define MAX_LOADSTRING 100

extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND g_hWnd;
Demo demo;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ENGINESAMPLE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_ENGINESAMPLE));


	if (!demo.setWindow(g_hWnd,hInstance))
		return 1;
	
	demo.Initialize();

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else{

			demo.run();

		}
	}


	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ENGINESAMPLE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_ENGINESAMPLE);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   int x = CW_USEDEFAULT, y = 0, w = CW_USEDEFAULT, h = 0;
   string voidStr = "";

   ifstream file("config.ini");
   if (file.is_open())
   {
	   int enabled;
	   file >> voidStr >> enabled;
	   if (enabled!=0)
	   {
		   file >> voidStr >> x >> voidStr >> y >> voidStr >> w >> voidStr >> h >> voidStr >> demo.fullscreen;
		   demo.screenW = w;
		   demo.screenH = h;
	   }
   }
   file.close();

   g_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      x, y, w, h, NULL, NULL, hInstance, NULL);

   if (!g_hWnd)
   {
      return FALSE;
   }

   ShowWindow(g_hWnd, nCmdShow);
   UpdateWindow(g_hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	//case WM_INPUT:
	//	wiInputManager::rawinput->RetrieveData(lParam);
	//	return 0;
	//	break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			//DestroyWindow(hWnd);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			if (wiBackLog::isActive())
				wiBackLog::historyPrev();
			break;
		case VK_DOWN:
			if (wiBackLog::isActive())
				wiBackLog::historyNext();
			break;
		default:
			break;
		}
		break;
	case WM_CHAR:
		switch (wParam)
		{
		case VK_BACK:
			if (wiBackLog::isActive())
				wiBackLog::deletefromInput();
			break;
		case VK_RETURN:
			if (wiBackLog::isActive())
				wiBackLog::acceptInput();
			break;
		default:
			if (wiBackLog::isActive()){
				const char c = (const char)(TCHAR)wParam;
				wiBackLog::input(c);
			}
			break;
		}
		break;
	case WM_MOUSEMOVE:
		if (demo.interactionType == Demo::INTERACT::WATER){
			if (wParam & MK_LBUTTON)
			{
				POINT p;
				GetCursorPos(&p);
				ScreenToClient(hWnd, &p);
				wiRenderer::Picked picked = wiRenderer::Pick(p.x, p.y, PICK_WATER);
				if (picked.object != nullptr){
					wiRenderer::PutWaterRipple("images/ripple.png", picked.position);
				}

			}
		}
		break;
	case WM_MOUSEWHEEL:
		switch (demo.interactionType)
		{
		case Demo::DECAL:
			demo.interactionType = Demo::WATER;
			break;
		case Demo::WATER:
			demo.interactionType = Demo::SPAWN_OBJECT;
			break;
		case Demo::SPAWN_OBJECT:
			demo.interactionType = Demo::SPAWN_LIGHT;
			break;
		case Demo::SPAWN_LIGHT:
			demo.interactionType = Demo::DECAL;
			break;
		default:
			break;
		}
		break;
	case WM_RBUTTONDOWN:
		demo.mousebuttondown = true;
		ShowCursor(false);
		break;
	case WM_RBUTTONUP:
		demo.mousebuttondown = false;
		ShowCursor(true);
		break;
	case WM_LBUTTONDOWN:
	{
		if (demo.demoScene == Demo::HELLOWORLD)
			break;
		if (demo.interactionType == Demo::INTERACT::DECAL){
			POINT p;
			GetCursorPos(&p);
			ScreenToClient(hWnd, &p);
			wiRenderer::Picked picked = wiRenderer::Pick(p.x, p.y, PICK_OPAQUE);
			if (picked.object != nullptr){
				XMFLOAT4 rot = wiRenderer::getCamera()->rotation;
				Decal* decal = new Decal(picked.position, XMFLOAT3(5, 5, 5), rot);
				decal->life = 200;
				decal->fadeStart = 50;
				switch (wiRandom::getRandom(0,2)){
				case 0:
					decal->addTexture("images/blood1.png");
					break;
				case 1:
					decal->addTexture("images/leaf.png");
					break;
				case 2:
					decal->addTexture("images/graffiti.png");
					break;
				case 3:
					decal->addTexture("images/blood2.png");
					break;
				default:break;
				};
				wiRenderer::PutDecal(decal);
				decal->attachTo(picked.object);
			}
		}
		else if (demo.interactionType == Demo::SPAWN_OBJECT){
			XMMATRIX spawnTrans = XMMatrixRotationQuaternion(XMLoadFloat4(&wiRenderer::getCamera()->rotation))*XMMatrixTranslationFromVector(XMVectorAdd(wiRenderer::getCamera()->GetEye(), wiRenderer::getCamera()->GetAt() * 5));
			switch (wiRandom::getRandom(0,3))
			{
			case 0:
				wiRenderer::LoadModel("models/barrel/", "barrel",
					spawnTrans
					, "common");
				break;
			case 1:
				wiRenderer::LoadModel("models/crate/", "crate",
					spawnTrans
					, "common");
				break;
			case 2:
				wiRenderer::LoadModel("models/monkey/", "monkey",
					spawnTrans
					, "common"
					);
				break;
			case 3:
				wiRenderer::LoadModel("models/golfball/", "golfball",
					spawnTrans
					, "common"
					);
				break;
			default:
				break;
			}
		}
		else if (demo.interactionType == Demo::SPAWN_LIGHT){
			XMMATRIX spawnTrans = XMMatrixRotationQuaternion(XMLoadFloat4(&wiRenderer::getCamera()->rotation))*XMMatrixTranslationFromVector(XMVectorAdd(wiRenderer::getCamera()->GetEye(), wiRenderer::getCamera()->GetAt() * 5));
			switch (wiRandom::getRandom(0,2))
			{
			case 0:
				wiRenderer::LoadModel("models/lightTube blue/", "lightTube blue",
					spawnTrans
					, "common");
				break;
			case 1:
				wiRenderer::LoadModel("models/lightTube green/", "lightTube green",
					spawnTrans
					, "common");
				break;
			case 2:
				wiRenderer::LoadModel("models/lightTube red/", "lightTube red",
					spawnTrans
					, "common");
				break;
			default:
				break;
			}
		}
	}
	break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
