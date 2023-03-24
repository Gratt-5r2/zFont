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
    for( int y = 0; y < Height; y++ ) {
      zCOLOR* row = Pixels[y];

      for( int x = 0; x < Width; x++ ) {
        zCOLOR& pixel = row[x];

        pixel.r = pixel.r - GetNoise(); //add_bytes_overflow( pixel.r, GetNoise() );
        pixel.g = pixel.g - GetNoise(); //add_bytes_overflow( pixel.g, GetNoise() );
        pixel.b = pixel.b - GetNoise(); //add_bytes_overflow( pixel.b, GetNoise() );
      }
    }
  }
}