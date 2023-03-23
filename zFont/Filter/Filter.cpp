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