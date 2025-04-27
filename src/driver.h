#include <stdlib.h>

#if defined(_WIN32)
    #define APIENTRY __stdcall
    #define API __declspec(dllexport)
#else
    #define APIENTRY
    #define API
#endif


#ifdef __cplusplus
extern "C" {
#endif

API const char* APIENTRY decompile_to_string(const char* input, size_t input_size);
API void APIENTRY free_compiled_string(const char* compiledString);

#ifdef __cplusplus
}
#endif