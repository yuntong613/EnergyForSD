#pragma once
#pragma warning (disable : 4996)

#include <strstream>
using namespace std;

class CUtils
{
public:
	CUtils(void);
	~CUtils(void);
	CString GetAppPath() const;
public:
	//************************************
	// Method:    HexToString
	// FullName:  CUtils::HexToString
	// Access:    public 
	// Returns:   CString
	// Qualifier:  HexToString
	// Parameter: const BYTE * pBuffer
	// Parameter: size_t iBytes
	//************************************
	CString HexToString(const BYTE *pBuffer,size_t iBytes);

	//************************************
	// Method:    StringToHex
	// FullName:  CUtils::StringToHex
	// Access:    public 
	// Returns:   void
	// Qualifier:  StringToHex
	// Parameter: const CString & str
	// Parameter: BYTE * pBuffer
	// Parameter: int & nBytes
	//************************************
	void StringToHex(const CString &str,BYTE *pBuffer,int& nBytes);
protected:
	BYTE Hex2Bin(CString strHex);
public:
	//************************************
	// Method:    ToString
	// FullName:  CUtils::ToString
	// Access:    public 
	// Returns:   CString
	// Qualifier:  MakeString
	// Parameter: T num
	//************************************
	template <class T>
	CString ToString( T num)
	{
		CString strNum = _T("");
		strstream buf;
		buf << num << std::ends;
		strNum = buf.str();
		buf.freeze(false);
		return strNum;
	}
	//************************************
	// Method:    ToBool
	// FullName:  CUtils::ToBool
	// Access:    public 
	// Returns:   bool
	// Qualifier:  Makebool
	// Parameter: const T & value
	//************************************
	template <class T>
	bool ToBool(const T &value){ return (0 != value);}

	//************************************
	// Method:    BOOL_to_bool
	// FullName:  CUtils::BOOL_to_bool
	// Access:    public 
	// Returns:   bool
	// Qualifier:   MakeBOOL
	// Parameter: const BOOL bResult
	//************************************
	inline bool BOOL_to_bool(const BOOL bResult);

	//************************************
	// Method:    ToHex
	// FullName:  CUtils::ToHex
	// Access:    public 
	// Returns:   CString
	// Qualifier:  MakeHex
	// Parameter: BYTE c
	//************************************
	CString ToHex(BYTE c);
public:
	//************************************
	// Method:    GetCurDirectory
	// FullName:  CUtils::GetCurDirectory
	// Access:    public 
	// Returns:   CString
	// Qualifier:  GetCurrentDir
	//************************************
	CString GetCurDirectory();

	//************************************
	// Method:    GetDateStamp
	// FullName:  CUtils::GetDateStamp
	// Access:    public 
	// Returns:   CString
	// Qualifier:
	// Parameter: int nType
	// nType=0 %Y-%m-%d %H:%M:%S
	// nType=1 %Y%m%d%H%M%S
	// nType=2 %Y%m%d
	// nType=3 %Y-%m-%d
	// nType=4 %H%M%S
	// nType=5 %H:%M:%S
	//************************************
	CString GetDateStamp(int nType = 0);
};

