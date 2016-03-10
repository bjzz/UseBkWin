#include "stdafx.h"
#include "NetworkDevice.h"

#define COL_DEFAULT_WHITE_BG		RGB(0xfb,0xfc,0xfd)
#define COL_DEFAULT_LINK			RGB(55,106,169)

#define __USE_BASE_LIST_VIEW		0

#define XX_RUN_LIST_ITEM_HEIGHT		45

#define XX_RUN_LIST_ITEM_HEIGHT_EXP	145
#define XX_RUN_LIST_ITEM_HEIGHT_COL	90

#define SUB_SHOW_TYPE_ALL			0
#define SUB_SHOW_TYPE_ENABLE		1
#define SUB_SHOW_TYPE_DISABLE		2

#define COL_STR_CPT_DEFAULT			RGB(0,0,0)
#define COL_STR_CPT_DISABLE			RGB(150,150,150)

#define COL_STR_STATE_ENABLE		RGB(0,153,0)
#define COL_STR_STATE_DISABLE		RGB(255,0,0)
#define COL_STR_ADVICE				RGB(0,0,0)
#define COL_STR_EXPAND				COL_STR_CPT_DISABLE

#define ICON_UNK_ID					-1
#define ICON_WIRED_ID				-2
#define ICON_WIRELESS_ID			-3
#define ICON_REMAIN					-4

#define TYPE_ITEM_MAIN				1
#define TYPE_ITEM_DESC				2
#define TYPE_ITEM_ADVICE			3
#define TYPE_ITEM_APPPATH			4

#define WM_USER_MSG_POST_CLICK		(WM_APP+1)

#define CLICK_INDEX_INVALID			-1
#define CLICK_INDEX_NULL			0
#define CLICK_INDEX_ENABLE			1
#define CLICK_INDEX_CHANGEIP		2
#define CLICK_INDEX_RECONFIGIP		3
#define CLICK_INDEX_CAPTION			4
#define CLICK_INDEX_DESC			5

class INetworkDeviceListViewCallback
{
public:
	virtual BOOL OnChangeDeviceStatus( LP_XY_NETWORK_DEVICE_INFO_DATA pNetworkDeviceInfoData) = 0;
	virtual BOOL OnChangeIPAddress(    LP_XY_NETWORK_DEVICE_INFO_DATA pNetworkDeviceInfoData) = 0;
	virtual BOOL OnReConfigIPAddress(  LP_XY_NETWORK_DEVICE_INFO_DATA pNetworkDeviceInfoData) = 0;
};

class CNetworkDeviceListView : 
#if __USE_BASE_LIST_VIEW
	public CWindowImpl<CNetworkDeviceListView, CListViewCtrl>
#else
	public CWindowImpl<CNetworkDeviceListView, CListBox>
#endif
	,public COwnerDraw<CNetworkDeviceListView>
{
public:
	DECLARE_WND_CLASS(_T("网卡列表自定义控件"))
	struct _NETWORK_DEVICE_ITEM_DATA
	{
        LP_XY_NETWORK_DEVICE_INFO_DATA		pNetworkDeviceInfoData;
        CRect								rcDeviceIcon;				// 每行刚开始的图标
        CRect								rcConnectionName;			// 网络连接的名称
        CRect								rcDriverDesc;				// DriverDesc/FriendlyName
        CRect								rcDHCPDesc;					// 显示DHCP信息的区域
        CRect								rcGateWay;
        CRect								rcDNSServer;
        CRect								rcSubNetMask;				// 显示子网掩码
        CRect								rcFullDriverPath;			// 显示驱动文件的完整路径

        CRect								rcEnableButton;				// 启用/禁用网卡的按钮
        CRect								rcChangeIPButton;			// 修改IP地址的按钮
        CRect								rcReConfigIPButton;			// 重新配置IP地址

        DWORD								nType;
        BOOL								bExpand;					// 是否展开

		_NETWORK_DEVICE_ITEM_DATA()
		{
			pNetworkDeviceInfoData	= NULL;
			rcDeviceIcon			= CRect(0,0,0,0);
			rcConnectionName		= CRect(0,0,0,0);
			rcDriverDesc			= CRect(0,0,0,0);
			rcDHCPDesc				= CRect(0,0,0,0);
			rcSubNetMask			= CRect(0,0,0,0);
			rcGateWay				= CRect(0,0,0,0);
			rcDNSServer				= CRect(0,0,0,0);
			rcFullDriverPath		= CRect(0,0,0,0);
			rcEnableButton			= CRect(0,0,0,0);
			rcChangeIPButton		= CRect(0,0,0,0);
			rcReConfigIPButton		= CRect(0,0,0,0);
			nType					= TYPE_ITEM_MAIN;
			bExpand					= FALSE;
		}

	};

	CNetworkDeviceListView()
	{
		m_rcDeviceIcon			= CRect(10,(XX_RUN_LIST_ITEM_HEIGHT-32)/2,10+32,(XX_RUN_LIST_ITEM_HEIGHT+32)/2);
		m_rcConnectionName		= CRect(58,8,108,20);
		//m_rcDriverDesc			= CRect(58,28,108,40);
		m_rcDriverDesc			= CRect(58,28,108,40);
		m_rcDHCPDesc			= CRect(58,48,108,60);
		m_rcGateWay				= CRect(58,68,108,80);
		m_rcDNSServer			= CRect(58,88,108,100);
		m_rcSubNetMask			= CRect(58,108,108,120);
		m_rcFullDriverPath		= CRect(58,128,108,140);
		m_rcEnableButton		= CRect(58,60,108,80);
		//m_rcEnableButton		= CRect(108,90,128,100);
		m_rcChangeIPButton		= CRect(58,60,108,80);
		m_rcReConfigIPButton	= CRect(58,100,108,120);

		m_opCallback			= NULL;
		m_nSubShowType			= SUB_SHOW_TYPE_ALL;

		m_hWiredAdapter			= NULL;
		m_hWirelessAdapter		= NULL;
		m_hUnknownAdapter		= NULL;
		m_pHoverTip				= 0;
	}

	~CNetworkDeviceListView(void)
	{
		ClearAll();
	
		if (m_hWiredAdapter!=NULL)
		{
			::DestroyIcon(m_hWiredAdapter);
			m_hWiredAdapter = NULL;
		}

		if (m_hWirelessAdapter!=NULL)
		{
			::DestroyIcon(m_hWirelessAdapter);
			m_hWirelessAdapter = NULL;
		}

		if (m_hUnknownAdapter!=NULL)
		{
			::DestroyIcon(m_hUnknownAdapter);
			m_hUnknownAdapter = NULL;
		}
	}

protected:
	CSimpleArray<_NETWORK_DEVICE_ITEM_DATA*>		m_listArray;
	HICON											m_hWiredAdapter;
	HICON											m_hWirelessAdapter;
	HICON											m_hUnknownAdapter;
	CSimpleArray<HICON>								m_listICON;

	CRect											m_rcDeviceIcon;
	CRect											m_rcConnectionName;
	CRect											m_rcDriverDesc;
	CRect											m_rcDHCPDesc;
	CRect											m_rcIPAddress;
	CRect											m_rcDNSServer;
	CRect											m_rcSubNetMask;
	CRect											m_rcGateWay;
	CRect											m_rcFullDriverPath;
	CRect											m_rcEnableButton;
	CRect											m_rcChangeIPButton;
	CRect											m_rcReConfigIPButton;

	INetworkDeviceListViewCallback*					m_opCallback;
	DWORD											m_nSubShowType;
	CToolTipCtrl									m_wndToolTip;
	DWORD											m_pHoverTip;
	CBrush											m_hBGBrush;

public:
	BEGIN_MSG_MAP(CNetworkDeviceListView)   
		MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST, WM_MOUSELAST, OnToolTipEvent)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_RBUTTONUP(OnRButtonUp)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_SIZE(OnSize)
		MSG_OCM_CTLCOLORLISTBOX(OnCtlColor)
		MESSAGE_HANDLER_EX(WM_USER_MSG_POST_CLICK,OnMsgPostClick)
		CHAIN_MSG_MAP_ALT(COwnerDraw<CNetworkDeviceListView>,1)
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()   

	LRESULT	OnCtlColor(HDC hDc, HWND hWnd)
	{
		if (hWnd==m_hWnd)
		{
			CDCHandle	dchan;
			dchan.Attach(hDc);

			dchan.SetBkMode(TRANSPARENT);
			SetMsgHandled(TRUE);

			if (m_hBGBrush.m_hBrush==NULL)
				m_hBGBrush.CreateSolidBrush(COL_DEFAULT_WHITE_BG);

			dchan.Detach();
			return (LRESULT)m_hBGBrush.m_hBrush;
		}
		SetMsgHandled(FALSE);
		return NULL;
	}

	LRESULT OnMsgPostClick(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		MsgDoExp((int)wParam);
		return S_OK;
	}

	LRESULT OnToolTipEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if(m_wndToolTip.IsWindow())
		{
			MSG msg = { m_hWnd, uMsg, wParam, lParam };

			m_wndToolTip.RelayEvent(&msg);
		}

		SetMsgHandled(FALSE);

		return 0;
	}

	void SetOpCallback(INetworkDeviceListViewCallback* pCallback)
	{
		m_opCallback = pCallback;
	}

	void GetNetworkDeviceInfoDataArray(CSimpleArray<LP_XY_NETWORK_DEVICE_INFO_DATA>& arrayx)
	{
		for (int i=0; i < m_listArray.GetSize(); i++)
		{
			if (m_listArray[i]->pNetworkDeviceInfoData )
			{
				arrayx.Add(m_listArray[i]->pNetworkDeviceInfoData);
			}
		}
	}

	void OnSize(UINT nType, CSize size)
	{
		SetMsgHandled(FALSE);
	}

	void CreateX()
	{
		m_wndToolTip.Create(m_hWnd);
		CToolInfo ti(0, m_hWnd);
		m_wndToolTip.AddTool(ti);
		m_wndToolTip.Activate(TRUE);
		m_wndToolTip.SetMaxTipWidth(500);
		m_wndToolTip.SetDelayTime(TTDT_AUTOPOP,5*1000);
		m_wndToolTip.SetDelayTime(TTDT_INITIAL,500);
	}

	int XBASE_HitTest(CPoint ptx)
	{
#if __USE_BASE_LIST_VIEW
		LVHITTESTINFO hti;
		hti.pt = ptx;
		SubItemHitTest( &hti );
		return hti.iItem;
#else
		BOOL	bOut = FALSE;
		int i = (int)CListBox::ItemFromPoint(ptx,bOut);

		if (!bOut)
			return i;
		else
			return -1;
#endif

	}

	int XBASE_InsertItem(int iIndex, LPCTSTR lpstr)
	{
#if __USE_BASE_LIST_VIEW
		return CListViewCtrl::InsertItem(iIndex,lpstr);
#else
		return CListBox::InsertString(iIndex,lpstr);
#endif
	}

	BOOL XBASE_GetItemRect(int i, LPRECT lprc)
	{
#if __USE_BASE_LIST_VIEW
		return CListViewCtrl::GetItemRect(i,lprc,LVIR_BOUNDS);
#else
		return CListBox::GetItemRect(i,lprc);
#endif
	}


	BOOL XBASE_DeleteItem(int i)
	{
#if __USE_BASE_LIST_VIEW
		return CListViewCtrl::DeleteItem(i);
#else
		return CListBox::DeleteString((UINT)i);
#endif
	}

	int XBASE_GetItemCount()
	{
#if __USE_BASE_LIST_VIEW
		return CListViewCtrl::GetItemCount();
#else
		return CListBox::GetCount();
#endif
	}

	BOOL XBASE_DeleteAllItems()
	{
#if __USE_BASE_LIST_VIEW
		return CListViewCtrl::DeleteAllItems();
#else
		CListBox::ResetContent();
		return TRUE;
#endif
	}

	DWORD_PTR XBASE_GetItemData(int nItem)
	{
#if __USE_BASE_LIST_VIEW
		return CListViewCtrl::GetItemData(nItem);
#else
		DWORD_PTR pvoid = CListBox::GetItemData(nItem);

		if ((DWORD)pvoid==-1)
			return NULL;
		return pvoid;
#endif
	}

	BOOL XBASE_SetItemData(int nItem,DWORD_PTR pdata)
	{
#if __USE_BASE_LIST_VIEW
		return CListViewCtrl::SetItemData(nItem,pdata);
#else
		return CListBox::SetItemData(nItem,pdata);
#endif
	}

	BOOL XBASE_SetItemText(int nItem, LPCTSTR lpstr)
	{
#if __USE_BASE_LIST_VIEW
		return CListViewCtrl::SetItemText(nItem,0,lpstr);
#else
		return FALSE;
#endif
	}

	int LabelHitTest(CPoint ptx)
	{
		int		iItem = XBASE_HitTest(ptx);

		CRect	rcItem;
		_NETWORK_DEVICE_ITEM_DATA* pdata = (_NETWORK_DEVICE_ITEM_DATA*)XBASE_GetItemData(iItem);

		if (pdata==NULL)
			return CLICK_INDEX_INVALID;
		
		XBASE_GetItemRect(iItem,&rcItem);
		ptx.Offset(-rcItem.left,-rcItem.top);

		if (pdata->rcEnableButton.PtInRect(ptx))
			return CLICK_INDEX_ENABLE;
		else if (pdata->rcChangeIPButton.PtInRect(ptx))
			return CLICK_INDEX_CHANGEIP;
		else if (pdata->rcReConfigIPButton.PtInRect(ptx))
			return CLICK_INDEX_RECONFIGIP;
		else
			return CLICK_INDEX_NULL;
	}

	// 通过修改这里自定义回调函数
	void OnLButtonDown(UINT uMsg, CPoint ptx)
	{
		BOOL	bHandle = FALSE;
		int		iItem = XBASE_HitTest(ptx);

		_NETWORK_DEVICE_ITEM_DATA* pdata = (_NETWORK_DEVICE_ITEM_DATA*)XBASE_GetItemData(iItem);

		if (pdata!=NULL && pdata->pNetworkDeviceInfoData )
		{
			LP_XY_NETWORK_DEVICE_INFO_DATA	pNetworkDeviceInfoData = pdata->pNetworkDeviceInfoData;
			int iIndex = LabelHitTest(ptx);

			if (iIndex == CLICK_INDEX_ENABLE)
			{
				//m_opCallback->OnChangeDeviceStatus(pNetworkDeviceInfoData);
				bHandle	= TRUE;
			}
			else if(iIndex== CLICK_INDEX_CHANGEIP )
			{
				//m_opCallback->OnChangeIPAddress(pNetworkDeviceInfoData);
				bHandle	= TRUE;
			}
			else if(iIndex == CLICK_INDEX_RECONFIGIP)
			{
				//m_opCallback->OnReConfigIPAddress(pNetworkDeviceInfoData);
				bHandle = TRUE;
			}
			
			if (!bHandle)
			{
				MsgDoExp(iItem);
			}
			
		}
	
		SetMsgHandled(bHandle);
		return;
	}

	void OnRButtonUp(UINT uMsg, CPoint ptx)
	{
		/* 鼠标右键暂时不处理
		int i = XBASE_HitTest(ptx);
		_NETWORK_DEVICE_ITEM_DATA* pdata = (_NETWORK_DEVICE_ITEM_DATA*)XBASE_GetItemData(i);
		if (i>=0 && pdata)
		{
			int iIndex = LabelHitTest(ptx);
			if (iIndex!=CLICK_INDEX_ENABLE&&
				iIndex!=CLICK_INDEX_OPEN_FOLDER)
			{
				CString	strFormat;
				if (pdata->pInfo->nType==KSRUN_TYPE_SERVICE)
					strFormat = BkString::Get(
                        (pdata->pInfo->bEnable) ? IDS_SYSOPT_4027
                                                : IDS_SYSOPT_4028);
				else
					strFormat = BkString::Get(
                        (pdata->pInfo->bEnable) ? IDS_SYSOPT_4029
                                                : IDS_SYSOPT_4030);

				CMenu	menu;
				menu.CreatePopupMenu();
				menu.AppendMenu(MF_BYCOMMAND,CLICK_INDEX_OPEN_FOLDER,BkString::Get(IDS_SYSOPT_4031));
				menu.AppendMenu(MF_SEPARATOR);
				menu.AppendMenu(MF_BYCOMMAND,CLICK_INDEX_ENABLE,strFormat);
				//针对服务项，不提供“删除此项”LinkText
				if (pdata->pInfo->nType != 2)
					menu.AppendMenu(MF_BYCOMMAND,CLICK_INDEX_DELETE,BkString::Get(IDS_SYSOPT_4032));
				else
				{
					if (!pdata->pInfo->bFileIsExist)
						menu.AppendMenu(MF_BYCOMMAND,CLICK_INDEX_DELETE,BkString::Get(IDS_SYSOPT_4032));
				}

				CPoint	ptx;
				::GetCursorPos(&ptx);
				int i = menu.TrackPopupMenu(TPM_RETURNCMD,ptx.x,ptx.y, m_hWnd);

				if (i==CLICK_INDEX_ENABLE||
					i==CLICK_INDEX_DELETE||
					i==CLICK_INDEX_OPEN_FOLDER)
				{
					m_opCallback->OnClickLabel(i,pdata->pInfo);
				}
			}
		}
		*/
		SetMsgHandled(TRUE);
	}

	void OnLButtonUp(UINT uMsg, CPoint ptx)
	{
		SetMsgHandled(FALSE);
	}

	void MsgDoExp(int iItem)
	{
		_NETWORK_DEVICE_ITEM_DATA* pdata = (_NETWORK_DEVICE_ITEM_DATA*)XBASE_GetItemData(iItem);
		
		SetRedraw(FALSE);

		CPoint	ptx;
		::GetCursorPos(&ptx);

		CRect	rcOffset(0,0,0,0);
		ScreenToClient(rcOffset);
		ptx.Offset(rcOffset.left,rcOffset.top);
		int nShowIndex = XBASE_HitTest(ptx);
		BOOL bExpand = pdata->bExpand;
	
		ExpandAllItem(FALSE);

		if ( true )
		{
			if (!bExpand)
			{
				ExpandItem(iItem);
			}
			else
			{
				ExpandItem(iItem,FALSE);
			}
		}

		SetRedraw(TRUE);
		InvalidateRect(NULL);
	}

	void ExpandAllItem(BOOL bExp=TRUE)
	{
		for (int i=0; i < XBASE_GetItemCount(); i++)
		{
			ExpandItem(i,bExp);
		}
	}

	// !TODO:这里需要调试
	void ExpandItem(int iItem, BOOL bExp = TRUE)//TRUE
	{
		_NETWORK_DEVICE_ITEM_DATA* pdata = (_NETWORK_DEVICE_ITEM_DATA*)XBASE_GetItemData(iItem);
		
#if __USE_BASE_LIST_VIEW
		if (bExp && !pdata->bExpand)		// 展开未展开的项
		{
			if (TRUE)
			{
				_NETWORK_DEVICE_ITEM_DATA* pNewData = new _NETWORK_DEVICE_ITEM_DATA(*pdata);
				pNewData->nType = TYPE_ITEM_DESC;
				XBASE_InsertItem(iItem+1,NULL);
				XBASE_SetItemData(iItem+1, (DWORD_PTR)pNewData);
			}
			if (TRUE)
			{
				_NETWORK_DEVICE_ITEM_DATA* pNewData = new _NETWORK_DEVICE_ITEM_DATA(*pdata);
				pNewData->nType = TYPE_ITEM_ADVICE;
				XBASE_InsertItem(iItem+2,NULL);
				XBASE_SetItemData(iItem+2, (DWORD_PTR)pNewData);
			}
			if (FALSE)
			{
				_NETWORK_DEVICE_ITEM_DATA* pNewData = new _NETWORK_DEVICE_ITEM_DATA(*pdata);
				pNewData->nType = TYPE_ITEM_APPPATH;
				XBASE_InsertItem(iItem+3,NULL);
				XBASE_SetItemData(iItem+3, (DWORD_PTR)pNewData);
			}
			pdata->bExpand = TRUE;
		}
		else if ( !bExp && pdata->bExpand )	// 收缩已展开的项
		{
			if (TRUE)
			{
				_NETWORK_DEVICE_ITEM_DATA* pNewdata = (_NETWORK_DEVICE_ITEM_DATA*)XBASE_GetItemData(iItem+1);
				delete pNewdata;
				XBASE_DeleteItem(iItem+1);
			}
			if (TRUE)
			{
				_NETWORK_DEVICE_ITEM_DATA* pNewdata = (_NETWORK_DEVICE_ITEM_DATA*)XBASE_GetItemData(iItem+1);
				delete pNewdata;
				XBASE_DeleteItem(iItem+1);
			}
			if (FALSE)
			{
				_NETWORK_DEVICE_ITEM_DATA* pNewdata = (_NETWORK_DEVICE_ITEM_DATA*)XBASE_GetItemData(iItem+1);
				delete pNewdata;
				XBASE_DeleteItem(iItem+1);
			}
			pdata->bExpand = FALSE;
		}

#else
		if ( bExp && !pdata->bExpand)
		{
			pdata->bExpand = bExp;
			CListBox::SetItemHeight(iItem,XX_RUN_LIST_ITEM_HEIGHT_EXP);
		}
		else if ( !bExp && pdata->bExpand )
		{
			pdata->bExpand = bExp;
			CListBox::SetItemHeight(iItem,XX_RUN_LIST_ITEM_HEIGHT_COL);
		}
#endif
	}

	XY_NETWORK_DEVICE_INFO_DATA* GetDataInfo(int nItem)
	{
		_NETWORK_DEVICE_ITEM_DATA*	pParam = (_NETWORK_DEVICE_ITEM_DATA*)XBASE_GetItemData(nItem);

		if (pParam!=NULL && (DWORD)pParam != -1 )
			return pParam->pNetworkDeviceInfoData ;
		else
			return NULL;
	}

	void OnMouseMove(UINT uMsg, CPoint ptx)
	{
		int			iItem = XBASE_HitTest(ptx);

		XY_NETWORK_DEVICE_INFO_DATA*	kinfo		= GetDataInfo(iItem);
		_NETWORK_DEVICE_ITEM_DATA*	pItemData	= (_NETWORK_DEVICE_ITEM_DATA*)XBASE_GetItemData(iItem);
		
		if (kinfo && pItemData)
		{
			int iIndex = LabelHitTest(ptx);
			if (iIndex == CLICK_INDEX_ENABLE)
				::SetCursor(::LoadCursor(NULL,IDC_HAND));
			else if (iIndex == CLICK_INDEX_CHANGEIP)
				::SetCursor(::LoadCursor(NULL,IDC_HAND));
			else if (iIndex == CLICK_INDEX_RECONFIGIP )
				::SetCursor(::LoadCursor(NULL,IDC_HAND));			
			
			/* !TODO:这里需要修改
			if (iIndex == CLICK_INDEX_DESC||
				iIndex == CLICK_INDEX_LBL_FOLDER||
				iIndex == CLICK_INDEX_CAPTION)
			{
				if ( ((DWORD)kinfo!=m_pHoverTip) && !kinfo->strExePath.IsEmpty() )
				{
					CString	strInfo;

					if (iIndex == CLICK_INDEX_DESC)
					{
						strInfo.Append( kinfo->strDesc.IsEmpty()?BkString::Get(IDS_SYSOPT_4033):kinfo->strDesc );
						if (!kinfo->bFileIsExist)
							strInfo.Format(BkString::Get(IDS_SYSOPT_4064),CString(strInfo));
					}
					else if (iIndex == CLICK_INDEX_LBL_FOLDER )
						strInfo.Append( kinfo->strParam );
					else if (iIndex == CLICK_INDEX_CAPTION)
					{
						if (kinfo->bFileIsExist)
							strInfo.Append( kinfo->strDisplay );
						else
							strInfo.Format(BkString::Get(IDS_SYSOPT_4061),kinfo->strDisplay);
					}

					m_wndToolTip.SetMaxTipWidth(500);
					m_wndToolTip.UpdateTipText((LPCTSTR)strInfo,m_hWnd);
					m_pHoverTip = (DWORD)kinfo;
				}
			}
			else
			{
				if (m_pHoverTip!=0)
				{
					m_pHoverTip = 0;
					m_wndToolTip.UpdateTipText((LPCTSTR)NULL,m_hWnd);
					m_wndToolTip.Pop();
				}
			}
			*/
		}
	}

	void DeleteItem(LPDELETEITEMSTRUCT lParam)
	{
		return;
	}

	HICON FindIcon(LP_XY_NETWORK_DEVICE_INFO_DATA pinfo)
	{
		int nIndex = pinfo->nNetworkType;

		switch(nIndex)
		{
		case WIRED_NETWORK_DEVICE:
			if (m_hWiredAdapter==NULL)
				m_hWiredAdapter = ::LoadIcon((HMODULE)&__ImageBase, MAKEINTRESOURCE(IDI_WIRED_NETWORK));
			return m_hWiredAdapter;
			break;
		case WIRELESS_NETWORK_DEVICE:
			if (m_hWirelessAdapter==NULL)
				m_hWirelessAdapter = ::LoadIcon((HMODULE)&__ImageBase, MAKEINTRESOURCE(IDI_WIRELESS_NETWORK));
			return m_hWirelessAdapter;
			break;
		case PPPoE_NETWORK_DEVICE:
			if (m_hUnknownAdapter==NULL)
				m_hUnknownAdapter = ::LoadIcon((HMODULE)&__ImageBase, MAKEINTRESOURCE(IDI_PPPOE_NETWORK));
			return m_hUnknownAdapter;
			break;
		case VIRTUAL_NETWORK_DEVICE:
			if (m_hUnknownAdapter==NULL)
				m_hUnknownAdapter = ::LoadIcon((HMODULE)&__ImageBase, MAKEINTRESOURCE(IDI_VIRTUAL_NETWORK));
			return m_hUnknownAdapter;
			break;
		}

		if (m_listICON.GetSize() > nIndex && nIndex>=0 )
			return m_listICON[nIndex];
		else
			return NULL;
		
		return NULL;
	}

	int GetItemIndexByNetworkDeviceInfoData(XY_NETWORK_DEVICE_INFO_DATA* pdata)
	{
		for ( int i=0; i<XBASE_GetItemCount(); i++)
		{
			if (GetDataInfo(i)==pdata)
				return i;
		}
		return -1;
	}

	void calcLeftRECT(CRect rcArea, CRect rcProbe, CRect& rcOut)
	{
		rcOut.left		= rcArea.left;
		rcOut.right		= rcOut.left + rcProbe.Width();
		rcOut.top		= rcArea.top;
		rcOut.bottom	= rcOut.top + rcProbe.Height();
	}

	void calcCenterRECT(CRect rcArea, CRect rcProbe, CRect& rcOut)
	{
		rcOut.left		= rcArea.left + (rcArea.Width()-rcProbe.Width())/2;
		rcOut.right		= rcOut.left + rcProbe.Width();
		rcOut.top		= rcArea.top + (rcArea.Height()-rcProbe.Height())/2;
		rcOut.bottom	= rcOut.top + rcProbe.Height();
	}

	// !TODO:这里是关键
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		_NETWORK_DEVICE_ITEM_DATA*		pItemData				= (_NETWORK_DEVICE_ITEM_DATA*)XBASE_GetItemData(lpDrawItemStruct->itemID);
		XY_NETWORK_DEVICE_INFO_DATA*	pNetworkDeviceInfoData	= GetDataInfo(lpDrawItemStruct->itemID);
		CDCHandle						dcx;
		COLORREF						clrOld;
		CRect							rcWin;

		GetWindowRect(&rcWin);
		dcx.Attach(lpDrawItemStruct->hDC);
		HFONT fntdef = BkFontPool::GetFont(BKF_DEFAULTFONT);
		HFONT fntBold = BkFontPool::GetFont(BKF_BOLDFONT);
		HFONT fntdeftmp = dcx.SelectFont(fntdef);
		clrOld = dcx.GetTextColor();

		if (pNetworkDeviceInfoData && pItemData)
		{
			CRect	rcItemX = lpDrawItemStruct->rcItem;

			rcItemX.right = rcItemX.left + rcWin.Width();
			
			if (lpDrawItemStruct->itemState&ODS_SELECTED)
				dcx.FillSolidRect(&rcItemX,RGB(243,246,250));
			else
				dcx.FillSolidRect(&rcItemX,COL_DEFAULT_WHITE_BG);

			DWORD	HEIGHT_BUTTONS		= 60;		// 两排按钮加起来一共占据的高度，包括按钮之间的空隙
			DWORD	LENGTH_STATE		= 60;		// 已启用/已禁用/修改IP地址等的显示位置的长度
			DWORD	LENGTH_ACTION		= 60;		// 禁用网卡/启用网卡/重新配置网络等的显示位置的长度
			DWORD	HEIGHT_BW_MARGIN	= 20;		// 禁用网卡和重新配置网络等之间的空隙
			DWORD	LENGTH_BW_MARGIN	= 20;		// STATE和ACTION之间的空隙的长度
			DWORD	LENGTH_RIGHT_MARGIN	= 5;		// 跟最右侧边缘的空隙长度

			DWORD	nTLength	= LENGTH_STATE + LENGTH_ACTION + LENGTH_BW_MARGIN + LENGTH_RIGHT_MARGIN;

			CRect	rcFirst;
			CRect	rcSecond;
			int		nSecWidth = nTLength+ LENGTH_BW_MARGIN; 

			rcFirst			= rcItemX;
			rcFirst.right	-= nSecWidth;
			rcSecond		= rcItemX;
			rcSecond.left	= rcFirst.right;

			if (pItemData->nType == TYPE_ITEM_MAIN )
			{
				// ------------------------------------ 绘制开头的图标 ----------------------------------------------------------------
				HICON	hIcon = FindIcon(pNetworkDeviceInfoData);
				if (hIcon)
				{
					CRect	rcIcon=m_rcDeviceIcon;
					rcIcon.OffsetRect(rcFirst.left,rcFirst.top);
					::DrawIconEx(dcx,rcIcon.left,rcIcon.top,hIcon,32,32,0,NULL,DI_NORMAL);
				}

				// 设置背景
				dcx.SetTextColor(pNetworkDeviceInfoData->bDeviceEnabled ?COL_STR_CPT_DEFAULT:COL_STR_CPT_DISABLE);
				dcx.SetBkMode(TRANSPARENT);

				
				CRect	rcCaption=m_rcConnectionName;
				CRect	rcProbe;
				CString	strShow;
				CRect	rcDriverDesc;
				CRect	rcDHCPDesc;
				CRect	rcGateWay;
				CRect	rcDNSServer;
				CRect	rcSubNetMask;

				CRect	rcEnabeDevice;
				CRect	rcChangeIPAddress;
				CRect	rcReConfigIPAddress;
				// ---------------------------------------- 绘制前面的信息区 ------------------------------------------------
				if (TRUE)
				{
					// --------------------------------  绘制网络连接名称 --------------------------------------------------
					
					rcCaption.left+=rcFirst.left;
					rcCaption.top+=rcFirst.top;
					rcCaption.bottom=rcCaption.top+m_rcConnectionName.Height();
					rcCaption.right=rcFirst.right;

					fntdef = dcx.SelectFont(fntBold);//启动项标题字体加粗
					
					dcx.DrawText(pNetworkDeviceInfoData->pszConnectionName ,-1,&rcProbe,DT_CALCRECT|DT_VCENTER|DT_SINGLELINE|DT_LEFT);
					dcx.DrawText(pNetworkDeviceInfoData->pszConnectionName,-1,&rcCaption,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_LEFT);
					
					dcx.SelectFont(fntdef);	//恢复默认字体
					m_rcConnectionName.right = rcFirst.Width();

					rcProbe.bottom = rcProbe.top+12;
					rcCaption.OffsetRect(-rcFirst.left, -rcFirst.top);
					calcLeftRECT(rcCaption,rcProbe,pItemData->rcConnectionName);		

					// ---------------------------------------- 显示网卡的DriverDesc/FirendlyName -------------------------------------
					rcDriverDesc	= m_rcDriverDesc;
					rcDriverDesc.left		+=rcFirst.left;
					rcDriverDesc.top		+=rcFirst.top;
					rcDriverDesc.bottom	=rcDriverDesc.top +m_rcDriverDesc.Height();
					rcDriverDesc.right	=rcFirst.right;

									
					strShow.Format(_T("%s"), pNetworkDeviceInfoData->pszDeviceFriendlyDesc.IsEmpty()?pNetworkDeviceInfoData->pszDeviceDesc:pNetworkDeviceInfoData->pszDeviceFriendlyDesc);

					dcx.DrawText(strShow,-1,&rcProbe,DT_CALCRECT|DT_VCENTER|DT_SINGLELINE|DT_LEFT);
					dcx.DrawText(strShow,-1,&rcDriverDesc,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_LEFT);

					rcProbe.bottom = rcProbe.top+12;
					rcDriverDesc.OffsetRect(-rcFirst.left, -rcFirst.top);
					calcLeftRECT(rcDriverDesc,rcProbe,pItemData->rcDriverDesc);

					// ------------------------------------------ 显示DHCP的信息 -------------------------------------------------------
					rcDHCPDesc			= m_rcDHCPDesc;
					rcDHCPDesc.left		+=rcFirst.left;
					rcDHCPDesc.top		+=rcFirst.top;
					rcDHCPDesc.bottom	=rcDHCPDesc.top +m_rcDHCPDesc.Height();
					rcDHCPDesc.right	=rcFirst.right;

					CString	pszDHCPDesc;
					if(pNetworkDeviceInfoData->bUsingDHCP)
					{						
						//pszDHCPDesc.Format(_T("DHCP服务器%s分配的动态地址：%s"),pNetworkDeviceInfoData->pszDHCPIPAddress,pNetworkDeviceInfoData->pszIPAddress);
						pszDHCPDesc.Format(_T("DHCP服务器%s分配的动态地址：%s"),pNetworkDeviceInfoData->pszDHCPIPAddress,pNetworkDeviceInfoData->pszIPAddress);
					}
					else
					{
						pszDHCPDesc.Format(_T("IP地址：%s"),pNetworkDeviceInfoData->pszIPAddress);
					}

					dcx.DrawText(pszDHCPDesc,-1,&rcProbe,DT_CALCRECT|DT_VCENTER|DT_SINGLELINE|DT_LEFT);
					dcx.DrawText(pszDHCPDesc,-1,&rcDHCPDesc,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_LEFT);

					rcProbe.bottom = rcProbe.top+12;
					rcDHCPDesc.OffsetRect(-rcFirst.left, -rcFirst.top);
					calcLeftRECT(rcDHCPDesc,rcProbe,pItemData->rcDHCPDesc );

					// ------------------------------------------ 显示网关地址的信息 -------------------------------------------------------
					rcGateWay				= m_rcGateWay;
					rcGateWay.left			+= rcFirst.left;
					rcGateWay.top			+= rcFirst.top;
					rcGateWay.bottom		= rcGateWay.top +m_rcGateWay.Height();
					rcGateWay.right			= rcFirst.right;

					CString	pszGateway;

					pszGateway.Format(_T("网关地址：%s"),pNetworkDeviceInfoData->pszGateway );


					dcx.DrawText(pszGateway,-1,&rcProbe,DT_CALCRECT|DT_VCENTER|DT_SINGLELINE|DT_LEFT);
					dcx.DrawText(pszGateway,-1,&rcGateWay,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_LEFT);

					rcProbe.bottom = rcProbe.top+12;
					rcGateWay.OffsetRect(-rcFirst.left, -rcFirst.top);
					calcLeftRECT(rcGateWay,rcProbe,pItemData->rcGateWay );
				}
				
				// 绘制按钮区域
				if (FALSE)
				{
					CRect	rcState		= rcSecond;
					CString	strState;

					rcState.left	= rcState.left + rcSecond.Width() - nTLength;
					rcState.right	= rcState.left+LENGTH_STATE;

					if ( !pItemData->bExpand)
					{
						rcState.top		= rcSecond.top+(XX_RUN_LIST_ITEM_HEIGHT_COL-HEIGHT_BUTTONS)/2;
						rcState.bottom	= rcState.top+	(HEIGHT_BUTTONS/2);		// 有两排要显示
					}
					else
					{
						rcState.top		+= m_rcConnectionName.top;
						rcState.bottom	= rcState.top + (HEIGHT_BUTTONS/2);
					}
					
					dcx.SetTextColor(pNetworkDeviceInfoData->bDeviceEnabled ?COL_STR_STATE_ENABLE:COL_STR_STATE_DISABLE);

					if(pNetworkDeviceInfoData->nNetworkType == PPPoE_NETWORK_DEVICE)
					{
						strState.Format(_T("%s"), pNetworkDeviceInfoData->bDeviceEnabled?_T("宽带已连接"):_T("宽带已断开"));
					}
					else
					{
						strState.Format(_T("%s"), pNetworkDeviceInfoData->bDeviceEnabled?_T("网卡已启用"):_T("网卡已禁用"));
					}

					dcx.DrawText(strState,-1,&rcState,DT_VCENTER|DT_SINGLELINE|DT_CENTER);

					// -------------------------------- 绘制 修改IP地址 按钮 -------------------------------------------
					rcChangeIPAddress			= rcState;
					rcChangeIPAddress.top		= rcState.bottom;
					rcChangeIPAddress.bottom	= rcChangeIPAddress.top + (HEIGHT_BUTTONS/2);

					dcx.SetTextColor(COL_DEFAULT_LINK);

					strState.Format(_T("%s"),_T("修改IP") );


					HFONT hfnt = BkFontPool::GetFont(FALSE,TRUE,FALSE);
					HFONT hTmp = dcx.SelectFont(hfnt);
					//dcx.DrawText(strState,-1,&rcChangeIPAddress,DT_VCENTER|DT_SINGLELINE|DT_CENTER);
					dcx.SelectFont(hTmp);
					pItemData->rcChangeIPButton = rcChangeIPAddress;
					pItemData->rcChangeIPButton.OffsetRect(-lpDrawItemStruct->rcItem.left,-lpDrawItemStruct->rcItem.top);

					// -------------------------------- 绘制 禁用网卡/启用网卡 按钮 -------------------------------------------
					rcEnabeDevice		= rcState;
					rcEnabeDevice.left	= rcState.right + LENGTH_BW_MARGIN;
					rcEnabeDevice.right	= rcEnabeDevice.left + LENGTH_STATE;

					dcx.SetTextColor(COL_DEFAULT_LINK);

					if(pNetworkDeviceInfoData->nNetworkType == PPPoE_NETWORK_DEVICE)
					{
						strState.Format(_T("%s"),_T("断开连接") );
					}
					else
					{
						if(pNetworkDeviceInfoData->bDeviceEnabled)
							strState.Format(_T("%s"),_T("禁用网卡") );
						else
							strState.Format(_T("%s"),_T("启用网卡") );
					}
					
					
					dcx.DrawText(strState,-1,&rcEnabeDevice,DT_VCENTER|DT_SINGLELINE|DT_CENTER);
					dcx.SelectFont(hTmp);
					pItemData->rcEnableButton = rcEnabeDevice;
					pItemData->rcEnableButton.OffsetRect(-lpDrawItemStruct->rcItem.left,-lpDrawItemStruct->rcItem.top);

					// -------------------------------- 绘制 重配网络 -------------------------------------------
					rcReConfigIPAddress			= rcEnabeDevice;
					rcReConfigIPAddress.top		= rcEnabeDevice.bottom;
					rcReConfigIPAddress.bottom	= rcReConfigIPAddress.top + (HEIGHT_BUTTONS/2);

					dcx.SetTextColor(COL_DEFAULT_LINK);

					strState.Format(_T("%s"),_T("重配网络") );

					//dcx.DrawText(strState,-1,&rcReConfigIPAddress,DT_VCENTER|DT_SINGLELINE|DT_CENTER);
					dcx.SelectFont(hTmp);
					pItemData->rcReConfigIPButton = rcReConfigIPAddress;
					pItemData->rcReConfigIPButton.OffsetRect(-lpDrawItemStruct->rcItem.left,-lpDrawItemStruct->rcItem.top);
				}

				// 绘制展开时应该显示的信息
				if ( pItemData->bExpand )
				{
					// ------------------------------------------ 显示DNS服务器的信息 -------------------------------------------------------
					rcDNSServer					= m_rcDNSServer;
					rcDNSServer.left			+= rcFirst.left;
					rcDNSServer.top				+= rcFirst.top;
					rcDNSServer.bottom			= rcDNSServer.top +m_rcDNSServer.Height();
					rcDNSServer.right			= rcSecond.right;

					if(pNetworkDeviceInfoData->pszSecondaryDNS.IsEmpty())
						strShow.Format(_T("DNS服务器：%s"),pNetworkDeviceInfoData->pszPrimaryDNS );
					else
						strShow.Format(_T("DNS服务器：%s,%s"),pNetworkDeviceInfoData->pszPrimaryDNS,pNetworkDeviceInfoData->pszSecondaryDNS);
				
					//dcx.DrawText(strShow,-1,&rcProbe,DT_CALCRECT|DT_VCENTER|DT_SINGLELINE|DT_LEFT);
					dcx.DrawText(strShow,-1,&rcDNSServer,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_LEFT);

					rcProbe.bottom = rcProbe.top+12;
					rcDNSServer.OffsetRect(-rcFirst.left, -rcFirst.top);
					calcLeftRECT(rcDNSServer,rcProbe,pItemData->rcDNSServer );

					// ------------------------------------------ 显示子网掩码的信息 -------------------------------------------------------
					rcSubNetMask				= m_rcSubNetMask;
					rcSubNetMask.left			+= rcFirst.left;
					rcSubNetMask.top			+= rcFirst.top;
					rcSubNetMask.bottom			= rcSubNetMask.top +m_rcSubNetMask.Height();
					rcSubNetMask.right			= rcSecond.right;

					strShow.Format(_T("子网掩码：%s"),pNetworkDeviceInfoData->pszNetMask );


					dcx.DrawText(strShow,-1,&rcProbe,DT_CALCRECT|DT_VCENTER|DT_SINGLELINE|DT_LEFT);
					dcx.DrawText(strShow,-1,&rcSubNetMask,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_LEFT);

					rcProbe.bottom = rcProbe.top+12;
					rcSubNetMask.OffsetRect(-rcFirst.left, -rcFirst.top);
					calcLeftRECT(rcSubNetMask,rcProbe,pItemData->rcSubNetMask );

					// ------------------------------------------ 显示完整的MAC地址的信息 -------------------------------------------------------
					CRect rcFullDriverPath		= m_rcFullDriverPath;
					rcFullDriverPath.left			+= rcFirst.left;
					rcFullDriverPath.top			+= rcFirst.top;
					rcFullDriverPath.bottom			= rcFullDriverPath.top +m_rcFullDriverPath.Height();
					rcFullDriverPath.right			= rcSecond.right;

					strShow.Format(_T("MAC地址：%s"),pNetworkDeviceInfoData->pszMACAddress );


					dcx.DrawText(strShow,-1,&rcProbe,DT_CALCRECT|DT_VCENTER|DT_SINGLELINE|DT_LEFT);
					dcx.DrawText(strShow,-1,&rcFullDriverPath,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_LEFT);

					rcProbe.bottom = rcProbe.top+12;
					rcFullDriverPath.OffsetRect(-rcFirst.left, -rcFirst.top);
					calcLeftRECT(rcFullDriverPath,rcProbe,pItemData->rcFullDriverPath );
					
				}
			}
	
			if ( (pItemData->nType==TYPE_ITEM_MAIN) ||
				 (pItemData->nType==TYPE_ITEM_APPPATH) )
			{
				CPen	penx;
				penx.CreatePen(PS_SOLID,1,RGB(200,200,200));
				HPEN	pentmp = dcx.SelectPen(penx);
				dcx.MoveTo(rcItemX.left,rcItemX.bottom-1);
				dcx.LineTo(rcItemX.right,rcItemX.bottom-1);
				dcx.SelectPen(pentmp);
			}
		}
		dcx.SelectFont(fntdeftmp);
		dcx.Detach();
	}

	void MeasureItem(LPMEASUREITEMSTRUCT lParam)
	{
		_NETWORK_DEVICE_ITEM_DATA*	px = (_NETWORK_DEVICE_ITEM_DATA*)XBASE_GetItemData(lParam->itemID);

		if (px && px->bExpand)
			lParam->itemHeight = XX_RUN_LIST_ITEM_HEIGHT_EXP;
		else
			lParam->itemHeight = XX_RUN_LIST_ITEM_HEIGHT_COL;
	}

	void InsertItemX(LP_XY_NETWORK_DEVICE_INFO_DATA pNetworkDeviceInfoData)
	{
		_NETWORK_DEVICE_ITEM_DATA*	pX = new _NETWORK_DEVICE_ITEM_DATA;
		pX->pNetworkDeviceInfoData = pNetworkDeviceInfoData;		
		
		if (IsItemCanShow(pNetworkDeviceInfoData,m_nSubShowType))
		{
			int iPos = XBASE_InsertItem(GetInsertIndex(pNetworkDeviceInfoData),pNetworkDeviceInfoData->pszConnectionName );
			XBASE_SetItemText(iPos,pNetworkDeviceInfoData->pszConnectionName );
			XBASE_SetItemData(iPos,(DWORD_PTR)pX);
		}

		m_listArray.Add(pX);
	}

	BOOL isItemInList(XY_NETWORK_DEVICE_INFO_DATA* pInfo)
	{
		for (int i=0; i<XBASE_GetItemCount(); i++)
		{
			if ( pInfo == GetDataInfo(i) )
				return TRUE;
		}
		return FALSE;
	}

	BOOL IsItemCanShow(XY_NETWORK_DEVICE_INFO_DATA* pInfo, DWORD nSubShowType)
	{
		if (nSubShowType==SUB_SHOW_TYPE_ALL)
			return TRUE;
		else if (nSubShowType==SUB_SHOW_TYPE_ENABLE)
			return (pInfo->bDeviceEnabled );
		else if (nSubShowType==SUB_SHOW_TYPE_DISABLE)
			return (!pInfo->bDeviceEnabled );
		
		return FALSE;
	}

	int GetInsertIndex(XY_NETWORK_DEVICE_INFO_DATA* pInfo)
	{
		int i=0;
		for (i=0; i<XBASE_GetItemCount(); i++)
		{
			XY_NETWORK_DEVICE_INFO_DATA* pItemInfo = GetDataInfo(i);
			if ( pInfo->bDeviceEnabled && !pItemInfo->bDeviceEnabled )
				return i;
			else if( (pItemInfo->bDeviceEnabled&&pInfo->bDeviceEnabled) || (!pInfo->bDeviceEnabled&&!pItemInfo->bDeviceEnabled) )
			{
				if ( pInfo->pszConnectionName.CompareNoCase(pItemInfo->pszConnectionName) <= 0 )
					return i;
			}
		}
		return i;
	}

	void SetSubShowType(DWORD nsubType)
	{
		SetShowType(nsubType);
	}

	void ResetExpandState(BOOL bExp=FALSE)
	{
		for (int i=0; i<m_listArray.GetSize(); i++)
		{
			m_listArray[i]->bExpand = bExp;
		}
	}

	void SetShowType(DWORD nSubShowType=SUB_SHOW_TYPE_ALL)
	{
		SetRedraw(FALSE);
		
		for (int i=XBASE_GetItemCount()-1; i>=0;i--)
		{
			if (!IsItemCanShow(GetDataInfo(i),nSubShowType))
			{
				XBASE_DeleteItem(i);
			}
		}

		for ( int i= 0; i<m_listArray.GetSize(); i++)
		{
			LP_XY_NETWORK_DEVICE_INFO_DATA	pInfoX = m_listArray[i]->pNetworkDeviceInfoData ;
			if (IsItemCanShow(pInfoX,nSubShowType))
			{
				if (!isItemInList(pInfoX))
				{
					int iPos = GetInsertIndex(pInfoX);
					iPos = XBASE_InsertItem(iPos,pInfoX->pszConnectionName );
					XBASE_SetItemData(iPos,(DWORD_PTR)m_listArray[i]);
				}
			}
		}
	
		SetRedraw(TRUE);

		m_nSubShowType = nSubShowType;
	}


	void DeleteItemByKSInfo(XY_NETWORK_DEVICE_INFO_DATA* pNetworkDeviceInfoData)
	{
		for (int i=0; i<XBASE_GetItemCount(); i++)
		{
			_NETWORK_DEVICE_ITEM_DATA*	px = (_NETWORK_DEVICE_ITEM_DATA*)XBASE_GetItemData(i);
			
			if (px && px->pNetworkDeviceInfoData ==pNetworkDeviceInfoData)
			{
				XBASE_DeleteItem(i);

				for (int k=0; k<m_listArray.GetSize(); k++)
				{
					if ( px == m_listArray[k])
					{
						m_listArray.RemoveAt(k);
						break;
					}
				}
				delete pNetworkDeviceInfoData;
				delete px;
				break;
			}
		}
	}

	void DeleteAllItemX()
	{
		for ( int i=0; i < XBASE_GetItemCount(); i++)
		{
			_NETWORK_DEVICE_ITEM_DATA*	px = (_NETWORK_DEVICE_ITEM_DATA*)XBASE_GetItemData(i);

			if (px->nType!=TYPE_ITEM_MAIN)
				delete px;
		}
		XBASE_DeleteAllItems();
	}

	int GetAllCount()
	{
		return m_listArray.GetSize();
	}

	_NETWORK_DEVICE_ITEM_DATA* GetRunDataByIndex(int i)
	{
		return m_listArray[i];
	}

	void ClearAll()
	{
		if (m_hWnd!=NULL)
			DeleteAllItemX();
		for (int i=0; i<m_listArray.GetSize(); i++)
		{
			_NETWORK_DEVICE_ITEM_DATA*	px = (_NETWORK_DEVICE_ITEM_DATA*)m_listArray[i];

			// pNetworkDeviceInfoData由其它地方负责添加的，不用这里来释放
			//if (px && px->pNetworkDeviceInfoData)
			//	delete px->pNetworkDeviceInfoData;
			
			if (px)
				delete px;
		}
		m_listArray.RemoveAll();

		for (int i=0; i<m_listICON.GetSize(); i++)
		{
			if (m_listICON[i]!=NULL)
				::DestroyIcon(m_listICON[i]);
		}

		m_listICON.RemoveAll();
	}

	int GetShowCount()
	{
		return XBASE_GetItemCount();
	}
};
