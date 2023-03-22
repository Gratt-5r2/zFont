// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  void Glyph::Free() {
    delete[] Image.pixels;
  }


  void Letter::GetScreenPositions( int x, int y, zVEC2& start, zVEC2& end ) {
    start[0] = x + Glyph->OffsetX;
    start[1] = y + Glyph->OffsetY + Map->OwnedFont->FontProto->Ascender;
    end[0] = start[0] + Width;
    end[1] = start[1] + Height;
  }
}