// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  Switch BlitEvent;
  Array<FontMapBlitContext> FontMap::BlitQueue;


  void FontMap::BlitRequest( Letter* letter ) {
    CumulativeBlitContext.Letters.Insert( letter );
  }


  void zCOLOR::Invert() {
    std::swap( r, alpha );
    std::swap( g, b );
  }


  void FontMap::BlitLetters( FontMapBlitContext& context ) {
    if( context.IsEmpty() )
      return;

    int pitch;
    union {
      void* contentPtr;
      WORD* content16;
      uint* content32;
    };
    TextureConvert->Lock( zTEX_LOCK_WRITE );
    TextureConvert->GetTextureBuffer( 0, contentPtr, pitch );

    if( FntTexFormat == zRND_TEX_FORMAT_BGRA_8888 ) {
      zCOLOR color = Color;
      //color.Invert();
      for each( auto letter in context.Letters ) {
        int width = letter->Width;
        int height = letter->Height;
        byte* mask = (byte*)letter->Glyph->Image.pixels;

        int rowLetter = 0;
        int rowContent = letter->X + letter->Y * Width;
        for( int y = 0; y < height; y++, rowLetter += width, rowContent += Width ) {
          for( int x = 0; x < width; x++ ) {
            int xyLetter = rowLetter + x;
            int xyContent = rowContent + x;
            // color.alpha = mask[xyLetter];
            color.alpha = mask[xyLetter];
            content32[xyContent] = color.dword;
          }
        }

        Filter_QuadraticShadow filter;
        filter.SetTextureRange( content32, letter->X, letter->Y, Width, Height );
        filter.SetFiltrationSize( letter->Width, letter->Height );
        filter.ApplyFilter();

        Filter_Highlight filter2;
        filter2.SetTextureRange(content32, letter->X, letter->Y, Width, Height);
        filter2.SetFiltrationSize(letter->Width, letter->Height);
        filter2.ApplyFilter();
      }

      zCTextureConvert* texConvert_DXT3 = zrenderer->CreateTextureConvert();
      zCTextureConvert::CopyContents( TextureConvert, texConvert_DXT3 );
      texConvert_DXT3->ConvertToNewFormat(zRND_TEX_FORMAT_ARGB_4444);
      zCTextureConvert::CopyContents( texConvert_DXT3, Texture[BackTextureID] );
      delete texConvert_DXT3;
      
#if 0
      zTRnd_TextureFormat currentFormat = Texture[BackTextureID]->GetTextureInfo().texFormat;
      zTRnd_TextureFormat nextFormat;
      if( currentFormat == zRND_TEX_FORMAT_ARGB_4444 )
        nextFormat = zRND_TEX_FORMAT_DXT3;
      else if( currentFormat != zRND_TEX_FORMAT_ARGB_4444 )
        nextFormat = zRND_TEX_FORMAT_ARGB_4444;
      
      zCTextureConvert* texConvert_RGBA4444 = zrenderer->CreateTextureConvert();
      zCTextureConvert::CopyContents( TextureConvert, texConvert_RGBA4444 );
      texConvert_RGBA4444->ConvertToNewFormat( nextFormat );
      zCTextureConvert::CopyContents( texConvert_RGBA4444, Texture[BackTextureID] );
      delete texConvert_RGBA4444;
      /// zCTextureConvert::CopyContents( TextureConvert, Texture[BackTextureID] );
#endif
    }
    else if( FntTexFormat == zRND_TEX_FORMAT_ARGB_4444 ) {
      union {
        struct {
        int r : 4;
        int g : 4;
        int b : 4;
        int alpha : 4;
        };
        WORD word;
      } color;
      // color.alpha = Color.alpha >> 1;
      color.r = Color.r / 2;
      color.g = Color.g / 2;
      color.b = Color.b / 2;

      for each( auto letter in context.Letters ) {
        int width = letter->Width;
        int height = letter->Height;
        byte* mask = (byte*)letter->Glyph->Image.pixels;

        int rowLetter = 0;
        int rowContent = letter->X + letter->Y * Width;
        for( int y = 0; y < height; y++, rowLetter += width, rowContent += Width ) {
          for( int x = 0; x < width; x++ ) {
            int xyLetter = rowLetter + x;
            int xyContent = rowContent + x;
            color.alpha = mask[xyLetter]  / 2;
            content16[xyContent] = color.word;
          }
        }
      }
      zCTextureConvert::CopyContents( TextureConvert, Texture[BackTextureID] );
    }

    TextureConvert->Unlock();
  }


  ThreadLocker FlushBlitContextLocker;
  void FontMap::FlushBlitContext() {
    FlushBlitContextLocker.Enter();
    // Wait for the completion of previous tasks
    // to do less blocking operations. Because
    // lock/unlock are the heaviest functions.
    // Too much efficient to accumulate more
    // letters and blit everything at once.
    if( BlitQueue.IsEmpty() ) {
      if( !CumulativeBlitContext.IsEmpty() ) {
        BlitQueue.Insert( CumulativeBlitContext );
        BlitEvent.TurnOn();
      }
      CumulativeBlitContext.Clear();
    }
    FlushBlitContextLocker.Leave();
  }


  ThreadLocker SwapTexturesLocker;
  void FontMap::SwapTextures() {
    SwapTexturesLocker.Enter();
    std::swap( Texture[FrontTextureID], Texture[BackTextureID] );
    SwapTexturesLocker.Leave();
  }


  zCTexture* FontMap::GetTexture( bool lock ) {
    if( lock ) SwapTexturesLocker.Enter();
    zCTexture* frontTexture = Texture[FrontTextureID];
    if( lock ) SwapTexturesLocker.Leave();
    return frontTexture;
  }


  void FontMap::BlitProcess() {
    while( true ) { // TODO exit
      BlitEvent.WaitOn();
      while( true ) {
        FlushBlitContextLocker.Enter();
        if( BlitQueue.IsEmpty() ) {
          FlushBlitContextLocker.Leave();
          break;
        }

        FontMapBlitContext context = BlitQueue[0];
        FlushBlitContextLocker.Leave();

        context.Map->BlitLetters( context );
        context.Map->SwapTextures();

        FlushBlitContextLocker.Enter();
        BlitQueue.RemoveAt( 0 );
        FlushBlitContextLocker.Leave();
      }
      BlitEvent.TurnOff();
    }
  }
}