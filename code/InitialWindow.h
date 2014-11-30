#include <windows.h>

LRESULT CALLBACK MainWindowCallBack(
	HWND	Window,
	UINT	Message,
	WPARAM	WParam,
	LPARAM	LParam
	);
	
LRESULT CreateInitialWindow(HINSTANCE Instance);