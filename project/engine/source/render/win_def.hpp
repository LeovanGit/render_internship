// win_def.hpp and win_undef.hpp are helping headers
// for including windows.h and d3d11_4.h
// without including Windows macros like min/max/near/far

#ifdef UNDEFINED_MIN
#pragma pop_macro("min")
#endif

#ifdef UNDEFINED_MAX
#pragma pop_macro("max")
#endif

#ifdef UNDEFINED_NEAR
#pragma pop_macro("near")
#endif

#ifdef UNDEFINED_FAR
#pragma pop_macro("far")
#endif
