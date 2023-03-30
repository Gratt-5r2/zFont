// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  int BilerpFiltering;
  int zBufferWriting;
  zTRnd_ZBufferCmp zBufferComparing;

  void PushPrintRenderOptions( int bilertFlt, int zbufWrt, zTRnd_ZBufferCmp zbufCmp ) {
    BilerpFiltering = zrenderer->GetBilerpFilterEnabled();
    zBufferWriting = zrenderer->GetZBufferWriteEnabled();
    zBufferComparing = zrenderer->GetZBufferCompare();
    auto AlphaBlendingFunc = zrenderer->GetAlphaBlendFunc();

    zrenderer->SetBilerpFilterEnabled( bilertFlt );
    zrenderer->SetZBufferWriteEnabled( zbufWrt );
    zrenderer->SetZBufferCompare( zbufCmp );

    if( (AlphaBlendingFunc != zRND_ALPHA_FUNC_ADD) && (AlphaBlendingFunc != zRND_ALPHA_FUNC_BLEND) )
      zrenderer->SetAlphaBlendFunc( zRND_ALPHA_FUNC_BLEND );
  }

  void PopPrintRenderOptions() {
    zrenderer->SetBilerpFilterEnabled( BilerpFiltering );
    zrenderer->SetZBufferWriteEnabled( zBufferWriting );
    zrenderer->SetZBufferCompare( zBufferComparing );
  }


  float GetCameraFarZ() {
#if ENGINE >= Engine_G2
    return zCCamera::activeCam ? zCCamera::activeCam->nearClipZ + 1.0f : 1.0f;
#else
    return 1.0f;
#endif
  }



  inline bool IsChineseSymbol( char32_t u32 ) {
    return
      u32 >= 0x04E00 && u32 <= 0x09FFF ||
      u32 >= 0x03400 && u32 <= 0x04DBF ||
      u32 >= 0x20000 && u32 <= 0x2A6DF ||
      u32 >= 0x2A700 && u32 <= 0x2B73F ||
      u32 >= 0x2B740 && u32 <= 0x2B81F ||
      u32 >= 0x2B820 && u32 <= 0x2CEAF ||
      u32 >= 0x2CEB0 && u32 <= 0x2EBEF ||
      u32 >= 0x30000 && u32 <= 0x3134F ||
      u32 >= 0x31350 && u32 <= 0x323AF ||
      u32 >= 0x0F900 && u32 <= 0x0FAFF ||
      u32 >= 0x2F800 && u32 <= 0x2FA1F;
  }


  inline bool IsWordSymbol( char32_t u32 ) {
    const char32_t seq[] = {
      L'.',L'…', L'!', L'?', L'¡', L'¿', L',', L';', L':', L'(', L')', L'\'', L'\"',
      L'。', L'“', L'”', L'（', L'）', L'《', L'》', L'？', L'！', L'，', L'、', L'：'
    };

    const int seqLen = sizeof( seq ) / sizeof( char32_t );
    for( int i = 0; i < seqLen; i++ )
      if( u32 == seq[i] )
        return true;

    if( IsChineseSymbol( u32 ) )
      return false;

    return iswalpha( u32 );
  }


  inline Array<zSTRING> ViewSplitLines( zSTRING text, int viewWidthPx, Font* ttf ) {
    Array<zSTRING> lines;

    std::u32string unicode;
    charPtr_2_utf32( text.ToChar(), unicode);
    uint index = 0;

    int lineStart = 0;
    int lineEnd = 0;
    int lineWidthPx = 0;
    int wordWidthPx = 0;

    for( int i = 0; i <= unicode.length(); i++ ) {
      char32_t u32 = unicode[i];
      Letter* letter = ttf->GetLetter( u32 );
      int letterWidth = letter ? letter->Glyph->PenWidth : 0;

      lineWidthPx += letterWidth;
      if( !IsWordSymbol( u32 ) ) {
        wordWidthPx = 0;
        lineEnd = i;
      }
      else
        wordWidthPx += letterWidth;

      if( u32 == '\n' || u32 == '\0' || lineWidthPx > viewWidthPx ) {
        std::u32string substr = unicode.substr( lineStart, lineEnd - lineStart );
        zSTRING charPtr;
        utf32_2_charPtr( substr, charPtr );
        lines.Insert( charPtr );
        lineStart = ++lineEnd;
        lineWidthPx = wordWidthPx;
      }
    }

    return lines;
  }
}