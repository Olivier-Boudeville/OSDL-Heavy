/*
 * No include guards wanted : to be included each time defines 
 * can be screwed up.
 *
 */

#ifdef OSDL_RUNS_ON_WINDOWS

// Microsoft stupidly managed to redefine symbols in a header (winuser.h) :

#ifdef LoadIcon
#undef LoadIcon
#endif // LoadIcon

#ifdef LoadImage
#undef LoadImage
#endif // LoadImage

#ifdef LoadImageA
#undef LoadImageA
#endif // LoadImageA

#endif // OSDL_RUNS_ON_WINDOWS
