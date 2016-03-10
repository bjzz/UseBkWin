#pragma once

//************************************
// Method:    TcpStatus2String
// FullName:  得到TCP连接状态
// Access:    public 
// Returns:   void
// Qualifier: withtaru@gmail.com
// Parameter: DWORD status
// Parameter: wchar_t * szStatus
//************************************
void TcpStatus2String(DWORD status, wchar_t* szStatus);


//************************************
// Method:    SetLangType
// FullName:  Set Language Type 1-Chinese  2-English 
// Access:    public 
// Returns:   void
// Qualifier: withtaru@gmail.com
// Parameter: DWORD dwLangType
//************************************
void SetLangType(DWORD dwLangType);


//************************************
// Method:    GetLangType
// FullName:  Get Language Type
// Access:    public 
// Returns:   DWORD
// Qualifier: withtaru@gmail.com
//************************************
DWORD GetLangType();

//************************************
// Method:    SetAutoRun
// FullName:  Set program autorun with OS
// Access:    public 
// Returns:   void
// Qualifier: withtaru@gmail.com
// Parameter: BOOL bAutoRun
//************************************
void SetAutoRun(BOOL bAutoRun);