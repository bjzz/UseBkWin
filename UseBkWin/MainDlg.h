#pragma once

#include "CustomDrawButton.h"
#include "OwnDrawNetFlowButton.h"

// Forward Declare
class CNetworkDeviceListView;

class CMainDlg
	: public CBkDialogImpl<CMainDlg>,
	  public CWHRoundRectFrameHelper<CMainDlg>
{
public:
    CMainDlg();
	~CMainDlg();
	
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);

	void InitTray();
	void DelTray();

	LRESULT OnScClose(UINT, INT, HWND);
	LRESULT OnCommand(UINT wParam, CPoint point);
	LRESULT OnSystemTrayIcon(UINT, WPARAM wParam, LPARAM lParam);

	LRESULT OnMenuTray(UINT, INT, HWND);
	LRESULT OnMenuMore(UINT, INT, HWND);

    void OnBkBtnMinmize();
    void OnBkBtnClose();
    void OnBkBtnMore();
    
	void InitDeviceList();
	void InsertDeviceList();
	
	void InitProcessList();
	void InsertProcessList();
	void OnProcessListReload();

	void InitTcpConnectionList();
	void InsertTcpConnectionList();
	void OnTcpStatusReload();

	void InitUdpConnectionList();
	void InsertUdpConnectionList();
	void OnUdpStatusReload();

	void InitRouteList();
	void InsertRouteList();
	void OnRouteSttusReload();

	// Net stat window
	void InitNetStatus();

	// Net Flow Window
	void InitNetFlow();

	void OnSubmitConfig();
	void OnCancelConfig();

	// 网速绘制设置
	void OnConfigNetBgColor();
	void OnConfigNetGridLineColor();
	void OnConfigNetUpColor();
	void OnConfigNetDownColor();
	

	// 流量绘制设置
	void OnConfigFlowBgColor();
	void OnConfigNetFlowGridLineColor();
	void OnConfigFlowUpColor();
	void OnConfigFlowDownColor();
	
	// Auto Run App
	void SetAutoRun(BOOL bAutoRun);
	
	// Auto flush data
	void OnTimer(UINT_PTR nIDEvent);

	void UpgradeProcessPrivilege(LPCTSTR lpPrivilegeName);

protected:
    NOTIFYICONDATA		    m_NotifyIconData;
    CNetworkDeviceListView*	m_NetworkDeviceListView;
	CListViewCtrl			m_ProcessList;
	CListViewCtrl			m_TcpConnectionList;
	CListViewCtrl			m_UdpConnectionList;
	CListViewCtrl			m_RouteList;
	BOOL					m_bHidden;
	BOOL					m_bAutoFlush;
	CCustomDrawButton		m_NetWindow;
	COwnDrawNetFlowButton	m_NetFlow;

public:
    BK_NOTIFY_MAP(IDC_RICHVIEW_WIN)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_CLOSE, OnBkBtnClose)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_MIN, OnBkBtnMinmize)
        BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_MORE, OnBkBtnMore)
		BK_NOTIFY_ID_COMMAND(IDC_BTN_PROCESS_RELOAD, OnProcessListReload)
		BK_NOTIFY_ID_COMMAND(IDC_BTN_TCP_STATUS_RELOAD, OnTcpStatusReload)
		BK_NOTIFY_ID_COMMAND(IDC_BTN_UDP_STATUS_RELOAD, OnUdpStatusReload)
		BK_NOTIFY_ID_COMMAND(IDC_BTN_ROUTE_STATUS_RELOAD, OnRouteSttusReload)
		BK_NOTIFY_ID_COMMAND(IDC_BTN_CONFIG_SUBMIT, OnSubmitConfig)
		BK_NOTIFY_ID_COMMAND(IDC_BTN_CONFIG_CANCEL, OnCancelConfig)
		BK_NOTIFY_ID_COMMAND(IDC_BUTTON_CONFIG_SPEED_BG_COLOR, OnConfigNetBgColor)
		BK_NOTIFY_ID_COMMAND(IDC_BUTTON_CONFIG_SPEED_UP_COLOR, OnConfigNetUpColor)
		BK_NOTIFY_ID_COMMAND(IDC_BUTTON_CONFIG_SPEED_DOWN_COLOR, OnConfigNetDownColor)
		BK_NOTIFY_ID_COMMAND(IDC_BUTTON_CONFIG_FLOW_BG_COLOR, OnConfigFlowBgColor)
		BK_NOTIFY_ID_COMMAND(IDC_BUTTON_CONFIG_FLOW_UP_COLOR, OnConfigFlowUpColor)
		BK_NOTIFY_ID_COMMAND(IDC_BUTTON_CONFIG_FLOW_DOWN_COLOR, OnConfigFlowDownColor)
		BK_NOTIFY_ID_COMMAND(IDC_BUTTON_CONFIG_SPEED_GRID_LINE, OnConfigNetGridLineColor)
		BK_NOTIFY_ID_COMMAND(IDC_BUTTON_CONFIG_FLOW_GRID_LINE, OnConfigNetFlowGridLineColor)
    BK_NOTIFY_MAP_END()
    BEGIN_MSG_MAP_EX_DECLARE(CMainDlg)
};