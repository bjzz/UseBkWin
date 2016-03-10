#include "stdafx.h"
#include "resource.h"
#include "HelpFuncion.h"
#include "NetCardListView.h"
#include "MainDlg.h"

#define WM_SYSTEMTRAYICON	(WM_USER + 1000)

BEGIN_MSG_MAP_EX_IMP(CMainDlg)
    MSG_BK_NOTIFY(IDC_RICHVIEW_WIN)
    CHAIN_MSG_MAP(CBkDialogImpl<CMainDlg>)
    CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CMainDlg>)
    MSG_WM_SYSCOMMAND(OnCommand)
    MSG_WM_INITDIALOG(OnInitDialog)
	MSG_WM_TIMER(OnTimer)
    MESSAGE_HANDLER_EX(WM_SYSTEMTRAYICON, OnSystemTrayIcon)
	COMMAND_ID_HANDLER_EX(ID_MENU_MORE_CONTACT, OnMenuMore)
	COMMAND_ID_HANDLER_EX(ID_MENU_MORE_ABOUT, OnMenuMore)
	COMMAND_ID_HANDLER_EX(ID_MENU_TRAY_SHOW, OnMenuTray)
	COMMAND_ID_HANDLER_EX(ID_MENU_TRAY_EXIT, OnMenuTray)
	COMMAND_ID_HANDLER_EX(ID_MENU_TRAY_SITE, OnMenuTray)
	COMMAND_ID_HANDLER_EX(ID_MENU_TRAY_CONTACT, OnMenuTray)
	REFLECT_NOTIFICATIONS_EX()
END_MSG_MAP_IMP()

CMainDlg::CMainDlg()
    :CBkDialogImpl<CMainDlg>()
{
    m_NetworkDeviceListView = new CNetworkDeviceListView();
}

CMainDlg::~CMainDlg()
{
	delete m_NetworkDeviceListView;
	m_NetworkDeviceListView = NULL;
}

void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO ��ʱ����
	if ( nIDEvent == 100 )
	{
		if ( m_bAutoFlush )
		{
			m_ProcessList.DeleteAllItems();
			this->InsertProcessList();

			m_TcpConnectionList.DeleteAllItems();
			this->InsertTcpConnectionList();

			m_UdpConnectionList.DeleteAllItems();
			this->InsertUdpConnectionList();

			m_RouteList.DeleteAllItems();
			this->InsertRouteList();
		}
	}
}

void CMainDlg::InitNetFlow()
{
	m_NetFlow.Create( 
		GetViewHWND(), 
		NULL, 
		NULL, 
		WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 
		0, 
		IDC_REALWND_NET_FLOW, 
		NULL 
		);
}

void CMainDlg::InitNetStatus()
{

	m_NetWindow.Create( 
		GetViewHWND(), 
		NULL, 
		NULL, 
		WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, 
		0, 
		IDC_REALWND_NET_STATUS, 
		NULL 
	);
// 	HWND Create(HWND hWndParent, ATL::_U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
// 		DWORD dwStyle = 0, DWORD dwExStyle = 0,
// 		ATL::_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
// 	{
// 		return TBase::Create(GetWndClassName(), hWndParent, rect.m_lpRect, szWindowName, dwStyle, dwExStyle, MenuOrID.m_hMenu, lpCreateParam);
// 	}
	/*m_NetWindow.Create( GetViewHWND(), NULL, NULL, 0, 0, IDC_REALWND_NET_STATUS, NULL );*/
	/*m_ctrlMenu.SubclassWindow(GetDlgItem(IDC_REALWND_NET_STATUS));*/
 	/*m_static.Create( GetViewHWND(), CWindow::rcDefault, _T("Click"), 0, 0, IDC_REALWND_NET_STATUS, 0 );*/
// 	m_ctrlMenu.SetMenu(ID__MENU_CONTACT);
 	
// 	int x=0;
// 	int y=0;
// 	for(int i=0; i<1000; ++i)
// 	{
// 		y = x*x/2;
// 		++x;
// 		m_NetWindow.AddCurve( _T("c1"), x, y);
// 		m_NetWindow.AddCurve( _T("c2"), x, y/2);
// 	}
// 	m_NetWindow.SetCurveColor( _T("c1"), RGB(255, 0, 0));
// 	m_NetWindow.SetCurveColor( _T("c2"), RGB(0, 0, 255));
// 	m_NetWindow.CalcXYCoordRange();
	//m_NetWindow.Create( GetViewHWND(), NULL, NULL, NULL, 0, IDC_REALWND_NET_STATUS );
	
// 	m_NetWindow.Create( GetViewHWND(), 
// 		NULL,
// 		NULL, 
// 		NULL,
// 		0, 
// 		IDC_REALWND_NET_STATUS, 
// 		NULL 
// 		);	

	/*m_NetWindow.SetBitmap( LoadBitmap( GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_MAIN_BG_BODY) ) );*/
// 
// 	m_btn.SetButtonStyle( BS_PUSHBUTTON );
// 	this->SetDlgItemText( IDC_REALWND_NET_STATUS, _T("adafa") );
}

void CMainDlg::SetAutoRun(BOOL bAutoRun)
{
	::SetAutoRun( bAutoRun );
}


BOOL CMainDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    SetIcon(::LoadIcon((HMODULE)&__ImageBase, MAKEINTRESOURCE(IDI_BEIKESAFE)));
    SetIcon(::LoadIcon((HMODULE)&__ImageBase, MAKEINTRESOURCE(IDI_BEIKESAFE)), FALSE);

	// ��ʼ���������б���ȡ��������Ϣ
	InitDeviceList();
	InsertDeviceList();
 	
	// ��ʼ�������б���ȡ������Ϣ
    InitProcessList();
	InsertProcessList();

	// ��ʼ�� TCP������Ϣ
	InitTcpConnectionList();
	InsertTcpConnectionList();

	// ��ʼ��UDP������Ϣ
	InitUdpConnectionList();
	InsertUdpConnectionList();

	// ��ʼ��·�ɱ���Ϣ
	InitRouteList();
	InsertRouteList();
	
	// Ĭ�ϵ���ر�ʱ�رճ���
	m_bHidden = FALSE;

	// Ĭ�ϲ��Զ�ˢ��ͳ����Ϣ
	m_bAutoFlush = FALSE;

	this->SetTimer( 100, 1000 * 3 );

	// ��ʼ��ϵͳ���̰�ť
    InitTray();

	// own draw button
	InitNetStatus();

	// net flow own draw button
	InitNetFlow();
	
    return TRUE;
}
void CMainDlg::OnCancelConfig()
{
// 	//MessageBox(_T("ȡ���޸�."));
// 	LRESULT ret = ::SendMessage( GetDlgItem(IDCK_CONFIG_AUTORUN), BM_GETCHECK, 0, 0 );
// 	if ( ret == BST_CHECKED )
// 	{
// 		MessageBox(_T("ѡ�� �����Զ�����."));
// 	}
// 	else
// 	{
// 		MessageBox(_T("δѡ�� �����Զ�����."));
// 	}
// 	ret = ::SendMessage( GetDlgItem(IDC_CHKBTN_AUTORUN_HIDE), BM_GETCHECK, 0, 0 );
// 	if ( ret == BST_CHECKED )
// 	{
// 		MessageBox(_T("ѡ�� ����������С��."));
// 	}
// 	else
// 	{
// 		MessageBox(_T("δѡ�� ����������С��."));
// 	}
// 	ret = ::SendMessage( GetDlgItem(IDC_CHKBTN_WINDOW_ON_TOP), BM_GETCHECK, 0, 0 );
// 	if ( ret == BST_CHECKED )
// 	{
// 		MessageBox(_T("ѡ�� ������ǰ��."));
// 	}
// 	else
// 	{
// 		MessageBox(_T("δѡ�� ������ǰ��."));
// 	}
// 	
	SetTabCurSel(IDC_TAB_GLOBAL, 0);
}
//////////////////////////////////////////////////
/////////////////////�����������ѡ��
void CMainDlg::OnConfigNetBgColor()
{
	CColorDialog dlg;
	dlg.DoModal();
	COLORREF color = dlg.GetColor();

	m_NetWindow.SetBgColor( color );
}
void CMainDlg::OnConfigNetUpColor()
{
	CColorDialog dlg;
	dlg.DoModal();
	COLORREF color = dlg.GetColor();
	m_NetWindow.SetUpColor( color );
	m_NetWindow.SetColorTextUpload( color );
}
void CMainDlg::OnConfigNetDownColor()
{
	CColorDialog dlg;
	dlg.DoModal();
	COLORREF color = dlg.GetColor();
	m_NetWindow.SetDownColor( color );
	m_NetWindow.SetColorTextDownload( color );
}
void CMainDlg::OnConfigNetGridLineColor()
{
	CColorDialog dlg;
	dlg.DoModal();
	COLORREF color = dlg.GetColor();
	m_NetWindow.SetPenGridLineColor( color );
}
////////////////////////////////////////////////////
/////////////////////�����������ѡ��
void CMainDlg::OnConfigFlowBgColor()
{
	CColorDialog dlg;
	dlg.DoModal();
	COLORREF color = dlg.GetColor();
	m_NetFlow.SetBgColor( color );
}
void CMainDlg::OnConfigFlowUpColor()
{
	CColorDialog dlg;
	dlg.DoModal();
	COLORREF color = dlg.GetColor();
	m_NetFlow.SetPenUploadColor( color );
	m_NetFlow.SetColorTextUpload( color );
}
void CMainDlg::OnConfigFlowDownColor()
{
	CColorDialog dlg;
	dlg.DoModal();
	COLORREF color = dlg.GetColor();
	m_NetFlow.SetPenDownloadColor( color );
	m_NetFlow.SetColorTextDownload( color );
	
}


void CMainDlg::OnConfigNetFlowGridLineColor()
{
	CColorDialog dlg;
	dlg.DoModal();
	COLORREF color = dlg.GetColor();
	m_NetFlow.SetPenGridColor( color );
}
///////////////////////////////////////////////////////////
void CMainDlg::OnSubmitConfig()
{
	/*m_NetWindow.Set();*/
// 	CColorDialog dlg;
// 	dlg.DoModal();
// 	COLORREF cor = dlg.GetColor();
	//////////////////////// д��ע��� �����Զ�����
	if ( GetItemCheck( IDCK_CONFIG_AUTORUN ) )
	{
		SetAutoRun(TRUE);
	}
	else
	{
		SetAutoRun(FALSE);
	}
	////////////////////// ��С������
	if ( GetItemCheck( IDC_CHKBTN_AUTORUN_HIDE ) )
	{

		m_bHidden = TRUE;
	}
	else
	{
		m_bHidden = FALSE;
	}
	//////////////////////// ������ǰ��
	if ( GetItemCheck( IDC_CHKBTN_WINDOW_ON_TOP ) )
	{
		// ������ǰ��
		::SetWindowPos( this->m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
	}
	else
	{
		::SetWindowPos( this->m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
	}

	if ( GetItemCheck( IDC_RADIO_BTN_AUTO_UPDATE ) || GetItemCheck( IDC_RADIO_BTN_CUSTOM_UPDATE ) )
	{
		MessageBox(_T("��ʱ���ṩ��������.лл���Ĺ�ע!"));
		SetItemCheck( IDC_RADIO_BTN_AUTO_UPDATE, FALSE );
		SetItemCheck( IDC_RADIO_BTN_CUSTOM_UPDATE, FALSE );
	}
	//////////////////////// �������л�
	if ( GetItemCheck( IDC_RADIO_BTN_LANGUAGE_CHS ) )
	{
		SetLangType( 1 );
	}
	if( GetItemCheck( IDC_RADIO_BTN_LANGUAGE_EN ) )
	{
		SetLangType( 2 );
	}
	////////////////////////  �Զ�ˢ��ͳ����Ϣ
	if ( GetItemCheck( IDC_CHECK_BTK_AUTO_FLUSH_INFO ) )
	{
		m_bAutoFlush = TRUE;
	}
	else
	{
		m_bAutoFlush = FALSE;
	}
	////////////////////////  ��ת����ҳ
	SetTabCurSel(IDC_TAB_GLOBAL, 0);
}


void CMainDlg::InitRouteList()
{
	m_RouteList.Create( GetViewHWND(), 
		NULL,
		NULL, 
		WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL, 
		0, 
		IDL_ROUTE_LIST, 
		NULL 
		);
	m_RouteList.AddColumn( _T("Ŀ������"),  0 );
	m_RouteList.SetColumnWidth(0, 105);

	m_RouteList.AddColumn( _T("��������"), 1 );
	m_RouteList.SetColumnWidth(1, 105);
	
	m_RouteList.AddColumn( _T("����"), 2 );
	m_RouteList.SetColumnWidth(2, 98);

	m_RouteList.AddColumn( _T("Ծ��"), 3 );
	m_RouteList.SetColumnWidth(3, 40);
}
void CMainDlg::InsertRouteList()
{
	PMIB_IPFORWARDTABLE table;
	ULONG ulSize;
	struct in_addr IpAddressTarget;
	struct in_addr IpAddrMask;
	struct in_addr IpAddrGate;

	CString strTarget;	// Ŀ���ַ
	CString strMask;	// ����
	CString strGate;	// ����
	CString strHop;		// Ծ����

	CString strTotalNum;// ·�ɱ����

	char szTarget[MAX_PATH];
	char szMask[MAX_PATH];
	char szGate[MAX_PATH];
	char szHop[MAX_PATH];
	char szToalNum[32];
	
	ZeroMemory(szTarget, sizeof(szTarget));
	ZeroMemory(szMask, sizeof(szMask));
	ZeroMemory(szGate, sizeof(szGate));
	ZeroMemory(szHop, sizeof(szHop));
	ZeroMemory(szToalNum, sizeof(szToalNum));

	ulSize = sizeof(MIB_IPFORWARDTABLE);
	table = reinterpret_cast<PMIB_IPFORWARDTABLE> ( GlobalAlloc( GPTR, ulSize ) );

	if ( GetIpForwardTable( table, &ulSize, TRUE ) == ERROR_INSUFFICIENT_BUFFER )
	{
		GlobalFree( table );
		table = reinterpret_cast<PMIB_IPFORWARDTABLE> ( GlobalAlloc( GPTR, ulSize ) );
	}

	if ( GetIpForwardTable( table, &ulSize, TRUE ) == NO_ERROR )
	{
		sprintf_s(szToalNum, sizeof(szToalNum), "%d", table->dwNumEntries);
		//cout << "total:" << table->dwNumEntries << endl;

		for ( int i=0; i<static_cast<int>( table->dwNumEntries ); i++ )
		{
			IpAddressTarget.S_un.S_addr = table->table[i].dwForwardDest;
			IpAddrMask.S_un.S_addr  = table->table[i].dwForwardMask;
			IpAddrGate.S_un.S_addr  = table->table[i].dwForwardNextHop;

			sprintf_s(szTarget, sizeof(szTarget), "%s", inet_ntoa(IpAddressTarget));
			sprintf_s(szMask, sizeof(szMask), "%s", inet_ntoa(IpAddrMask));
			sprintf_s(szGate, sizeof(szGate), "%s", inet_ntoa(IpAddrGate));

			sprintf_s(szHop, sizeof(szHop), "%d", table->table[i].dwForwardMetric1);
			//cout << inet_ntoa(IpAddrLocal) << " | " << inet_ntoa(IpAddrMask) << " | " << inet_ntoa(IpAddrGate) << endl;
			//cout << table->table[i].dwForwardMetric1 << endl;
			//cout << "--------" << endl;

			strTarget		= szTarget;
			strMask			= szMask;
			strGate			= szGate;
			strHop			= szHop;

			m_RouteList.AddItem(0, 0, strTarget);
			m_RouteList.AddItem(0, 1, strMask);
			m_RouteList.AddItem(0, 2, strGate);
			m_RouteList.AddItem(0, 3, strHop);
		}
		strTotalNum = szToalNum;
		SetRichText( IDC_TEXT_ROUTE_CONNECTION_COUNT, strTotalNum );

		GlobalFree(table);
	}
}

void CMainDlg::OnRouteSttusReload()
{
	m_RouteList.DeleteAllItems();
	InsertRouteList();
}

void CMainDlg::InitTcpConnectionList()
{
	m_TcpConnectionList.Create( GetViewHWND(), 
		NULL,
		NULL, 
		WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL, 
		0, 
		IDL_TCPCONNECTION_LIST, 
		NULL 
		);
	m_TcpConnectionList.AddColumn( _T("PID"),  0 );
	m_TcpConnectionList.SetColumnWidth( 0, 40 );

	m_TcpConnectionList.AddColumn( _T("���ص�ַ"), 1 );
	m_TcpConnectionList.SetColumnWidth( 1, 100 );
	
	m_TcpConnectionList.AddColumn( _T("�˿�"), 2 );
	m_TcpConnectionList.SetColumnWidth( 2, 50 );
	
	m_TcpConnectionList.AddColumn( _T("Զ�̵�ַ"), 3 );
	m_TcpConnectionList.SetColumnWidth( 3, 100 );
	
	m_TcpConnectionList.AddColumn( _T("�˿�"), 4 );
	m_TcpConnectionList.SetColumnWidth( 4, 50 );
}

void CMainDlg::InsertTcpConnectionList()
{
	CString strPID;
	CString strLocalAddress;
	CString strLocalPort;
	CString strRemoteAddress;
	CString strRemotePort;
	CString strStatus;
	CString strNumOfConnection;

	wchar_t szPID[32];
	char szLocalAddress[MAX_PATH];
	wchar_t szLocalPort[MAX_PATH];
	char szRemoteAddress[MAX_PATH];
	wchar_t szRemotePort[MAX_PATH];
	wchar_t szStatus[MAX_PATH];
	DWORD dwNumConnections;

	ZeroMemory(szPID, sizeof(szPID));
	ZeroMemory(szLocalAddress, sizeof(szLocalAddress));
	ZeroMemory(szLocalPort, sizeof(szLocalPort));
	ZeroMemory(szRemoteAddress, sizeof(szRemoteAddress));
	ZeroMemory(szRemotePort, sizeof(szRemotePort));
	ZeroMemory(szStatus, sizeof(szStatus));
	dwNumConnections = 0;

	PMIB_TCPTABLE_OWNER_PID pTable;
	ULONG ulSize;
	struct in_addr AddressLocal;
	struct in_addr AddressRemote;
	DWORD dwPID;

	ulSize = sizeof( MIB_TCPTABLE_OWNER_PID );
	pTable = reinterpret_cast<PMIB_TCPTABLE_OWNER_PID> ( GlobalAlloc( GPTR, ulSize ) );

	if ( GetExtendedTcpTable( pTable, &ulSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0 ) == ERROR_INSUFFICIENT_BUFFER )
	{
		GlobalFree( pTable );
		pTable = reinterpret_cast<PMIB_TCPTABLE_OWNER_PID> ( GlobalAlloc( GPTR, ulSize ) );
	}

	if ( GetExtendedTcpTable( pTable, &ulSize, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0 ) == NO_ERROR)
	{
		dwNumConnections = pTable->dwNumEntries;

		for ( int i=0; i<static_cast<int>(pTable->dwNumEntries); i++ )
		{
			AddressLocal.S_un.S_addr  = (u_long)pTable->table[i].dwLocalAddr;
			AddressRemote.S_un.S_addr = (u_long)pTable->table[i].dwRemoteAddr;

			TcpStatus2String( pTable->table[i].dwState, szStatus );

			dwPID = pTable->table[i].dwOwningPid;
			wsprintf(szPID, _T("%d"), pTable->table[i].dwOwningPid);
			
			//wsprintf(szLocalAddress, _T("%s"), inet_ntoa(AddressLocal));
			sprintf_s(szLocalAddress, sizeof(szLocalAddress), "%s", inet_ntoa(AddressLocal));
			
			//wsprintf(szRemoteAddress, _T("%s"), inet_ntoa(AddressRemote));
			sprintf_s(szRemoteAddress, sizeof(szRemoteAddress), "%s", inet_ntoa(AddressRemote));

			wsprintf(szLocalPort, _T("%d"), pTable->table[i].dwLocalPort);
			wsprintf(szRemotePort, _T("%d"), pTable->table[i].dwRemotePort);

			strPID				= szPID;
			strLocalAddress		= szLocalAddress;
			strLocalPort		= szLocalPort;
			strRemoteAddress	= szRemoteAddress;
			strRemotePort		= szRemotePort;
			strStatus			= szStatus;

			m_TcpConnectionList.AddItem( 0, 0, strPID );
			m_TcpConnectionList.AddItem( 0, 1, strLocalAddress);
			//m_TcpConnectionList.AddItem( 0, 1, reinterpret_cast<LPCTSTR>( str1.m_psz ) );
			m_TcpConnectionList.AddItem( 0, 2, strLocalPort);
			m_TcpConnectionList.AddItem( 0, 3, strRemoteAddress);
			m_TcpConnectionList.AddItem( 0, 4, strRemotePort);

			strNumOfConnection.Format(_T("%d"), dwNumConnections);
			SetRichText( IDC_TEXT_TCP_CONNECTION_COUNT, strNumOfConnection );
		}

		GlobalFree(pTable);
	}
}

void CMainDlg::OnTcpStatusReload()
{
	m_TcpConnectionList.DeleteAllItems();
	InsertTcpConnectionList();
}

void CMainDlg::InitUdpConnectionList()
{
	m_UdpConnectionList.Create( GetViewHWND(), 
		NULL,
		NULL, 
		WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL, 
		0, 
		IDL_UDPCONNECTION_LIST, 
		NULL 
		);
	m_UdpConnectionList.AddColumn( _T("PID"),  0 );
	m_UdpConnectionList.SetColumnWidth(0, 40);

	m_UdpConnectionList.AddColumn( _T("���ص�ַ"), 1 );
	m_UdpConnectionList.SetColumnWidth(1, 100);
	
	m_UdpConnectionList.AddColumn( _T("���ض˿�"), 2 );
	m_UdpConnectionList.SetColumnWidth(2, 60);

}
void CMainDlg::InsertUdpConnectionList()
{
	PMIB_UDPTABLE_OWNER_PID pTable;
	ULONG ulSize;
	struct in_addr Address;
	DWORD dwPort;
	DWORD dwPID;
	
	CString strPID;
	CString strAddress;
	CString strPort;
	CString strNumOfUdpConnection;

	wchar_t szPID[32];
	char szAddress[MAX_PATH];
	wchar_t szPort[64];

	ZeroMemory(szPID, sizeof(szPID));
	ZeroMemory(szAddress, sizeof(szAddress));
	ZeroMemory(szPort, sizeof(szPort));

	ulSize = sizeof(MIB_UDPTABLE_OWNER_PID);
	pTable = reinterpret_cast<PMIB_UDPTABLE_OWNER_PID>( GlobalAlloc( GPTR, ulSize ) );

	if ( GetExtendedUdpTable( pTable, &ulSize, TRUE, AF_INET, UDP_TABLE_OWNER_PID, 0 ) == ERROR_INSUFFICIENT_BUFFER )
	{
		GlobalFree( pTable );
		pTable = reinterpret_cast<PMIB_UDPTABLE_OWNER_PID>( GlobalAlloc( GPTR, ulSize ) );
	}

	if ( GetExtendedUdpTable( pTable, &ulSize, TRUE, AF_INET, UDP_TABLE_OWNER_PID, 0 ) == NO_ERROR )
	{
		for ( int i=0; i< static_cast<int>(pTable->dwNumEntries) ; i++ )
		{
			Address.S_un.S_addr = (u_long)pTable->table[i].dwLocalAddr;
			dwPort = pTable->table[i].dwLocalPort;
			dwPID = pTable->table[i].dwOwningPid;

			sprintf_s( szAddress, sizeof(szAddress), "%s", inet_ntoa(Address) );
			wsprintf(szPort, _T("%d"), dwPort);
			wsprintf(szPID, _T("%d"), dwPID);

			strPID = szPID;
			strAddress = szAddress;
			strPort = szPort;

			m_UdpConnectionList.AddItem( 0, 0, strPID );
			m_UdpConnectionList.AddItem( 0, 1, strAddress );
			m_UdpConnectionList.AddItem( 0, 2, strPort );

			//cout << inet_ntoa(Address) << " : " << dwPort << "." << dwPID << endl;
		}
		// �ͷŲ��õ��ڴ�
		strNumOfUdpConnection.Format( _T("%d"), pTable->dwNumEntries );
		SetRichText( IDC_TEXT_UDP_CONNECTION_COUNT, strNumOfUdpConnection );
		GlobalFree( pTable );
	}
}

void CMainDlg::OnUdpStatusReload()
{
	m_UdpConnectionList.DeleteAllItems();
	InsertUdpConnectionList();
}

void CMainDlg::InitProcessList()
{

	
	m_ProcessList.Create( GetViewHWND(), NULL, NULL, WS_VISIBLE|WS_CHILD|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL, 0, IDL_PROCESS_LIST, NULL );

	m_ProcessList.AddColumn( _T("PID"), 0);
	m_ProcessList.SetColumnWidth(0, 40);

	m_ProcessList.AddColumn( _T("��������"),1);
	m_ProcessList.SetColumnWidth(1, 100);
	
	m_ProcessList.AddColumn( _T("����·��"),2);
	m_ProcessList.SetColumnWidth(2, 205);
}
void CMainDlg::UpgradeProcessPrivilege(LPCWSTR lpPrivilegeName)
{
	HANDLE hToken;

	if ( OpenProcessToken( GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken ) )
	{
		LUID luid;
		if ( LookupPrivilegeValue( NULL, lpPrivilegeName, &luid ) )
		{
			TOKEN_PRIVILEGES tp;
			tp.PrivilegeCount			= 1;
			tp.Privileges[0].Attributes	= SE_PRIVILEGE_ENABLED;
			tp.Privileges[0].Luid		= luid;
			AdjustTokenPrivileges( hToken, FALSE, &tp, 0, NULL, NULL );

			if (GetLastError() != ERROR_SUCCESS)
			{
				MessageBox( _T("��ȡ����ԱȨ��ʧ��!") );
			}
		}
	}
	CloseHandle( hToken );
}


void CMainDlg::InsertProcessList()
{
	UpgradeProcessPrivilege( SE_DEBUG_NAME );

	HANDLE hSnap;
	PROCESSENTRY32 pn = { sizeof(pn) };
	CString strPID;
	CString strFileName;
	CString strFilePath;
	CString strNumOfProcess;

	wchar_t szPID[32];
	wchar_t szFileName[MAX_PATH];
	wchar_t szFilePath[MAX_PATH];
	wchar_t szNumOfProcess[32];
	DWORD dwNumCount;

	dwNumCount = 0;
	ZeroMemory(szNumOfProcess, sizeof(szNumOfProcess));

	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, -1 );
	if ( hSnap == INVALID_HANDLE_VALUE )
		return ;

	if ( Process32First(hSnap, &pn) == TRUE )
	{
		do 
		{
			dwNumCount ++;

			ZeroMemory(szPID, sizeof(szPID));
			ZeroMemory(szFileName, sizeof(szFileName));
			ZeroMemory(szFilePath, sizeof(szFilePath));
			wsprintf(szPID, _T("%d"), pn.th32ProcessID);
			wsprintf(szFileName, _T("%s"), pn.szExeFile);

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pn.th32ProcessID);
			if ( hProcess != NULL )
			{				
				if ( GetModuleFileNameEx(hProcess, NULL, szFilePath, sizeof(szFilePath)) != 0 )
				{
					//strFilePath = szFilePath;
				}
				else
				{
					//cout << "Can not Get Process Path." << endl; 
					//strFilePath = _T("Need more Permition .");
					wsprintf(szFilePath, _T("%s"), _T("Permission denied.") );
				}
			}
			else
			{
				//cout << "Need More primition." << endl;
				//strFilePath = _T("Need more Permition .");
				wsprintf(szFilePath, _T("%s"), _T("Permission denied.") );
			}
			strPID		= szPID;
			strFileName = szFileName;
			strFilePath	= szFilePath;

			m_ProcessList.AddItem(0, 0, strPID);
			m_ProcessList.AddItem(0, 1, strFileName);
			m_ProcessList.AddItem(0, 2, strFilePath);

		} while ( Process32Next(hSnap, &pn) );

		wsprintf(szNumOfProcess, _T("%d"), dwNumCount);
		strNumOfProcess = szNumOfProcess;
	}
	SetRichText(IDC_TEXT_PROCESS_COUNT, strNumOfProcess);
	CloseHandle( hSnap );
}

void CMainDlg::OnProcessListReload()
{
	//MessageBox(_T("tes"));
	m_ProcessList.DeleteAllItems();
	InsertProcessList();
}

void CMainDlg::InitDeviceList()
{
	m_NetworkDeviceListView->Create( 
		GetViewHWND(), NULL, NULL, 
		WS_CLIPCHILDREN | WS_CHILD |WS_VSCROLL | LBS_OWNERDRAWVARIABLE| LBS_HASSTRINGS , 
		0, IDC_NETCONFIG_NETWORK_DEVICE_LISTVIEW, NULL);

	m_NetworkDeviceListView->CreateX();
}

// �汾�� ���Ի�ȡ��������������Ϣ
void CMainDlg::InsertDeviceList()
{
	PIP_ADAPTER_INFO pTable;
	ULONG ulSize;

	char szDesc[MAX_PATH];
	char szIpAddress[MAX_PATH];
	char szGateAddress[MAX_PATH];
	char szDnsAddress[MAX_PATH];
	char szNetMask[MAX_PATH];
	char szMac[MAX_PATH];

	CString strDesc;
	CString strIpAddress;
	CString strGateAddress;
	CString strDnsAddress;
	CString strNetMask;
	CString strMac;
	
	////////////////// ������ȡ DNS ��Ϣ /////////////////
	PFIXED_INFO pInfo;
	ULONG ulSize2;

	ZeroMemory(szDnsAddress, sizeof(szDnsAddress));
	ulSize2 = sizeof(FIXED_INFO);
	pInfo = reinterpret_cast<PFIXED_INFO>( GlobalAlloc(GPTR, ulSize2) );
	
	DWORD dwRet = GetNetworkParams( pInfo, &ulSize2 ) ;

	if ( dwRet == ERROR_INSUFFICIENT_BUFFER )
	{
		GlobalFree( pInfo );
		pInfo = reinterpret_cast<PFIXED_INFO>( GlobalAlloc(GPTR, ulSize2) );
	}
	if ( dwRet == ERROR_BUFFER_OVERFLOW )
	{
		GlobalFree( pInfo );
		pInfo = reinterpret_cast<PFIXED_INFO>( GlobalAlloc(GPTR, ulSize2) );
		//MessageBox(_T("ERROR_BUFFER_OVERFLOW"));
	}
	if ( dwRet == ERROR_INVALID_PARAMETER )
	{
		MessageBox(_T("ERROR_INVALID_PARAMETER"));
	}
	if ( dwRet == ERROR_NO_DATA )
	{
		MessageBox(_T("ERROR_NO_DATA"));
	}
	if ( dwRet == ERROR_NOT_SUPPORTED )
	{
		MessageBox(_T("ERROR_NOT_SUPPORTED"));
	}

	if ( GetNetworkParams( pInfo, &ulSize2 ) == NO_ERROR )
	{
		sprintf_s(szDnsAddress, sizeof(szDnsAddress), "%s", pInfo->DnsServerList.IpAddress.String);
	}
	///////////////// ��ȡ������Ϣ  /////////////////////

	// 648 bytes
	ulSize = sizeof(IP_ADAPTER_INFO);
	pTable = reinterpret_cast<PIP_ADAPTER_INFO>( GlobalAlloc( GPTR, ulSize ) );

	GetAdaptersInfo( pTable, &ulSize );
	GlobalFree( pTable );

	pTable = reinterpret_cast<PIP_ADAPTER_INFO>( GlobalAlloc( GPTR, ulSize ) );
	/*DWORD dwCount = 0;*/
	if ( GetAdaptersInfo( pTable, &ulSize ) == NO_ERROR )
	{
		do 
		{
			LP_XY_NETWORK_DEVICE_INFO_DATA	pNetworkDeviceInfoData;
			pNetworkDeviceInfoData = new XY_NETWORK_DEVICE_INFO_DATA;
// 			pNetworkDeviceInfoData		= reinterpret_cast<LP_XY_NETWORK_DEVICE_INFO_DATA>( HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(XY_NETWORK_DEVICE_INFO_DATA) ) );
			/*pNetworkDeviceInfoData	= reinterpret_cast<LP_XY_NETWORK_DEVICE_INFO_DATA>( GlobalAlloc( GPTR, sizeof( XY_NETWORK_DEVICE_INFO_DATA ) ) );*/
			/*XY_NETWORK_DEVICE_INFO_DATA	pNetworkDeviceInfoData;*/
			///////////////////////////////////
			ZeroMemory(szDesc, sizeof(szDesc));
			ZeroMemory(szIpAddress, sizeof(szIpAddress));
			ZeroMemory(szGateAddress, sizeof(szGateAddress));
			ZeroMemory(szNetMask, sizeof(szNetMask));
			ZeroMemory(szMac, sizeof(szMac));
			

			//////////////////////////////////
			sprintf_s(szDesc, sizeof(szDesc), "%s", pTable->Description);
			sprintf_s(szIpAddress, sizeof(szIpAddress), "%s", pTable->IpAddressList.IpAddress.String);
			sprintf_s(szGateAddress, sizeof(szGateAddress), "%s", pTable->GatewayList.IpAddress.String);
			// DNS NETMASK
			sprintf_s(szNetMask, sizeof(szNetMask), "%s", pTable->IpAddressList.IpMask.String);

			sprintf_s(szMac, sizeof(szMac), "%.2X-%.2X-%.2X-%.2X-%.2X-%.2X", 
				pTable->Address[0],
				pTable->Address[1],
				pTable->Address[2],
				pTable->Address[3],
				pTable->Address[4],
				pTable->Address[5]
			);


			strDesc = szDesc;
			strIpAddress = szIpAddress;
			strGateAddress = szGateAddress;
			strDnsAddress = szDnsAddress;
			strNetMask = szNetMask;
			strMac = szMac;
			//////////////////////////////////
			//pNetworkDeviceInfoData		= reinterpret_cast<LP_XY_NETWORK_DEVICE_INFO_DATA>(GlobalAlloc(GPTR, sizeof(XY_NETWORK_DEVICE_INFO_DATA)));
			
			

// 			pNetworkDeviceInfoData->bDeviceEnabled			= TRUE;
// 			pNetworkDeviceInfoData->bAutoObtainDNS          = TRUE;
// 			pNetworkDeviceInfoData->nNetworkType			= WIRED_NETWORK_DEVICE;
// 			pNetworkDeviceInfoData->pszConnectionName		= _T("��������");
// 			pNetworkDeviceInfoData->pszIPAddress			= strIpAddress;
// 			pNetworkDeviceInfoData->pszGateway				= strGateAddress;
// 			pNetworkDeviceInfoData->pszDeviceDesc			= strDesc;
// 			pNetworkDeviceInfoData->pszNetMask				= strNetMask;
// 			pNetworkDeviceInfoData->pszMACAddress			= strMac;
// 			pNetworkDeviceInfoData->pszPrimaryDNS			= strDnsAddress;

			pNetworkDeviceInfoData->bDeviceEnabled			= TRUE;
			pNetworkDeviceInfoData->bAutoObtainDNS          = TRUE;
			pNetworkDeviceInfoData->nNetworkType			= WIRED_NETWORK_DEVICE;
			pNetworkDeviceInfoData->pszConnectionName		= _T("��������");
			pNetworkDeviceInfoData->pszIPAddress			= strIpAddress;
			pNetworkDeviceInfoData->pszGateway				= strGateAddress;
			pNetworkDeviceInfoData->pszDeviceDesc			= strDesc;
			pNetworkDeviceInfoData->pszNetMask				= strNetMask;
			pNetworkDeviceInfoData->pszMACAddress			= strMac;
			pNetworkDeviceInfoData->pszPrimaryDNS			= strDnsAddress;			
			m_NetworkDeviceListView->InsertItemX(pNetworkDeviceInfoData);
			/*dwCount ++;*/
			
			/////////////////////////
			pTable = pTable->Next;
		} while ( pTable  );

		//GlobalFree( pTable );
	}
}

// �汾һ ֻ�ܻ�ȡһ������������
// void CMainDlg:: InsertDeviceList()
// {
// 	LP_XY_NETWORK_DEVICE_INFO_DATA	pNetworkDeviceInfoData;
// 	pNetworkDeviceInfoData	= new XY_NETWORK_DEVICE_INFO_DATA;
// 	DWORD dwNumAdapter;
// 	PIP_ADAPTER_INFO pInfo;
// 	ULONG ulSize;
// 	CString strIpAddress;
// 	CString strGateAddress;
// 	CString strAdapterDesc;
// 	CString strNetMask;
// 	CString strMAC;
// 	CString strDNSAddress;
// 	char szIpAddress[17];
// 	char szGateAddress[17];
// 	char szNetMask[17];
// 	char szMAC[64];
// 	char szDNSAddress[17];
// 	ZeroMemory( szIpAddress, 17 );
// 	ZeroMemory( szGateAddress, 17 );
// 	ZeroMemory( szNetMask, 17 );
// 	ZeroMemory( szMAC, 64 );
// 	ZeroMemory( szDNSAddress, 17 );
// 	ulSize = sizeof( IP_ADAPTER_INFO );
// 	pInfo  = reinterpret_cast<PIP_ADAPTER_INFO> (GlobalAlloc(GPTR, ulSize));
// 	if ( GetAdaptersInfo( pInfo, &ulSize) == ERROR_INSUFFICIENT_BUFFER )
// 	{
// 		GlobalFree( pInfo );
// 	}	
// 	// ��ȡ���������ĸ���
// 	dwNumAdapter = ulSize / sizeof(IP_ADAPTER_INFO);
// 	pInfo = reinterpret_cast<PIP_ADAPTER_INFO> ( GlobalAlloc( GPTR, ulSize ) );
// 	// ��ȡ�����ַ�����ص�ַ����������
// 	if ( GetAdaptersInfo( pInfo, &ulSize ) == NO_ERROR )
// 	{
// 		for ( int i=0; i<16; i++ )
// 		{
// 			szIpAddress[i]   = pInfo->IpAddressList.IpAddress.String[i];
// 			szGateAddress[i] = pInfo->GatewayList.IpAddress.String[i];
// 			szNetMask[i]	 = pInfo->IpAddressList.IpMask.String[i];
// 		}
// 		strIpAddress   = szIpAddress;
// 		strGateAddress = szGateAddress;
// 		strNetMask	   = szNetMask;
// 		strAdapterDesc = pInfo->Description;
// 	}
// 	sprintf( szMAC, "%.2X-%.2X-%.2X-%.2X-%.2X-%.2X",
// 		pInfo->Address[0],
// 		pInfo->Address[1],
// 		pInfo->Address[2],
// 		pInfo->Address[3],
// 		pInfo->Address[4],
// 		pInfo->Address[5]
// 	);
// 	strMAC = szMAC;
// 
// 	PFIXED_INFO pFixInfo;
// 	ULONG ulSize2 = sizeof( FIXED_INFO );
// 	pFixInfo = reinterpret_cast<PFIXED_INFO> ( GlobalAlloc( GPTR, ulSize2 ) );
// 	if ( GetNetworkParams( pFixInfo, &ulSize2 ) == ERROR_INSUFFICIENT_BUFFER )
// 	{
// 		GlobalFree( pFixInfo );
// 	}
// 	pFixInfo = reinterpret_cast<PFIXED_INFO> ( GlobalAlloc( GPTR, ulSize2 ) );
// 	if ( GetNetworkParams( pFixInfo, &ulSize2 ) == NO_ERROR )
// 	{
// 		// ��ȡDNS��ַ
// 		for ( int k=0; k<16; k++ )
// 		{
// 			szDNSAddress[k] = pFixInfo->DnsServerList.IpAddress.String[k];
// 		}
// 	}
// 	strDNSAddress = szDNSAddress;
// 
// 	pNetworkDeviceInfoData->bDeviceEnabled			= TRUE;
// 	pNetworkDeviceInfoData->bAutoObtainDNS          = TRUE;
// 	pNetworkDeviceInfoData->nNetworkType			= WIRED_NETWORK_DEVICE;
// 	pNetworkDeviceInfoData->pszConnectionName		= _T("��������");
// 	pNetworkDeviceInfoData->pszIPAddress			= strIpAddress;
// 	pNetworkDeviceInfoData->pszGateway				= strGateAddress;
// 	pNetworkDeviceInfoData->pszDeviceDesc			= strAdapterDesc;
// 	pNetworkDeviceInfoData->pszNetMask				= strNetMask;
// 	pNetworkDeviceInfoData->pszMACAddress			= strMAC;
// 	pNetworkDeviceInfoData->pszPrimaryDNS			= strDNSAddress;
// 
// 	m_NetworkDeviceListView->InsertItemX(pNetworkDeviceInfoData);
// }





LRESULT CMainDlg::OnSystemTrayIcon(UINT, WPARAM wParam, LPARAM lParam)
{
    ATLASSERT(wParam == 1);
    switch(lParam)
    {
	case WM_LBUTTONDOWN:
		if ( GetWindowLongPtr( GWL_STYLE ) & WS_VISIBLE )
		{
			ShowWindow( SW_HIDE );
		}
		else
		{
			ShowWindow( SW_SHOW );
		}
		
		break;
    case WM_LBUTTONDBLCLK:
        //SendMessage(WM_COMMAND, SC_RESTORE);
        SetForegroundWindow(m_hWnd);
        ShowWindow(SW_SHOW);
        BringWindowToTop();
        break;
    case WM_RBUTTONUP:
        {
            SetForegroundWindow(m_hWnd);
            CMenu menuPopup;
            menuPopup.LoadMenu(IDR_TRAYMENU);

            CMenuHandle Menu = menuPopup.GetSubMenu(0);;
            CPoint Position;
            ATLVERIFY(GetCursorPos(&Position));
            Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_BOTTOMALIGN, Position.x, Position.y, m_hWnd);
        }
        break;
    }
    return 0;
}

void CMainDlg::OnBkBtnMinmize()
{
    ::ShowWindow(m_hWnd, SW_MINIMIZE );
}
void CMainDlg::OnBkBtnClose()
{
	if ( m_bHidden )
	{
		this->ShowWindow( SW_HIDE );
	}
	else
	{
		DelTray();
		EndDialog(IDCANCEL);
	}
}

void CMainDlg::OnBkBtnMore()
{
    CPoint	pos;
    CRect	rc;
    GetClientRect(&rc);
    ClientToScreen(&rc);

    pos.x = rc.right - 108;
    pos.y = rc.top + 22;

    CMenu menuPopup;
    menuPopup.LoadMenu(IDR_MAINMENU);

    CMenuHandle Menu = menuPopup.GetSubMenu(0);;

    Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN, pos.x, pos.y, m_hWnd);    
}

LRESULT CMainDlg::OnMenuTray(UINT, INT code, HWND)
{
	switch ( code )
	{
	case ID_MENU_TRAY_EXIT:
		DelTray();
		EndDialog( 0 );
		break;
	case ID_MENU_TRAY_SHOW:
		SetForegroundWindow(m_hWnd);
		ShowWindow(SW_SHOW);
		BringWindowToTop();
		break;
	case ID_MENU_TRAY_CONTACT:
		SetForegroundWindow(m_hWnd);
		ShowWindow(SW_SHOW);
		BringWindowToTop();
		this->SetTabCurSel(IDC_TAB_GLOBAL, 5);
		break;
	case ID_MENU_TRAY_SITE:
		ShellExecute( m_hWnd, _T("open"), _T("http://mktime.org"), NULL, NULL, SW_HIDE );
		break;
	default:
		break;
	}
	return TRUE;
}
LRESULT CMainDlg::OnMenuMore(UINT, INT code, HWND)
{
	switch ( code )
	{
	case ID_MENU_MORE_CONTACT:
		/*MessageBox(_T("ID_MENU_MORE_CONTACT"));*/
		this->SetTabCurSel(IDC_TAB_GLOBAL, 5);
		break;
	case ID_MENU_MORE_ABOUT:
		/*MessageBox(_T("ID_MENU_MORE_ABOUT"));*/
		this->SetTabCurSel(IDC_TAB_GLOBAL, 0);
		break;
	default:
		break;
	}
	return TRUE;
}

// LRESULT CMainDlg::OnMenuAbout(UINT, INT, HWND)
// {
// 	//MessageBox(_T("OK"));
// 	this->SetTabCurSel(IDC_TAB_GLOBAL, 0);
// 	return 0;
// }
// LRESULT CMainDlg::OnMenuContact(UINT, INT, HWND)
// {
// 	//MessageBox(_T("OK"));
// 	this->SetTabCurSel(IDC_TAB_GLOBAL, 5);
// 	return 0;
// }


// LRESULT CMainDlg::OnChangeSkin(UINT, INT, HWND)
// {
// 	//MessageBox(_T("OK"));
// 	//BkSkin::LoadSkins( IDR_BK_SKIN_2 );
// 
// 	return 0;
// }
LRESULT CMainDlg::OnScClose(UINT, INT, HWND)
{
    DelTray();

    EndDialog(IDCANCEL);

    return 0;
}

LRESULT CMainDlg::OnCommand(UINT wParam, CPoint point)
{
    switch ( wParam  )
    {
    case SC_CLOSE:
		if ( m_bHidden )
		{
			this->ShowWindow( SW_HIDE );
		}
		else
		{
			DelTray();
			EndDialog( 0 );
		}
        break;
    }
    DefWindowProc();
    return 0;
}

void CMainDlg::InitTray()
{
	ZeroMemory(&m_NotifyIconData, sizeof m_NotifyIconData);
    if(!m_NotifyIconData.cbSize)
    {
        m_NotifyIconData.cbSize = NOTIFYICONDATAA_V1_SIZE;
        m_NotifyIconData.hWnd = m_hWnd;
        m_NotifyIconData.uID = 1;
        m_NotifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        m_NotifyIconData.uCallbackMessage = WM_SYSTEMTRAYICON;
        m_NotifyIconData.hIcon = AtlLoadIconImage(IDI_BEIKESAFE, LR_DEFAULTCOLOR, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));
        CString sWindowText;
        GetWindowText(sWindowText);
        _tcscpy_s(m_NotifyIconData.szTip, sWindowText);
        if(!Shell_NotifyIcon(NIM_ADD, &m_NotifyIconData))
        {
            SetMsgHandled(FALSE);
        }
    }
}

void CMainDlg::DelTray()
{
    if(m_NotifyIconData.cbSize)
    {
        Shell_NotifyIcon(NIM_DELETE, &m_NotifyIconData);
        ZeroMemory(&m_NotifyIconData, sizeof m_NotifyIconData);
    }
}