#include "stdafx.h"
#include "HelpFuncion.h"

void TcpStatus2String(DWORD status, wchar_t* szStatus)
{
	switch ( status )
	{
	case MIB_TCP_STATE_CLOSED:
		wsprintf(szStatus, _T("%s"), _T("CLOSED"));
		break;
	case MIB_TCP_STATE_LISTEN:
		wsprintf(szStatus, _T("%s"), _T("LISTEN"));
		break;
	case MIB_TCP_STATE_SYN_SENT:
		wsprintf(szStatus, _T("%s"), _T("SYN-SENT"));
		break;
	case MIB_TCP_STATE_SYN_RCVD:
		wsprintf(szStatus, _T("%s"), _T("SYN-RECEIVED"));
		break;
	case MIB_TCP_STATE_ESTAB:
		wsprintf(szStatus, _T("%s"), _T("ESTABLISHED"));
		break;
	case MIB_TCP_STATE_FIN_WAIT1:
		wsprintf(szStatus, _T("%s"), _T("FIN-WAIT-1"));
		break;
	case MIB_TCP_STATE_FIN_WAIT2:
		wsprintf(szStatus, _T("%s"), _T("FIN-WAIT-2"));
		break;
	case MIB_TCP_STATE_CLOSE_WAIT:
		wsprintf(szStatus, _T("%s"), _T("CLOSE-WAIT"));
		break;
	case MIB_TCP_STATE_CLOSING:
		wsprintf(szStatus, _T("%s"), _T("CLOSING"));
		break;
	case MIB_TCP_STATE_LAST_ACK:
		wsprintf(szStatus, _T("%s"), _T("LAST-ACK"));
		break;
	case MIB_TCP_STATE_TIME_WAIT:
		wsprintf(szStatus, _T("%s"), _T("TIME-WAIT"));
		break;
	case MIB_TCP_STATE_DELETE_TCB:
		wsprintf(szStatus, _T("%s"), _T("TCB"));
		break;
	default:
		wsprintf(szStatus, _T("%s"), _T("ERROR"));
		break;
	}
}

void SetLangType(DWORD dwLangType)
{
	HKEY	hkey;
	HKEY	hKeyLocation;
	LONG	ret;
	ULONG	lpdwDispositidon;
	CHAR	szSubKey[MAX_PATH];

	sprintf_s(szSubKey, "%s", "Software\\NetTraffic");

	ret		= RegOpenKeyExA(HKEY_CURRENT_USER, szSubKey, 0, KEY_CREATE_SUB_KEY | KEY_QUERY_VALUE | KEY_SET_VALUE, &hkey);

	ret		= RegCreateKeyExA(HKEY_CURRENT_USER, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | KEY_QUERY_VALUE | KEY_SET_VALUE, NULL, &hKeyLocation, &lpdwDispositidon);

	if ( ret == ERROR_SUCCESS )
	{
		ret = RegSetValueExA(hKeyLocation, "LANG", 0, REG_DWORD, (BYTE*)&dwLangType, sizeof(dwLangType));
	}
	RegCloseKey(hkey);
	RegCloseKey(hKeyLocation);
}

DWORD GetLangType()
{
	HKEY	hkey;
	HKEY	hKeyLocation;
	LONG	ret;
	ULONG	lpdwDispositidon;
	CHAR	szSubKey[MAX_PATH];

	DWORD	dwLangType;
	DWORD	dwSize;


	dwLangType  = 1000;
	dwSize		= sizeof(dwLangType);

	sprintf_s(szSubKey, "%s", "Software\\NetTraffic");

	ret		= RegOpenKeyExA(HKEY_CURRENT_USER, szSubKey, 0, KEY_CREATE_SUB_KEY | KEY_QUERY_VALUE | KEY_SET_VALUE, &hkey);

	ret		= RegCreateKeyExA(HKEY_CURRENT_USER, szSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_CREATE_SUB_KEY | KEY_QUERY_VALUE | KEY_SET_VALUE, NULL, &hKeyLocation, &lpdwDispositidon);

	if ( ret == ERROR_SUCCESS )
	{
		ret = RegQueryValueExA(hKeyLocation, "LANG", 0, NULL, (BYTE*)&dwLangType, &dwSize);
	}
	RegCloseKey(hkey);
	RegCloseKey(hKeyLocation);

	return dwLangType;
}


void SetAutoRun(BOOL bAutoRun)
{
	CHAR		szFileName[MAX_PATH];
	HKEY		hKeyAutoStart;

	::GetModuleFileNameA(::GetModuleHandle(NULL), szFileName, MAX_PATH);
	LONG ret = RegOpenKeyExA(
		HKEY_CURRENT_USER, 
		"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		0, 
		KEY_ALL_ACCESS, 
		&hKeyAutoStart);
	if ( ret != ERROR_SUCCESS )
	{
		MessageBox(NULL, _T("开机启动失败!请检查权限，是否被杀毒软件拦截."), _T("提示"), MB_OK);
		return;
	}
	if ( bAutoRun )
	{
		ret = RegSetValueExA(hKeyAutoStart, "NetTraffic", NULL, REG_SZ, (const BYTE*)szFileName, strlen(szFileName) + 1);

		if ( ret != ERROR_SUCCESS )
		{
			MessageBox(NULL, _T("开机启动失败!请检查权限，是否被杀毒软件拦截."), _T("提示"), MB_OK);
			return;
		}
		ret = RegFlushKey(HKEY_CURRENT_USER);

		if ( ret != ERROR_SUCCESS )
		{
			MessageBox(NULL, _T("开机启动失败!请检查权限，是否被杀毒软件拦截."), _T("提示"), MB_OK);
			return;
		}
	}
	else
	{
		RegDeleteValue(hKeyAutoStart, TEXT("NetTraffic"));
		RegFlushKey(HKEY_CURRENT_USER);
	}
}