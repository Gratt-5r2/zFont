// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  inline bool ansi_2_utf32( const char* input, std::u32string& output ) {
    int utf16len = MultiByteToWideChar( GetACP(), MB_ERR_INVALID_CHARS, input, strlen( input ), 0, 0 ); // TODO auto CP
    wchar_t* utf16ptr = new wchar_t[utf16len];
    int len = MultiByteToWideChar( GetACP(), 0, input, strlen( input ), utf16ptr, utf16len );
    for( int i = 0; i < utf16len; i++ )
      output.push_back( utf16ptr[i] );
    delete[] utf16ptr;
    return len > 0;
  }


  inline bool charPtr_2_utf32( const char* input, std::u32string& output ) {
    try {
      std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> utf32conv;
      output = utf32conv.from_bytes( input );
      return true;
    }
    catch( std::range_error ex ) {
      return ansi_2_utf32( input, output );
    }
  }


  inline bool utf8_2_utf32( const char* input, std::u32string& output ) {
    try {
      std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
      output = conv.from_bytes( input );
      return true;
    }
    catch( std::range_error ex ) {
      return ansi_2_utf32( input, output );
    }
  }


  inline bool utf32_2_utf8( const char32_t* input, std::string& output ) {
    try {
      std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
      output = conv.to_bytes( input );
      return true;
    }
    catch( std::range_error ex ) {
      // TODO ?
      return false;
    }
  }
}