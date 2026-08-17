#ifndef __IL_EXR_COMPAT_H__
#define __IL_EXR_COMPAT_H__

// compatibility fix for OpenEXR 3.x

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <cstdint>
#include <ImfNamespace.h>

OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_ENTER

typedef uint64_t Int64;

OPENEXR_IMF_INTERNAL_NAMESPACE_HEADER_EXIT

#endif //__IL_EXR_COMPAT_H__
