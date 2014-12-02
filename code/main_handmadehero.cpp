//This is my first game from scratch!!

#include <windows.h>

#define local_persist static
#define global_variable static
#define internal static

global_variable bool Running;
global_variable BITMAPINFO BitMapInfo;
global_variable void *BitMapMemory;
global_variable HBITMAP BitMapHandle;
global_variable HDC BitMapDeviceContext;

LRESULT CALLBACK Win32MainWindowCallBack(
	HWND	Window,
	UINT	Message,
	WPARAM	WParam,
	LPARAM	LParam
	);
	
LRESULT Win32CreateInitialWindow(HINSTANCE Instance);

internal void Win32ResizeDIBSection(int, int);

internal void Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width, int Hight);

internal void Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width, int Hight)
{
	StretchDIBits(	DeviceContext,
		X, Y, Width, Hight,
		X, Y, Width, Hight,
		BitMapMemory,
		&BitMapInfo,
		DIB_RGB_COLORS,
		SRCCOPY
		);

}

internal void Win32ResizeDIBSection(int Width, int Hight)
{
	//TODO(thadeu): Free our DIB Section

	if (BitMapHandle)
	{
		DeleteObject(BitMapHandle);
	}
	if (!BitMapDeviceContext)
	{
		BitMapDeviceContext = CreateCompatibleDC(0);
	}

	BitMapInfo.bmiHeader.biSize = sizeof(BitMapInfo.bmiHeader);
	BitMapInfo.bmiHeader.biHeight = Hight;
	BitMapInfo.bmiHeader.biWidth = Width;
	BitMapInfo.bmiHeader.biPlanes = 1;
	BitMapInfo.bmiHeader.biBitCount = 32;
	BitMapInfo.bmiHeader.biCompression = BI_RGB;


	BitMapHandle = CreateDIBSection(BitMapDeviceContext,
									&BitMapInfo,
									DIB_RGB_COLORS,
									&BitMapMemory,
									0,
									0);

}



LRESULT Win32CreateInitialWindow(HINSTANCE Instance){
	WNDCLASS WindowClass = {};
	//TODO(casey) : Check if

	WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	WindowClass.lpfnWndProc = Win32MainWindowCallBack;
	WindowClass.hInstance = Instance;
	WindowClass.lpszClassName = "HandmadeHeroWindowClass";

	if (RegisterClass(&WindowClass))
	{
		HWND WindowHandler = CreateWindowExA(
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
			Running = true;
			while (Running){
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

LRESULT CALLBACK Win32MainWindowCallBack(
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
			
			Win32UpdateWindow(DeviceContext, X, Y, Width, Hight);
			EndPaint(Window, &Paint);
		} break;
		case WM_SIZE:
		{
			RECT ClientRect;
			GetClientRect(Window, &ClientRect);
			int Width = ClientRect.right - ClientRect.left;
			int Hight = ClientRect.bottom - ClientRect.top;
			Win32ResizeDIBSection(Width, Hight);
			OutputDebugStringA("WM_SIZE\n");
		} break;
		case WM_DESTROY:
		{
			//TODO: This is a ERRO situation, deal with it!!
			Running = false;
			OutputDebugStringA("WM_DESTROY\n");
		} break;
		case WM_CLOSE:
		{
			Running = false;
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
	Win32CreateInitialWindow(Instance);

	return 0;
}
