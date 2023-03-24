// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  static Array<FontGeneric*> FontsGeneric;
  static Map<string, Font*> Fonts;


  Glyph* FontGeneric::GetGlyph( wchar_t id ) {
    auto& pair = Glyphs[id];
    if( !pair.IsNull() )
      return pair;

    SFT_Glyph gl;
    SFT_Image img;
    SFT_GMetrics mtx;

    if( sft_lookup( Schrift, id, &gl ) < 0 )
      return Null;

    if( sft_gmetrics( Schrift, gl, &mtx ) < 0 )
      return Null;

    img.width = mtx.minWidth;
    img.height = mtx.minHeight;
    img.pixels = new byte[img.width * img.height];
    sft_render( Schrift, gl, img );

    Glyph* glyph = new Glyph();
    glyph->Id = id;
    glyph->OffsetX = mtx.leftSideBearing;
    glyph->OffsetY = mtx.yOffset;
    glyph->PenWidth = mtx.advanceWidth;
    glyph->Image = img;
    Glyphs.Insert( id, glyph );
    return glyph;
  }


  inline double CorrectHeightGp( SFT_Font* font, double height ) {
    SFT sft;
    ZeroMemory( &sft, sizeof( SFT ) );
    sft.font = font;
    sft.xScale = height;
    sft.yScale = height;
    sft.flags = SFT_DOWNWARD_Y;

    SFT_LMetrics lmtx;
    sft_lmetrics( &sft, &lmtx );
    double realHeight = lmtx.ascender - lmtx.descender;
    double ratio = height / realHeight;
    return height * ratio;
  }


  FontGeneric* FontGeneric::GetFont( const string& name, double size, FontMetrix metrix ) {
    size *= FontScale;
    bool gothicPoints = metrix == FontMetrix::Gp;
    if( gothicPoints )
      metrix = FontMetrix::Px;

    for each( auto font in FontsGeneric )
    {
      if( font->Name == name ) {
        if( metrix == FontMetrix::Px && font->SizePx == size ) return font;
        if( metrix == FontMetrix::Pt && font->SizePt == size ) return font;
      }
    }

    VirtualFile vfile( name );
    if( !vfile.IsExists() )
      return Null;

    byte* memory = new byte[vfile.Size()];
    vfile.ReadToEnd( memory );
    SFT_Font* fnt = sft_loadmem( memory, vfile.Size() );
    vfile.Close();

    if( !fnt )
      return Null;

    FontGeneric* font = new FontGeneric();
    font->Name = name;
    if( gothicPoints )
      size = CorrectHeightGp( fnt, size );

    if( metrix == FontMetrix::Px ) {
      font->SizePx = size;
      font->SizePt = Px2Pt( size );
    }
    else {
      font->SizePx = Pt2Px( size );
      font->SizePt = size;
    }

    SFT* sft = new SFT();
    ZeroMemory( sft, sizeof( SFT ) );
    sft->font = fnt;
    sft->xScale = font->SizePx;
    sft->yScale = font->SizePx;
    sft->flags = SFT_DOWNWARD_Y;
    font->Schrift = sft;

    SFT_LMetrics lmtx;
    sft_lmetrics( sft, &lmtx );
    font->Ascender = lmtx.ascender;
    font->Descender = lmtx.descender;
    font->CapHeight = font->SizePx - lmtx.ascender;

    FontsGeneric.Insert( font );
    return font;
  }


  FontGeneric::~FontGeneric() {
    sft_freefont( Schrift->font );
    delete Schrift;
    FontsGeneric.Remove( this );

    for each( auto glyph in Glyphs )
      glyph.GetValue()->Free();

    Glyphs.Clear();
    delete[] Memory;
  }



  FontMapBlitContext::FontMapBlitContext( const FontMapBlitContext& other ) {
    Map = other.Map;
    Letters = other.Letters;
  }


  bool FontMapBlitContext::IsEmpty() {
    return Letters.IsEmpty();
  }


  void FontMapBlitContext::Clear() {
    Letters.Clear();
  }



  const int OutlineSize = 4;

  bool FontMap::HasFreeSpace( int width, int height ) {
    if( Width - FilledX < (width + OutlineSize) ) {
      FilledX = 0;
      FilledY += LineHeight + OutlineSize;
      LineHeight = 0;
    }

    if( Height - FilledY < (height + OutlineSize) )
      return false;

    // TODO ?
    return true;
  }


  Letter* FontMap::CreateLetter( Glyph* glyph ) {
    int w = glyph->Image.width;
    int h = glyph->Image.height;
    if( !HasFreeSpace( w, h ) )
      return Null;

    Letter* letter = new Letter();
    letter->Glyph = glyph;
    letter->Map = this;
    letter->X = FilledX + (OutlineSize >> 1);
    letter->Y = FilledY + (OutlineSize >> 1);
    letter->Width = glyph->Image.width;
    letter->Height = glyph->Image.height;
    
    float u0 = (float)(letter->X - 2) / (float)Width;
    float u1 = (float)(letter->X + letter->Width + 2) / (float)Width;
    float v0 = (float)(letter->Y - 2) / (float)Height;
    float v1 = (float)(letter->Y + letter->Height + 2) / (float)Height;

    letter->UV[VA] = zVEC2( u0, v0 );
    letter->UV[VB] = zVEC2( u1, v1 );
    
    FilledX += glyph->Image.width + OutlineSize;
    if( LineHeight < h )
      LineHeight = h;

    Letters.Insert( letter );
    BlitRequest( letter );
    return letter;
  }


  extern Switch BlitEvent;
  void FontMap::Free() {
    BlitEvent.WaitOff();
    Texture[FrontTextureID]->Release();
    Texture[BackTextureID]->Release();
    delete TextureConvert;
  }


  FontMap* FontMap::Create( Font* font, const zCOLOR& color, int width, int height ) {
    FontMap* map = new FontMap();
    map->OwnedFont = font;
    map->CumulativeBlitContext.Map = map;
    map->Width = width;
    map->Height = height;
    map->Color = color;

    zCTextureInfo texInfo;
    texInfo.averageColor = GFX_BLACK;
    texInfo.numMipMap = 1;
    texInfo.sizeX = width;
    texInfo.sizeY = height;
    texInfo.texFormat = FntTexFormat;
    map->TextureConvert = zrenderer->CreateTextureConvert();
    map->TextureConvert->SetTextureInfo( texInfo );

    for( int i = 0; i < MapTexturesCount; i++ ) {
      map->Texture[i] = zrenderer->CreateTexture();
      map->Texture[i]->CacheIn( -1 );
      map->Texture[i]->hasAlpha = True;
      // auto texD3D = map->Texture[i]->CastTo<zCTex_D3D>();
      // if( texD3D ) {
      //   texD3D->xtex_texinfo.
      // }
    }

    return map;
  }


  Font* Font::GetFont( const string& name, const zCOLOR& color, double defaultSize, FontMetrix metrix ) {
    string nameUpper = string::Combine( "%s%F", name, defaultSize ).Upper();
    auto& pair = Fonts[nameUpper];
    if( !pair.IsNull() )
      return pair;

    string nameNoExt = nameUpper.GetWord( "." );
    FontGeneric* fontGeneric = FontGeneric::GetFont( nameNoExt + ".ttf", defaultSize, metrix ); // TODO optional names
    if( !fontGeneric ) {
      fontGeneric = FontGeneric::GetFont( "Default.ttf", defaultSize, metrix );
      if( !fontGeneric )
        return Null;
    }

    Font* font = new Font();
    font->Name = nameNoExt;
    font->FontProto = fontGeneric;
    font->Color = color;
    font->PreRender();
    Fonts.Insert( nameUpper, font );
    return font;
  }


  Font* Font::GetFont( zCFont* ingameFont ) {
    if( !ingameFont )
      return GetFontDefault();

    string fontName = ingameFont->name;
    float fontSize = ingameFont->GetFontY();
    string nameUpper = string::Combine( "%z%F", fontName, fontSize ).Upper();
    auto& pair = Fonts[nameUpper];
    if( !pair.IsNull() )
      return pair;

    // Parse base color
    zCOLOR color = zCOLOR( 230, 205, 155 );
    if( fontName.HasWordI( "WHITE" ) ) {
      if( fontName.HasWordI( "_HI" ) )
        color = zCOLOR( 240, 240, 240 );
    }
    else if( fontName.HasWordI( "BOOK" ) ) {
      color = zCOLOR( 25, 25, 25 );
    }

    Font* font = GetFont( fontName, color, fontSize, FontMetrix::Gp );
    return font;
  }


  Font* Font::GetFontDefault( double defaultSize, FontMetrix metrix ) {
    return GetFont( "Default.ttf", GFX_WHITE, defaultSize, metrix );
  }


  Letter* Font::GetLetter( wchar_t id ) {
    if( id == 0 )
      return Null;

    auto& pair = Letters[id];
    if( !pair.IsNull() )
      return pair;

    Glyph* glyph = FontProto->GetGlyph( id );
    if( !glyph )
      return Null;

    FontMap* freeMap = GetMap( glyph->Image.width, glyph->Image.height );
    Letter* letter = freeMap->CreateLetter( glyph );
    if( !letter )
      return Null;

    Letters.Insert( id, letter );
    return letter;
  }


  FontMap* Font::GetMap( int requiredWidth, int requiredHeight ) {
    FontMap* map = Null;
    if( Maps.GetNum() > 0 ) {
      map = Maps.GetLast();
      if( map->HasFreeSpace( requiredWidth, requiredHeight ) )
        return map;
    }

    map = FontMap::Create( this, Color );
    Maps.Insert( map );
    return map;
  }


  void Font::BlitLetters() {
    for each( auto font in Fonts )
      for each( auto map in font.GetValue()->Maps )
        map->FlushBlitContext();
  }


  void Font::Free() {
    if( FontProto )
      FontProto->Release();

    for each( auto map in Maps )
      map->Free();

    Maps.Clear();
    Letters.Clear();
  }


#define COMPILE_LETTERS(from, to) for( int i = from; i <= to; i++ ) { GetLetter( i ); /*cmd << wstring( (wchar_t)i ) << " ";*/ } /*cmd << endl;*/

  void Font::PreRender() {
  //  cmd << "creating default chars begin... ";
  //  COMPILE_LETTERS( 0x0020, 0x007F ); // Basic Latin
  //  COMPILE_LETTERS( 0x00A0, 0x00FF ); // Latin-1 Supplement
  //  COMPILE_LETTERS( 0x0100, 0x017F ); // Latin Extended-A	
  //  COMPILE_LETTERS( 0x0180, 0x024F ); // Latin Extended-B	
  //  COMPILE_LETTERS( 0x0250, 0x02AF ); // IPA Extensions	 	
  //  COMPILE_LETTERS( 0x0300, 0x036F ); // Combining Diacritical Marks
  //  COMPILE_LETTERS( 0x0370, 0x03FF ); // Greek and Coptic
  //  COMPILE_LETTERS( 0x0400, 0x04FF ); // Cyrillic
  //  BlitLetters();
  //  BlitEvent.WaitOff();
  //  cmd << " done." << endl;
  }


  Font::~Font() {
    Free();
    Fonts.Remove( Name );
  }
}