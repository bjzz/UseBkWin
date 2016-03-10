#pragma once

class CCustomDrawButton:
	public CWindowImpl<CCustomDrawButton, CButton>,
	public COwnerDraw<CCustomDrawButton>
{
protected:
	typedef struct _SPEED_INFO
	{
		double fUploadSpeed;
		double fDownloadSpeed;
		_SPEED_INFO()
		{
			fUploadSpeed	= 0.00;
			fDownloadSpeed	= 0.00;
		}
	}SPEED_INFO, LPSPEED_INFO;

protected:
	/* default pen width */
	DWORD		m_dwPenGridWidth;
	DWORD		m_dwPenDownloadWidth;
	DWORD		m_dwPenUploadWidth;
	/* default pen style */
	CPen		m_hPen;
	CPen		m_hPenDownloadSpeedLine;
	CPen		m_hPenUploadSpeedLine;
	CPen		m_hPenText;
	COLORREF	m_ColorTextUpload;
	COLORREF	m_ColorTextDownload;
	COLORREF	m_ColorBackGround;
	/* for cheating your eyes */
	DWORD		m_dwBeginPos;
	/* grid width */
	DWORD		m_dwGridWidth;
	/* grid height */
	DWORD		m_dwGridHeight;
protected:
	/* frequency for redraw */
	DWORD		m_dwDrawSpeed;
	/* frequency for calcuate */
	DWORD		m_dwCalcSpeed;
	/* redraw timer */
	DWORD		m_dwTimerDraw;
	/* calcuate timer */
	DWORD		m_dwTimerCalc;
protected:
	/* total download bytes */
	DWORD					m_dwDownloadCount;
	/* total upload bytes */
	DWORD					m_dwUploadCount;
	CString					m_strDownloadCount;
	CString					m_strUploadCount;
	CString					m_strDownloadSpeed;
	CString					m_strUploadSpeed;
	std::vector<SPEED_INFO>	m_SpeedInfo;
	/* device index list */
	std::vector<IP_ADAPTER_INDEX_MAP>		m_NetInterfaceList;

public:
	BEGIN_MSG_MAP(CCustomDrawButton)	
		MSG_WM_CREATE(OnCreate)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
		CHAIN_MSG_MAP_ALT(COwnerDraw<CCustomDrawButton>, 1)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

public:
	void OnRButtonDown(UINT nFlags, CPoint point)
	{
		CMenu menu;
		menu.LoadMenu( IDR_MENU_NET );
		CMenuHandle Menu = menu.GetSubMenu(0);
		CPoint Position;
		ATLVERIFY(GetCursorPos(&Position));
		for ( int i=menu.GetMenuItemCount(); i>0; i-- )
		{
			menu.RemoveMenu(i, MF_BYPOSITION );
		}
		std::vector<IP_ADAPTER_INDEX_MAP>::iterator i = m_NetInterfaceList.begin();
		for ( ; i!=m_NetInterfaceList.end(); i++ )
		{
			Menu.AppendMenu( MF_STRING | MF_POPUP, -1, i->Name );
		}
		
		Menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_TOPALIGN, Position.x, Position.y, m_hWnd );
// 		CMenuHandle Menu;
// 		CPoint Position;
// 		
// 		Menu.CreateMenu();
// 		Menu.AppendMenu( MF_STRING | MF_POPUP, 1, _T("http://mktime.org") );
// 		Menu.AppendMenu( MF_STRING | MF_POPUP, 1, _T("http://mktime.org") );
// 		Menu.AppendMenu( MF_STRING | MF_POPUP, 1, _T("http://mktime.org") );
// 		Menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_BOTTOMALIGN, Position.x, Position.y, m_hWnd );
		/*MessageBox( _T("Got Right mouse message.") );*/
// 		CMenu menuPopup;
// 		menuPopup.LoadMenu(IDR_MENU_NET);
// 
// 		CMenuHandle Menu = menuPopup.GetSubMenu(0);
// 		Menu.AppendMenu(MF_STRING | MF_POPUP, 1, _T("http://mktime.org"));
// 		CPoint Position;
// 		ATLVERIFY(GetCursorPos(&Position));
// 		Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_BOTTOMALIGN, Position.x, Position.y, m_hWnd);

	}
	void SetBgColor( COLORREF c )
	{
		m_ColorBackGround = c;
	}
	void SetPenGridLineColor( COLORREF c )
	{
		m_hPen.DeleteObject();
		m_hPen.CreatePen( PS_SOLID, m_dwPenGridWidth, c );
	}
	void SetUpColor( COLORREF c )
	{
		m_hPenUploadSpeedLine.DeleteObject();
		m_hPenUploadSpeedLine.CreatePen( PS_SOLID, m_dwPenUploadWidth, c );
	}
	void SetDownColor( COLORREF c )
	{
		m_hPenDownloadSpeedLine.DeleteObject();
		m_hPenDownloadSpeedLine.CreatePen( PS_SOLID, m_dwPenDownloadWidth, c );
	}
	void SetColorTextDownload( COLORREF c )
	{
		m_ColorTextDownload = c;
	}
	void SetColorTextUpload( COLORREF c )
	{
		m_ColorTextUpload	= c;
	}

	
	int OnCreate(LPCREATESTRUCT /*lpCreateStruct*/)
	{
		////////////////////default info for draw
		m_dwPenGridWidth		= 1;
		m_dwPenDownloadWidth	= 1;
		m_dwPenUploadWidth		= 1;

		m_hPen.CreatePen( PS_SOLID, m_dwPenGridWidth, RGB(0x00, 0x99, 0x33) );
		/* 下载速度线条 */
		m_hPenDownloadSpeedLine.CreatePen( PS_SOLID, m_dwPenDownloadWidth, RGB(0xFF, 0xFF, 0x00) );
		/* 上传速度线条 */
		m_hPenUploadSpeedLine.CreatePen( PS_SOLID, m_dwPenUploadWidth, RGB(0x99, 0xFF, 0xFF) );
		m_hPenText.CreatePen( PS_SOLID, m_dwPenUploadWidth, RGB(0xFF, 0x00, 0x00) );
		
		m_ColorTextUpload			= RGB(0xFF, 0xFF, 0xFF);
		m_ColorTextDownload			= RGB(0xFF, 0xFF, 0x00);
		m_ColorBackGround			= RGB(0x00, 0x00, 0x00);

		// default grid width and height
		m_dwGridHeight	 = 15;
		m_dwGridWidth	 = 15;
		// default begin pos for cheating your eyes
		m_dwBeginPos	 = 0;
		////////////////////init info for calc and draw speed
		m_dwDrawSpeed	 = 1000 / 20;		//视觉暂留
		//m_dwCalcSpeed	= 1000 * 5;			// debug
		m_dwCalcSpeed	= 1000 / 3;			// 计算速度 一秒钟计算5此 那么在数据那里要乘以5
		/////////////////// timer ID for calc and draw
		m_dwTimerCalc	= 100;
		m_dwTimerDraw	= 101;

		////////////////// init the first speed info
		GeNetTrafficCount();

		GetNetInterfaceIndex();

		//////////////// init for tiemr
		SetTimer( m_dwTimerDraw, m_dwDrawSpeed, NULL );
		SetTimer( m_dwTimerCalc, m_dwCalcSpeed, NULL );
		return TRUE;
	}
	void OnTimer(UINT_PTR nIDEvent)
	{
		// get the y start pos
		if ( nIDEvent == m_dwTimerDraw )
		{
			m_dwBeginPos ++;
			m_dwBeginPos %= m_dwGridWidth;
			InvalidateRect( NULL, FALSE );
		}
		// insert data for speed
		if ( nIDEvent == m_dwTimerCalc )
		{
			AddSpeedInfo();
		}
	}
	void GeNetTrafficCount()
	{
		PMIB_IFROW pRow = NULL;
		pRow = reinterpret_cast<PMIB_IFROW>( GlobalAlloc( GPTR, sizeof(MIB_IFROW) ) );
		pRow->dwIndex = 10;

		if ( GetIfEntry( pRow ) == NO_ERROR )
		{
			m_dwDownloadCount	= pRow->dwInOctets;
			m_dwUploadCount		= pRow->dwOutOctets; 
		}
		GlobalFree( pRow );
	} 
	void GetNetInterfaceIndex()
	{
		DWORD dwNumOfDevie;
		if ( GetNumberOfInterfaces( &dwNumOfDevie ) == NO_ERROR )
		{
			PIP_INTERFACE_INFO pInfo;
			ULONG ulSize;

			ulSize	 = sizeof( IP_INTERFACE_INFO );
			pInfo	 = reinterpret_cast<PIP_INTERFACE_INFO>( GlobalAlloc( GPTR, ulSize ) );
			if ( GetInterfaceInfo( pInfo, &ulSize ) == ERROR_INSUFFICIENT_BUFFER )
			{
				GlobalFree( pInfo );
				pInfo	 = reinterpret_cast<PIP_INTERFACE_INFO>( GlobalAlloc( GPTR, ulSize ) );
			}
			if ( GetInterfaceInfo( pInfo, &ulSize ) == NO_ERROR )
			{
				for ( int i=0; i<pInfo->NumAdapters; i++ )
				{
					/*m_NetInterfaceList.push_back( pInfo->Adapter[i].Index );*/
					m_NetInterfaceList.push_back( pInfo->Adapter[i] );
				}
			}
			GlobalFree( pInfo );
		}
	}
	void AddSpeedInfo()
	{
		// 模拟真实下载速度
		SPEED_INFO tmpSpeed;
		// 		tmpSpeed.fDownloadSpeed = (double)(rand() % 150);
		// 		tmpSpeed.fUploadSpeed	= (double)(rand() % 60);
		PMIB_IFROW pRow = NULL;
		
		pRow = reinterpret_cast<PMIB_IFROW>( GlobalAlloc( GPTR, sizeof(MIB_IFROW) ) );
		/*std::vector<DWORD>::iterator i = m_NetInterfaceList.begin();*/
		std::vector<IP_ADAPTER_INDEX_MAP>::iterator i = m_NetInterfaceList.begin();
		// the first device index
		pRow->dwIndex = i->Index;

		if ( GetIfEntry( pRow ) == NO_ERROR )
		{
			DOUBLE dwSpeedDown = 0;
			DOUBLE dwSpeedUp = 0;

			DWORD dwDownCount = pRow->dwInOctets;
			DWORD dwUpCount   = pRow->dwOutOctets;

			//////// really speed divide 1024  byte/s -> kb/s
			dwSpeedDown = ( dwDownCount - m_dwDownloadCount ) / 1024;
			dwSpeedUp	= ( dwUpCount - m_dwUploadCount ) / 1024;

			tmpSpeed.fDownloadSpeed	= dwSpeedDown ;
			tmpSpeed.fUploadSpeed	= dwSpeedUp ;

			m_dwDownloadCount	= dwDownCount;
			m_dwUploadCount		= dwUpCount;
		}
		GlobalFree( pRow );

		m_SpeedInfo.insert(m_SpeedInfo.begin(), tmpSpeed);
	}

	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		CMemoryDC dcMem(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem);
		
		dcMem.SelectPen( m_hPen );
		/* back ground color */
		dcMem.FillSolidRect( &lpDrawItemStruct->rcItem, m_ColorBackGround );
		/* rect of drawing */
		int cx = lpDrawItemStruct->rcItem.right-lpDrawItemStruct->rcItem.left;
		int cy = lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top;

		// 画横线
		for ( int y=0; y<cy; y+=m_dwGridHeight )
		{
			dcMem.MoveTo( 0, y );
			dcMem.LineTo( cx, y );
		}
		// 画竖线 静态效果 调试
// 		for ( int x=0; x<cx; x+=m_dwGridHeight )
// 		{
// 			dcMem.MoveTo( x, 0 );
// 			dcMem.LineTo( x, cy );
// 		}

		// 画竖线 动态效果
 		for ( int x=m_dwBeginPos; x<cx; x+=m_dwGridHeight )
 		{
 			dcMem.MoveTo( x, 0 );
 			dcMem.LineTo( x, cy );
 		}

		if ( m_SpeedInfo.size() > 2 )
		{
			DWORD dwSize;
			/* count of point to draw */
			dwSize = ( m_SpeedInfo.size() >= cx ) ? cx :m_SpeedInfo.size();
			std::vector<SPEED_INFO>::iterator it = m_SpeedInfo.begin();
			// 向左便宜35pix
			int j= 35;
			int k= 35;
			// first item of list.the first postion to move to.
			DOUBLE dFirstDownload = it->fDownloadSpeed;
			DOUBLE dFirstUpload	  = it->fUploadSpeed;

			m_strDownloadSpeed.Format( _T("下载速度:%.2f kb/s"), dFirstDownload );
			m_strUploadSpeed.Format( _T("上传速度:%.2f kb/s"), dFirstUpload );
			
			dcMem.SetBkMode( TRANSPARENT );
			/*dcMem.SelectPen( m_hPenText );*/
			dcMem.SetTextColor( m_ColorTextDownload );
			dcMem.DrawText( m_strDownloadSpeed, m_strDownloadSpeed.GetLength(), CRect(cx-140, cy-40, cx,cy), DT_VCENTER | DT_CENTER );
			dcMem.SetTextColor( m_ColorTextUpload );
			dcMem.DrawText( m_strUploadSpeed, m_strUploadSpeed.GetLength(), CRect(cx-140, cy-20, cx,cy), DT_VCENTER | DT_CENTER );
// 			dcMem.TextOut( cx-140, cy-40, m_strDownloadSpeed );
// 			dcMem.TextOut( cx-140, cy-20, m_strUploadSpeed );
 			
			it ++;		// now second
			
			// draw speed graph
			for ( it = m_SpeedInfo.begin(); it != m_SpeedInfo.end() && dwSize; it++, dwSize--, j<cx )
			{
				if ( it->fDownloadSpeed >= cy - 50 )
				{
					it->fDownloadSpeed = cy - 50;
				}

				dcMem.SelectPen( m_hPenDownloadSpeedLine );
				dcMem.MoveTo( j, cy -dFirstDownload - 50 );
				j += 2 ;
				dcMem.LineTo( j, cy -it->fDownloadSpeed - 50 );

				dcMem.SelectPen( m_hPenUploadSpeedLine );
				dcMem.MoveTo( k, cy -dFirstUpload - 53 );
				k += 2;
				dcMem.LineTo( k, cy -it->fUploadSpeed - 53 );
				
				dFirstDownload = it->fDownloadSpeed;
				dFirstUpload   = it->fUploadSpeed;
			}
			///////////////// draw net count and net speed
 			
// 			m_strDownloadSpeed.Format( _T("Download: %.2f kb/s"), dFirstDownload );
// 			m_strUploadCount.Format( _T("Upload: %.2f kb/s"), dFirstUpload );
			
// 			m_strDownloadCount.Format( _T("总下载：%.2f MB"), (DOUBLE)(m_dwDownloadCount / 1024 / 1024) );
// 			m_strUploadCount.Format( _T("总上传:%.2f MB"), (DOUBLE)(m_dwUploadCount / 1024 / 1024) );

// 			dcMem.TextOut( cx-100, cy-20, m_strDownloadCount );
// 			dcMem.TextOut( cx-100, cy-40, m_strUploadCount);
		}
	}

};