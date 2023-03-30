// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  HOOK Hook_zCViewPrint_PrintLines PATCH( &zCViewPrint::PrintLines, &zCViewPrint::PrintLines_Union );


  void zCViewPrint::PrintLines_Union( zSTRING& text ) {
    Font* ttf = Font::GetFont( ViewFont.Font );
    if( !ttf )
      return;

    zCPosition lside, rside;
    GetPixelBorder( lside, rside );
    int viewWidth = rside.X - lside.X - 14;

    Array<zSTRING> lines = ViewSplitLines( text, viewWidth, ttf );
    for( int i = 0; i < lines.GetNum(); i++ )
      PrintLine( lines[i] );
  }


  HOOK Hook_zCViewPrint_BlitTextCharacters PATCH( &zCViewPrint::BlitTextCharacters, &zCViewPrint::BlitTextCharacters_Union );

  void zCViewPrint::BlitTextCharacters_Union( zCViewText2* viewText, zCFont* ingameFont, zCOLOR& fontColor ) {
    Font* ttf = Font::GetFont( ingameFont );
    if( !ttf )
      return;

    PushPrintRenderOptions( True, False, zRND_ZBUFFER_CMP_ALWAYS );

    zCPosition lside, rside;
    GetPixelBorder( lside, rside );
    float farZ = GetCameraFarZ();

    std::u32string unicode;
    charPtr_2_utf32( viewText->Text.ToChar(), unicode);
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