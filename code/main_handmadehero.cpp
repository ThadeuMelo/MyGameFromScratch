//This is my first game from scratch!!

#include <windows.h>
#include <stdint.h>
#include <xinput.h>

#define local_persist static
#define global_variable static
#define internal static

#define PIXEL_BIT_COUNT 32
#define BYTES_PER_PIXEL 4

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

global_variable bool GlobalRunning;

struct Win32_Off_Screen_Buffer
{
	 BITMAPINFO Info;
	 void *Memory;
	 int Width;
	 int Height;
	 int Pitch;
};


struct Win32_Window_Dimension
{
	int Width;
	int Height;
};

struct ButtonActions
{
	bool Up;
	bool Down;
	bool Left;
	bool Right; 
	bool ButA;
	bool ButB;
	bool ButX;
	bool ButY;
	bool LeftSh;
	bool RigtSh;
	
	int16 StickX;
	int16 StickY;
}

global_variable Win32_Off_Screen_Buffer GlobalBackBuffer;

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex,  XINPUT_STATE* pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
	return ERROR_DEVICE_NOT_CONNECTED;
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

#define X_INPUT_SET_STATE(name)  DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
	return ERROR_DEVICE_NOT_CONNECTED;
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

LRESULT CALLBACK Win32MainWindowCallBack(
	HWND	Window,
	UINT	Message,
	WPARAM	WParam,
	LPARAM	LParam
	);
	
LRESULT Win32CreateInitialWindow(HINSTANCE Instance);

internal void Win32ResizeDIBSection(Win32_Off_Screen_Buffer *Buffer, int, int);

internal void Win32UpdateWindow(	Win32_Off_Screen_Buffer Buffer, HDC DeviceContext,
									int WindowWidth, int WindowHeight);

internal Win32_Window_Dimension Win32GetWindowDimension(HWND Window);

internal void Wind32LoadXInput(void);

internal ButtonActions getButtonAction(XINPUT_GAMEPAD *Pad)
{
	ButtonActions tempButtAct;
	tempButtAct.Up 	=	(Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
	tempButtAct.Down = 	(Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
	tempButtAct.Left = 	(Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
	tempButtAct.Right = 	(Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
	tempButtAct.ButA = 	(Pad->wButtons & XINPUT_GAMEPAD_A);
	tempButtAct.ButB = 	(Pad->wButtons & XINPUT_GAMEPAD_B);
	tempButtAct.ButX = 	(Pad->wButtons & XINPUT_GAMEPAD_X);
	tempButtAct.ButY = 	(Pad->wButtons & XINPUT_GAMEPAD_Y);
	tempButtAct.LeftSh = 	(Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
	tempButtAct.RigtSh = 	(Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
	tempButtAct.StickX = 	Pad->sThumbLX;
	tempButtAct.StickY = 	Pad->sThumbLY;
	
	return tempButtAct;
}

internal void 
Wind32LoadXInput(void)
{
	HMODULE XInputLibrary = LoadLibrary("xinput1_4.dll");
	if(!XInputLibrary)
	{
		XInputLibrary = LoadLibrary("xinput1_3.dll");
	}
	if (XInputLibrary)
	{
		XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibray, "XInputGetState");
		XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibray, "XInputSetState");
	}
}

internal Win32_Window_Dimension 
Win32GetWindowDimension(HWND Window)
{
	Win32_Window_Dimension Result;
	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	Result.Width = ClientRect.right - ClientRect.left;
	Result.Height = ClientRect.bottom - ClientRect.top;

	return Result;
}


internal void 
RenderWierdGradient(Win32_Off_Screen_Buffer *Buffer, int XOffset, int YOffset)
{

	
	int Pitch = Buffer->Width*(BYTES_PER_PIXEL);
	uint8 *Row = (uint8 *) Buffer->Memory;
	
	for(int Y = 0; Y < Buffer->Height; ++Y)
	{
		uint32 *Pixel = (uint32 *) Row;
		for(int X = 0; X < Buffer->Width; ++X)
		{
			uint8 Blue ;
			uint8 Green ;
			uint8 Red = 0;
			
			if (X > Buffer->Width/2){
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

internal void 
Win32UpdateWindow(	Win32_Off_Screen_Buffer &Buffer, HDC DeviceContext,
					int WindowWidth, int WindowHeight)
{

	StretchDIBits(	DeviceContext,
		0, 0, WindowWidth, WindowHeight,
		0, 0, Buffer.Width, Buffer.Height,
		Buffer->Memory,
		&Buffer->Info,
		DIB_RGB_COLORS,
		SRCCOPY
		);

}

internal void 
Win32ResizeDIBSection(Win32_Off_Screen_Buffer *Buffer, int Width, int Height)
{

	if(Buffer->Memory)
	{
		VirtualFree(Buffer->Memory, NULL, MEM_RELEASE);
	}
	Buffer->Width = Width;
	Buffer->Height = Height;
	
	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biHeight = - Buffer->Height;
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = PIXEL_BIT_COUNT; //24 RGB and 8 for pading 
	Buffer->Info.bmiHeader.biCompression = BI_RGB;

	int BitMapMemSize = (Height*Width)*(BYTES_PER_PIXEL);
	
	Buffer->Memory = VirtualAlloc(NULL, BitMapMemSize, MEM_COMMIT, PAGE_READWRITE);
	
	
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
			HDC DeviceContext = GetDC(Window);
			uint8 XOffset = 0;
			uint8 YOffset = 0;
			
			GlobalRunning = true;
			while (GlobalRunning){
				MSG Message;
				while(PeekMessage( &Message,  0,0,0, PM_REMOVE)) 
				{
					if(Message.message == WM_QUIT)
					{
						GlobalRunning = false;
					}
					TranslateMessage(&Message);
					DispatchMessage(&Message);				
				}
				XINPUT_VIBRATION Vibration;
				for (DWORD ControllerIndex = 0; ControllerIndex< XUSER_MAX_COUNT; ControllerIndex++ )
				{
					XINPUT_STATE controlerState;
					ZeroMemory( &controlerState, sizeof(XINPUT_STATE) );

					// Simply get the controlerState of the controller from XInput.	

					if (XInputGetState(ControllerIndex, &controlerState) == ERROR_SUCCESS) // Controller is connected 
					{
						XINPUT_GAMEPAD *Pad = &controlerState.Gamepad;
						ActionButton actionButt = getButtonAction(Pad);
					/*	bool Up =	(Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
						bool Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
						bool Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
						bool Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
						bool ButA = (Pad->wButtons & XINPUT_GAMEPAD_A);
						bool ButB = (Pad->wButtons & XINPUT_GAMEPAD_B);
						bool ButX = (Pad->wButtons & XINPUT_GAMEPAD_X);
						bool ButY = (Pad->wButtons & XINPUT_GAMEPAD_Y);
						bool LeftSh = (Pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
						bool RigtSh = (Pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);

						int16 StickX = Pad->sThumbLX;
						int16 StickY = Pad->sThumbLY;
					*/
						Vibration.wLeftMotorSpeed = 0;
						Vibration.wLeftMotorSpeed = 0;
						
						if(actionButt.Up) 
						{
							YOffset++;
							Vibration.wLeftMotorSpeed = 1024;
							Vibration.wRightMotorSpeed = 1024;
							XInputSetState(0, &Vibration);
							
						}
						if(actionButt.Down) YOffset--;
						if (actionButt.Left)
						{
							Vibration.wLeftMotorSpeed = 1024;
							XOffset++;
							XInputSetState(0, &Vibration);
						}
						if (actionButt.Right){
							Vibration.wRightMotorSpeed = 1024;
							XOffset--;
							XInputSetState(0, &Vibration);
						}

						XInputSetState(0, &Vibration);
					}
					else
					{
					// Controller is not connected 
					}
				}
				Vibration.wRightMotorSpeed = 0;
				Vibration.wRightMotorSpeed = 0;

				XInputSetState(0, &Vibration);
				RenderWierdGradient(&GlobalBackBuffer, XOffset, YOffset);

				Win32_Window_Dimension Dimensiton = Win32GetWindowDimension(Window);
				Win32UpdateWindow(&GlobalBackBuffer, DeviceContext, Dimensiton.Width, Dimensiton.Height);



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
	Wind32LoadXInput();
	LRESULT Result = 0;
	Win32_Window_Dimension Dimension = Win32GetWindowDimension(Window);
	Win32ResizeDIBSection(&GlobalBackBuffer, 1200, 720);
	
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
			
			Win32_Window_Dimension Dimension = Win32GetWindowDimension(Window);
			RenderWierdGradient(GlobalBackBuffer, Width, Height);
			Win32UpdateWindow(DeviceContext, Dimension.Width, Dimension.Height, GlobalBackBuffer);
			EndPaint(Window, &Paint);
		} break;
		case WM_SIZE:
		{
			OutputDebugStringA("WM_SIZE\n");
		} break;
		case WM_DESTROY:
		{
			//TODO: This is a ERRO situation, deal with it!!
			GlobalRunning = false;
			OutputDebugStringA("WM_DESTROY\n");
		} break;
		case WM_CLOSE:
		{
			GlobalRunning = false;
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
