// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  HOOK Hook_zCView_Printwin PATCH( &zCView::Printwin, &zCView::Printwin_Union );

  void zCView::Printwin_Union( const zSTRING& text ) {
    Font* ttf = Font::GetFont( font );
    if( !ttf )
      return;

    if( (intflags & VIEW_I_TEXT) == 0 )
      InitPrintwin();
    int viewWidth = nax( px2 - px1 );

    while( true ) {
      int index = text.Search( "\\n", 0 );
      if( index == -1 )
        break;
      text.Deleted( "\\n", zSTR_ONLY );
      text.Inserted( index, "\n" );
    }

    Array<zSTRING> lines = ViewSplitLines( text, viewWidth, ttf );
    for( int i = 0; i < lines.GetNum(); i++ ) {
      lines[i].TrimLeft( ' ' );
      Nextline( lines[i], 0.0f, Null, Null );
    }
  }


  HOOK Hook_zCView_PrintMessage PATCH( &zCView::PrintMessage, &zCView::PrintMessage_Union );

  void zCView::PrintMessage_Union( const zSTRING& npcName, const zSTRING& text, float time, zCOLOR& color ) {
    Font* ttf = Font::GetFont( font );
    if( !ttf )
      return;

    ClrPrintwin();
    int viewWidth = nax( px2 - px1 );
    if( !npcName.IsEmpty() ) {
      zCOLOR npcNameColor = GFX_WHITE;
      Nextline( npcName, time, &npcNameColor, Null );
    }

    Array<zSTRING> lines = ViewSplitLines( text, viewWidth, ttf );
    for( int i = 0; i < lines.GetNum(); i++ ) {
      lines[i].TrimLeft( ' ' );
      Nextline( lines[i], 0.0f, &color, Null );
    }
  }


  HOOK Hook_zCView_PrintMessageCXY PATCH( &zCView::PrintMessageCXY, &zCView::PrintMessageCXY_Union );

  void zCView::PrintMessageCXY_Union( const zSTRING& npcName, const zSTRING& text, float time, zCOLOR& color ) {
    Font* ttf = Font::GetFont( font );
    if( !ttf )
      return;

    ClrPrintwin();
    int viewWidthPx = nax( px2 - px1 );
    int viewHeightVt = py2 - py1;
    int fontYVt = FontY();

    Array<zSTRING> lines = ViewSplitLines( text, viewWidthPx, ttf );
    int textHeightVt = lines.GetNum() * fontYVt;
    if( !npcName.IsEmpty() )
      textHeightVt += fontYVt;

    int padding = (viewHeightVt - textHeightVt) / -2 - (fontYVt / 2);

    if( !npcName.IsEmpty() ) {
      zCOLOR npcNameColor = GFX_WHITE;
      Nextline( npcName, time, &npcNameColor, &padding );
    }

    for( int i = 0; i < lines.GetNum(); i++ ) {
      lines[i].TrimLeft( ' ' );
      Nextline( lines[i], 0.0f, &color, &padding );
    }
  }
}