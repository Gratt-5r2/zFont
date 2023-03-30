// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  bool ansi_2_utf32( const std::string& input, std::u32string& output, int cp ) {
    int utf16len = MultiByteToWideChar( cp, MB_ERR_INVALID_CHARS, input.c_str(), input.length(), 0, 0);
    wchar_t* utf16ptr = new wchar_t[utf16len];
    int len = MultiByteToWideChar( cp, 0, input.c_str(), input.length(), utf16ptr, utf16len);
    for( int i = 0; i < utf16len; i++ )
      output.push_back( utf16ptr[i] );
    delete[] utf16ptr;
    return len > 0;
  }


  bool utf32_2_ansi( const std::u32string& input, std::string& output, int cp ) {
    std::wstring utf16;
    for( int i = 0; i <= input.length(); i++ )
      utf16.push_back( input[i] );

    int utf16len = utf16.length();
    char* ansiPtr = new char[utf16len + 1];
    WideCharToMultiByte( cp, 0, utf16.c_str(), -1, ansiPtr, utf16len, 0, 0);
    output = ansiPtr;
    delete[] ansiPtr;
    return output.length() > 0;
  }


  bool utf8_2_utf32( const std::string& input, std::u32string& output ) {
    try {
      std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
      output = conv.from_bytes( input );
      return true;
    }
    catch( std::range_error ex ) {
      return ansi_2_utf32( input, output, GetACP() ); // TODO script CP 
    }
    return true;
  }


  bool utf32_2_utf8( const std::u32string& input, std::string& output ) {
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


  bool charPtr_2_utf32( const std::string& input, std::u32string& output ) {
    int encoding = StaticEncoding != 0 ?
      StaticEncoding :
      GameEncoding;

    int result = encoding == CP_UTF8 ?
      utf8_2_utf32( input, output ) :
      ansi_2_utf32( input, output, encoding );

    return result;
  }


  bool utf32_2_charPtr( const std::u32string& input, std::string& output ) {
    int encoding = StaticEncoding != 0 ?
      StaticEncoding :
      GameEncoding;

    return encoding == CP_UTF8 ?
      utf32_2_utf8( input, output ) :
      utf32_2_ansi( input, output, encoding );
  }


  bool utf32_2_charPtr( const std::u32string& input, zSTRING& output ) {
    std::string ansi;
    int ok = utf32_2_charPtr( input, ansi );
    output = ansi.c_str();
    return ok;
  }
}