// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  HOOK Hook_zCFont_GetFontY PATCH( &zCFont::GetFontY, &zCFont::GetFontY_Union );

  int zCFont::GetFontY_Union() {
    int value = THISCALL( Hook_zCFont_GetFontY )();
    return value * FontScale;
  }


  HOOK Hook_zCFont_GetFontX PATCH( &zCFont::GetFontX, &zCFont::GetFontX_Union );

  int zCFont::GetFontX_Union( zSTRING const& line ) {
    int sizeX = 0;
    Font* ttf = Font::GetFont( this );
    if( !ttf )
      return 0;

    std::u32string unicode;
    charPtr_2_utf32( line.ToChar(), unicode);
    for( int i = 0; i < unicode.length(); i++ ) {
      char32_t u32 = unicode[i];
      if( u32 == '\r' || u32 == '\n' )
        continue;

      Letter* letter = ttf->GetLetter( u32 );
      if( letter )
        sizeX += letter->Glyph->PenWidth;
    }

    return sizeX;
  }


  HOOK Hook_zCView_FontY PATCH( &zCView::FontY, &zCView::FontY_Union );

  int zCView::FontY_Union() {
    if( font ) {
      Font* ttf = Font::GetFont( font );
      if( ttf )
        return any( ttf->FontProto->SizePx );
    }
    return THISCALL( Hook_zCView_FontY )();
  }


  HOOK Hook_zCView_FontSize PATCH( &zCView::FontSize, &zCView::FontSize_Union );

  int zCView::FontSize_Union( zSTRING& line ) {
    if( font )
      return anx( font->GetFontX_Union( line ) );
    return 0;
  }
}