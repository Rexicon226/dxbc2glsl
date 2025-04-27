#include "driver.h"
#include <dxbc_module.h>
#include <log/log.h>
#include <spirv_cross/spirv_glsl.hpp>
#include <spirv_cross/spirv_hlsl.hpp>

API const char* APIENTRY decompile_to_glsl(const char* input, size_t inputSize) {
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

API const char* APIENTRY decompile_to_hlsl(const char* input, size_t inputSize) {
  try {
      dxvk::DxbcReader reader(input, inputSize);
      dxvk::DxbcModule module(reader);

      dxvk::DxbcModuleInfo info;
      auto result = module.compile(info, "test");

      spirv_cross::CompilerHLSL hlsl(result.code.data(), result.code.dwords());
      spirv_cross::CompilerGLSL::Options glsl_options;
      glsl_options.version = 310;
      glsl_options.es = true;
      spirv_cross::CompilerHLSL::Options hlsl_options;

      hlsl.set_common_options(glsl_options);
      hlsl.set_hlsl_options(hlsl_options);
      
      std::string compiledString = hlsl.compile();
      
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


