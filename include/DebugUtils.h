/*
  DebugUtils.h - Library for simple serial debugging.
  Created by Primoz Flander 23.11.2020.
  Released into the public domain.
*/

#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

#define DEBUG
#ifdef DEBUG
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif
#endif
