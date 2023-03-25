// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
#if ENGINE == Engine_G2A
#define DEFDDPF_ADR 0x009D1678
#define DEFDDPF_OP  0x00655598
#elif ENGINE == Engine_G1
#define DEFDDPF_ADR 0x008FE9E0
#define DEFDDPF_OP  0x0071CE54
#endif


  inline void SetPixelFormatBGRA8888( LPDDPIXELFORMAT ddpf ) {
    ddpf->dwFlags           = DDPF_RGB | DDPF_ALPHAPIXELS;
    ddpf->dwRGBBitCount     = 32;
    ddpf->dwBBitMask        = 0x000000FF;
    ddpf->dwGBitMask        = 0x0000FF00;
    ddpf->dwRBitMask        = 0x00FF0000;
    ddpf->dwRGBAlphaBitMask = 0xFF000000;
  }


  HOOK Hook_zCTex_D3D_XTEX_BuildSurfaces PATCH( &zCTex_D3D::XTEX_BuildSurfaces, &zCTex_D3D::XTEX_BuildSurfaces_Union );

  int zCTex_D3D::XTEX_BuildSurfaces_Union( int decompress ) {
    if( xtex_texinfo.texFormat == zRND_TEX_FORMAT_BGRA_8888 ) {
      DDPIXELFORMAT ddpf_bgra8888;
      DDPIXELFORMAT& ddpf_default = *(DDPIXELFORMAT*)DEFDDPF_ADR;
      memcpy( &ddpf_bgra8888, &ddpf_default, sizeof( DDPIXELFORMAT ) );

      // Set bgra8888 pixel format
      SetPixelFormatBGRA8888( &ddpf_bgra8888 );

      // Unlock the default case block
      LPDDPIXELFORMAT& pixelFormat = *(LPDDPIXELFORMAT*)DEFDDPF_OP;
      TMemUnlocker unlocker( &pixelFormat, sizeof( LPDDPIXELFORMAT ), True );

      // Set bgra8888 as default pixel format temporary
      // while original function is executing
      hasAlpha = 1;
      pixelFormat = &ddpf_bgra8888;
      int ok = THISCALL( Hook_zCTex_D3D_XTEX_BuildSurfaces )(decompress);
      pixelFormat = &ddpf_default;

      return ok;
    }

    return THISCALL( Hook_zCTex_D3D_XTEX_BuildSurfaces )(decompress);
  }
}