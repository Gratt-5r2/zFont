// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	class Filter {
	protected:
		Array<zCOLOR*> Pixels;
		int Width;
		int Height;
	public:
		Filter() { }
		Filter( Filter& filter );
		void __fastcall SetTextureRange( void* pixels, int x, int y, int w, int h );
		void __fastcall SetFiltrationSize( int w, int h );
	};


	class Filter_QuadraticShadow : public Filter {
		void ApplyShadowHorizontal( int x0, int x1, int y );
	public:
		void ApplyFilter();
	};


	class Filter_Glitter : public Filter {
		void ApplyGlitter( int x0, int x1, int y );
	public:
		void ApplyFilter();
	};
}