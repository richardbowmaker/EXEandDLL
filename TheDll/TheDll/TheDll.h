// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the THEDLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// THEDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef THEDLL_EXPORTS
#define THEDLL_API __declspec(dllexport)
#else
#define THEDLL_API __declspec(dllimport)
#endif

extern "C"
{
	THEDLL_API int __cdecl fnTheDll(HWND parent);
	THEDLL_API void* __cdecl GetFnPtr();
	THEDLL_API void __cdecl SetFnPtr(void* fnPtr);
	THEDLL_API int __cdecl UseFnPtr(int n);
	THEDLL_API int __cdecl HookWindow();
	THEDLL_API void __cdecl UnhookWindow();
}

LRESULT SendEditor(UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0);

void SetAStyle(int style, COLORREF fore, COLORREF back = RGB(255, 255, 255), int size = -1, const TCHAR *face = 0);
void ConfigureEditor();

int MyDouble(int n);