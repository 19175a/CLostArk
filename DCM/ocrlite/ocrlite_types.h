#pragma once

/**************************************************/
/********** imports / exports HPSocket4C **********/

#ifdef OCRLITE_STATIC_LIB
#define OCRLITE_API		EXTERN_C
#else
#ifdef OCRLITE_EXPORTS
#define OCRLITE_API	EXTERN_C __declspec(dllexport)
#else
#define OCRLITE_API	EXTERN_C __declspec(dllimport)
#endif
#endif

#define __OCRLITE_CALL __stdcall
