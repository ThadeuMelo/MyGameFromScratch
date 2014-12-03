//This is my first game from scratch!!

#include <windows.h>
#include <stdint.h>

#define local_persist static
#define global_variable static
#define internal static

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

global_variable bool Running;
global_variable BITMAPINFO BitMapInfo;
global_variable void *BitMapMemory;
global_variable int BitMapWidth, BitMapHeight;
global_variable int BytesPerPixel = 4; 

LRESULT CALLBACK Win32MainWindowCallBack(
	HWND	Window,
	UINT	Message,
	WPARAM	WParam,
	LPARAM	LParam
	);
	
LRESULT Win32CreateInitialWindow(HINSTANCE Instance);

internal void Win32ResizeDIBSection(int, int);

internal void Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int X, int Y, int Width, int Height);

internal void RenderWierdGradient(int XOffset, int YOffset)
{
	int Width = BitMapWidth;
	int Height = BitMapHeight;
	
	int Pitch = Width*BytesPerPixel;
	uint8 *Row = (uint8 *) BitMapMemory;
	
	for(int Y = 0; Y < BitMapHeight; ++Y)
	{
		uint32 *Pixel = (uint32 *) Row;
		for(int X = 0; X < BitMapWidth; ++X)
		{
			uint8 Blue ;
			uint8 Green ;
			uint8 Red = 0;
			
			if (X > Width/2){
				Blue = 0;
				Red = (X + XOffset);
			}else
			{
				Blue = (X + XOffset);
				Green = (Y + YOffset);
				Red = 0;
			}
			*Pixel++ = ((Red<<16)|(Green<<8)|Blue);
		}
		Row += Pitch;
	}
}

internal void Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int X, int Y, int Width, int Height)
{
	int WindowWidth = WindowRect->right - WindowRect->left;
	int WindowHeight = WindowRect->bottom - WindowRect->top;
	StretchDIBits(	DeviceContext,
	/*
		X, Y, Width, Height,
		X, Y, Width, Height,
	*/
		0,0, BitMapWidth, BitMapHeight,
		0,0, WindowWidth, WindowHeight,
		BitMapMemory,
		&BitMapInfo,
		DIB_RGB_COLORS,
		SRCCOPY
		);

}

internal void Win32ResizeDIBSection(int Width, int Height)
{
	
	if(BitMapMemory)
	{
		VirtualFree(BitMapMemory, NULL, MEM_RELEASE);
	}
	BitMapWidth = Width;
	BitMapHeight = Height;
	
	BitMapInfo.bmiHeader.biSize = sizeof(BitMapInfo.bmiHeader);
	BitMapInfo.bmiHeader.biHeight = - BitMapHeight;
	BitMapInfo.bmiHeader.biWidth = BitMapWidth;
	BitMapInfo.bmiHeader.biPlanes = 1;
	BitMapInfo.bmiHeader.biBitCount = 32; //24 RGB and 8 for pading 
	BitMapInfo.bmiHeader.biCompression = BI_RGB;


	int BitMapMemSize = (Height*Width)*BytesPerPixel;
	
	BitMapMemory = VirtualAlloc(NULL, BitMapMemSize, MEM_COMMIT, PAGE_READWRITE);
	
	
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
		HWND Window = CreateWindowExA(
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
		if (Window)
		{
			MSG Message;
			Running = true;
			uint8 XOffset = 0;
			uint8 YOffset = 0;
			while (Running){
				while(PeekMessage( &Message,  0,0,0, PM_REMOVE)) 
				{
					if(Message.message == WM_QUIT)
					{
						Running = false;
					}
					TranslateMessage(&Message);
					DispatchMessage(&Message);				
				}
				
				RenderWierdGradient(XOffset, YOffset);
				RECT ClientRect;
				HDC DeviceContext = GetDC(Window);
				GetClientRect(Window, &ClientRect);
				int Width = ClientRect.right - ClientRect.left;
				int Height = ClientRect.bottom - ClientRect.top;
				Win32UpdateWindow(DeviceContext, &ClientRect, XOffset, YOffset, Width, Height);
				ReleaseDC(Window, DeviceContext);
				XOffset++;
				YOffset--;

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
			int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;
			
			RECT ClientRect;
			GetClientRect(Window, &ClientRect);
			RenderWierdGradient(0,0);
			Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
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
