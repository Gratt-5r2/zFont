#define _CRT_SECURE_NO_WARNINGS
#include <string> 
#include <locale> 
#include <codecvt>

#include <MLang.h>
#include <comdef.h>
using namespace std;

// string (utf8) -> u16string -> wstring
wstring utf8_to_utf16( const string& utf8 )
{
  wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> convert;
  u16string utf16 = convert.from_bytes( utf8 );
  wstring wstr( utf16.begin(), utf16.end() );
  return wstr;
}

// wstring -> u16string -> string (utf8)
string utf16_to_utf8( const wstring& utf16 ) {
  u16string u16str( utf16.begin(), utf16.end() );
  wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t> convert;
  string utf8 = convert.to_bytes( u16str );
  return utf8;
}


bool isUtf8String( LPCSTR psz )
{
  if( psz == NULL || *psz == 0 )
    return false;

  typedef _com_ptr_t <_com_IIID<IMultiLanguage2, &__uuidof(IMultiLanguage2)>> IMultiLanguage2Ptr;
  IMultiLanguage2Ptr spML;

  spML.CreateInstance( CLSID_CMultiLanguage );
  if( spML == NULL ) {
    return false;
  }

  DetectEncodingInfo enc = { 0 };
  int iEncLen = 1;
  LPSTR pszBuf = new char[max( strlen( psz ), 1024 ) + 1];
  strcpy( pszBuf, psz );
  while( strlen( pszBuf ) < 300 )
    strcat( pszBuf, psz );
  int iLen = strlen( pszBuf );
  if( S_OK == spML->DetectInputCodepage( MLDETECTCP_8BIT, 0, pszBuf, &iLen, &enc, &iEncLen ) && iEncLen == 1 )
  {
    delete[] pszBuf;
    return enc.nCodePage == CP_UTF8;
  }

  delete[] pszBuf;
  return false;
}