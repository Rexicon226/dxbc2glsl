#include "driver.h"
#include <dxbc_module.h>
#include <log/log.h>
#include <spirv_cross/spirv_glsl.hpp>

API const char* APIENTRY decompile_to_string(const char* input, size_t inputSize) {
  try {
      dxvk::DxbcReader reader(input, inputSize);
      dxvk::DxbcModule module(reader);

      dxvk::DxbcModuleInfo info;
      auto result = module.compile(info, "test");

      spirv_cross::CompilerGLSL glsl(result.code.data(), result.code.dwords());
      spirv_cross::CompilerGLSL::Options options;
      options.version = 310;
      options.es = true;
      glsl.set_common_options(options);
      
      std::string compiledString = glsl.compile();
      
      char* resultStr = new char[compiledString.size() + 1];
      std::copy(compiledString.begin(), compiledString.end(), resultStr);
      resultStr[compiledString.size()] = '\0';
      return resultStr;
  } catch (const std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
      return nullptr;
  }
}

API void APIENTRY free_compiled_string(const char* compiledString) {
  if (compiledString) {
      delete[] compiledString;
  }
}


