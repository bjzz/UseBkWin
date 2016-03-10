#pragma once

#define		WIRED_NETWORK_DEVICE			0
#define		WIRELESS_NETWORK_DEVICE			1
#define		PPPoE_NETWORK_DEVICE			2
#define		VIRTUAL_NETWORK_DEVICE			3

typedef		struct		XY_NETWORK_DEVICE_INFO_DATA
{
	CString						pszConnectionName;			// 连接名称

	unsigned	int				nNetworkType;				// 网卡类型

	CString						pszDeviceDesc;				// 网卡描述
	CString						pszDeviceFriendlyDesc;		// 网卡描述
	
	BOOL						bDeviceEnabled;				// 是否已开启网卡
	DWORD						dwDeviceStatus;				// 网卡状态

	CString						pszIPAddress;				// IP地址
	CString						pszNetMask;                 // 子网掩码
	CString						pszGateway;                 // 网关

	BOOL						bAutoObtainDNS;             // 自动获取的DNS
	CString						pszPrimaryDNS;              // 主DNS
	CString						pszSecondaryDNS;            // 从DNS

	BOOL						bUsingDHCP;                 // 是否使用DHCP
	CString						pszDHCPIPAddress;           // DHCP server

	CString						pszMACAddress;              // 物理地址

	XY_NETWORK_DEVICE_INFO_DATA()
	{
		
		pszConnectionName.Empty();
		pszDeviceDesc.Empty();
		pszDeviceFriendlyDesc.Empty();
		

		bDeviceEnabled		= TRUE;

		pszIPAddress.Empty();
		pszNetMask.Empty();
		pszGateway.Empty();
		pszPrimaryDNS.Empty();
		pszSecondaryDNS.Empty();

		bUsingDHCP = FALSE;
		pszDHCPIPAddress.Empty();

		pszMACAddress.Empty();
	}
	

}*LP_XY_NETWORK_DEVICE_INFO_DATA, XY_NETWORK_DEVICE_INFO_DATA;
