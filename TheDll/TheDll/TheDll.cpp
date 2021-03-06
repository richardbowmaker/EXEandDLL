// TheDll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "TheDll.h"

#include <Windows.h>
#include <tchar.h>

#include "Scintilla.h"
#include "SciLexer.h"
#include "HeyHo_stub.h"


HWND hScn = NULL;

const COLORREF red = RGB(0xFF, 0, 0);
const COLORREF offWhite = RGB(0xFF, 0xFB, 0xF0);
const COLORREF darkGreen = RGB(0, 0x80, 0);
const COLORREF darkBlue = RGB(0, 0, 0x80);
const COLORREF black = RGB(0, 0, 0);
const COLORREF white = RGB(0xff, 0xff, 0xff);

struct MyStruct {
	double d;
	char c;
	int32_t i;
};

// This is an example of an exported function.
THEDLL_API int fnTheDll(HWND parent)
{
	HINSTANCE hLib = ::LoadLibrary(_T("SciLexer64.DLL"));

	HMODULE hMod = GetModuleHandle(NULL);

	HWND hScn = ::CreateWindow(
		_T("Scintilla"),
		_T("Source"),
		WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN,
		0, 0,
		100, 100,
		parent,
		0,

		hMod,
		0);

	ConfigureEditor();

	::ShowWindow(hScn, SW_SHOW);
	::SetWindowPos(hScn, parent, 100, 100, 800, 800, SWP_SHOWWINDOW);


    return 42;

}

THEDLL_API void* GetFnPtr()
{
	MyStruct* x = new MyStruct();
	x->d = 12.345;
	x->c = 0x30;
	x->i = 567;

	SCNotification* y = new SCNotification();

	y->nmhdr.hwndFrom	= (void*)0x9999999999999999;
	y->nmhdr.idFrom		= 0x8888888888888888;
	y->nmhdr.code		= 0xaaaaaaaa;

	y->position			= 0x1011111111111101;
	y->ch				= 0x22222222;
	y->modifiers		= 0x33333333;
	y->modificationType = 0x44444444;
	y->text				= (const char*)0x9999999999999999;
	y->length			= 0x1111111111111111;
	y->linesAdded		= 0x2222222222222222;
	y->message			= 0x33333333;
	y->wParam			= 0x8088888888888808;
	y->lParam			= 0x3333333333333333;
	y->line				= 0x4444444444444444;
	y->foldLevelNow		= 0x55555555;
	y->foldLevelPrev	= 0x44444444;
	y->margin			= 0x33333333;
	y->listType			= 0x22222222;
	y->x				= 0x11111111;
	y->y				= 0x77777777;
	y->token			= 0x66666666;
	y->annotationLinesAdded = 0x1111111111111111;
	y->updated				= 0x55555555;
	y->listCompletionMethod = 0x40444404;

	int z = sizeof(SCNotification);



	return (void*)y;


	//void* p = reinterpret_cast<void*>(&MyDouble);
	//return p;
}

int MyDouble(int n)
{
	return n * 2;
}

typedef int(*FnPtr)(int);

FnPtr thePtr = 0;

THEDLL_API void __cdecl SetFnPtr(void* fnPtr)
{
	thePtr = (FnPtr)fnPtr;
}

THEDLL_API int __cdecl UseFnPtr(int n)
{
	int n3 = (thePtr)(n);
	return n3;
}

HHOOK hhk;

LRESULT WINAPI MyHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
	{
		return CallNextHookEx(hhk, nCode, wParam, lParam);
	}

	LPCWPSTRUCT pData = reinterpret_cast<LPCWPSTRUCT>(lParam);


	TCHAR buff[100];
	_stprintf(buff, _T("hook: nCode: %x, msg: %x, wParam: %I64x, lParam: %I64x\n"), nCode, pData->message, pData->wParam, pData->lParam);
	OutputDebugString(buff);

	if (pData->message == WM_NOTIFY)
	{
		OutputDebugString(_T("got a notify !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"));
		int xx = UseFnPtr(4);

	}
	return CallNextHookEx(hhk, nCode, wParam, lParam);
}

THEDLL_API int __cdecl HookWindow()
{
	hhk = SetWindowsHookEx(WH_CALLWNDPROC, &MyHook, 0, ::GetCurrentThreadId());
	return hhk != NULL;
}

THEDLL_API void __cdecl UnhookWindow()
{
	UnhookWindowsHookEx(hhk);
}

LRESULT SendEditor(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)SendMessage(hScn, Msg, wParam, lParam);
}

void ConfigureEditor()
{
	SendEditor(SCI_SETLEXER, SCLEX_HTML);
	SendEditor(SCI_SETSTYLEBITS, 7);

	const TCHAR keyWords[] =
		_T("break case catch continue default ")
		_T("do else for function if return throw try var while");

	SendEditor(SCI_SETKEYWORDS, 0,
		reinterpret_cast<LPARAM>(keyWords));

	// Set up the global default style. These attributes are used wherever no explicit choices are made.
	SetAStyle(STYLE_DEFAULT, black, white, 9, _T("Consolas"));
	SendEditor(SCI_STYLECLEARALL);	// Copies global style to all others

	const COLORREF red = RGB(0xFF, 0, 0);
	const COLORREF offWhite = RGB(0xFF, 0xFB, 0xF0);
	const COLORREF darkGreen = RGB(0, 0x80, 0);
	const COLORREF darkBlue = RGB(0, 0, 0x80);

	// Hypertext default is used for all the document's text
	SetAStyle(SCE_H_DEFAULT, black, white, 9, _T("Consolas"));

	// Unknown tags and attributes are highlighed in red. 
	// If a tag is actually OK, it should be added in lower case to the htmlKeyWords string.
	SetAStyle(SCE_H_TAG, darkBlue);
	SetAStyle(SCE_H_TAGUNKNOWN, red);
	SetAStyle(SCE_H_ATTRIBUTE, darkBlue);
	SetAStyle(SCE_H_ATTRIBUTEUNKNOWN, red);
	SetAStyle(SCE_H_NUMBER, RGB(0x80, 0, 0x80));
	SetAStyle(SCE_H_DOUBLESTRING, RGB(0, 0x80, 0));
	SetAStyle(SCE_H_SINGLESTRING, RGB(0, 0x80, 0));
	SetAStyle(SCE_H_OTHER, RGB(0x80, 0, 0x80));
	SetAStyle(SCE_H_COMMENT, RGB(0x80, 0x80, 0));
	SetAStyle(SCE_H_ENTITY, RGB(0x80, 0, 0x80));

	SetAStyle(SCE_H_TAGEND, darkBlue);
	SetAStyle(SCE_H_XMLSTART, darkBlue);	// <?
	SetAStyle(SCE_H_XMLEND, darkBlue);		// ?>
	SetAStyle(SCE_H_SCRIPT, darkBlue);		// <script
	SetAStyle(SCE_H_ASP, RGB(0x4F, 0x4F, 0), RGB(0xFF, 0xFF, 0));	// <% ... %>
	SetAStyle(SCE_H_ASPAT, RGB(0x4F, 0x4F, 0), RGB(0xFF, 0xFF, 0));	// <%@ ... %>

	SetAStyle(SCE_HB_DEFAULT, black);
	SetAStyle(SCE_HB_COMMENTLINE, darkGreen);
	SetAStyle(SCE_HB_NUMBER, RGB(0, 0x80, 0x80));
	SetAStyle(SCE_HB_WORD, darkBlue);
	SendEditor(SCI_STYLESETBOLD, SCE_HB_WORD, 1);
	SetAStyle(SCE_HB_STRING, RGB(0x80, 0, 0x80));
	SetAStyle(SCE_HB_IDENTIFIER, black);

	// This light blue is found in the windows system palette so is safe to use even in 256 colour modes.
	const COLORREF lightBlue = RGB(0xA6, 0xCA, 0xF0);
	// Show the whole section of VBScript with light blue background
	for (int bstyle = SCE_HB_DEFAULT; bstyle <= SCE_HB_STRINGEOL; bstyle++)
	{
		SendEditor(SCI_STYLESETFONT, bstyle,
			reinterpret_cast<LPARAM>("Georgia"));
		SendEditor(SCI_STYLESETBACK, bstyle, lightBlue);
		// This call extends the backround colour of the last style on the line to the edge of the window
		SendEditor(SCI_STYLESETEOLFILLED, bstyle, 1);
	}
	SendEditor(SCI_STYLESETBACK, SCE_HB_STRINGEOL, RGB(0x7F, 0x7F, 0xFF));
	SendEditor(SCI_STYLESETFONT, SCE_HB_COMMENTLINE,
		reinterpret_cast<LPARAM>("Comic Sans MS"));

	SetAStyle(SCE_HBA_DEFAULT, black);
	SetAStyle(SCE_HBA_COMMENTLINE, darkGreen);
	SetAStyle(SCE_HBA_NUMBER, RGB(0, 0x80, 0x80));
	SetAStyle(SCE_HBA_WORD, darkBlue);
	SendEditor(SCI_STYLESETBOLD, SCE_HBA_WORD, 1);
	SetAStyle(SCE_HBA_STRING, RGB(0x80, 0, 0x80));
	SetAStyle(SCE_HBA_IDENTIFIER, black);

	// Show the whole section of ASP VBScript with bright yellow background
	for (int bastyle = SCE_HBA_DEFAULT; bastyle <= SCE_HBA_STRINGEOL; bastyle++)
	{
		SendEditor(SCI_STYLESETFONT, bastyle,
			reinterpret_cast<LPARAM>("Georgia"));
		SendEditor(SCI_STYLESETBACK, bastyle, RGB(0xFF, 0xFF, 0));
		// This call extends the backround colour of the last style on the line to the edge of the window
		SendEditor(SCI_STYLESETEOLFILLED, bastyle, 1);
	}
	SendEditor(SCI_STYLESETBACK, SCE_HBA_STRINGEOL, RGB(0xCF, 0xCF, 0x7F));
	SendEditor(SCI_STYLESETFONT, SCE_HBA_COMMENTLINE,
		reinterpret_cast<LPARAM>("Comic Sans MS"));


}

void SetAStyle(int style, COLORREF fore, COLORREF back, int size, const TCHAR *face)
{
	SendEditor(SCI_STYLESETFORE, style, fore);
	SendEditor(SCI_STYLESETBACK, style, back);
	if (size >= 1)
		SendEditor(SCI_STYLESETSIZE, style, size);
	if (face)
		SendEditor(SCI_STYLESETFONT, style, reinterpret_cast<LPARAM>(face));
}

