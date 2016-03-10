#pragma once

class COwnDrawNetFlowButton:
	public CWindowImpl<COwnDrawNetFlowButton, CButton>,
	public COwnerDraw<COwnDrawNetFlowButton>
{
protected:
	typedef struct _NET_FLOW_INFO
	{
		DOUBLE dwDownloadMB;
		DOUBLE dwUploadMB;
		
		_NET_FLOW_INFO()
		{
			dwDownloadMB	= 0;
			dwUploadMB		= 0;
		}
	}NET_FLOW_INFO, LP_NET_FLOW_INFO;

protected:
	// 网络数据
	std::vector<DWORD>			m_InterfaceIndexList;
	std::vector<NET_FLOW_INFO>	m_NetFlowList;
	DOUBLE						m_dwCurrDownloadMB;
	DOUBLE						m_dwCurrUplodMB;
protected:
	// 上传与下载大小
	CString		m_strDownloadMB;
	CString		m_strUploadMB;

protected:
	// 绘图相关变量
	DWORD		m_dwGridWidth;
	DWORD		m_dwGridHeight;
	CPen		m_hPenGridLine;
	CPen		m_hPenDownloadLine;
	CPen		m_hPenUploadLine;
	COLORREF	m_ColorBackground;
	COLORREF	m_ColorTextDownload;
	COLORREF	m_ColorTextUpoad;

protected:
	// 两个定时器ID
	UINT_PTR	m_uTimerIDClac;
	UINT_PTR	m_uTimerIDDraw;
public:
	BEGIN_MSG_MAP(COwnDrawNetFlowButton)	
		MSG_WM_CREATE(OnCreate)
		MSG_WM_TIMER(OnTimer)
		CHAIN_MSG_MAP_ALT(COwnerDraw<COwnDrawNetFlowButton>, 1)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

	void SetBgColor( COLORREF c )
	{
		m_ColorBackground	= c;
	}
	void SetPenGridColor( COLORREF c )
	{
		m_hPenGridLine.DeleteObject();
		m_hPenGridLine		= CreatePen( PS_SOLID, 1, c );
	}
	void SetPenDownloadColor( COLORREF c )
	{
		m_hPenDownloadLine.DeleteObject();
		m_hPenDownloadLine	= CreatePen( PS_SOLID, 1, c );
	}
	void SetPenUploadColor( COLORREF c )
	{
		m_hPenUploadLine.DeleteObject();
		m_hPenUploadLine	= CreatePen( PS_SOLID, 1, c );
	}
	void SetColorTextDownload( COLORREF c )
	{
		m_ColorTextDownload	= c;
	}
	void SetColorTextUpload( COLORREF c )
	{
		m_ColorTextUpoad	= c;
	}

	int OnCreate(LPCREATESTRUCT /*lpCreateStruct*/)
	{
		m_dwGridWidth		= 15;
		m_dwGridHeight		= 15;

		m_hPenGridLine		= CreatePen( PS_SOLID, 1, RGB( 0x00, 0x99, 0x33 ) );
		m_hPenDownloadLine	= CreatePen( PS_SOLID, 1, RGB( 0xFF, 0xFF, 0x00 ) );
		m_hPenUploadLine	= CreatePen( PS_SOLID, 1, RGB( 0x00, 0x00, 0xFF ) );

		m_ColorBackground	= RGB( 0x00, 0x00, 0x00 );
		m_ColorTextDownload	= RGB( 0xFF, 0xFF, 0x00 );
		m_ColorTextUpoad	= RGB( 0xFF, 0xFF, 0xFF );

		m_uTimerIDClac		= 100;
		m_uTimerIDDraw		= 101;
		/* 10秒钟计算一次数据  */
		SetTimer( m_uTimerIDClac, 1000 / 2, NULL );
		/* 5秒钟重绘一次界面 */
		SetTimer( m_uTimerIDDraw, 1000 * 1, NULL );

		GetNetInterfaceIndex();

		m_dwCurrDownloadMB	= 0;
		m_dwCurrUplodMB		= 0;

		return TRUE;
	}
	

	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		DWORD dwCx;
		DWORD dwCy;
		CMemoryDC dcMem( lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem );

		dcMem.FillSolidRect( &lpDrawItemStruct->rcItem, m_ColorBackground );

		dcMem.SelectPen( m_hPenGridLine );
		
		dwCx	= lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left;
		dwCy	= lpDrawItemStruct->rcItem.bottom- lpDrawItemStruct->rcItem.top;
		
		// 画横线
		for ( int y=0; y<dwCy; y+=m_dwGridWidth )
		{
			dcMem.MoveTo( 0, y );
			dcMem.LineTo( dwCx, y );
		}
		// 画竖线 静态效果 调试
		for ( int x=0; x<dwCx; x+=m_dwGridHeight )
		{
			dcMem.MoveTo( x, 0 );
			dcMem.LineTo( x, dwCy );
		}
		///////////////////////  绘制文字
		/*std::vector<NET_FLOW_INFO>::iterator it = m_NetFlowList.begin();*/
		m_strDownloadMB.Format( _T("总下载:%.2f MB"), m_dwCurrDownloadMB );
		m_strUploadMB.Format( _T("总上传:%.2f MB"), m_dwCurrUplodMB );
		dcMem.SetBkMode( TRANSPARENT );
		dcMem.SetTextColor( m_ColorTextDownload );
		dcMem.DrawText( m_strDownloadMB, m_strDownloadMB.GetLength(), CRect(dwCx-140, dwCy-40, dwCx,dwCy), DT_VCENTER | DT_CENTER );
		dcMem.SetTextColor( m_ColorTextUpoad );
		dcMem.DrawText( m_strUploadMB, m_strUploadMB.GetLength(), CRect(dwCx-140, dwCy-20, dwCx,dwCy), DT_VCENTER | DT_CENTER );

 		DWORD dwSuoXiao = ( m_dwCurrDownloadMB > m_dwCurrUplodMB) 
 						? m_dwCurrDownloadMB / dwCy + 5 
 						: m_dwCurrUplodMB / dwCy + 5;
		
		DWORD dwSize	= ( m_NetFlowList.size() < dwCx )
						? ( m_NetFlowList.size() -1 )
						: dwCx;
		
		std::vector<NET_FLOW_INFO>::iterator it = m_NetFlowList.begin();
		DWORD dwFirstDownload = it->dwDownloadMB;
		DWORD dwFirstUpload	  = it->dwUploadMB;
		it ++;
		DWORD dwBeginDown	= 1;
		DWORD dwBeginUp		= 1;

		for ( ; it!=m_NetFlowList.end() && dwSize; it++, dwSize-- )
		{
			dcMem.SelectPen( m_hPenDownloadLine );
			dcMem.MoveTo( dwBeginDown, ( dwCy - dwFirstDownload / dwSuoXiao ) );
			dwBeginDown += 2;
			dcMem.LineTo( dwBeginDown, ( dwCy - it->dwDownloadMB/ dwSuoXiao ));

			dcMem.SelectPen( m_hPenUploadLine );
			dcMem.MoveTo( dwBeginUp, ( dwCy - dwFirstUpload / dwSuoXiao ) );
			dwBeginUp += 2;
			dcMem.LineTo( dwBeginUp, ( dwCy - it->dwUploadMB / dwSuoXiao ));

			dwFirstDownload = it->dwDownloadMB;
			dwFirstUpload = it->dwUploadMB;
		}

		
		
// 		for ( ; it!=m_NetFlowList.end() && dwSize; it++, dwSize-- )
// 		{
// 			dcMem.MoveTo( dwBeginUp, ( dwCy - dwFirstUpload  ) );
// 			dwBeginUp += 2;
// 			dcMem.LineTo( dwBeginUp, ( dwCy - it->dwUploadMB ));
// 
// 			
// 		}




		// 队首删除
		while ( m_NetFlowList.size() > dwCx )
		{
			m_NetFlowList.erase( m_NetFlowList.begin() );
		}

	}

// 	void GeNetTrafficCount()		// 模拟真实数据
// 	{
// 		NET_FLOW_INFO temp;
// 		temp.dwDownloadBytes = m_dwCurrDownloadBytes + rand() % 1000;
// 		temp.dwUploadBytes	 = m_dwCurrUplodByte + rand() % 9000;
// 
// 		m_NetFlowList.push_back( temp );
// 
// 		m_dwCurrDownloadBytes	= temp.dwDownloadBytes;
// 		m_dwCurrUplodByte		= temp.dwUploadBytes;
// 
// 		while ( m_NetFlowList.size() > 500 )
// 		{
// 			m_NetFlowList.erase( m_NetFlowList.begin() );
// 		}
// 	}
	
	void GeNetTrafficCount()
	{
		NET_FLOW_INFO netflow;
		std::vector<DWORD>::iterator it = m_InterfaceIndexList.begin();

		PMIB_IFROW pRow = NULL;
		pRow = reinterpret_cast<PMIB_IFROW>( GlobalAlloc( GPTR, sizeof(MIB_IFROW) ) );
		pRow->dwIndex = *it;

		if ( GetIfEntry( pRow ) == NO_ERROR )
		{
			netflow.dwDownloadMB		= static_cast<DOUBLE>( pRow->dwInOctets / 1024 / 1024 );;
			netflow.dwUploadMB			= static_cast<DOUBLE>( pRow->dwOutOctets/ 1024 / 1024 ); 
		}
		// 队尾添加
 		m_NetFlowList.push_back( netflow );

		m_dwCurrDownloadMB		= static_cast<DOUBLE>( pRow->dwInOctets / 1024 / 1024 );
		m_dwCurrUplodMB			= static_cast<DOUBLE>( pRow->dwOutOctets/ 1024 / 1024 );

		GlobalFree( pRow );

	}

	void OnTimer(UINT_PTR nIDEvent)
	{
		if ( nIDEvent == m_uTimerIDClac )
		{
			GeNetTrafficCount();
		}
		if ( nIDEvent == m_uTimerIDDraw )
		{
			InvalidateRect( NULL, FALSE );
		}
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
					m_InterfaceIndexList.push_back( pInfo->Adapter[i].Index );
				}
			}
			/*GlobalFree( pInfo );*/
		}
	}
	
	
	
};