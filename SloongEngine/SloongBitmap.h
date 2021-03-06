#pragma once

#define BITMAP_ID            0x4D42 // universal id for a bitmap
#define BITMAP_ATTR_LOADED   128

namespace SoaringLoong
{
	namespace Graphics
	{
		enum BITMAP_LOAD_MODE
		{
			BITMAP_EXTRACT_MODE_ABS = 0,
			BITMAP_EXTRACT_MODE_CELL = 1,
		};

		class CBitmap
		{
		public:
			CBitmap();
			~CBitmap();

			HRESULT Initialize();
			HRESULT LoadBitmapFromFile(LPCTSTR strFileName);

			LONG GetBitmapHeight();
			LONG GetBitmapWidth();
			int GetBiemapBPP();

			HRESULT FlipBitmap(int nNumLine, int nHeight);
			int Scan_Image_Bitmap(LPRECT rcBmp, LPDIRECTDRAWSURFACE7 lpdds, LPRECT rcSurface);
			int Scan_Image_Bitmap(LPDIRECTDRAWSURFACE7 lpdds, int cx, int cy);
			HRESULT Shutdown();

		public:
			BITMAPFILEHEADER m_stFileHeader;
			BITMAPINFOHEADER m_stInfoHeader;

			LPBYTE			 m_lpBuffer;

		};
	}
}
