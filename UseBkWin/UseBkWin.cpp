#include "stdafx.h"
#include "resource.h"
#include "bkwinres.h"
#include "HelpFuncion.h"
#include "MainDlg.h"

CAppModule _Module;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HANDLE hMutex;
	hMutex				= CreateMutex(NULL, FALSE, _T("DFEC13D6-424A-479D-8B5D-2C7D31D9F1AC"));
	if ( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		MessageBox(NULL, _T("已经有一个实例正在运行."), _T("提示"), MB_OK);
		return -1;
	}

	HRESULT hRes = ::CoInitialize(NULL);

	ATLASSERT(SUCCEEDED(hRes));

	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_BAR_CLASSES);

	hRes = _Module.Init(NULL, hInstance);

	ATLASSERT(SUCCEEDED(hRes));

	// multi language
	DWORD dwType = GetLangType();
	if ( dwType != 1000 )
	{
		switch ( dwType )
		{
		case 1:		// chinese
			BkString::Load( IDR_BK_STRING_DEF );
			SetLangType( 1 );
			break;
		case 2:		// english
			BkString::Load( IDR_BK_STRING_EN );
			SetLangType( 2 );
			break;
		default:	// chinese
			BkString::Load( IDR_BK_STRING_DEF );
			SetLangType( 1 );
			break;
		}
	}
	else
	{
		BkString::Load( IDR_BK_STRING_DEF );
		SetLangType( 1 );
	}
	
	BkFontPool::SetDefaultFont(BkString::Get(IDS_APP_FONT), -12);

	BkSkin::LoadSkins(IDR_BK_SKIN_DEF);
	
	BkStyle::LoadStyles(IDR_BK_STYLE_DEF);

	CMainDlg dlgMain;
	if ( dwType != -1 )
	{
		switch ( dwType )
		{
		case 1:		// chinese
			dlgMain.Load( IDR_BK_MAIN_DIALOG );
			SetLangType( 1 );
			break;
		case 2:		// english
			dlgMain.Load( IDR_BK_MAIN_DIALOG_EN );
			SetLangType( 2 );
			break;
		default:	// chinese
			dlgMain.Load( IDR_BK_MAIN_DIALOG );
			SetLangType( 1 );
			break;
		}
	}
	
	dlgMain.DoModal();
	/*int nRet = Run(lpstrCmdLine, nCmdShow);*/

	_Module.Term();

	::CoUninitialize();

	CloseHandle( hMutex );

	return 0;
}
