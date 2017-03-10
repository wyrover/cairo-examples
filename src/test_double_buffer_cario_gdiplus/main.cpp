// draw_01.cpp : main source file for draw_01.exe
//

#include "stdafx.h"

#include "resource.h"

#include "MainDlg.h"


class CTAppModule : public CAppModule
{
public:
	void Init(HINSTANCE hInstance)
	{
		GUID guid;
		__super::Init(NULL, hInstance, &guid);		
	}

	void Main()
	{	
		CMainDlg dlg;
		dlg.DoModal(NULL);
	}
	
};





CTAppModule g_module;

using namespace Gdiplus;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int /*nCmdShow*/)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	g_module.Init(hInstance);
	g_module.Main();

	GdiplusShutdown(gdiplusToken);
	return 0;
}
