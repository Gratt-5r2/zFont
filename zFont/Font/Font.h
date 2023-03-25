// Supported with union (c) 2020 Union team
// Union HEADER file
#include <schrift/schrift.h>
#pragma comment(lib, "schrift/schrift.lib")

namespace GOTHIC_ENGINE {
#define Pt2Px(value) (value * 1.32814723273650)
#define Px2Pt(value) (value * 0.75292857248934)
#define FontTextureFormat zTRnd_TextureFormat::zRND_TEX_FORMAT_BGRA_8888
#define DefaultMapSize 1024


	bool CustomDirectDraw = false;
	float FontScale = 1.0;
	bool DrawShadow = true;
	bool DrawHighlight = true;
	string DefaultSystemFont = "timesbd.ttf";


	struct Glyph;
	struct Letter;
	struct FontMap;
	struct Font;


	enum class FontUnits {
		Px,
		Pt,
		Gp // 'Gothic parrots': aligns ascender and descender of the
			 // generated font by ingame font top and bottom edges
	};


	struct FontGeneric : public zCObject {
		string Name;
		double SizePx;
		double SizePt;
		double Ascender;
		double Descender;
		double CapHeight;
		Map<char32_t, Glyph*> Glyphs;
		byte* Memory;
		SFT* Schrift;
		Glyph* GetGlyph( char32_t id );
		static FontGeneric* GetFont(const string& name, double size, FontUnits units = FontUnits::Pt);
		static FontGeneric* GetFontDefault( double defaultSize = 18.0, FontUnits units = FontUnits::Pt );
		~FontGeneric();
	};


	struct FontMapBlitContext {
		bool Invalidate;
		FontMap* Map;
		Array<Letter*> Letters;
		FontMapBlitContext() : Invalidate( false ) { }
		FontMapBlitContext( const FontMapBlitContext& other );
		void InsertLetter( Letter* letter );
		bool IsEmpty() const;
		bool NeedToBlit() const;
		void Clear();
	};


	struct FontMap {
		FontMapBlitContext CumulativeBlitContext;

		Font* OwnedFont;
		zCTex_D3D* Texture;
		zCOLOR Color;
		int Width;
		int Height;
		Array<Letter*> Letters;
		int FilledY;
		int FilledX;
		int LineHeight;
		bool HasFreeSpace( int width, int height );
		Letter* CreateLetter( Glyph* glyph );
		void Free();
		static zCTex_D3D* CreateTexture();
		static FontMap* Create( Font* font, const zCOLOR& color, int width = DefaultMapSize, int height = DefaultMapSize );

		static Array<FontMapBlitContext> BlitQueue;
		void BlitRequest( Letter* letter );
		void BlitLetters( const FontMapBlitContext& context );
		void FlushBlitContext();
		zCTex_D3D* GetTexture();
		static void BlitProcess();
		static void BlitProcessAsync();
	};


	struct Font {
		string Name;
		FontGeneric* FontProto;
		zCOLOR Color;
		Array<FontMap*> Maps;
		Map<char32_t, Letter*> Letters;
		Letter* GetLetter( char32_t id );
		FontMap* GetMap( int requiredWidth, int requiredHeight );
		void Free();
		void PrepareLettersForText( std::u32string unicode );
		static Font* GetFont( const string& name, const zCOLOR& color, double defaultSize = 18.0, FontUnits units = FontUnits::Pt );
		static Font* GetFont( zCFont* ingameFont );
		static void BlitLetters();
		~Font();
	};
}