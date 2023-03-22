// This file added in headers queue
// File: "Sources.h"
#include "resource.h"
extern bool isUtf8String( LPCSTR psz );

#include "zNppUniversalDetector.h"

namespace GOTHIC_ENGINE {
#if 0
  __declspec(dllimport) void Dic_RegisterString( const string& str, int csId );
  __declspec(dllimport) int Dic_GetStringCodepage( const string& str, bool knownOnly );
  __declspec(dllimport) bool Dic_StringIsUtf8( const string& str, bool knownOnly );
#pragma comment(lib, "zUniversalDictionary.lib")

  wstring UTF8toUnicode( const string& s )
  {
    wstring ws;
    wchar_t wc;
    for( int i = 0; i < s.Length(); )
    {
      char c = s[i];
      if( (c & 0x80) == 0 )
      {
        wc = c;
        ++i;
      }
      else if( (c & 0xE0) == 0xC0 )
      {
        wc = (s[i] & 0x1F) << 6;
        wc |= (s[i + 1] & 0x3F);
        i += 2;
      }
      else if( (c & 0xF0) == 0xE0 )
      {
        wc = (s[i] & 0xF) << 12;
        wc |= (s[i + 1] & 0x3F) << 6;
        wc |= (s[i + 2] & 0x3F);
        i += 3;
      }
      else if( (c & 0xF8) == 0xF0 )
      {
        wc = (s[i] & 0x7) << 18;
        wc |= (s[i + 1] & 0x3F) << 12;
        wc |= (s[i + 2] & 0x3F) << 6;
        wc |= (s[i + 3] & 0x3F);
        i += 4;
      }
      else if( (c & 0xFC) == 0xF8 )
      {
        wc = (s[i] & 0x3) << 24;
        wc |= (s[i] & 0x3F) << 18;
        wc |= (s[i] & 0x3F) << 12;
        wc |= (s[i] & 0x3F) << 6;
        wc |= (s[i] & 0x3F);
        i += 5;
      }
      else if( (c & 0xFE) == 0xFC )
      {
        wc = (s[i] & 0x1) << 30;
        wc |= (s[i] & 0x3F) << 24;
        wc |= (s[i] & 0x3F) << 18;
        wc |= (s[i] & 0x3F) << 12;
        wc |= (s[i] & 0x3F) << 6;
        wc |= (s[i] & 0x3F);
        i += 6;
      }
      ws += wc;
    }
    return ws;
  }

  string UnicodeToUTF8( const wstring& ws )
  {
    string s;
    for( int i = 0; i < ws.Length(); ++i )
    {
      wchar_t wc = ws[i];
      if( 0 <= wc && wc <= 0x7f )
      {
        s += (char)wc;
      }
      else if( 0x80 <= wc && wc <= 0x7ff )
      {
        s += (0xc0 | (wc >> 6));
        s += (0x80 | (wc & 0x3f));
      }
      else if( 0x800 <= wc && wc <= 0xffff )
      {
        s += (0xe0 | (wc >> 12));
        s += (0x80 | ((wc >> 6) & 0x3f));
        s += (0x80 | (wc & 0x3f));
      }
      else if( 0x10000 <= wc && wc <= 0x1fffff )
      {
        s += (0xf0 | (wc >> 18));
        s += (0x80 | ((wc >> 12) & 0x3f));
        s += (0x80 | ((wc >> 6) & 0x3f));
        s += (0x80 | (wc & 0x3f));
      }
      else if( 0x200000 <= wc && wc <= 0x3ffffff )
      {
        s += (0xf8 | (wc >> 24));
        s += (0x80 | ((wc >> 18) & 0x3f));
        s += (0x80 | ((wc >> 12) & 0x3f));
        s += (0x80 | ((wc >> 6) & 0x3f));
        s += (0x80 | (wc & 0x3f));
      }
      else if( 0x4000000 <= wc && wc <= 0x7fffffff )
      {
        s += (0xfc | (wc >> 30));
        s += (0x80 | ((wc >> 24) & 0x3f));
        s += (0x80 | ((wc >> 18) & 0x3f));
        s += (0x80 | ((wc >> 12) & 0x3f));
        s += (0x80 | ((wc >> 6) & 0x3f));
        s += (0x80 | (wc & 0x3f));
      }
    }
    return s;
  }
#endif


  // TO DO
  // Your code ...

  void Game_Entry() {
    Union.GetSysPackOption().Read( FontScale, "Font", "Scale", FontScale );
  }

#if 0
  zCView* GetFontView( string fontName, int xpos ) {
    zCTexture* tex = Null;
    for each( auto font in Fonts )
    {
      if( font.GetValue()->Name.HasWordI( fontName ) ) {
        cmd << font.GetValue()->Name << endl;
        tex = font.GetValue()->Maps[0]->Texture;
        break;
      }
    }
    if( !tex )
      return Null;

    // auto font = Font::GetFont( fontName, 42.5001, FontMetrix::Px );
    time_t time0 = Timer::GetTime();
    //for( int i = ' '; i < 'z'; i++ ) font->GetLetter( i );
    // for( int i = L'а'; i < L'я'; i++ ) font->GetLetter( i );
    // for( int i = L'А'; i < L'Я'; i++ ) font->GetLetter( i );
    // for( int i = 0; i < 1000; i++ ) font->GetLetter( i + L'思' );

    //font->BlitLetters();
    time_t time1 = Timer::GetTime();
    cmd << "time:" << time1 - time0 << endl;

    zCView* view = new zCView();
    screen->InsertItem( view );
    view->SetSize( screen->anx( DefMapSize ), screen->any( DefMapSize ) );
    view->SetPos( xpos, 500 );
    view->InsertBack( tex );
    cmd << AHEX32( view ) << endl;
    return view;
  }
#endif
  
  // 0x008FE9A0 struct _DDPIXELFORMAT* pflist
  _DDPIXELFORMAT* pflist = (_DDPIXELFORMAT*)0x008FE9A0;
  void Game_Init() {
    for( int i = 0; i < 32; i++ )
    {
      auto fmt = pflist[i];
      cmd << i << "  " << fmt.dwRGBBitCount << endl;

    }






    //system( "pause" );
#if 0
    // GetFontView( "Ubuntu-Regular.ttf", 500 );
    // GetFontView( "FONT_OLD_20_WHITE ", 500 );
    // GetFontView( "SourceHanSansSC-VF.ttf", 5500 );
    Array<wchar_t> letters;

    string AllAnsi;
    string AllUtf8;

    auto& array = parser->symtab.table;
    for( int i = 0; i < array.GetNum(); i++ ) {
      auto sym = array[i];
      if( sym->type == zPAR_TYPE_STRING && sym->HasFlag( zPAR_FLAG_CONST ) ) {
        int capacity = sym->ele;
        // cmd << sym->name << "  " << capacity << endl;
        zSTRING value;
        for( int j = 0; j < capacity; j++ ) {
          sym->GetValue( value, j );
          if( !Dic_StringIsUtf8( value, value.Length() ) ) {
            // cmd << sym->name << "  " << capacity << endl;
            // cmd << "  >" << value << "  " << value.Length() << endl;
            AllAnsi += A (parser->GetFileName(sym->filenr) + " " + zSTRING((int)sym->filenr) + "  " + (value + "\r\n"));
          }
          else
            AllUtf8 += A( parser->GetFileName( sym->filenr ) + " " + zSTRING( (int)sym->filenr ) + "  " + value + "\r\n" );

          //   cmd << "  >" << value << "  " << value.Length() << endl;
          // if( sym->name == "я14475" ) {
          //   system( "pause" );
          //   continue;
          // }
          // wstring utf16 = UTF8toUnicode( value );
          // cmd << "  <" << utf16 << endl;
          // for( int k = 0; k < utf16.Length(); k++ )
          //   letters |= utf16[k];
        }
      }
    }

    AllAnsi.WriteToFile( "AllAnsi.txt" );
    AllUtf8.WriteToFile( "AllUtf8.txt" );
    Message::Box( letters.GetNum() );
#endif

    Thread( FontMap::BlitProcess ).Detach();
  }

  void Game_Exit() {
  }

  void Game_PreLoop() {
  }

  void Game_Loop() {
    // static auto view = GetFontView( "FONT_OLD_20_WHITE", 500 );
     Font::BlitLetters();
 //   FontMap::BlitProcess();
    // screen->Print( 6000, 4000, "Hello world with printing texts" );
  }

  void Game_PostLoop() {
  }

  void Game_MenuLoop() {
     Font::BlitLetters();
  //  FontMap::BlitProcess();
  }

  // Information about current saving or loading world
  TSaveLoadGameInfo& SaveLoadGameInfo = UnionCore::SaveLoadGameInfo;

  void Game_SaveBegin() {
  }

  void Game_SaveEnd() {
  }

  void LoadBegin() {
  }

  void LoadEnd() {
  }

  void Game_LoadBegin_NewGame() {
    LoadBegin();
  }

  void Game_LoadEnd_NewGame() {
    LoadEnd();
  }

  void Game_LoadBegin_SaveGame() {
    LoadBegin();
  }

  void Game_LoadEnd_SaveGame() {
    LoadEnd();
  }

  void Game_LoadBegin_ChangeLevel() {
    LoadBegin();
  }

  void Game_LoadEnd_ChangeLevel() {
    LoadEnd();
  }

  void Game_LoadBegin_Trigger() {
  }
  
  void Game_LoadEnd_Trigger() {
  }
  
  void Game_Pause() {
  }
  
  void Game_Unpause() {
  }
  
  void Game_DefineExternals() {
  }

  void Game_ApplyOptions() {
  }

  /*
  Functions call order on Game initialization:
    - Game_Entry           * Gothic entry point
    - Game_DefineExternals * Define external script functions
    - Game_Init            * After DAT files init
  
  Functions call order on Change level:
    - Game_LoadBegin_Trigger     * Entry in trigger
    - Game_LoadEnd_Trigger       *
    - Game_Loop                  * Frame call window
    - Game_LoadBegin_ChangeLevel * Load begin
    - Game_SaveBegin             * Save previous level information
    - Game_SaveEnd               *
    - Game_LoadEnd_ChangeLevel   *
  
  Functions call order on Save game:
    - Game_Pause     * Open menu
    - Game_Unpause   * Click on save
    - Game_Loop      * Frame call window
    - Game_SaveBegin * Save begin
    - Game_SaveEnd   *
  
  Functions call order on Load game:
    - Game_Pause              * Open menu
    - Game_Unpause            * Click on load
    - Game_LoadBegin_SaveGame * Load begin
    - Game_LoadEnd_SaveGame   *
  */

#define AppDefault True
  CApplication* lpApplication = !CHECK_THIS_ENGINE ? Null : CApplication::CreateRefApplication(
    Enabled( AppDefault ) Game_Entry,
    Enabled( AppDefault ) Game_Init,
    Enabled( AppDefault ) Game_Exit,
    Enabled( AppDefault ) Game_PreLoop,
    Enabled( AppDefault ) Game_Loop,
    Enabled( AppDefault ) Game_PostLoop,
    Enabled( AppDefault ) Game_MenuLoop,
    Enabled( AppDefault ) Game_SaveBegin,
    Enabled( AppDefault ) Game_SaveEnd,
    Enabled( AppDefault ) Game_LoadBegin_NewGame,
    Enabled( AppDefault ) Game_LoadEnd_NewGame,
    Enabled( AppDefault ) Game_LoadBegin_SaveGame,
    Enabled( AppDefault ) Game_LoadEnd_SaveGame,
    Enabled( AppDefault ) Game_LoadBegin_ChangeLevel,
    Enabled( AppDefault ) Game_LoadEnd_ChangeLevel,
    Enabled( AppDefault ) Game_LoadBegin_Trigger,
    Enabled( AppDefault ) Game_LoadEnd_Trigger,
    Enabled( AppDefault ) Game_Pause,
    Enabled( AppDefault ) Game_Unpause,
    Enabled( AppDefault ) Game_DefineExternals,
    Enabled( AppDefault ) Game_ApplyOptions
  );
}