// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
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


  void Filter_QuadraticShadow::ApplyFilter() {
    for( int y = 0; y < Height; y++ ) {
      zCOLOR* row = Pixels[y];
      for( int x = 0; x < Width; x++ ) {
        zCOLOR& pixel = row[x];
        if( pixel.alpha > 20 ) {
          pixel = GFX_RED;
        }
      }
    }
    // TODO
  }
}