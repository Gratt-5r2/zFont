// Supported with union (c) 2020 Union team
// Union SOURCE file
extern std::wstring utf8_to_utf16( const std::string& utf8 );
extern std::string utf16_to_utf8( const std::wstring& utf16 );

#include <locale>
#include <codecvt>

namespace GOTHIC_ENGINE {
  wstring MultiBytePtrToUnicodePtr( const char* mbptr, int mblen, uint codepage = CP_ACP ) {
    if( mblen == 0 )
      return L"";

    int wlen = mblen;
    if( mblen && codepage == CP_UTF8 ) {
      wlen = MultiByteToWideChar( codepage, MB_ERR_INVALID_CHARS, mbptr, mblen, 0, 0 );
      if( wlen == 0 )
        codepage = CP_ACP;
    }

    std::unique_ptr<wchar_t[]> wptr(new wchar_t[wlen + 1]);
    wptr.get()[wlen] = 0;
    if( mblen )
      MultiByteToWideChar( codepage, 0, mbptr, mblen, wptr.get(), wlen );

    return wstring( wptr.get(), wlen );
  }


#if 0
  class CvtUTF16 {
    static CvtUTF16 Cash[20];
    static int CashPosition;

    string Utf8;
    wstring Utf16;
  public:

    static wstring ConvertToUtf16( const string& utf8 ) {
      const int utf16Length = ::MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        utf8,
        utf8.Length(),
        nullptr,
        0
      );

      wchar_t* utf16 = new wchar_t[utf16Length];
      int result = ::MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        utf8,
        utf8.Length(),
        utf16,
        utf16Length
      );
      wstring Utf16 = wstring( utf16, utf16Length );
      delete[] utf16;
      return Utf16;
    }

#if 0
    static string ConvertToUtf8( const wstring& utf16 ) {
      const int utf8Length = ::WideCharToMultiByte(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        utf16,
        utf16.Length(),
        nullptr,
        0
      );

      char* utf8 = new char[utf8Length];
      int result = ::WideCharToMultiByte(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        utf16,
        utf16.Length(),
        utf8,
        utf8Length
      );
      string Utf8 = string( utf8, utf8Length );
      delete[] utf8;
      return Utf8;
    }
#endif

    const CvtUTF16& GetUtf16( const string& utf8 ) {
      for each( auto& cash in Cash )
        if( cash.Utf8.Compare( utf8 ) )
          return cash;
      
      auto& cash = Cash[CashPosition++ % 20];
      cash.Utf8 = utf8;
      cash.Utf16 = ConvertToUtf16( utf8 );
      return cash;
    }

#if 0
    const CvtUTF16& GetUtf8( const wstring& utf16 ) {
      static CvtUTF16 instance;
      instance.Utf16 = utf16;
      instance.Utf8 = ConvertToUtf8( utf16 );
      return instance;
    }
#endif
  };
#endif


  HOOK Hook_zCFont_GetFontY PATCH( &zCFont::GetFontY, &zCFont::GetFontY_Union );
  
  int zCFont::GetFontY_Union() {
    int value = THISCALL( Hook_zCFont_GetFontY )();
    return value * FontScale;
  }
  
  
  HOOK Hook_zCFont_GetFontX PATCH_IF( &zCFont::GetFontX, &zCFont::GetFontX_Union, 1 );
  
  int zCFont::GetFontX_Union( zSTRING const& line ) {
    int sizeX = 0;
    Font* fontUtf8 = Font::GetFont( this );
    if( !fontUtf8 )
      return 0;

    wstring unicode = MultiBytePtrToUnicodePtr( line, line.Length(), DefaultCodepage );
    for( int i = 0; i < unicode.Length(); i++ ) {
      wchar_t wc = unicode[i];
      if( wc == '\r' || wc == '\n' )
        continue;

      Letter* letter = fontUtf8->GetLetter( wc );
      if( letter )
        sizeX += letter->Glyph->PenWidth;
    }

    return sizeX;
  }



  HOOK Hook_zCView_FontY PATCH_IF( &zCView::FontY, &zCView::FontY_Union, 1 );

  int zCView::FontY_Union() {
    if( font ) {
      Font* fontUtf8 = Font::GetFont( font );
      if( fontUtf8 )
        return any( fontUtf8->FontProto->SizePx );
    }
    return THISCALL( Hook_zCView_FontY )();
  }


  HOOK Hook_zCView_FontSize PATCH_IF( &zCView::FontSize, &zCView::FontSize_Union, 1 );

  int zCView::FontSize_Union( zSTRING& line ) {
    if( font )
      return anx( font->GetFontX_Union( line ) );
    return 0;
  }


  const zCOLOR& zCFont::GetAverageColor() {
    if( fontTex ) {
      zCTex_D3D* texRnd = (zCTex_D3D*)fontTex;
      return texRnd->xtex_texinfo.averageColor;
    }
    return GFX_RED;
  }



	HOOK Hook_zCView_PrintChars PATCH_IF( &zCView::PrintChars, &zCView::PrintChars_Union, 1 );

  extern ThreadLocker SwapTexturesLocker;
	void zCView::PrintChars_Union( int x, int y, const zSTRING& line ) {
    Font* fontUtf8 = Null;
    if( font )
      fontUtf8 = Font::GetFont( font );

		if( !fontUtf8 )
			return;

		int	bilerpFiltering                = zrenderer->GetBilerpFilterEnabled();
		int	zBufferWriting                 = zrenderer->GetZBufferWriteEnabled();
		zTRnd_ZBufferCmp zBufferComparing  = zrenderer->GetZBufferCompare();
		zTRnd_AlphaBlendFunc alphaBlending = zrenderer->GetAlphaBlendFunc();

		zrenderer->SetBilerpFilterEnabled( True );
		zrenderer->SetZBufferWriteEnabled( False );
		zrenderer->SetZBufferCompare( zRND_ZBUFFER_CMP_ALWAYS );

		if( (alphaBlending != zRND_ALPHA_FUNC_ADD) && (alphaBlending != zRND_ALPHA_FUNC_BLEND) )
			zrenderer->SetAlphaBlendFunc( zRND_ALPHA_FUNC_BLEND );

		int leftSide = pposx;
		int rightSize = pposx + psizex - 1;

#if ENGINE >= Engine_G2
		float farZ = zCCamera::activeCam ? zCCamera::activeCam->nearClipZ + 1.0f : 1.0f;
#else
    float farZ = 1.0f;
#endif

    zVEC2 pos1;
    zVEC2 pos2;
    wstring unicode = MultiBytePtrToUnicodePtr( line, line.Length(), DefaultCodepage );
    for( int i = 0; i < unicode.Length(); i++ ) {
      wchar_t wc = unicode[i];
      if( wc == '\r' || wc == '\n' )
        continue;

      Letter* letter = fontUtf8->GetLetter( wc );
      if( !letter )
        continue;

      letter->GetScreenPositions( x, y, pos1, pos2 );

      SwapTexturesLocker.Enter();
      zCTexture* texture = letter->Map->GetTexture( false );
      zrenderer->SetPixelWriteEnabled( 1 );

      // zCOLOR color = letter->Map->OwnedFont->Color;
      // byte* pixels = (byte*)letter->Glyph->Image.pixels;
      // int row = 0;
      // for( int y = 0; y < letter->Height; y++, row += letter->Width ) {
      //   for( int x = 0; x < letter->Width; x++ ) {
      //     int pos = row + x;
      //     color.alpha = pixels[pos];
      //     color.r = color.alpha;
      //     zrenderer->SetPixel( pos1[VX] + x, pos1[VY] + y, color );
      //   }
      // }

      zrenderer->SetPixelWriteEnabled( 0 );
      zrenderer->DrawTile( texture, pos1, pos2, farZ, letter->UV[VA], letter->UV[VB], fontColor );
      SwapTexturesLocker.Leave();
      x += letter->Glyph->PenWidth;
    }

		zrenderer->SetBilerpFilterEnabled( bilerpFiltering );
		zrenderer->SetZBufferWriteEnabled( zBufferWriting );
		zrenderer->SetZBufferCompare( zBufferComparing );
	}

#if 0
  HOOK Hook_zCViewPrint_BlitTextCharacters PATCH_IF( &zCViewPrint::BlitTextCharacters, &zCViewPrint::BlitTextCharacters_Union, 1 );

  void zCViewPrint::BlitTextCharacters_Union( zCViewText2* viewText, zCFont* ingameFont, zCOLOR& color )
  {
    Font* fontUtf = Font::GetFont( ingameFont );
    if( !fontUtf )
      return THISCALL( Hook_zCViewPrint_BlitTextCharacters )( viewText, ingameFont, color );

    // THISCALL( Hook_zCViewPrint_BlitTextCharacters )(viewText, ingameFont, color);

    wstring utf16 = CvtUTF16::ConvertToUtf16( viewText->Text );
    zCPosition pen0  = PixelPosition;
               pen0 += viewText->PixelPosition;
               pen0 += OffsetTextPixel;
    zCPosition pen = pen0;

    zPOS leftSide   = PixelPosition;
    zPOS rightSize  = PixelPosition;
         rightSize += PixelSize;

    int fontY = ingameFont->GetFontY();


    // int letterDist = pFont->GetLetterDistance();
    // int spaceWidth = pFont->GetWidth( ' ' );
    zVEC2 pos1, pos2, texuv1, texuv2;

#if ENGINE >= Engine_G2
    float farZ = zCCamera::activeCam ? zCCamera::activeCam->nearClipZ + 1.0f : 1.0f;
#else
    float farZ = 1.0f;
#endif

    for( uint i = 0; i < utf16.Length(); i++ ) {
      wchar_t c = utf16[i];
      if( c == '\r' || c == '\r' )
        continue;

      if( c == '\t' ) {
        pen.X += fontUtf->GetLetter( ' ' )->Glyph->PenWidth * 4;
        continue;
      }

      Letter* letter = fontUtf->GetLetter( c );
      if( !letter )
        continue;

      if( c == '\n' || pen.X + letter->Glyph->PenWidth >= rightSize.X ) {
        pen.X = pen0.X;
        pen.Y += ingameFont->GetFontY();
      }

      pos1[0] = pen.X + letter->Glyph->OffsetX;
      pos1[1] = pen.Y + letter->Glyph->OffsetY + (fontUtf->FontProto->Ascender);
      pos2[0] = pos1[0] + letter->Width;
      pos2[1] = pos1[1] + letter->Height;

      SwapTexturesLocker.Enter();
      zCTexture* texture = letter->Map->GetTexture( false );
      zrenderer->DrawTile( texture, pos1, pos2, farZ, letter->UV[VA], letter->UV[VB], color );
      SwapTexturesLocker.Leave();
      pen.X += letter->Glyph->PenWidth;
    }
  }

  HOOK Hook_zCViewPrint_PrintLines PATCH_IF( &zCViewPrint::PrintLines, &zCViewPrint::PrintLines_Union, 1 );

#if 1
  void zCViewPrint::PrintLines_Union( zSTRING& text ) {
    if( !ViewFont.Font )
      return;

    Font* font = Font::GetFont( ViewFont.Font );
    if( !font )
      return THISCALL( Hook_zCViewPrint_PrintLines )(text);

    // PrintLine( text );

#if 1
    zPOS leftSize, rightSize;
    GetPixelBorder( leftSize, rightSize );
    int pen = leftSize.X;

    wstring utf16 = utf8_to_utf16( text.ToChar() ).c_str(); // CvtUTF16::ConvertToUtf16( text );
    uint index = 0;

    int length0 = 0;
    int length = 0;
    int start = 0;

    for( uint end = 0; end < utf16.Length(); end++ )
    {
      wchar_t c = utf16[end];
      Letter* letter = font->GetLetter( c );
      if( !letter )
        continue;

      length = letter->Glyph->PenWidth;
      if( c == L' ' || c == L'\n' || c >= 2000 /*L'機'*/ ) {
        if( c == '\n' || leftSize.X + length0 + length > rightSize.X ) {
          length0 = length;
          wstring wline = utf16.Copy( start, end - start );
          zSTRING aline = utf16_to_utf8( wline.ToWChar() ).c_str();
          PrintLine( aline );
          start = end;
          length = 0;
        }
        else {
          length0 += length;
        }
      }
      else {
        length0 += length;
      }
    }
#endif
  }
#endif
#endif


#if 0
  void zCViewPrint::PrintLines_Union( zSTRING& strText )
  {
    //
    //	CHECK FONT
    //
    if( !this->ViewFont.Font ) return;

    //
    //	CREATE LINES
    //
    zPOS	ptBorder[2];	this->GetPixelBorder( ptBorder[0], ptBorder[1] );

    zSTRING	strMessage = strText;
    zSTRING strWord = "";
    zSTRING strLine = "";
    zSTRING strDelimiters = " ,\n";
    int		nWordWidth = 0;
    int		nWord = 1;
    int		nPosX = ptBorder[0].X;

    //
    //	CALC WIDTH OF ONE SPACE 
    //
    zVEC2	vecRubbish[2];
    int		nWidthSpace = 0;

    this->ViewFont.Font->GetFontData( 'i', nWidthSpace, vecRubbish[0], vecRubbish[1] );

    //
    //	PRINT WORD-BY-WORD
    //
    strMessage += "\n";

    do
    {
      strWord = strMessage.PickWord( nWord, strDelimiters, zSTRING("\r\t ") );
      nWordWidth = this->ViewFont.Font->GetFontX( strWord );

      if( strWord == "\n" )
      {
        this->PrintLine( strLine );

        nPosX = ptBorder[0].X;
        strLine = "";
      }
      else if( strWord.Length() > 0 )
      {
        if( (nPosX + nWordWidth) < ptBorder[1].X )
        {
          strLine += strWord;
          strLine += " ";
          nPosX += nWordWidth + nWidthSpace;
        }
        else
        {
          this->PrintLine( strLine );

          strLine = strWord + " ";
          nPosX = ptBorder[0].X + nWordWidth + nWidthSpace;
        }
      }

      nWord++;
    } while( strWord.Length() > 0 );



    zSTRING line;
    for( uint i = 0; i < strMessage.Length(); i++ ) {
      char c = strMessage[i];
      if( c >= 0xC0 && c <= 0xFF )
    }
  }
#endif
}