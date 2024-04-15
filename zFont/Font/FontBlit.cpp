// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  Array<FontMapBlitContext> FontMap::BlitQueue;


  void FontMap::BlitRequest( Letter* letter ) {
    CumulativeBlitContext.InsertLetter( letter );
  }


  void FontMap::BlitLetters( const FontMapBlitContext& context ) {
    if( context.IsEmpty() )
      return;

    union {
      void* contentPtr;
      uint* content32;
    };

    int pitch;
    Texture->Lock( 0 );
    Texture->GetTextureBuffer( 0, contentPtr, pitch );
    int contentWidth = pitch / 4;

    zCOLOR color = Color;
    bool colorIsDark = color.GetBrightness() < 150;

    int contentMax = contentWidth * Height;
    for each( auto letter in context.Letters ) {
      int width = letter->Width;
      int height = letter->Height;
      byte* mask = (byte*)letter->Glyph->Image.pixels;

      int rowLetter = 0;
      int rowContent = letter->X + letter->Y * contentWidth;
      for( int y = 0; y < height; y++, rowLetter += width, rowContent += contentWidth ) {
        for( int x = 0; x < width; x++ ) {
          int xyLetter = rowLetter + x;
          int xyContent = rowContent + x;
          color.alpha = mask[xyLetter];
          content32[xyContent] = color.dword;
        }
      }

      Filter_QuadraticShadow filterShadow;
      filterShadow.SetTextureRange( content32, letter->X, letter->Y, Width, Height );
      filterShadow.SetFiltrationSize( letter->Width, letter->Height );
      if( DrawShadow && !colorIsDark )
        filterShadow.ApplyFilter();
      
      Filter_Highlight filterHighlight;
      filterHighlight.SetPreviousFilter( filterShadow );
      if( DrawHighlight && !colorIsDark )
        filterHighlight.ApplyFilter();
    }

    Texture->Unlock();
  }


  void FontMap::FlushBlitContext() {
    if( CumulativeBlitContext.NeedToBlit() ) {
      BlitQueue.Insert( CumulativeBlitContext );
      CumulativeBlitContext.Invalidate = false;
      CumulativeBlitContext.Clear();
    }
  }


  zCTex_D3D* FontMap::GetTexture() {
    return Texture;
  }


  void FontMap::BlitProcess() {
    for each( auto& context in BlitQueue )
      context.Map->BlitLetters( context );
    BlitQueue.Clear();
  }
}