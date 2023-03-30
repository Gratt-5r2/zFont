// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  HOOK Hook_zCView_BlitText PATCH( &zCView::BlitText, &zCView::BlitText_Union );

  void zCView::BlitText_Union() {
    CheckAutoScroll();
    CheckTimedText();
    if( !isOpen )
      return;

    zCList<zCViewText>* list = textLines.GetNextInList();
    while( list ) {
      zCViewText* textView = list->GetData();
      list = list->GetNextInList();
      int x = pposx + nax( textView->posx );
      int y = pposy + nay( textView->posy );
      SetFont( textView->font );
      if( textView->colored )
        SetFontColor( textView->color );
      PrintChars_Union( x, y, textView->text );
    }
  }


  HOOK Hook_zCView_Print PATCH( &zCView::Print, &zCView::Print_Union );

  void zCView::Print_Union( int x, int y, const zSTRING& line ) {
    if( !font )
      return;

    scrollTimer = 0;
    if( viewID == VIEW_VIEWPORT || this == screen ) {
      zTRnd_AlphaBlendFunc oldBlendFunc = zrenderer->GetAlphaBlendFunc();
      if( alphafunc != zRND_ALPHA_FUNC_BLEND && alphafunc != zRND_ALPHA_FUNC_ADD )
        zrenderer->SetAlphaBlendFunc( zRND_ALPHA_FUNC_BLEND );
      else
        zrenderer->SetAlphaBlendFunc( alphafunc );

      PrintChars_Union( nax( x ), nay( y ), line );
      zrenderer->SetAlphaBlendFunc( oldBlendFunc );
    }
    else
      CreateText( x, y, line );
  }


  HOOK Hook_zCView_PrintChars PATCH( &zCView::PrintChars, &zCView::PrintChars_Union );

  void zCView::PrintChars_Union( int x, int y, const zSTRING& line ) {
    Font* ttf = Font::GetFont( font );
    if( !ttf )
      return;

    PushPrintRenderOptions( True, False, zRND_ZBUFFER_CMP_ALWAYS );

    int lside = pposx;
    int rside = pposx + psizex - 1;
    float farZ = GetCameraFarZ();

    std::u32string unicode;
    charPtr_2_utf32( line.ToChar(), unicode);
    ttf->PrepareLettersForText( unicode );

    for( int i = 0; i < unicode.length(); i++ ) {
      char32_t u32 = unicode[i];
      if( u32 == '\r' || u32 == '\n' )
        continue;

      Letter* letter = ttf->GetLetter( u32 );
      if( !letter )
        continue;

      zVEC2 pos1, pos2;
      letter->GetScreenPositions( x, y, pos1, pos2 );
      zCTex_D3D* texture = letter->Map->GetTexture();
      zrenderer->DrawTile( texture, pos1, pos2, farZ, letter->UV[VA], letter->UV[VB], fontColor );
      x += letter->Glyph->PenWidth;
    }

    PopPrintRenderOptions();
  }
}