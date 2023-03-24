// Supported with union (c) 2020 Union team
// Union HEADER file
#include <schrift/schrift.h>
#pragma comment(lib, "schrift/schrift.lib")

namespace GOTHIC_ENGINE {
#define async
#define Pt2Px(value) (value * 1.32814723273650)
#define Px2Pt(value) (value * 0.75292857248934)
#define FntTexFormat zTRnd_TextureFormat::zRND_TEX_FORMAT_BGRA_8888
// #define FntTexFormat zTRnd_TextureFormat::zRND_TEX_FORMAT_ARGB_4444
#define DefMapSize 1024
#define FrontTextureID 0
#define BackTextureID 1
#define MapTexturesCount 2

	int DefaultCodepage = CP_UTF8;
	float FontScale = 1.0;
	bool TTFOnly = true; // TODO
	bool Multithreading = false;
#define MP if( Multithreading )

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
		Map<wchar_t, Glyph*> Glyphs;
		byte* Memory;
		SFT* Schrift;
		Glyph* GetGlyph( wchar_t id );
		static FontGeneric* GetFont(const string& name, double size, FontUnits metrix = FontUnits::Pt);
		~FontGeneric();
	};


	struct FontMapBlitContext {
		FontMap* Map;
		Array<Letter*> Letters;
		FontMapBlitContext() { }
		FontMapBlitContext( const FontMapBlitContext& other );
		bool IsEmpty();
		void Clear();
	};


	struct FontMap {
		FontMapBlitContext CumulativeBlitContext;

		Font* OwnedFont;
		zCTex_D3D* Texture[2];
		zCTextureConvert* TextureConvert;
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
		static FontMap* Create( Font* font, const zCOLOR& color, int width = DefMapSize, int height = DefMapSize );

		static Array<FontMapBlitContext> BlitQueue;
		void BlitRequest( Letter* letter );
		void BlitLetters( FontMapBlitContext& context );
		void FlushBlitContext();
		void SwapTextures();
		zCTex_D3D* GetTexture( bool lock );
		static void BlitProcess();
		static void BlitProcessAsync();
	};


	struct Font {
		string Name;
		FontGeneric* FontProto;
		zCOLOR Color;
		Array<FontMap*> Maps;
		Map<wchar_t, Letter*> Letters;
		Letter* GetLetter( wchar_t id );
		FontMap* GetMap( int requiredWidth, int requiredHeight );
		void Free();
		void PreRender();
		static Font* GetFont( const string& name, const zCOLOR& color, double defaultSize = 18.0, FontUnits metrix = FontUnits::Pt );
		static Font* GetFont( zCFont* ingameFont );
		static Font* GetFontDefault( double defaultSize = 18.0, FontUnits metrix = FontUnits::Pt );
		static void BlitLetters();
		~Font();
	};
}