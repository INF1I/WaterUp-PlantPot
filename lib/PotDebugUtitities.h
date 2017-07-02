//
// Created by FuckMicrosoft on 2-7-2017.
//

#ifndef WATERUP_PLANTPOT_POTDEBUGUTITITIES_H
#define WATERUP_PLANTPOT_POTDEBUGUTITITIES_H

#include <Arduino.h>
#include <Streaming.h>

#define POT_DEBUG_PRINTER Serial // The object to stream the out put to.
#define APPEND << // Alias to make the print functions easier to understand.
#define NEW_LINE << endl << // Alias to make the print functions easier to understand.

#ifdef POT_DEBUG // Is debugging mode enabled?
    #define POT_DEBUG_PRINTLN(...) { POT_DEBUG_PRINTER << __VA_ARGS__ << endl; } // Stream arguments to Serial library and terminate the line.

    // This macro can be used to measure the amount the execution of an function takes.
    #define POT_DEBUG_MEASURE_EXECUTION_TIME_OF( functionPointer, functionName ) { unsigned long startTimerTestMicros = micros(); f();\
        POT_DEBUG_PRINTLN( "[debug] - The function: " APPEND functionName APPEND " took: " APPEND ( micros() - startTimerTestMicros) " to execute." )}
#else
    #define POT_DEBUG_PRINTLN( ... ) {}
    #define MEASURE_TIME_START() {}
#endif

#ifdef POT_ERROR // Is printing errors enabled?
    #define POT_ERROR_PRINTLN(...) { POT_DEBUG_PRINTER << __VA_ARGS__ << endl; } // Stream arguments to Serial library and terminate the line.
#else
    #define POT_ERROR_PRINTLN( ... ) {}
#endif

#endif //WATERUP_PLANTPOT_POTDEBUGUTITITIES_H
