#pragma once
#include <string>
#include <windows.h>
using std::string;
using std::wstring;


class IniEditor
{
public :

    IniEditor( const wstring& wstrFilePath ) : m_wstrFile( wstrFilePath ) {}

	IniEditor() {}

	~IniEditor() {}

	const wstring& GetFile() const { return m_wstrFile; }

	BOOL SetFile(const wstring& wstrFilePath) { m_wstrFile = wstrFilePath; return TRUE;}

	wstring ReadString(
		const wstring& strSec, 
		const wstring& strKey ) const;

    BOOL WriteString(
		const wstring& strSec, 
		const wstring& strKey, 
		const wstring& strVal );


	BOOL ReadDWORD( 
		const wstring& strSec, 
		const wstring& strKey, 
		int& dwVal) const;

	BOOL WriteDWORD(
		const wstring& strSec, 
		const wstring& strKey, 
		DWORD dwVal);

	BOOL IncrDWORD(const wstring& strSec, const wstring& strKey);

	BOOL Del( const wstring& strSec, const wstring& strKey );
	
	BOOL DelFile();

private :

    wstring m_wstrFile;
};
