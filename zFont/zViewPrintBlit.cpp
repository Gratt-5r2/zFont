// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  HOOK Hook_zCViewPrint_PrintLines PATCH_IF( &zCViewPrint::PrintLines, &zCViewPrint::PrintLines_Union, 1 );

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


  inline bool IsSequentalSymbol( char32_t u32 ) {
    const char32_t seq[] = { 
      L'.', L',', L';', L':', L'(', L')', L'\'',
      L'。', L'“', L'”', L'（', L'）', L'《',
      L'》', L'？', L'！', L'，', L'、', L'：' 
    };

    const int seqLen = sizeof( seq ) / sizeof( char32_t );
    for( int i = 0; i < seqLen; i++ )
      if( u32 == seq[i] )
        return true;

    if( IsChineseSymbol( u32 ) )
      return false;

    return iswalpha( u32 );
  }


  void zCViewPrint::PrintLines_Union( zSTRING& text ) {
    Font* ttf = Font::GetFont( ViewFont.Font );
    if( !ttf )
      return;

    std::u32string unicode;
    charPtr_2_utf32( text, unicode );
    uint index = 0;

    zCPosition lside, rside;
    GetPixelBorder( lside, rside );
    int viewWidth = rside.X - lside.X - 14;

    int start   = 0;
    int lineLen = 0;
    int wordLen = 0;

    for( uint end = 0; end <= unicode.length(); end++ ) {
      char32_t u32 = unicode[end];
      Letter* letter = ttf->GetLetter( u32 );
      int u32len = letter ? letter->Glyph->PenWidth : 0;

      if( u32 == '\t' ) {
        lineLen += ttf->GetLetter( ' ' )->Glyph->PenWidth * 4;
        continue;
      }

      if( !IsSequentalSymbol( u32 ) ) {
        if( u32 == '\n' || u32 == '\0' || lineLen + wordLen > viewWidth ) {
          if( u32 == ' ' )
            end++;

          std::u32string subUtf32 = unicode.substr( start, end - start );
          std::string subUtf8;
          utf32_2_utf8( subUtf32.c_str(), subUtf8);
          zSTRING subZstr( subUtf8.c_str() );
          PrintLine( subZstr );
          start = end;
          lineLen = wordLen;
          wordLen = 0;
        }
        else {
          lineLen += wordLen + u32len;
          wordLen = 0;
        }
      }
      else
        wordLen += u32len;
    }
  }


  HOOK Hook_zCViewPrint_BlitTextCharacters PATCH_IF( &zCViewPrint::BlitTextCharacters, &zCViewPrint::BlitTextCharacters_Union, 1 );

  void zCViewPrint::BlitTextCharacters_Union( zCViewText2* viewText, zCFont* ingameFont, zCOLOR& fontColor ) {
    Font* ttf = Font::GetFont( ingameFont );
    if( !ttf )
      return;

    PushPrintRenderOptions( True, False, zRND_ZBUFFER_CMP_ALWAYS );

    zPOS lside, rside;
    GetPixelBorder( lside, rside );
    float farZ = GetCameraFarZ();

    std::u32string unicode;
    utf8_2_utf32( viewText->Text, unicode );
    ttf->PrepareLettersForText( unicode );

    zCPosition pen0  = PixelPosition;
               pen0 += viewText->PixelPosition;
               pen0 += OffsetTextPixel;
    zCPosition pen   = pen0;

    for( int i = 0; i < unicode.length(); i++ ) {
      char32_t u32 = unicode[i];
      if( u32 == '\n' || u32 == '\r' )
        continue;

      if( u32 == '\t' ) {
        pen.X += ttf->GetLetter( ' ' )->Glyph->PenWidth * 4;
        continue;
      }

      Letter* letter = ttf->GetLetter( u32 );
      if( !letter )
        continue;

      int u32len = letter->Glyph->PenWidth;

      zVEC2 pos1, pos2;
      letter->GetScreenPositions( pen.X, pen.Y, pos1, pos2 );
      zCTex_D3D* texture = letter->Map->GetTexture();
      zrenderer->DrawTile( texture, pos1, pos2, farZ, letter->UV[VA], letter->UV[VB], fontColor );
      pen.X += u32len;
    }

    PopPrintRenderOptions();
  }
}