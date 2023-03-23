// Supported with union (c) 2020 Union team
// Union HEADER file

namespace GOTHIC_ENGINE {
	class Filter {
	protected:
		Array<zCOLOR*> Pixels;
		int Width;
		int Height;
	public:
		void SetTextureRange( void* pixels, int x, int y, int w, int h );
		void SetFiltrationSize( int w, int h );
		virtual void ApplyFilter()=0;
	};


	class Filter_QuadraticShadow : public Filter {
	public:
		virtual void ApplyFilter();
	};

	class Filter_Highlight : public Filter {
	public:
		virtual void ApplyFilter();
	};
}