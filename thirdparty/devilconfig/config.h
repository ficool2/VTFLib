#ifndef __CONFIG_H__
#define __CONFIG_H__

// handwritten config

/* formats we aren't supporting */
#define IL_NO_JP2    // JasPer
#define IL_NO_LCMS   // Little CMS
#define IL_NO_MNG    // libmng
#define IL_NO_TIF    // libtiff
#define IL_NO_WDP    // Microsoft HD Photo

/* built from source */
/* #undef IL_NO_PNG */
/* #undef IL_NO_JPG */
/* #undef IL_NO_EXR */

/* DevIL handle this */
/* #undef IL_USE_DXTC_NVIDIA */
/* #undef IL_USE_DXTC_SQUISH */

/* linked through CMake */
/* #undef IL_USE_PRAGMA_LIBS */

/* little endian */
/* #undef WORDS_BIGENDIAN */

#endif //__CONFIG_H__
