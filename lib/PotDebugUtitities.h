//
// Created by FuckMicrosoft on 2-7-2017.
//

#ifndef WATERUP_PLANTPOT_POTDEBUGUTITITIES_H
#define WATERUP_PLANTPOT_POTDEBUGUTITITIES_H

#include <Streaming.h>

#define POT_DEBUG_PRINTER Serial // Where should wil

#ifdef POT_DEBUG
    #define POT_DEBUG_PRINT(...) { POT_DEBUG_PRINTER << __VA_ARGS__; }
    #define POT_DEBUG_PRINTLN(...) { POT_DEBUG_PRINTER << __VA_ARGS__ << endl; }
#else
    #define POT_DEBUG_PRINT( ... ) {}
    #define POT_DEBUG_PRINTLN( ... ) {}
#endif

#ifdef POT_ERROR
    #define POT_ERROR_PRINT(...) { POT_DEBUG_PRINTER << __VA_ARGS__; }
    #define POT_ERROR_PRINTLN(...) { POT_DEBUG_PRINTER << __VA_ARGS__ << endl; }
#else
    #define POT_ERROR_PRINT( ... ) {}
    #define POT_ERROR_PRINTLN( ... ) {}
#endif

#endif //WATERUP_PLANTPOT_POTDEBUGUTITITIES_H
