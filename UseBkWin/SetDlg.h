#pragma once

class CSetDlg
	: public CBkDialogImpl<CSetDlg>,
	CWHRoundRectFrameHelper<CSetDlg>
{
public:
	CSetDlg()
		: CBkDialogImpl<CSetDlg>(IDR_BK_SECOND_DIALOG)
	{
	}

public:
	BK_NOTIFY_MAP(IDC_RICHVIEW_WIN)
		BK_NOTIFY_ID_COMMAND(IDC_BTN_SYS_CLOSE, OnBkBtnClose)
		BK_NOTIFY_MAP_END()

		BEGIN_MSG_MAP_EX(CSetDlg)
		MSG_BK_NOTIFY(IDC_RICHVIEW_WIN)
		MSG_WM_SYSCOMMAND(OnCommand)
		MSG_WM_INITDIALOG(OnInitDialog)
		CHAIN_MSG_MAP(CBkDialogImpl<CSetDlg>)
		CHAIN_MSG_MAP(CWHRoundRectFrameHelper<CSetDlg>)
		END_MSG_MAP()

public:

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		SetIcon( LoadIcon( ::GetModuleHandle( NULL ), MAKEINTRESOURCE( IDI_BEIKESAFE ) ));
		return TRUE;
	}
	void OnBkBtnClose()
	{
		EndDialog(IDCANCEL);
	}

	LRESULT OnCommand(UINT wParam, CPoint point)
	{
		switch ( wParam  )
		{
		case SC_CLOSE:
			EndDialog( 0 );
			break;
		}

		DefWindowProc();
		return 0;
	}
};