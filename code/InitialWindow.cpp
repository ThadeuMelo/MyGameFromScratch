#include <windows.h>
#include "InitialWindow.h"

LRESULT CALLBACK MainWindowCallBack(
	HWND	Window,
	UINT	Message,
	WPARAM	WParam,
	LPARAM	LParam
	)
{
	LRESULT Result = 0;
	
	switch(Message)
	{
		case WM_SIZE:
		{
			OutputDebugStringA("WM_SIZE");
		} break;
		case WM_DESTROY:
		{
			OutputDebugStringA("WM_DESTROY");
		} break;
		case WM_CLOSE:
		{
			OutputDebugStringA("WM_CLOSE");
		} break;
		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP");
		} break;
		default:
		{
			Result =  DefWindowProc(Window, Message,WParam,LParam);
		} break;

	}

	return Result;

}

LRESULT CreateInitialWindow(HINSTANCE Instance){
	WNDCLASS WindowClass = {};
	//TODO(casey) : Check if
	WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	WindowClass.lpfnWndProc = MainWindowCallBack;
	WindowClass.hInstance = Instance;
	WindowClass.lpszClassName = "HandmadeHeroWindowClass";

	if (RegisterClass(&WindowClass))
	{
		HWND WindowHandler = CreateWindowEx(
			0,
			WindowClass.lpszClassName,
			"Handmade Hero",
			WS_OVERLAPPEDWINDOW|WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			Instance,
			0
			);
		if (WindowHandler)
		{
			MSG Message;
			for (;;){
				BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
				if (MessageResult > 0)
				{
					TranslateMessage(&Message);
					DispatchMessage(&Message);
				}
				else
				{
					break;
				}
			}

		}
		else
		{
			//TODO() LOGIN
		}
	}
	else
	{
		//TODO() LOGING
	}

	return 0;
}
