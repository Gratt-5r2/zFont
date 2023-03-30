// Supported with union (c) 2020 Union team
// Union SOURCE file

#include "compact_enc_det/compact_enc_det.h"
#include "uchardet/uchardet.h"
#pragma comment(lib, "compact_enc_det/compact_enc_det.lib")
#pragma comment(lib, "uchardet/uchardet.lib")

namespace GOTHIC_ENGINE {
  int GuessOuEncoding() {
    string file;
    auto ouList = ogame->csMan->library->ouList;
    for( int i = 0; i < ouList.GetNum(); i++ ) {
      auto ou = ouList[i];
      auto& blocks = ou->blocks;

      if( !ou->roleName.StartWith( "SVM" ) )
        continue;

      for( int j = 0; j < blocks.GetNum() && j < 100; j++ ) {
        auto block = blocks[j];
        auto atomic = block.block->CastTo<zCCSAtomicBlock>();
        if( atomic ) {
          auto msg = atomic->command->CastTo<oCMsgConversation>();
          if( msg && msg->text.Length() > 20 )
            file += A msg->text;
        }
      }
    }

    bool is_reliable;
    int bytes_consumed;
    Encoding encoding = CompactEncDet::DetectEncoding(
      file, file.Length(),
      nullptr, nullptr, nullptr,
      UNKNOWN_ENCODING,
      UNKNOWN_LANGUAGE,
      CompactEncDet::WEB_CORPUS,
      false,
      &bytes_consumed,
      &is_reliable );

    int winEnc = ToWindowsEncoding( encoding );
    cmd << "Guessed OU CP: " << winEnc << endl;
    return winEnc;
  }


  int GuessGameEncoding() {
    int ouEnc = GuessOuEncoding();
    if( ouEnc == CP_UTF8 )
      return ouEnc;

    int arrowId = parser->GetIndex( "ItAmArrow" );
    if( arrowId == Invalid ) arrowId = parser->GetIndex( "ItRw_Arrow" );
    if( arrowId == Invalid ) arrowId = parser->GetIndex( "ItAm_Arrow" );

    if( arrowId != Invalid ) {
      oCItem* item = new oCItem();
      item->InitByScript( arrowId, 0 );
      zSTRING itemName = item->name;
      item->Release();
      
      if( itemName == "Šíp"        ) return 1250; // cz
      if( itemName == "Nyílvessző" ) return 1250; // hu
      if( itemName == "Strzała"    ) return 1250; // pl
      if( itemName == "Стрела"     ) return 1251; // ru
      if( itemName == "Стріла"     ) return 1251; // uk
      if( itemName == "Pfeil"      ) return 1252; // de
      if( itemName == "Arrow"      ) return 1252; // en
      if( itemName == "Flèche"     ) return 1252; // fr
      if( itemName == "Freccia"    ) return 1252; // it
      if( itemName == "Flecha"     ) return 1252; // es
      if( itemName == "Ok"         ) return 1254; // tr
    }

    return ouEnc;
  }


  int GameEncoding = GetACP();


  Map<std::string, int> GuessedEncodings[2];
  uint GuessedFrameGET = 0;
  uint GuessedFrameSET = 1;
  void SwapGuessedEncodings() {
    GuessedEncodings[GuessedFrameGET].Clear();
    std::swap( GuessedFrameGET, GuessedFrameSET );
  }


  int GuessStringEncoding( const std::string& input ) {
    int result0 = GameEncoding;

    auto& pr = GuessedEncodings[GuessedFrameGET][input];
    if( pr.IsNull() )
    {
      static uchardet_t uchardet_handle = uchardet_new();
      uchardet_reset( uchardet_handle );
      uchardet_handle_data( uchardet_handle, input.c_str(), input.length() );
      uchardet_data_end( uchardet_handle );
      int candidates = uchardet_get_n_candidates( uchardet_handle );

      if( candidates > 0 ) {
        const char* charset = uchardet_get_encoding( uchardet_handle, 0 );
        double confidence = uchardet_get_confidence( uchardet_handle, 0 );
        cmd << charset << endl;
        if( confidence > 0.5 ) {
          bool isReliable;
          int bytesConsumed;
          Encoding encoding = CompactEncDet::DetectEncoding(
            input.c_str(), input.length(),
            nullptr, nullptr, nullptr,
            UNKNOWN_ENCODING,
            UNKNOWN_LANGUAGE,
            CompactEncDet::QUERY_CORPUS,
            false, &bytesConsumed, &isReliable );
      
          if( isReliable ) {
            int result1 = ToWindowsEncoding( charset );
            int result2 = ToWindowsEncoding( encoding );
            result0 = result1 == result2 ?
              result1 :
              GameEncoding;

            cmd << "Guessed " << result0 << " for " << input.c_str() << endl;
          }
        }
      }
    }
    else
      result0 = pr;

    GuessedEncodings[GuessedFrameGET].Insert( input, result0 );
    GuessedEncodings[GuessedFrameSET].Insert( input, result0 );
    return result0;
  }
}