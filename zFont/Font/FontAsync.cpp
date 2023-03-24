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

    union {
      void* contentPtr;
      WORD* content16;
      uint* content32;
    };
    // TextureConvert->Lock( zTEX_LOCK_WRITE );
    // TextureConvert->GetTextureBuffer( 0, contentPtr, pitch );

    DDSURFACEDESC2 ddsd;
    ZeroMemory( &ddsd, sizeof( ddsd ) );
    ddsd.dwSize = sizeof( ddsd );
    zCTex_D3D* texture = Texture[BackTextureID];
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = Width;
    rect.bottom = Height;

    // while( zCRnd_D3D::xd3d_dd7inuse ) Sleep( 0 );

    // texture->Lock(0);
    // if( texture->xtex_pddtex[0]->IsLost() )
    //   texture->xtex_pddtex[0]->Restore();


    texture->xtex_pddtex[0]->Lock( Null, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_NOSYSLOCK | DDLOCK_WAIT, Null );
    contentPtr = ddsd.lpSurface;
    int pitch = ddsd.lPitch / 4;

#if 0
    int pitch;
    texture->Lock( zTEX_LOCK_READ | zTEX_LOCK_WRITE );
    texture->GetTextureBuffer( 0, contentPtr, pitch );
    pitch /= 4;
    cmd << pitch << endl;
#endif

    if( FntTexFormat == zRND_TEX_FORMAT_BGRA_8888 ) {
      zCOLOR color = Color;
      //color.Invert();
      for each( auto letter in context.Letters ) {
        int width = letter->Width;
        int height = letter->Height;
        byte* mask = (byte*)letter->Glyph->Image.pixels;

        int rowLetter = 0;
        int rowContent = letter->X + letter->Y * pitch; // Width;
        for( int y = 0; y < height; y++, rowLetter += width, rowContent += pitch ) {
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
    }


    texture->xtex_pddtex[0]->Unlock( Null );
    texture->Unlock();
    //Vermerke, dass die Textur nun eingecacht ist
    //texture->xtex_locked = FALSE;
    //texture->cacheState=(zRES_CACHED_IN);
    //texture->xtex_decompress = FALSE;
    // texture->Unlock();
    // TextureConvert->Unlock();
  }


  ThreadLocker FlushBlitContextLocker;
  void FontMap::FlushBlitContext() {
    MP FlushBlitContextLocker.Enter();
    // Wait for the completion of previous tasks
    // to do less blocking operations. Because
    // lock/unlock are the heaviest functions.
    // Too much efficient to accumulate more
    // letters and blit everything at once.
    if( !Multithreading || BlitQueue.IsEmpty() ) {
      if( !CumulativeBlitContext.IsEmpty() ) {
        BlitQueue.Insert( CumulativeBlitContext );
        BlitEvent.TurnOn();
      }
      CumulativeBlitContext.Clear();
    }
    MP FlushBlitContextLocker.Leave();
  }


  ThreadLocker SwapTexturesLocker;
  void FontMap::SwapTextures() {
    MP SwapTexturesLocker.Enter();
    // std::swap( Texture[FrontTextureID], Texture[BackTextureID] ); // TODO
    MP SwapTexturesLocker.Leave();
  }


  zCTex_D3D* FontMap::GetTexture( bool lock ) {
    MP if( lock ) SwapTexturesLocker.Enter();
    // zCTex_D3D* frontTexture = Texture[FrontTextureID]; // TODO
    zCTex_D3D* frontTexture = Texture[BackTextureID];
    MP if( lock ) SwapTexturesLocker.Leave();
    return frontTexture;
  }


  void FontMap::BlitProcess() {
    MP BlitEvent.WaitOn();
    while( true ) {
      MP FlushBlitContextLocker.Enter();
      if( BlitQueue.IsEmpty() ) {
        MP FlushBlitContextLocker.Leave();
        break;
      }

      FontMapBlitContext context = BlitQueue[0];
      MP FlushBlitContextLocker.Leave();

      context.Map->BlitLetters( context );
      context.Map->SwapTextures();

      MP FlushBlitContextLocker.Enter();
      BlitQueue.RemoveAt( 0 );
      MP FlushBlitContextLocker.Leave();
    }
    MP BlitEvent.TurnOff();
  }


  void FontMap::BlitProcessAsync() {
    while( true ) { // TODO exit
      BlitProcess();
    }
  }
}