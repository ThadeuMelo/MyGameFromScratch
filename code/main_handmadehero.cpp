//This is my first game from scratch!!

#include <windows.h>
#include <stdint.h>
#include <xinput.h>
#include <dsound.h>

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
};

global_variable Win32_Off_Screen_Buffer GlobalBackBuffer;
global_variable LPDIRECTSOUNDBUFFER SecondaryBuffer;

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

#define DIRECT_SOUND_CREATE(name)_Check_return_ HRESULT WINAPI name(_In_opt_ LPCGUID pcGuidDevice, _Outptr_ LPDIRECTSOUND *ppDS, _Pre_null_ LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);


LRESULT CALLBACK Win32MainWindowCallBack(
	HWND	Window,
	UINT	Message,
	WPARAM	WParam,
	LPARAM	LParam
	);
	
LRESULT Win32CreateInitialWindow(HINSTANCE Instance);

internal void Win32ResizeDIBSection(Win32_Off_Screen_Buffer *Buffer, int, int);

internal void Win32UpdateWindow(Win32_Off_Screen_Buffer *Buffer, HDC DeviceContext,
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
		XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
		XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
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
Win32InitDSound(HWND Window, int32 sampesPerSecond, int32 BufferSize)
{
	//Load Direct Sound Library
	HMODULE DSoundLibrary = LoadLibrary("dsound.dll");

	if (DSoundLibrary)
	{
		//Get a DirectSound object!
		direct_sound_create *DirectSoundCreate = (direct_sound_create *) GetProcAddress(DSoundLibrary, "DirectSoundCreate");

		LPDIRECTSOUND DirectSound;
		if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0)))
		{
			WAVEFORMATEX WaveFormat = {};
			WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
			WaveFormat.nChannels = 2;
			WaveFormat.nSamplesPerSec = sampesPerSecond;
			WaveFormat.nBlockAlign = (WaveFormat.nChannels*WaveFormat.wBitsPerSample) / 8;
			WaveFormat.nAvgBytesPerSec = WaveFormat.nBlockAlign*WaveFormat.nBlockAlign;
			WaveFormat.wBitsPerSample = 16;
			WaveFormat.cbSize;

			if (SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY)))
			{
				DSBUFFERDESC BufferDescription = {};
				BufferDescription.dwSize = sizeof(BufferDescription);
				BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
				LPDIRECTSOUNDBUFFER PrimaryBuffer;
				if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0)))
				{

					if (SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat)))
					{

					}
					else
					{

					}
				}
			}

			DSBUFFERDESC BufferDescription = {};
			BufferDescription.dwSize = sizeof(BufferDescription);
			BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
			BufferDescription.dwBufferBytes = BufferSize;
			BufferDescription.lpwfxFormat = &WaveFormat;


			if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &SecondaryBuffer, 0)))
			{

				if (SUCCEEDED(SecondaryBuffer->SetFormat(&WaveFormat)))
				{
				}
			}
			else
			{

			}
			//"Create" a primary buffer

			//"Create" a secundary buffer

			//Start it playing
		}
		else
		{
			//TODO log errors here
		}
	}
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
Win32UpdateWindow( Win32_Off_Screen_Buffer *Buffer, HDC DeviceContext,
					int WindowWidth, int WindowHeight)
{

	StretchDIBits(	DeviceContext,
		0, 0, WindowWidth, WindowHeight,
		0, 0, Buffer->Width, Buffer->Height,
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
			int samplesPersecond = 48000;
			int squareWaveCounter = 0;

			int Hz = 440;
			int squareWavePeriod = 48000/440;
			int bytesPersample =  sizeof(int16) * 2;

			Win32InitDSound(Window, samplesPersecond, samplesPersecond *bytesPersample);
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
						ButtonActions actionButt = getButtonAction(Pad);
					
						Vibration.wLeftMotorSpeed = 0;
						Vibration.wLeftMotorSpeed = 0;
						
						if(actionButt.Up) 
						{
							YOffset++;
							Vibration.wLeftMotorSpeed = 6024;
							Vibration.wRightMotorSpeed = 6024;
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

				XInputSetState(0, &Vibration);

				RenderWierdGradient(&GlobalBackBuffer, XOffset, YOffset);
				DWORD writerPointer;
				DWORD bytesToWrite;
				VOID *region1;
				DWORD region1Size;
				VOID *region2;
				DWORD region2Size;
				DWORD resion1SampleCounter = region1Size / bytesPersample;
				SecondaryBuffer->Lock(writerPointer, bytesToWrite, &region1, &region1Size, &region2, &region2Size,0);

				int16 *sampleOut = (int16 *)region1;
				for (DWORD sampleIndex = 0; sampleIndex < resion1SampleCounter; ++sampleIndex)
				{
					if (squareWaveCounter)
					{
						squareWaveCounter = squareWavePeriod;

					}
					int16 sampleValue = (squareWaveCounter > (squareWavePeriod/2))?10000:-10000;
					*sampleOut++ = sampleValue;
					*sampleOut++ = sampleValue;
					squareWaveCounter--;
				}

				DWORD resion2SampleCounter = region2Size / bytesPersample;

				for (DWORD sampleIndex = 0; sampleIndex < resion2SampleCounter; ++sampleIndex)
				{
					if (squareWaveCounter)
					{
						squareWaveCounter = squareWavePeriod;

					}
					int16 sampleValue = (squareWaveCounter >(squareWavePeriod / 2)) ? 10000 : -10000;
					*sampleOut++ = sampleValue;
					*sampleOut++ = sampleValue;
					squareWaveCounter--;
				}

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
			RenderWierdGradient(&GlobalBackBuffer, Width, Height);
			Win32UpdateWindow(&GlobalBackBuffer, DeviceContext, Dimension.Width, Dimension.Height);
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
