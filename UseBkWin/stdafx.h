#pragma once
#pragma warning(disable: 4995)
#pragma warning(disable: 4996)
#pragma warning(disable: 4819)
#pragma warning(disable: 4018)
#pragma warning(disable: 4244)
#define WINVER          0x0500
#define _WIN32_WINNT    0x0501
#define _WIN32_IE       0x0600
#define _RICHEDIT_VER   0x0200
#define _SECURE_ATL 0
#include <atlbase.h>
#include <atlwin.h>
#include <atlstr.h>
#include <atltrace.h>


#define _WTL_NO_CSTRING

#include <wtl/atlapp.h>
#include <wtl/atlframe.h>
#include <wtl/atlctrls.h>
#include <wtl/atlctrlx.h>
#include <wtl/atldlgs.h>
#include <wtl/atlcrack.h>
#include <wtl/atlmisc.h>

#include <tinyxml/tinyxml.h>
#include <bkwin/bkatldefine.h>
#include <bkwin/bklistview.h>
#include <bkwin/bkwnd.h>
#include <bkres/bkres.h>
#include <bkwin/bkwndnotify.h>

#include <wtlhelper/whwindow.h>

#include <Shlwapi.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <IPHlpApi.h>

#include <vector>

#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Psapi.lib")

extern CAppModule _Module;



#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif