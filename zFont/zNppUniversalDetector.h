
#pragma comment(lib, "zNppUniversalDetector.lib")

extern "C" {
__declspec(dllimport)
int detectCodepage( const char* buf, size_t len );
}