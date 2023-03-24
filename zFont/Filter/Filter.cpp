// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  Filter::Filter( Filter& filter ) : Pixels( filter.Pixels ), Width( filter.Width ), Height( filter.Height ) {
  }


  void Filter::SetTextureRange( void* pixels, int x, int y, int w, int h ) {
    zCOLOR* content = (zCOLOR*)pixels;
    int start = y * w + x;
    for( int i = 0; i < h; i++ ) {
      Pixels.Insert( content + start );
      start += w;
    }
  }


  void Filter::SetFiltrationSize( int w, int h ) {
    Width = w;
    Height = h;
  }

 inline zCOLOR Mix_Color(zCOLOR fg, zCOLOR bg) {
    // fg - foreground
    // bg - background
    zCOLOR new_color;
    if (fg.alpha == 255) return fg;
    unsigned int alpha = fg.alpha + 1;
    unsigned int inv_alpha = 256 - fg.alpha;
    new_color.r = (unsigned char)((alpha * fg.r + inv_alpha * bg.r) >> 8);
    new_color.g = (unsigned char)((alpha * fg.g + inv_alpha * bg.g) >> 8);
    new_color.b = (unsigned char)((alpha * fg.b + inv_alpha * bg.b) >> 8);
    new_color.alpha = (unsigned char)((alpha * fg.alpha + inv_alpha * bg.alpha) >> 8);;
    new_color.alpha = new_color.alpha >> 1;
    return new_color;
  }

 __declspec(naked) byte __fastcall add_bytes_overflow(byte b0, byte b1) {
   _asm {
     mov al, cl
     add al, ch
     jnc nof
     mov al, 0xFF
     nof:
     ret
   }
 }

 const zCOLOR highlight = zCOLOR(12, 12, 12);

 inline zCOLOR Brighten(zCOLOR c) {
   c.r = add_bytes_overflow(c.r, highlight.r );
   c.g = add_bytes_overflow(c.g, highlight.g );
   c.b = add_bytes_overflow(c.b, highlight.b );
   return c;
 }

  void Filter_QuadraticShadow::ApplyShadowHorizontal( int x0, int x1, int y ) {
    int length = x1 - x0;
    if( length <= 2 )
      return;

    zCOLOR& leftPx  = Pixels[y][x0];
    leftPx.r >>= 1;
    leftPx.g >>= 1;
    leftPx.b >>= 1;

    zCOLOR& rightPx = Pixels[y][x1];
    rightPx.r = rightPx.r > 127 ? 255 : rightPx.r >> 1;
    rightPx.g = rightPx.g > 127 ? 255 : rightPx.g >> 1;
    rightPx.b = rightPx.b > 127 ? 255 : rightPx.b >> 1;
  }


  // /*__declspec(naked)*/ byte __fastcall add_bytes_overflow( byte b0, byte b1 ) {
  //   int r = int( b0 ) + int( b1 );
  //   return r > 255 ? 255 : r;
  // 
  //   _asm {
  //     mov al, b0
  //     add al, b1
  //     jnc nof
  //     mov al, 0xFF
  //   nof:
  //     ret
  //   }
  // }

#if 1
  __declspec(naked) byte __fastcall add_bytes_overflow( byte b0, byte b1 ) {
    _asm {
      mov al, cl
      add al, ch
      jnc nof
      mov al, 0xFF
    nof:
      ret
    }
  }
#else
  byte add_bytes_overflow( byte b0, byte b1 ) {
    int r = int( b0 ) + int( b1 );
    return r > 255 ? 255 : r;
  }
#endif



  byte NoiseIterator = 0;
  const int NoiseLength = 0xFF;
  const byte* Noise = []() {
    byte* array = new byte[NoiseLength];
    for( int i = 0; i < NoiseLength; i++ ) {
      array[i] = (rand() % 4) * 10;
      cmd << array[i] << " ";
    }
    cmd << endl;
    return array;
  }();

  inline byte GetNoise() {
    return Noise[NoiseIterator++];
  }


  void Filter_QuadraticShadow::ApplyFilter() {
    int bnd = 2; // diagonal boundary width in pixels
    for( int y = Height; y > bnd; y-- ) {
      zCOLOR* row = Pixels[y];
      for( int x = Width; x > bnd; x-- ) {
        zCOLOR& pixel = row[x];
          pixel = Mix_Color(Pixels[y][x], zCOLOR(0, 0, 0, Pixels[y-bnd][x-bnd].alpha));
      }
    }
  }

  void Filter_Highlight::ApplyFilter() {
    bool found = false;
    const int high_width = 2;
    int w=0;
    for (int i = 0; i < Height; i++) {
      for (int j = 0; j < i; j++) {
        for (int k = i; k >= 0; k--) {
          if (Pixels[j][k].alpha > 192) {
            Pixels[j][k] = Brighten(Pixels[j][k]);
            if (!found) w = i;
            found = true;
          }
        }
      }
      if (found && i == w + high_width) return;
    }
  }
}