//This is my first game from scratch!!


#include "handmadehero.cpp"

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


struct Win32_output_sound
{
	int samplesPersecond = 48000;
	int waveCounter = 0;
	int sTone = 120;
	int wavePeriod = samplesPersecond / sTone;
	int bytesPersample = sizeof(int16) * 2;
	int SecondaryBufferSize = samplesPersecond*bytesPersample;
	uint32 runningSampleIndex = 0;
	real32 tSine;
	int latancySampleCount = samplesPersecond / 60;

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

float platformImpl(char *value)
{
	OutputDebugStringA("This is Windows32 \n");
	return 2;
}

internal void
Win32ProcessXInputDigitalButton(DWORD XInputButtonState,
                                game_button_state *OldState, DWORD ButtonBit,
                                game_button_state *NewState)
{
    NewState->EndedDown = ((XInputButtonState & ButtonBit) == ButtonBit);
    NewState->HalfTransitionCount = (OldState->EndedDown != NewState->EndedDown) ? 1 : 0;
}


internal ButtonActions WIN32_getButtonAction(DWORD wButtons)
{
	ButtonActions tempButtAct;
	tempButtAct.Up 	=	(wButtons & XINPUT_GAMEPAD_DPAD_UP);
	tempButtAct.Down = (wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
	tempButtAct.Left = (wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
	tempButtAct.Right = (wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
	tempButtAct.ButA = (wButtons & XINPUT_GAMEPAD_A);
	tempButtAct.ButB = (wButtons & XINPUT_GAMEPAD_B);
	tempButtAct.ButX = (wButtons & XINPUT_GAMEPAD_X);
	tempButtAct.ButY = (wButtons & XINPUT_GAMEPAD_Y);
	tempButtAct.LeftSh = (wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
	tempButtAct.RigtSh = (wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
	return tempButtAct;
}


internal void
Win32ClearSoundBuffer(Win32_output_sound *soundOutput)
{
	VOID *region1;
	DWORD region1Size;
	VOID *region2;
	DWORD region2Size;
	if (SUCCEEDED(SecondaryBuffer->Lock(0, soundOutput->SecondaryBufferSize, &region1, &region1Size, &region2, &region2Size, 0)))
	{
		uint8 *sampleOut = (uint8 *)region1;
		for (DWORD sampleIndex = 0; sampleIndex < region1Size; ++sampleIndex)
		{
			*sampleOut++ = 0;
		}

		sampleOut = (uint8*)region2;
		for (DWORD sampleIndex = 0; sampleIndex < region2Size; ++sampleIndex)
		{
			*sampleOut++ = 0;
		}
	}
	SecondaryBuffer->Unlock(region1, region1Size, region2, region2Size);
}

internal void
Win32FillSoundBuffer(Win32_output_sound *soundOutput, game_sound_output_buffer *SourceBuffer,   DWORD bytesToLock, DWORD bytesToWrite)
{
	VOID *region1;
	DWORD region1Size;
	VOID *region2;
	DWORD region2Size;
	
	if (SUCCEEDED(SecondaryBuffer->Lock(bytesToLock, bytesToWrite, &region1, &region1Size, &region2, &region2Size, 0)))
	{
		DWORD region1SampleCounter = region1Size / soundOutput->bytesPersample;
		int16 *sampleOut = (int16 *)region1;
		int16 *sampleIn = SourceBuffer->samples;
		for (DWORD sampleIndex = 0; sampleIndex < region1SampleCounter; ++sampleIndex)
		{
			*sampleOut++ = *sampleIn++;
			*sampleOut++ = *sampleIn++;
			soundOutput->runningSampleIndex++;

		}
	
		DWORD region2SampleCounter = region2Size / soundOutput->bytesPersample;
		sampleOut = (int16 *)region2;

		for (DWORD sampleIndex = 0; sampleIndex < region2SampleCounter; ++sampleIndex)
		{
			*sampleOut++ = *sampleIn++;
			*sampleOut++ = *sampleIn++;
			soundOutput->runningSampleIndex++;
		}
	}
	SecondaryBuffer->Unlock(region1, region1Size, region2, region2Size);

}
internal void 
Wind32LoadXInput(void)
{
	HMODULE XInputLibrary = LoadLibraryA("xinput1_4.dll");
	if(!XInputLibrary)
	{
		XInputLibrary = LoadLibraryA("xinput1_3.dll");
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
Win32InitDSound(HWND Window, int32 samplesPerSecond, int32 BufferSize)
{
	//Load Direct Sound Library
	HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");

	if (DSoundLibrary)
	{
		//Get a DirectSound object!
		direct_sound_create *DirectSoundCreate = (direct_sound_create *) GetProcAddress(DSoundLibrary, "DirectSoundCreate");

		LPDIRECTSOUND DirectSound = {};
		HRESULT Error = DirectSoundCreate(0, &DirectSound, 0);
		if (DirectSoundCreate && SUCCEEDED(Error))
		{
			WAVEFORMATEX WaveFormat = {};
			WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
			WaveFormat.nChannels = 2;
			WaveFormat.nSamplesPerSec = samplesPerSecond;
			WaveFormat.wBitsPerSample = 16;
			WaveFormat.nBlockAlign = (WaveFormat.nChannels*WaveFormat.wBitsPerSample) / 8;
			WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;

			if (SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY)))
			{
				DSBUFFERDESC BufferDescription = {};
				BufferDescription.dwSize = sizeof(BufferDescription);
				BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
				LPDIRECTSOUNDBUFFER PrimaryBuffer;
				
				if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0)))
				{
					HRESULT Error_cd = PrimaryBuffer->SetFormat(&WaveFormat);
					if (SUCCEEDED(Error_cd))
					{
						OutputDebugStringA("PrimaryBuffer set");
					}
					else
					{
						OutputDebugStringA("PrimaryBuffer not set");
					}
				}
			}

			DSBUFFERDESC BufferDescription = {};
			BufferDescription.dwSize = sizeof(BufferDescription);
			BufferDescription.dwFlags = 0;
			BufferDescription.dwBufferBytes = BufferSize;
			BufferDescription.lpwfxFormat = &WaveFormat;

			HRESULT Error_cd = DirectSound->CreateSoundBuffer(&BufferDescription, &SecondaryBuffer, 0);
			if (SUCCEEDED(Error_cd))
			{
				OutputDebugStringA("Secondary set");
			}
			else
			{
				OutputDebugStringA("Secondary not set");
			}

		}
		else
		{
			//TODO log errors here
		}
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
	WNDCLASSA WindowClass = {};
	//TODO(casey) : Check if

	WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	WindowClass.lpfnWndProc = Win32MainWindowCallBack;
	WindowClass.hInstance = Instance;
	WindowClass.lpszClassName = "HandmadeHeroWindowClass";

	LARGE_INTEGER performanceFrequencyResult;
	QueryPerformanceFrequency(&performanceFrequencyResult);

	int64 perfFreqCount = performanceFrequencyResult.QuadPart;

	if (RegisterClassA(&WindowClass))
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
			uint16 sTone = 440;
			Win32_output_sound soundOutput = {};
			
			soundOutput.samplesPersecond = 48000;
            soundOutput.bytesPersample = sizeof(int16)*2;
            soundOutput.SecondaryBufferSize = soundOutput.samplesPersecond*soundOutput.bytesPersample;
            soundOutput.latancySampleCount = soundOutput.samplesPersecond / 15;
			
			Win32InitDSound(Window, soundOutput.samplesPersecond, soundOutput.SecondaryBufferSize);
			Win32ClearSoundBuffer(&soundOutput);
			
			GlobalRunning = true;
			BOOL isSoundPlaying = false;
			
			int16 *Samples = (int16 *)VirtualAlloc(0, soundOutput.SecondaryBufferSize,
                                                   MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
			
			game_input Input[2] = {};
            game_input *NewInput = &Input[0];
            game_input *OldInput = &Input[1];

			LARGE_INTEGER begCounter; // Starting the clock
			QueryPerformanceCounter(&begCounter);
			
			int64 LastCycleCount =  __rdtsc();

			while (GlobalRunning){

				mainGameLoop();

				begCounter.QuadPart;
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
				ButtonActions actionButt;				
				
				int MaxControllerCount = XUSER_MAX_COUNT;
                if(MaxControllerCount > ArrayCount(NewInput->Controllers))
                {
                    MaxControllerCount = ArrayCount(NewInput->Controllers);
                }
				
				for (DWORD ControllerIndex = 0; ControllerIndex< XUSER_MAX_COUNT; ControllerIndex++ )
				{
					game_controller_input *OldController = &OldInput->Controllers[ControllerIndex];
                    game_controller_input *NewController = &NewInput->Controllers[ControllerIndex];
                    
                    XINPUT_STATE ControllerState;
                    if(XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
                    {
                        // NOTE(casey): This controller is plugged in
                        // TODO(casey): See if ControllerState.dwPacketNumber increments too rapidly
                        XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;

                        // TODO(casey): DPad
                        bool Up = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                        bool Down = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                        bool Left = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                        bool Right = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);

                        NewController->IsAnalog = true;
                        NewController->StartX = OldController->EndX;
                        NewController->StartY = OldController->EndY;

                        // TODO(casey): Dead zone processing!!
                        // XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
                        // XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 

                        // TODO(casey): Min/Max macros!!!
                        // TODO(casey): Collapse to single function
                        real32 X;
                        if(Pad->sThumbLX < 0)
                        {
                            X = (real32)Pad->sThumbLX / 32768.0f;
                        }
                        else
                        {
                            X = (real32)Pad->sThumbLX / 32767.0f;
                        }
                        NewController->MinX = NewController->MaxX = NewController->EndX = X;

                        real32 Y;
                        if(Pad->sThumbLY < 0)
                        {
                            Y = (real32)Pad->sThumbLY / 32768.0f;
                        }
                        else
                        {
                            Y = (real32)Pad->sThumbLY / 32767.0f;
                        }
                        NewController->MinY = NewController->MaxY = NewController->EndY = Y;

                        Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                        &OldController->Down, XINPUT_GAMEPAD_A,
                                                        &NewController->Down);
                        Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                        &OldController->Right, XINPUT_GAMEPAD_B,
                                                        &NewController->Right);
                        Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                        &OldController->Left, XINPUT_GAMEPAD_X,
                                                        &NewController->Left);
                        Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                        &OldController->Up, XINPUT_GAMEPAD_Y,
                                                        &NewController->Up);
                        Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                        &OldController->LeftShoulder, XINPUT_GAMEPAD_LEFT_SHOULDER,
                                                        &NewController->LeftShoulder);
                        Win32ProcessXInputDigitalButton(Pad->wButtons,
                                                        &OldController->RightShoulder, XINPUT_GAMEPAD_RIGHT_SHOULDER,
                                                        &NewController->RightShoulder);

                        // bool32 Start = (Pad->wButtons & XINPUT_GAMEPAD_START);
                        // bool32 Back = (Pad->wButtons & XINPUT_GAMEPAD_BACK);
					}
					else
					{
					// Controller is not connected 
					}
				}

				DWORD playCursor = 0;
				DWORD writeCursor = 0;
				DWORD bytesToLock = 0;
				DWORD bytesToWrite = 0;
				DWORD targetCursor = 0;
				bool isSoundValid = false;
				if (SUCCEEDED(SecondaryBuffer->GetCurrentPosition(&playCursor, &writeCursor)))
				{
					bytesToLock = (soundOutput.runningSampleIndex*soundOutput.bytesPersample)%soundOutput.SecondaryBufferSize;
					targetCursor = (playCursor +
										soundOutput.latancySampleCount*soundOutput.bytesPersample) 
										% soundOutput.SecondaryBufferSize;

					if (bytesToLock > targetCursor)
					{
						bytesToWrite = soundOutput.SecondaryBufferSize - bytesToLock;
						bytesToWrite += targetCursor;
					}
					else
					{
						bytesToWrite = targetCursor - bytesToLock;
					}
					isSoundValid = true;
				}

				game_sound_output_buffer SoundBuffer ={};

				SoundBuffer.samplesPerSecond = soundOutput.samplesPersecond ;
				SoundBuffer.sampleCount = bytesToWrite/soundOutput.bytesPersample;
				SoundBuffer.samples = Samples;

				game_Off_Screen_Buffer GameBuffer = {};
				
				GameBuffer.Memory = GlobalBackBuffer.Memory;
				GameBuffer.Width = GlobalBackBuffer.Width;
				GameBuffer.Height = GlobalBackBuffer.Height;
				GameBuffer.Pitch = GlobalBackBuffer.Pitch;
				
				GameUpdateAndRander(NewInput, &GameBuffer, &SoundBuffer);
				
				if (isSoundValid)
				{
					Win32FillSoundBuffer(&soundOutput, &SoundBuffer, bytesToLock, bytesToWrite);

				}
				if (!isSoundPlaying)
				{
					SecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
				
					isSoundPlaying = true;
				}
				Win32_Window_Dimension Dimensiton = Win32GetWindowDimension(Window);
				Win32UpdateWindow(&GlobalBackBuffer, DeviceContext, Dimensiton.Width, Dimensiton.Height);

				int64 endCycleCount =  __rdtsc();
				int64 elapsedCycleCount =  endCycleCount - LastCycleCount;
				
				LARGE_INTEGER endCounter;
				QueryPerformanceCounter(&endCounter);
				
				int64 elapsedCounter = endCounter.QuadPart - begCounter.QuadPart; // how much time has passed in the running cycle
				int64 milSecPerFrem = (1000*elapsedCounter )/ perfFreqCount;
				int32 MCPF = (int32)(elapsedCycleCount/(1000*1000)); //Mega cycles per frame
				int32 FPS = perfFreqCount/elapsedCounter;
				char strBuffer[264];
				wsprintfA(strBuffer, "Milisecond/Frame = %d\n _ FPS = %d _ MC/F = %d\n", milSecPerFrem, FPS, MCPF );
				OutputDebugStringA(strBuffer);
				begCounter = endCounter;
				LastCycleCount = endCycleCount;
				
				game_input *Temp = NewInput;
                NewInput = OldInput;
                OldInput = Temp;
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
			//RenderWierdGradient(&GlobalBackBuffer, Width, Height);
			
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
