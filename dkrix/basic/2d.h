#ifndef __2D__
#define __2D__

#include "Typedef.h"
#ifdef PLATFORM_WINDOWS
#include <DDraw.h>
#endif
#include "DLL.h"

 
typedef struct
{
   void  *p_surface;
   int   width;
   int   height;
   int   pitch;

} S_SURFACEINFO;

 
typedef struct
{
   int   x;    
   int   y;    
   int   w;    
   int   h;    

} S_RECT;

#endif
