#pragma once

#ifdef _WIN32
  #pragma push_macro("min")
  #pragma push_macro("max")

  #ifdef min
    #undef min
  #endif

  #ifdef max
    #undef max
  #endif
#endif
