// Supported with union (c) 2020 Union team
// Union SOURCE file

namespace GOTHIC_ENGINE {
  int	BilerpFiltering;
  int	zBufferWriting;
  zTRnd_ZBufferCmp zBufferComparing;

  void PushPrintRenderOptions( int bilertFlt, int zbufWrt, zTRnd_ZBufferCmp zbufCmp ) {
    BilerpFiltering = zrenderer->GetBilerpFilterEnabled();
    zBufferWriting = zrenderer->GetZBufferWriteEnabled();
    zBufferComparing = zrenderer->GetZBufferCompare();
    auto AlphaBlendingFunc = zrenderer->GetAlphaBlendFunc();

    zrenderer->SetBilerpFilterEnabled( bilertFlt );
    zrenderer->SetZBufferWriteEnabled( zbufWrt );
    zrenderer->SetZBufferCompare( zbufCmp );

    if( (AlphaBlendingFunc != zRND_ALPHA_FUNC_ADD) && (AlphaBlendingFunc != zRND_ALPHA_FUNC_BLEND) )
      zrenderer->SetAlphaBlendFunc( zRND_ALPHA_FUNC_BLEND );
  }

  void PopPrintRenderOptions() {
    zrenderer->SetBilerpFilterEnabled( BilerpFiltering );
    zrenderer->SetZBufferWriteEnabled( zBufferWriting );
    zrenderer->SetZBufferCompare( zBufferComparing );
  }

  float GetCameraFarZ() {
#if ENGINE >= Engine_G2
    return zCCamera::activeCam ? zCCamera::activeCam->nearClipZ + 1.0f : 1.0f;
#else
    return 1.0f;
#endif
  }
}