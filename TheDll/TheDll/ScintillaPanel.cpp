#include "ScintillaPanel.h"
#include "Scintilla.h"
#include "SciLexer.h"

#include "wx/menu.h"
#include "wx/toolbar.h"
#include "wx/statusbr.h"

#include <wx/wx.h>

const COLORREF red = RGB(0xFF, 0, 0);
const COLORREF offWhite = RGB(0xFF, 0xFB, 0xF0);
const COLORREF darkGreen = RGB(0, 0x80, 0);
const COLORREF darkBlue = RGB(0, 0, 0x80);
const COLORREF black = RGB(0, 0, 0);
const COLORREF white = RGB(0xff, 0xff, 0xff);

HINSTANCE CScintillaPanel::m_hLibrary = 0;

CScintillaPanel::CScintillaPanel(wxWindow *parent,
	wxWindowID winid,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& name)
	: wxPanel(parent, winid, pos, size, style, name)
{
	m_pParent = parent;
	m_fn = NULL;
	m_ptr = NULL;
	m_handler = NULL;
}

CScintillaPanel::~CScintillaPanel()
{
}

bool CScintillaPanel::Initialise(HINSTANCE app)
{
	m_app = app;

	// load DLL
	if (m_hLibrary == 0)
	{
		m_hLibrary = ::LoadLibrary(_T("SciLexer.DLL"));
	}
	assert(m_hLibrary);

	m_scintilla = ::CreateWindow(
		_T("Scintilla"),
		_T("Source"),
		WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN,
		0, 0,
		100, 100,
		GetHWND(),
		0,
		m_app,
		0);

	// get direct call function pointer
	m_fn = (int(__cdecl *)(void *, int, int, int))SendMessage(m_scintilla, SCI_GETDIRECTFUNCTION, 0, 0);
	m_ptr = (void *)SendMessage(m_scintilla, SCI_GETDIRECTPOINTER, 0, 0);

	ConfigureEditor();

	// event handlers
	Bind(wxEVT_SIZE, &CScintillaPanel::OnSize, this);

	return TRUE;
}

void CScintillaPanel::ConfigureEditor()
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

void CScintillaPanel::SetAStyle(int style, COLORREF fore, COLORREF back, int size, const TCHAR *face) 
{
	SendEditor(SCI_STYLESETFORE, style, fore);
	SendEditor(SCI_STYLESETBACK, style, back);
	if (size >= 1)
		SendEditor(SCI_STYLESETSIZE, style, size);
	if (face)
		SendEditor(SCI_STYLESETFONT, style, reinterpret_cast<LPARAM>(face));
}

void CScintillaPanel::OnSize(wxSizeEvent& event)
{
	// We need to update the position as well since changing the parent does not
	// adjust it automatically.
	SetWindowPos(m_scintilla, NULL, 0, 0, event.GetSize().x, event.GetSize().y, SWP_SHOWWINDOW);
}

LRESULT CScintillaPanel::SendEditor(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	return m_fn(m_ptr, Msg, wParam, lParam);
}

void CScintillaPanel::AddText(const wxString& text)
{
	char buf[100];
	strcpy(buf, (const char*)text.mb_str(wxConvUTF8));

	SendEditor(SCI_ADDTEXT, text.Len(), (LPARAM)buf);
}

void CScintillaPanel::SetModEventMask(int mask)
{
	SendEditor(SCI_SETMODEVENTMASK, (WPARAM)mask);
}

int CScintillaPanel::GetModEventMask()
{
	return SendEditor(SCI_GETMODEVENTMASK);
}

bool CScintillaPanel::MSWHandleMessage(WXLRESULT *result,
	WXUINT message,
	WXWPARAM wParam,
	WXLPARAM lParam)
{
	bool b = wxPanel::MSWHandleMessage(result, message, wParam, lParam);

	if (message == WM_NOTIFY)
	{
		LPNMHDR lpnmhdr = (LPNMHDR)lParam;

		if (lpnmhdr->hwndFrom == m_scintilla)
		{
			SCNotification* scn = (SCNotification*)lParam;

			if (m_handler != NULL)
			{
				(m_handler)(lpnmhdr->code, scn);
			}

			switch (lpnmhdr->code)
			{

			case SCN_CHARADDED:
				/* Hey, Scintilla just told me that a new */
				/* character was added to the Edit Control.*/
				/* Now i do something cool with that char. */

				break;
			}
		}
	}

	return b;
}

void CScintillaPanel::SetNotificationHandler(ScintillaNotify handler)
{
	m_handler = handler;
}

wxString CScintillaPanel::GetAllText()
{
	int len = (int)SendEditor(SCI_GETLENGTH);
	char* buff = (char*)malloc(len + 1);
	int len1 = SendEditor(SCI_GETTEXT, (WPARAM)len + 1, (LPARAM)buff);
	wxString str(buff);
	return str;

}


