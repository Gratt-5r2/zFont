// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	struct Glyph {
    char32_t Id;
    double OffsetX;
    double OffsetY;
    double PenWidth;
    SFT_Image Image;
    void Free();
	};


  struct Letter {
    Glyph* Glyph;
    FontMap* Map;
    zVEC2 UV[2];
    int X;
    int Y;
    int Width;
    int Height;
    void GetScreenPositions( int x, int y, zVEC2& start, zVEC2& end );
  };
}