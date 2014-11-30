#include <windows.h>
LRESULT CALLBACK MainWindowCallBack(
	HWND	Window,
	UINT	Message,
	WPARAM	WParam,
	LPARAM	LParam
	);
	
LRESULT CreateInitialWindow(HINSTANCE Instance);

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
		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.top;
			int Hight = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;

			PatBlt(DeviceContext, X, Y, Width, Hight, WHITENESS);
			EndPaint(Window, &Paint);
		} break;
		case WM_SIZE:
		{
			RECT ClientRect;
			GetClientRect(Window, &ClientRect);
			int Width = ClientRect.right - ClientRect.left;
			int Hight = ClientRect.bottom - ClientRect.top;
			OutputDebugStringA("WM_SIZE\n");
		} break;
		case WM_DESTROY:
		{
			OutputDebugStringA("WM_DESTROY\n");
		} break;
		case WM_CLOSE:
		{
			OutputDebugStringA("WM_CLOSE\n");
		} break;
		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		} break;
		default:
		{
			Result =  DefWindowProc(Window, Message,WParam,LParam);
		} break;

	}

	return Result;

}

int CALLBACK WinMain(
	HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR CommandLine,
	int ShowCode
	)
{
	CreateInitialWindow(Instance);

	return 0;
}
