#include <cstring>

#include "spirv_module.h"

namespace dxvk {
  
  API SpirvModule::SpirvModule(uint32_t version)
  : m_version(version) {
    this->instImportGlsl450();
  }
  
  
  SpirvModule::~SpirvModule() {
    
  }
  
  
  API SpirvCodeBuffer SpirvModule::compile() const {
    SpirvCodeBuffer result;
    result.putHeader(m_version, m_id);
    result.append(m_capabilities);
    result.append(m_extensions);
    result.append(m_instExt);
    result.append(m_memoryModel);
    result.append(m_entryPoints);
    result.append(m_execModeInfo);
    result.append(m_debugNames);
    result.append(m_annotations);
    result.append(m_typeConstDefs);
    result.append(m_variables);
    result.append(m_code);
    return result;
  }
  
  
  API uint32_t APIENTRY SpirvModule::allocateId() {
    return m_id++;
  }
  
  
  bool SpirvModule::hasCapability(
          spv::Capability         capability) {
    for (auto ins : m_capabilities) {
      if (ins.opCode() == spv::OpCapability && ins.arg(1) == capability)
        return true;
    }

    return false;
  }

  API void APIENTRY SpirvModule::enableCapability(
          spv::Capability         capability) {
    // Scan the generated instructions to check
    // whether we already enabled the capability.
    if (!hasCapability(capability)) {
      m_capabilities.putIns (spv::OpCapability, 2);
      m_capabilities.putWord(capability);
    }
  }
  
  
  API void APIENTRY SpirvModule::enableExtension(
    const char*                   extensionName) {
    m_extensions.putIns (spv::OpExtension, 1 + m_extensions.strLen(extensionName));
    m_extensions.putStr (extensionName);
  }
  
  
  API void APIENTRY SpirvModule::addEntryPoint(
          uint32_t                entryPointId,
          spv::ExecutionModel     executionModel,
    const char*                   name) {
    m_entryPoints.putIns  (spv::OpEntryPoint, 3 + m_entryPoints.strLen(name) + m_interfaceVars.size());
    m_entryPoints.putWord (executionModel);
    m_entryPoints.putWord (entryPointId);
    m_entryPoints.putStr  (name);
    
    for (uint32_t varId : m_interfaceVars)
      m_entryPoints.putWord(varId);
  }
  
  
  API void APIENTRY SpirvModule::setMemoryModel(
          spv::AddressingModel    addressModel,
          spv::MemoryModel        memoryModel) {
    m_memoryModel.putIns  (spv::OpMemoryModel, 3);
    m_memoryModel.putWord (addressModel);
    m_memoryModel.putWord (memoryModel);
  }
  
    
  API void APIENTRY SpirvModule::setExecutionMode(
          uint32_t                entryPointId,
          spv::ExecutionMode      executionMode) {
    m_execModeInfo.putIns (spv::OpExecutionMode, 3);
    m_execModeInfo.putWord(entryPointId);
    m_execModeInfo.putWord(executionMode);
  }
  
  
  API void APIENTRY SpirvModule::setExecutionMode(
          uint32_t                entryPointId,
          spv::ExecutionMode      executionMode,
          uint32_t                argCount,
    const uint32_t*               args) {
    m_execModeInfo.putIns (spv::OpExecutionMode, 3 + argCount);
    m_execModeInfo.putWord(entryPointId);
    m_execModeInfo.putWord(executionMode);

    for (uint32_t i = 0; i < argCount; i++)
      m_execModeInfo.putWord(args[i]);
  }


  API void APIENTRY SpirvModule::setInvocations(
          uint32_t                entryPointId,
          uint32_t                invocations) {
    m_execModeInfo.putIns  (spv::OpExecutionMode, 4);
    m_execModeInfo.putWord (entryPointId);
    m_execModeInfo.putWord (spv::ExecutionModeInvocations);
    m_execModeInfo.putInt32(invocations);
  }
  
  
  API void APIENTRY SpirvModule::setLocalSize(
          uint32_t                entryPointId,
          uint32_t                x,
          uint32_t                y,
          uint32_t                z) {
    m_execModeInfo.putIns  (spv::OpExecutionMode, 6);
    m_execModeInfo.putWord (entryPointId);
    m_execModeInfo.putWord (spv::ExecutionModeLocalSize);
    m_execModeInfo.putInt32(x);
    m_execModeInfo.putInt32(y);
    m_execModeInfo.putInt32(z);
  }
  
  
  API void APIENTRY SpirvModule::setOutputVertices(
          uint32_t                entryPointId,
          uint32_t                vertexCount) {
    m_execModeInfo.putIns (spv::OpExecutionMode, 4);
    m_execModeInfo.putWord(entryPointId);
    m_execModeInfo.putWord(spv::ExecutionModeOutputVertices);
    m_execModeInfo.putWord(vertexCount);
  }
  
  
  API uint32_t APIENTRY SpirvModule::addDebugString(
    const char*                   string) {
    uint32_t resultId = this->allocateId();
    
    m_debugNames.putIns (spv::OpString,
      2 + m_debugNames.strLen(string));
    m_debugNames.putWord(resultId);
    m_debugNames.putStr (string);
    return resultId;
  }
  
  
  API void APIENTRY SpirvModule::setDebugSource(
          spv::SourceLanguage     language,
          uint32_t                version,
          uint32_t                file,
    const char*                   source) {
    uint32_t strLen = source != nullptr
      ? m_debugNames.strLen(source) : 0;
    
    m_debugNames.putIns (spv::OpSource, 4 + strLen);
    m_debugNames.putWord(language);
    m_debugNames.putWord(version);
    m_debugNames.putWord(file);
    
    if (source != nullptr)
      m_debugNames.putStr(source);
  }
  
  API void APIENTRY SpirvModule::setDebugName(
          uint32_t                expressionId,
    const char*                   debugName) {
    m_debugNames.putIns (spv::OpName, 2 + m_debugNames.strLen(debugName));
    m_debugNames.putWord(expressionId);
    m_debugNames.putStr (debugName);
  }
  
  
  API void APIENTRY SpirvModule::setDebugMemberName(
          uint32_t                structId,
          uint32_t                memberId,
    const char*                   debugName) {
    m_debugNames.putIns (spv::OpMemberName, 3 + m_debugNames.strLen(debugName));
    m_debugNames.putWord(structId);
    m_debugNames.putWord(memberId);
    m_debugNames.putStr (debugName);
  }
  
  
  API uint32_t APIENTRY SpirvModule::constBool(
          bool                    v) {
    return this->defConst(v
        ? spv::OpConstantTrue
        : spv::OpConstantFalse,
      this->defBoolType(),
      0, nullptr);
  }
  
  
  API uint32_t APIENTRY SpirvModule::consti32(
          int32_t                 v) {
    std::array<uint32_t, 1> data;
    std::memcpy(data.data(), &v, sizeof(v));
    
    return this->defConst(
      spv::OpConstant,
      this->defIntType(32, 1),
      data.size(),
      data.data());
  }
  
  
  API uint32_t APIENTRY SpirvModule::consti64(
          int64_t                 v) {
    std::array<uint32_t, 2> data;
    std::memcpy(data.data(), &v, sizeof(v));
    
    return this->defConst(
      spv::OpConstant,
      this->defIntType(64, 1),
      data.size(),
      data.data());
  }
  
  
  API uint32_t APIENTRY SpirvModule::constu32(
          uint32_t                v) {
    std::array<uint32_t, 1> data;
    std::memcpy(data.data(), &v, sizeof(v));
    
    return this->defConst(
      spv::OpConstant,
      this->defIntType(32, 0),
      data.size(),
      data.data());
  }
  
  
  API uint32_t APIENTRY SpirvModule::constu64(
          uint64_t                v) {
    std::array<uint32_t, 2> data;
    std::memcpy(data.data(), &v, sizeof(v));
    
    return this->defConst(
      spv::OpConstant,
      this->defIntType(64, 0),
      data.size(),
      data.data());
  }
  
  
  API uint32_t APIENTRY SpirvModule::constf32(
          float                   v) {
    std::array<uint32_t, 1> data;
    std::memcpy(data.data(), &v, sizeof(v));
    
    return this->defConst(
      spv::OpConstant,
      this->defFloatType(32),
      data.size(),
      data.data());
  }
  
  
  API uint32_t APIENTRY SpirvModule::constf64(
          double                  v) {
    std::array<uint32_t, 2> data;
    std::memcpy(data.data(), &v, sizeof(v));
    
    return this->defConst(
      spv::OpConstant,
      this->defFloatType(64),
      data.size(),
      data.data());
  }
  
  
  API uint32_t APIENTRY SpirvModule::constvec4i32(
          int32_t                 x,
          int32_t                 y,
          int32_t                 z,
          int32_t                 w) {
    std::array<uint32_t, 4> args = {{
      this->consti32(x), this->consti32(y),
      this->consti32(z), this->consti32(w),
    }};
    
    uint32_t scalarTypeId = this->defIntType(32, 1);
    uint32_t vectorTypeId = this->defVectorType(scalarTypeId, 4);
    
    return this->constComposite(vectorTypeId, args.size(), args.data());
  }


  API uint32_t APIENTRY SpirvModule::constvec4b32(
          bool                    x,
          bool                    y,
          bool                    z,
          bool                    w) {
    std::array<uint32_t, 4> args = {{
      this->constBool(x), this->constBool(y),
      this->constBool(z), this->constBool(w),
    }};
    
    uint32_t scalarTypeId = this->defBoolType();
    uint32_t vectorTypeId = this->defVectorType(scalarTypeId, 4);
    
    return this->constComposite(vectorTypeId, args.size(), args.data());
  }
  
  
  API uint32_t APIENTRY SpirvModule::constvec4u32(
          uint32_t                x,
          uint32_t                y,
          uint32_t                z,
          uint32_t                w) {
    std::array<uint32_t, 4> args = {{
      this->constu32(x), this->constu32(y),
      this->constu32(z), this->constu32(w),
    }};
    
    uint32_t scalarTypeId = this->defIntType(32, 0);
    uint32_t vectorTypeId = this->defVectorType(scalarTypeId, 4);
    
    return this->constComposite(vectorTypeId, args.size(), args.data());
  }
  

  API uint32_t APIENTRY SpirvModule::constvec2f32(
          float                   x,
          float                   y) {
    std::array<uint32_t, 2> args = {{
      this->constf32(x), this->constf32(y),
    }};
    
    uint32_t scalarTypeId = this->defFloatType(32);
    uint32_t vectorTypeId = this->defVectorType(scalarTypeId, 2);
    
    return this->constComposite(vectorTypeId, args.size(), args.data());
  }
  

  API uint32_t APIENTRY SpirvModule::constvec3f32(
          float                   x,
          float                   y,
          float                   z) {
    std::array<uint32_t, 3> args = {{
      this->constf32(x), this->constf32(y),
      this->constf32(z),
    }};
    
    uint32_t scalarTypeId = this->defFloatType(32);
    uint32_t vectorTypeId = this->defVectorType(scalarTypeId, 3);
    
    return this->constComposite(vectorTypeId, args.size(), args.data());
  }

  
  API uint32_t APIENTRY SpirvModule::constvec4f32(
          float                   x,
          float                   y,
          float                   z,
          float                   w) {
    std::array<uint32_t, 4> args = {{
      this->constf32(x), this->constf32(y),
      this->constf32(z), this->constf32(w),
    }};
    
    uint32_t scalarTypeId = this->defFloatType(32);
    uint32_t vectorTypeId = this->defVectorType(scalarTypeId, 4);
    
    return this->constComposite(vectorTypeId, args.size(), args.data());
  }


  API uint32_t APIENTRY SpirvModule::constfReplicant(
          float                   replicant,
          uint32_t                count) {
    uint32_t value = this->constf32(replicant);

    std::array<uint32_t, 4> args = { value, value, value, value };

    // Can't make a scalar composite.
    if (count == 1)
      return args[0];
    
    uint32_t scalarTypeId = this->defFloatType(32);
    uint32_t vectorTypeId = this->defVectorType(scalarTypeId, count);
    
    return this->constComposite(vectorTypeId, count, args.data());
  }


  API uint32_t APIENTRY SpirvModule::constbReplicant(
          bool                    replicant,
          uint32_t                count) {
    uint32_t value = this->constBool(replicant);

    std::array<uint32_t, 4> args = { value, value, value, value };

    // Can't make a scalar composite.
    if (count == 1)
      return args[0];
    
    uint32_t scalarTypeId = this->defBoolType();
    uint32_t vectorTypeId = this->defVectorType(scalarTypeId, count);
    
    return this->constComposite(vectorTypeId, count, args.data());
  }


  API uint32_t APIENTRY SpirvModule::constiReplicant(
          int32_t                 replicant,
          uint32_t                count) {
    uint32_t value = this->consti32(replicant);

    std::array<uint32_t, 4> args = { value, value, value, value };

    // Can't make a scalar composite.
    if (count == 1)
      return args[0];
    
    uint32_t scalarTypeId = this->defIntType(32, 1);
    uint32_t vectorTypeId = this->defVectorType(scalarTypeId, count);
    
    return this->constComposite(vectorTypeId, count, args.data());
  }


  API uint32_t APIENTRY SpirvModule::constuReplicant(
          int32_t                 replicant,
          uint32_t                count) {
    uint32_t value = this->constu32(replicant);

    std::array<uint32_t, 4> args = { value, value, value, value };

    // Can't make a scalar composite.
    if (count == 1)
      return args[0];
    
    uint32_t scalarTypeId = this->defIntType(32, 0);
    uint32_t vectorTypeId = this->defVectorType(scalarTypeId, count);
    
    return this->constComposite(vectorTypeId, count, args.data());
  }
  
  
  API uint32_t APIENTRY SpirvModule::constComposite(
          uint32_t                typeId,
          uint32_t                constCount,
    const uint32_t*               constIds) {
    return this->defConst(
      spv::OpConstantComposite,
      typeId, constCount, constIds);
  }
  
  
  API uint32_t APIENTRY SpirvModule::constUndef(
          uint32_t                typeId) {
    return this->defConst(spv::OpUndef,
      typeId, 0, nullptr);
  }


  API uint32_t APIENTRY SpirvModule::lateConst32(
          uint32_t                typeId) {
    uint32_t resultId = this->allocateId();
    m_lateConsts.insert(resultId);

    m_typeConstDefs.putIns (spv::OpConstant, 4);
    m_typeConstDefs.putWord(typeId);
    m_typeConstDefs.putWord(resultId);
    m_typeConstDefs.putWord(0);
    return resultId;
  }


  API void APIENTRY SpirvModule::setLateConst(
            uint32_t                constId,
      const uint32_t*               argIds) {
    for (auto ins : m_typeConstDefs) {
      if (ins.opCode() != spv::OpConstant
       && ins.opCode() != spv::OpConstantComposite)
        continue;
      
      if (ins.arg(2) != constId)
        continue;

      for (uint32_t i = 3; i < ins.length(); i++)
        ins.setArg(i, argIds[i - 3]);

      return;
    }
  }


  API uint32_t APIENTRY SpirvModule::specConstBool(
          bool                    v) {
    uint32_t typeId   = this->defBoolType();
    uint32_t resultId = this->allocateId();
    
    const spv::Op op = v
      ? spv::OpSpecConstantTrue
      : spv::OpSpecConstantFalse;
    
    m_typeConstDefs.putIns  (op, 3);
    m_typeConstDefs.putWord (typeId);
    m_typeConstDefs.putWord (resultId);
    return resultId;
  }
    
  
  API uint32_t APIENTRY SpirvModule::specConst32(
          uint32_t                typeId,
          uint32_t                value) {
    uint32_t resultId = this->allocateId();
    
    m_typeConstDefs.putIns  (spv::OpSpecConstant, 4);
    m_typeConstDefs.putWord (typeId);
    m_typeConstDefs.putWord (resultId);
    m_typeConstDefs.putWord (value);
    return resultId;
  }
  
  
  API void APIENTRY SpirvModule::decorate(
          uint32_t                object,
          spv::Decoration         decoration) {
    m_annotations.putIns  (spv::OpDecorate, 3);
    m_annotations.putWord (object);
    m_annotations.putWord (decoration);
  }
  
  
  API void APIENTRY SpirvModule::decorateArrayStride(
          uint32_t                object,
          uint32_t                stride) {
    m_annotations.putIns  (spv::OpDecorate, 4);
    m_annotations.putWord (object);
    m_annotations.putWord (spv::DecorationArrayStride);
    m_annotations.putInt32(stride);
  }
  
  
  API void APIENTRY SpirvModule::decorateBinding(
          uint32_t                object,
          uint32_t                binding) {
    m_annotations.putIns  (spv::OpDecorate, 4);
    m_annotations.putWord (object);
    m_annotations.putWord (spv::DecorationBinding);
    m_annotations.putInt32(binding);
  }
  
  
  API void APIENTRY SpirvModule::decorateBlock(uint32_t object) {
    m_annotations.putIns  (spv::OpDecorate, 3);
    m_annotations.putWord (object);
    m_annotations.putWord (spv::DecorationBlock);
  }
  
  
  API void APIENTRY SpirvModule::decorateBuiltIn(
          uint32_t                object,
          spv::BuiltIn            builtIn) {
    m_annotations.putIns  (spv::OpDecorate, 4);
    m_annotations.putWord (object);
    m_annotations.putWord (spv::DecorationBuiltIn);
    m_annotations.putWord (builtIn);
  }
  
  
  API void APIENTRY SpirvModule::decorateComponent(
          uint32_t                object,
          uint32_t                location) {
    m_annotations.putIns  (spv::OpDecorate, 4);
    m_annotations.putWord (object);
    m_annotations.putWord (spv::DecorationComponent);
    m_annotations.putInt32(location);
  }
  
  
  API void APIENTRY SpirvModule::decorateDescriptorSet(
          uint32_t                object,
          uint32_t                set) {
    m_annotations.putIns  (spv::OpDecorate, 4);
    m_annotations.putWord (object);
    m_annotations.putWord (spv::DecorationDescriptorSet);
    m_annotations.putInt32(set);
  }
  
  
  API void APIENTRY SpirvModule::decorateIndex(
          uint32_t                object,
          uint32_t                index) {
    m_annotations.putIns  (spv::OpDecorate, 4);
    m_annotations.putWord (object);
    m_annotations.putWord (spv::DecorationIndex);
    m_annotations.putInt32(index);
  }


  API void APIENTRY SpirvModule::decorateLocation(
          uint32_t                object,
          uint32_t                location) {
    m_annotations.putIns  (spv::OpDecorate, 4);
    m_annotations.putWord (object);
    m_annotations.putWord (spv::DecorationLocation);
    m_annotations.putInt32(location);
  }
  
  
  API void APIENTRY SpirvModule::decorateSpecId(
          uint32_t                object,
          uint32_t                specId) {
    m_annotations.putIns  (spv::OpDecorate, 4);
    m_annotations.putWord (object);
    m_annotations.putWord (spv::DecorationSpecId);
    m_annotations.putInt32(specId);
  }
  

  API void APIENTRY SpirvModule::decorateXfb(
          uint32_t                object,
          uint32_t                streamId,
          uint32_t                bufferId,
          uint32_t                offset,
          uint32_t                stride) {
    m_annotations.putIns  (spv::OpDecorate, 4);
    m_annotations.putWord (object);
    m_annotations.putWord (spv::DecorationStream);
    m_annotations.putInt32(streamId);

    m_annotations.putIns  (spv::OpDecorate, 4);
    m_annotations.putWord (object);
    m_annotations.putWord (spv::DecorationXfbBuffer);
    m_annotations.putInt32(bufferId);

    m_annotations.putIns  (spv::OpDecorate, 4);
    m_annotations.putWord (object);
    m_annotations.putWord (spv::DecorationXfbStride);
    m_annotations.putInt32(stride);

    m_annotations.putIns  (spv::OpDecorate, 4);
    m_annotations.putWord (object);
    m_annotations.putWord (spv::DecorationOffset);
    m_annotations.putInt32(offset);
  }
  
  
  API void APIENTRY SpirvModule::memberDecorateBuiltIn(
          uint32_t                structId,
          uint32_t                memberId,
          spv::BuiltIn            builtIn) {
    m_annotations.putIns  (spv::OpMemberDecorate, 5);
    m_annotations.putWord (structId);
    m_annotations.putWord (memberId);
    m_annotations.putWord (spv::DecorationBuiltIn);
    m_annotations.putWord (builtIn);
  }


  API void APIENTRY SpirvModule::memberDecorate(
          uint32_t                structId,
          uint32_t                memberId,
          spv::Decoration         decoration) {
    m_annotations.putIns  (spv::OpMemberDecorate, 4);
    m_annotations.putWord (structId);
    m_annotations.putWord (memberId);
    m_annotations.putWord (decoration);
  }


  API void APIENTRY SpirvModule::memberDecorateMatrixStride(
          uint32_t                structId,
          uint32_t                memberId,
          uint32_t                stride) {
    m_annotations.putIns  (spv::OpMemberDecorate, 5);
    m_annotations.putWord (structId);
    m_annotations.putWord (memberId);
    m_annotations.putWord (spv::DecorationMatrixStride);
    m_annotations.putWord (stride);
  }
  
  
  API void APIENTRY SpirvModule::memberDecorateOffset(
          uint32_t                structId,
          uint32_t                memberId,
          uint32_t                offset) {
    m_annotations.putIns  (spv::OpMemberDecorate, 5);
    m_annotations.putWord (structId);
    m_annotations.putWord (memberId);
    m_annotations.putWord (spv::DecorationOffset);
    m_annotations.putWord (offset);
  }
  
  
  API uint32_t APIENTRY SpirvModule::defVoidType() {
    return this->defType(spv::OpTypeVoid, 0, nullptr);
  }
  
  
  API uint32_t APIENTRY SpirvModule::defBoolType() {
    return this->defType(spv::OpTypeBool, 0, nullptr);
  }
  
  
  API uint32_t APIENTRY SpirvModule::defIntType(
          uint32_t                width,
          uint32_t                isSigned) {
    std::array<uint32_t, 2> args = {{ width, isSigned }};
    return this->defType(spv::OpTypeInt,
      args.size(), args.data());
  }
  
  
  API uint32_t APIENTRY SpirvModule::defFloatType(
          uint32_t                width) {
    std::array<uint32_t, 1> args = {{ width }};
    return this->defType(spv::OpTypeFloat,
      args.size(), args.data());
  }
  
  
  API uint32_t APIENTRY SpirvModule::defVectorType(
          uint32_t                elementType,
          uint32_t                elementCount) {
    std::array<uint32_t, 2> args =
      {{ elementType, elementCount }};
    
    return this->defType(spv::OpTypeVector,
      args.size(), args.data());
  }
  
  
  API uint32_t APIENTRY SpirvModule::defMatrixType(
          uint32_t                columnType,
          uint32_t                columnCount) {
    std::array<uint32_t, 2> args =
      {{ columnType, columnCount }};
    
    return this->defType(spv::OpTypeMatrix,
      args.size(), args.data());
  }
  
  
  API uint32_t APIENTRY SpirvModule::defArrayType(
          uint32_t                typeId,
          uint32_t                length) {
    std::array<uint32_t, 2> args = {{ typeId, length }};
    
    return this->defType(spv::OpTypeArray,
      args.size(), args.data());
  }
  
  
  API uint32_t APIENTRY SpirvModule::defArrayTypeUnique(
          uint32_t                typeId,
          uint32_t                length) {
    uint32_t resultId = this->allocateId();
    
    m_typeConstDefs.putIns (spv::OpTypeArray, 4);
    m_typeConstDefs.putWord(resultId);
    m_typeConstDefs.putWord(typeId);
    m_typeConstDefs.putWord(length);
    return resultId;
  }
  
  
  API uint32_t APIENTRY SpirvModule::defRuntimeArrayType(
          uint32_t                typeId) {
    std::array<uint32_t, 1> args = { typeId };
    
    return this->defType(spv::OpTypeRuntimeArray,
      args.size(), args.data());
  }
  
  
  API uint32_t APIENTRY SpirvModule::defRuntimeArrayTypeUnique(
          uint32_t                typeId) {
    uint32_t resultId = this->allocateId();
    
    m_typeConstDefs.putIns (spv::OpTypeRuntimeArray, 3);
    m_typeConstDefs.putWord(resultId);
    m_typeConstDefs.putWord(typeId);
    return resultId;
  }
  
  
  API uint32_t APIENTRY SpirvModule::defFunctionType(
          uint32_t                returnType,
          uint32_t                argCount,
    const uint32_t*               argTypes) {
    std::vector<uint32_t> args;
    args.push_back(returnType);
    
    for (uint32_t i = 0; i < argCount; i++)
      args.push_back(argTypes[i]);
    
    return this->defType(spv::OpTypeFunction,
      args.size(), args.data());
  }
  
  
  API uint32_t APIENTRY SpirvModule::defStructType(
          uint32_t                memberCount,
    const uint32_t*               memberTypes) {
    return this->defType(spv::OpTypeStruct,
      memberCount, memberTypes);
  }
  
  
  API uint32_t APIENTRY SpirvModule::defStructTypeUnique(
          uint32_t                memberCount,
    const uint32_t*               memberTypes) {
    uint32_t resultId = this->allocateId();
    
    m_typeConstDefs.putIns (spv::OpTypeStruct, 2 + memberCount);
    m_typeConstDefs.putWord(resultId);
    
    for (uint32_t i = 0; i < memberCount; i++)
      m_typeConstDefs.putWord(memberTypes[i]);
    return resultId;
  }
  
  
  API uint32_t APIENTRY SpirvModule::defPointerType(
          uint32_t                variableType,
          spv::StorageClass       storageClass) {
    std::array<uint32_t, 2> args = {{
      static_cast<uint32_t>(storageClass),
      variableType,
    }};
    
    return this->defType(spv::OpTypePointer,
      args.size(), args.data());
  }
  
  
  API uint32_t APIENTRY SpirvModule::defSamplerType() {
    return this->defType(spv::OpTypeSampler, 0, nullptr);
  }
  
  
  API uint32_t APIENTRY SpirvModule::defImageType(
          uint32_t                sampledType,
          spv::Dim                dimensionality,
          uint32_t                depth,
          uint32_t                arrayed,
          uint32_t                multisample,
          uint32_t                sampled,
          spv::ImageFormat        format) {
    std::array<uint32_t, 7> args = {{
      sampledType,
    static_cast<uint32_t>(dimensionality),
      depth, arrayed,
      multisample,
      sampled,
    static_cast<uint32_t>(format)
    }};
    
    return this->defType(spv::OpTypeImage,
      args.size(), args.data());
  }
  
  
  API uint32_t APIENTRY SpirvModule::defSampledImageType(
          uint32_t                imageType) {
    return this->defType(spv::OpTypeSampledImage, 1, &imageType);
  }
  
  
  API uint32_t APIENTRY SpirvModule::newVar(
          uint32_t                pointerType,
          spv::StorageClass       storageClass) {
    uint32_t resultId = this->allocateId();
    
    if (isInterfaceVar(storageClass))
      m_interfaceVars.push_back(resultId);

    auto& code = storageClass != spv::StorageClassFunction
      ? m_variables : m_code;

    code.putIns  (spv::OpVariable, 4);
    code.putWord (pointerType);
    code.putWord (resultId);
    code.putWord (storageClass);
    return resultId;
  }
  
  
  API uint32_t APIENTRY SpirvModule::newVarInit(
          uint32_t                pointerType,
          spv::StorageClass       storageClass,
          uint32_t                initialValue) {
    uint32_t resultId = this->allocateId();
    
    if (isInterfaceVar(storageClass))
      m_interfaceVars.push_back(resultId);

    auto& code = storageClass != spv::StorageClassFunction
      ? m_variables : m_code;
    
    code.putIns  (spv::OpVariable, 5);
    code.putWord (pointerType);
    code.putWord (resultId);
    code.putWord (storageClass);
    code.putWord (initialValue);
    return resultId;
  }
  
  
  API void APIENTRY SpirvModule::functionBegin(
          uint32_t                returnType,
          uint32_t                functionId,
          uint32_t                functionType,
    spv::FunctionControlMask      functionControl) {
    m_code.putIns (spv::OpFunction, 5);
    m_code.putWord(returnType);
    m_code.putWord(functionId);
    m_code.putWord(functionControl);
    m_code.putWord(functionType);
  }
  
  
  API uint32_t APIENTRY SpirvModule::functionParameter(
          uint32_t                parameterType) {
    uint32_t parameterId = this->allocateId();
    
    m_code.putIns (spv::OpFunctionParameter, 3);
    m_code.putWord(parameterType);
    m_code.putWord(parameterId);
    return parameterId;
  }
  
  
  API void APIENTRY SpirvModule::functionEnd() {
    m_code.putIns (spv::OpFunctionEnd, 1);
  }


  API uint32_t APIENTRY SpirvModule::opAccessChain(
          uint32_t                resultType,
          uint32_t                composite,
          uint32_t                indexCount,
    const uint32_t*               indexArray) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpAccessChain, 4 + indexCount);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(composite);
    
    for (uint32_t i = 0; i < indexCount; i++)
      m_code.putInt32(indexArray[i]);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opArrayLength(
          uint32_t                resultType,
          uint32_t                structure,
          uint32_t                memberId) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpArrayLength, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(structure);
    m_code.putWord(memberId);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opAny(
          uint32_t                resultType,
          uint32_t                vector) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpAny, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opAll(
          uint32_t                resultType,
          uint32_t                vector) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpAll, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector);
    return resultId;
  }
  
    
    API uint32_t APIENTRY SpirvModule::opAtomicLoad(
          uint32_t                resultType,
          uint32_t                pointer,
          uint32_t                scope,
          uint32_t                semantics) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpAtomicLoad, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(pointer);
    m_code.putWord(scope);
    m_code.putWord(semantics);
    return resultId;
  }
  
  
  API void APIENTRY SpirvModule::opAtomicStore(
          uint32_t                pointer,
          uint32_t                scope,
          uint32_t                semantics,
          uint32_t                value) {
    m_code.putIns (spv::OpAtomicStore, 5);
    m_code.putWord(pointer);
    m_code.putWord(scope);
    m_code.putWord(semantics);
    m_code.putWord(value);
  }
  
  
    API uint32_t APIENTRY SpirvModule::opAtomicExchange(
          uint32_t                resultType,
          uint32_t                pointer,
          uint32_t                scope,
          uint32_t                semantics,
          uint32_t                value) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpAtomicExchange, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(pointer);
    m_code.putWord(scope);
    m_code.putWord(semantics);
    m_code.putWord(value);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opAtomicCompareExchange(
          uint32_t                resultType,
          uint32_t                pointer,
          uint32_t                scope,
          uint32_t                equal,
          uint32_t                unequal,
          uint32_t                value,
          uint32_t                comparator) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpAtomicCompareExchange, 9);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(pointer);
    m_code.putWord(scope);
    m_code.putWord(equal);
    m_code.putWord(unequal);
    m_code.putWord(value);
    m_code.putWord(comparator);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opAtomicIIncrement(
          uint32_t                resultType,
          uint32_t                pointer,
          uint32_t                scope,
          uint32_t                semantics) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpAtomicIIncrement, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(pointer);
    m_code.putWord(scope);
    m_code.putWord(semantics);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opAtomicIDecrement(
          uint32_t                resultType,
          uint32_t                pointer,
          uint32_t                scope,
          uint32_t                semantics) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpAtomicIDecrement, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(pointer);
    m_code.putWord(scope);
    m_code.putWord(semantics);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opAtomicIAdd(
          uint32_t                resultType,
          uint32_t                pointer,
          uint32_t                scope,
          uint32_t                semantics,
          uint32_t                value) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpAtomicIAdd, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(pointer);
    m_code.putWord(scope);
    m_code.putWord(semantics);
    m_code.putWord(value);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opAtomicISub(
          uint32_t                resultType,
          uint32_t                pointer,
          uint32_t                scope,
          uint32_t                semantics,
          uint32_t                value) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpAtomicISub, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(pointer);
    m_code.putWord(scope);
    m_code.putWord(semantics);
    m_code.putWord(value);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opAtomicSMin(
          uint32_t                resultType,
          uint32_t                pointer,
          uint32_t                scope,
          uint32_t                semantics,
          uint32_t                value) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpAtomicSMin, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(pointer);
    m_code.putWord(scope);
    m_code.putWord(semantics);
    m_code.putWord(value);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opAtomicSMax(
          uint32_t                resultType,
          uint32_t                pointer,
          uint32_t                scope,
          uint32_t                semantics,
          uint32_t                value) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpAtomicSMax, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(pointer);
    m_code.putWord(scope);
    m_code.putWord(semantics);
    m_code.putWord(value);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opAtomicUMin(
          uint32_t                resultType,
          uint32_t                pointer,
          uint32_t                scope,
          uint32_t                semantics,
          uint32_t                value) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpAtomicUMin, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(pointer);
    m_code.putWord(scope);
    m_code.putWord(semantics);
    m_code.putWord(value);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opAtomicUMax(
          uint32_t                resultType,
          uint32_t                pointer,
          uint32_t                scope,
          uint32_t                semantics,
          uint32_t                value) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpAtomicUMax, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(pointer);
    m_code.putWord(scope);
    m_code.putWord(semantics);
    m_code.putWord(value);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opAtomicAnd(
          uint32_t                resultType,
          uint32_t                pointer,
          uint32_t                scope,
          uint32_t                semantics,
          uint32_t                value) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpAtomicAnd, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(pointer);
    m_code.putWord(scope);
    m_code.putWord(semantics);
    m_code.putWord(value);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opAtomicOr(
          uint32_t                resultType,
          uint32_t                pointer,
          uint32_t                scope,
          uint32_t                semantics,
          uint32_t                value) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpAtomicOr, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(pointer);
    m_code.putWord(scope);
    m_code.putWord(semantics);
    m_code.putWord(value);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opAtomicXor(
          uint32_t                resultType,
          uint32_t                pointer,
          uint32_t                scope,
          uint32_t                semantics,
          uint32_t                value) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpAtomicXor, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(pointer);
    m_code.putWord(scope);
    m_code.putWord(semantics);
    m_code.putWord(value);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opBitcast(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpBitcast, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opBitCount(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpBitCount, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opBitReverse(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpBitReverse, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opFindILsb(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450FindILsb);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opFindUMsb(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450FindUMsb);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opFindSMsb(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450FindSMsb);
    m_code.putWord(operand);
    return resultId;
  }
  
            
    API uint32_t APIENTRY SpirvModule::opBitFieldInsert(
          uint32_t                resultType,
          uint32_t                base,
          uint32_t                insert,
          uint32_t                offset,
          uint32_t                count) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpBitFieldInsert, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(base);
    m_code.putWord(insert);
    m_code.putWord(offset);
    m_code.putWord(count);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opBitFieldSExtract(
          uint32_t                resultType,
          uint32_t                base,
          uint32_t                offset,
          uint32_t                count) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpBitFieldSExtract, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(base);
    m_code.putWord(offset);
    m_code.putWord(count);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opBitFieldUExtract(
          uint32_t                resultType,
          uint32_t                base,
          uint32_t                offset,
          uint32_t                count) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpBitFieldUExtract, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(base);
    m_code.putWord(offset);
    m_code.putWord(count);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opBitwiseAnd(
          uint32_t                resultType,
          uint32_t                operand1,
          uint32_t                operand2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpBitwiseAnd, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand1);
    m_code.putWord(operand2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opBitwiseOr(
          uint32_t                resultType,
          uint32_t                operand1,
          uint32_t                operand2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpBitwiseOr, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand1);
    m_code.putWord(operand2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opBitwiseXor(
          uint32_t                resultType,
          uint32_t                operand1,
          uint32_t                operand2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpBitwiseXor, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand1);
    m_code.putWord(operand2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opNot(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpNot, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opShiftLeftLogical(
          uint32_t                resultType,
          uint32_t                base,
          uint32_t                shift) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpShiftLeftLogical, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(base);
    m_code.putWord(shift);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opShiftRightArithmetic(
          uint32_t                resultType,
          uint32_t                base,
          uint32_t                shift) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpShiftRightArithmetic, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(base);
    m_code.putWord(shift);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opShiftRightLogical(
          uint32_t                resultType,
          uint32_t                base,
          uint32_t                shift) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpShiftRightLogical, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(base);
    m_code.putWord(shift);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opConvertFtoS(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpConvertFToS, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opConvertFtoU(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpConvertFToU, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opConvertStoF(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpConvertSToF, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opConvertUtoF(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpConvertUToF, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opCompositeConstruct(
          uint32_t                resultType,
          uint32_t                valueCount,
    const uint32_t*               valueArray) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpCompositeConstruct, 3 + valueCount);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    
    for (uint32_t i = 0; i < valueCount; i++)
      m_code.putWord(valueArray[i]);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opCompositeExtract(
          uint32_t                resultType,
          uint32_t                composite,
          uint32_t                indexCount,
    const uint32_t*               indexArray) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpCompositeExtract, 4 + indexCount);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(composite);
    
    for (uint32_t i = 0; i < indexCount; i++)
      m_code.putInt32(indexArray[i]);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opCompositeInsert(
          uint32_t                resultType,
          uint32_t                object,
          uint32_t                composite,
          uint32_t                indexCount,
    const uint32_t*               indexArray) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpCompositeInsert, 5 + indexCount);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(object);
    m_code.putWord(composite);
    
    for (uint32_t i = 0; i < indexCount; i++)
      m_code.putInt32(indexArray[i]);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opDpdx(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpDPdx, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opDpdy(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpDPdy, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opDpdxCoarse(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpDPdxCoarse, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opDpdyCoarse(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpDPdyCoarse, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opDpdxFine(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpDPdxFine, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opDpdyFine(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpDPdyFine, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opVectorExtractDynamic(
          uint32_t                resultType,
          uint32_t                vector,
          uint32_t                index) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpVectorExtractDynamic, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector);
    m_code.putWord(index);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opVectorShuffle(
          uint32_t                resultType,
          uint32_t                vectorLeft,
          uint32_t                vectorRight,
          uint32_t                indexCount,
    const uint32_t*               indexArray) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpVectorShuffle, 5 + indexCount);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vectorLeft);
    m_code.putWord(vectorRight);
    
    for (uint32_t i = 0; i < indexCount; i++)
      m_code.putInt32(indexArray[i]);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opSNegate(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpSNegate, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opFNegate(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpFNegate, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opSAbs(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450SAbs);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opFAbs(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450FAbs);
    m_code.putWord(operand);
    return resultId;
  }


      API uint32_t APIENTRY SpirvModule::opFSign(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();

    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450FSign);
    m_code.putWord(operand);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opFMix(
          uint32_t                resultType,
          uint32_t                x,
          uint32_t                y,
          uint32_t                a) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 8);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450FMix);
    m_code.putWord(x);
    m_code.putWord(y);
    m_code.putWord(a);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opCross(
          uint32_t                resultType,
          uint32_t                x,
          uint32_t                y) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450Cross);
    m_code.putWord(x);
    m_code.putWord(y);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opIAdd(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpIAdd, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opISub(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpISub, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opFAdd(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpFAdd, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opFSub(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpFSub, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opSDiv(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpSDiv, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opUDiv(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpUDiv, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opSRem(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpSRem, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opUMod(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpUMod, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opFDiv(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpFDiv, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opIMul(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpIMul, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
  
    
    API uint32_t APIENTRY SpirvModule::opFMul(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpFMul, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opVectorTimesScalar(
    uint32_t                resultType,
    uint32_t                vector,
    uint32_t                scalar) {
    uint32_t resultId = this->allocateId();

    m_code.putIns(spv::OpVectorTimesScalar, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector);
    m_code.putWord(scalar);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opMatrixTimesMatrix(
    uint32_t                resultType,
    uint32_t                a,
    uint32_t                b) {
    uint32_t resultId = this->allocateId();

    m_code.putIns(spv::OpMatrixTimesMatrix, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opMatrixTimesVector(
    uint32_t                resultType,
    uint32_t                matrix,
    uint32_t                vector) {
    uint32_t resultId = this->allocateId();

    m_code.putIns(spv::OpMatrixTimesVector, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(matrix);
    m_code.putWord(vector);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opVectorTimesMatrix(
    uint32_t                resultType,
    uint32_t                vector,
    uint32_t                matrix) {
    uint32_t resultId = this->allocateId();

    m_code.putIns(spv::OpVectorTimesMatrix, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector);
    m_code.putWord(matrix);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opTranspose(
    uint32_t                resultType,
    uint32_t                matrix) {
    uint32_t resultId = this->allocateId();

    m_code.putIns(spv::OpTranspose, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(matrix);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opInverse(
    uint32_t                resultType,
    uint32_t                matrix) {
    uint32_t resultId = this->allocateId();

    m_code.putIns(spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450MatrixInverse);
    m_code.putWord(matrix);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opFFma(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b,
          uint32_t                c) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 8);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450Fma);
    m_code.putWord(a);
    m_code.putWord(b);
    m_code.putWord(c);
    return resultId;
  }
    
  
    API uint32_t APIENTRY SpirvModule::opFMax(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450FMax);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opFMin(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450FMin);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
    
  
    API uint32_t APIENTRY SpirvModule::opNMax(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450NMax);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opNMin(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450NMin);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opSMax(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450SMax);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opSMin(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450SMin);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opUMax(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450UMax);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opUMin(
          uint32_t                resultType,
          uint32_t                a,
          uint32_t                b) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450UMin);
    m_code.putWord(a);
    m_code.putWord(b);
    return resultId;
  }
  
  
  API uint32_t APIENTRY SpirvModule::opFClamp(
          uint32_t                resultType,
          uint32_t                x,
          uint32_t                minVal,
          uint32_t                maxVal) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 8);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450FClamp);
    m_code.putWord(x);
    m_code.putWord(minVal);
    m_code.putWord(maxVal);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opNClamp(
          uint32_t                resultType,
          uint32_t                x,
          uint32_t                minVal,
          uint32_t                maxVal) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 8);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450NClamp);
    m_code.putWord(x);
    m_code.putWord(minVal);
    m_code.putWord(maxVal);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opIEqual(
          uint32_t                resultType,
          uint32_t                vector1,
          uint32_t                vector2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpIEqual, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector1);
    m_code.putWord(vector2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opINotEqual(
          uint32_t                resultType,
          uint32_t                vector1,
          uint32_t                vector2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpINotEqual, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector1);
    m_code.putWord(vector2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opSLessThan(
          uint32_t                resultType,
          uint32_t                vector1,
          uint32_t                vector2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpSLessThan, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector1);
    m_code.putWord(vector2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opSLessThanEqual(
          uint32_t                resultType,
          uint32_t                vector1,
          uint32_t                vector2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpSLessThanEqual, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector1);
    m_code.putWord(vector2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opSGreaterThan(
          uint32_t                resultType,
          uint32_t                vector1,
          uint32_t                vector2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpSGreaterThan, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector1);
    m_code.putWord(vector2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opSGreaterThanEqual(
          uint32_t                resultType,
          uint32_t                vector1,
          uint32_t                vector2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpSGreaterThanEqual, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector1);
    m_code.putWord(vector2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opULessThan(
          uint32_t                resultType,
          uint32_t                vector1,
          uint32_t                vector2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpULessThan, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector1);
    m_code.putWord(vector2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opULessThanEqual(
          uint32_t                resultType,
          uint32_t                vector1,
          uint32_t                vector2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpULessThanEqual, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector1);
    m_code.putWord(vector2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opUGreaterThan(
          uint32_t                resultType,
          uint32_t                vector1,
          uint32_t                vector2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpUGreaterThan, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector1);
    m_code.putWord(vector2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opUGreaterThanEqual(
          uint32_t                resultType,
          uint32_t                vector1,
          uint32_t                vector2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpUGreaterThanEqual, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector1);
    m_code.putWord(vector2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opFOrdEqual(
          uint32_t                resultType,
          uint32_t                vector1,
          uint32_t                vector2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpFOrdEqual, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector1);
    m_code.putWord(vector2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opFOrdNotEqual(
          uint32_t                resultType,
          uint32_t                vector1,
          uint32_t                vector2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpFOrdNotEqual, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector1);
    m_code.putWord(vector2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opFOrdLessThan(
          uint32_t                resultType,
          uint32_t                vector1,
          uint32_t                vector2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpFOrdLessThan, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector1);
    m_code.putWord(vector2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opFOrdLessThanEqual(
          uint32_t                resultType,
          uint32_t                vector1,
          uint32_t                vector2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpFOrdLessThanEqual, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector1);
    m_code.putWord(vector2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opFOrdGreaterThan(
          uint32_t                resultType,
          uint32_t                vector1,
          uint32_t                vector2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpFOrdGreaterThan, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector1);
    m_code.putWord(vector2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opFOrdGreaterThanEqual(
          uint32_t                resultType,
          uint32_t                vector1,
          uint32_t                vector2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpFOrdGreaterThanEqual, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector1);
    m_code.putWord(vector2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opLogicalEqual(
          uint32_t                resultType,
          uint32_t                operand1,
          uint32_t                operand2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpLogicalEqual, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand1);
    m_code.putWord(operand2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opLogicalNotEqual(
          uint32_t                resultType,
          uint32_t                operand1,
          uint32_t                operand2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpLogicalNotEqual, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand1);
    m_code.putWord(operand2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opLogicalAnd(
          uint32_t                resultType,
          uint32_t                operand1,
          uint32_t                operand2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpLogicalAnd, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand1);
    m_code.putWord(operand2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opLogicalOr(
          uint32_t                resultType,
          uint32_t                operand1,
          uint32_t                operand2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpLogicalOr, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand1);
    m_code.putWord(operand2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opLogicalNot(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpLogicalNot, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opDot(
          uint32_t                resultType,
          uint32_t                vector1,
          uint32_t                vector2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpDot, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(vector1);
    m_code.putWord(vector2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opSin(
          uint32_t                resultType,
          uint32_t                vector) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450Sin);
    m_code.putWord(vector);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opCos(
          uint32_t                resultType,
          uint32_t                vector) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450Cos);
    m_code.putWord(vector);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opSqrt(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450Sqrt);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opInverseSqrt(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450InverseSqrt);
    m_code.putWord(operand);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opNormalize(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450Normalize);
    m_code.putWord(operand);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opReflect(
          uint32_t                resultType,
          uint32_t                incident,
          uint32_t                normal) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450Reflect);
    m_code.putWord(incident);
    m_code.putWord(normal);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opLength(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450Length);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opExp2(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450Exp2);
    m_code.putWord(operand);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opExp(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450Exp);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opLog2(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450Log2);
    m_code.putWord(operand);
    return resultId;
  }

    API uint32_t APIENTRY SpirvModule::opPow(
    uint32_t                resultType,
    uint32_t                base,
    uint32_t                exponent) {
    uint32_t resultId = this->allocateId();

    m_code.putIns(spv::OpExtInst, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450Pow);
    m_code.putWord(base);
    m_code.putWord(exponent);
    return resultId;
  }
  
    API uint32_t APIENTRY SpirvModule::opFract(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450Fract);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opCeil(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450Ceil);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opFloor(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450Floor);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opRound(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450Round);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opRoundEven(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450RoundEven);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opTrunc(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450Trunc);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opFConvert(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();

    m_code.putIns (spv::OpFConvert, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opPackHalf2x16(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450PackHalf2x16);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opUnpackHalf2x16(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450UnpackHalf2x16);
    m_code.putWord(operand);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opSelect(
          uint32_t                resultType,
          uint32_t                condition,
          uint32_t                operand1,
          uint32_t                operand2) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpSelect, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(condition);
    m_code.putWord(operand1);
    m_code.putWord(operand2);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opIsNan(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpIsNan, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opIsInf(
          uint32_t                resultType,
          uint32_t                operand) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpIsInf, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(operand);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opFunctionCall(
          uint32_t                resultType,
          uint32_t                functionId,
          uint32_t                argCount,
    const uint32_t*               argIds) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpFunctionCall, 4 + argCount);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(functionId);
    
    for (uint32_t i = 0; i < argCount; i++)
      m_code.putWord(argIds[i]);
    return resultId;
  }
  
  
  API void APIENTRY SpirvModule::opLabel(uint32_t labelId) {
    m_code.putIns (spv::OpLabel, 2);
    m_code.putWord(labelId);

    m_blockId = labelId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opLoad(
          uint32_t                typeId,
          uint32_t                pointerId) {
    return opLoad(typeId, pointerId, SpirvMemoryOperands());
  }


    API uint32_t APIENTRY SpirvModule::opLoad(
          uint32_t                typeId,
          uint32_t                pointerId,
    const SpirvMemoryOperands&    operands) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpLoad, 4 + getMemoryOperandWordCount(operands));
    m_code.putWord(typeId);
    m_code.putWord(resultId);
    m_code.putWord(pointerId);

    putMemoryOperands(operands);
    return resultId;
  }
  
  
  API void APIENTRY SpirvModule::opStore(
          uint32_t                pointerId,
          uint32_t                valueId) {
    opStore(pointerId, valueId, SpirvMemoryOperands());
  }


  API void APIENTRY SpirvModule::opStore(
          uint32_t                pointerId,
          uint32_t                valueId,
    const SpirvMemoryOperands&    operands) {
    m_code.putIns (spv::OpStore, 3 + getMemoryOperandWordCount(operands));
    m_code.putWord(pointerId);
    m_code.putWord(valueId);

    putMemoryOperands(operands);
  }


    API uint32_t APIENTRY SpirvModule::opInterpolateAtCentroid(
          uint32_t                resultType,
          uint32_t                interpolant) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450InterpolateAtCentroid);
    m_code.putWord(interpolant);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opInterpolateAtSample(
          uint32_t                resultType,
          uint32_t                interpolant,
          uint32_t                sample) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450InterpolateAtSample);
    m_code.putWord(interpolant);
    m_code.putWord(sample);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opInterpolateAtOffset(
          uint32_t                resultType,
          uint32_t                interpolant,
          uint32_t                offset) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpExtInst, 7);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(m_instExtGlsl450);
    m_code.putWord(GLSLstd450InterpolateAtOffset);
    m_code.putWord(interpolant);
    m_code.putWord(offset);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opImage(
          uint32_t                resultType,
          uint32_t                sampledImage) {
    uint32_t resultId = this->allocateId();

    m_code.putIns(spv::OpImage, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(sampledImage);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opImageRead(
          uint32_t                resultType,
          uint32_t                image,
          uint32_t                coordinates,
    const SpirvImageOperands&     operands) {
    uint32_t resultId = this->allocateId();
    
    spv::Op op = operands.sparse
      ? spv::OpImageSparseRead
      : spv::OpImageRead;

    m_code.putIns(op, 5 + getImageOperandWordCount(operands));
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(image);
    m_code.putWord(coordinates);
    
    putImageOperands(operands);
    return resultId;
  }
  
  
  API void APIENTRY SpirvModule::opImageWrite(
          uint32_t                image,
          uint32_t                coordinates,
          uint32_t                texel,
    const SpirvImageOperands&     operands) {
    m_code.putIns (spv::OpImageWrite,
      4 + getImageOperandWordCount(operands));
    m_code.putWord(image);
    m_code.putWord(coordinates);
    m_code.putWord(texel);
    
    putImageOperands(operands);
  }
  
  
    API uint32_t APIENTRY SpirvModule::opImageSparseTexelsResident(
          uint32_t                resultType,
          uint32_t                residentCode) {
    uint32_t resultId = this->allocateId();

    m_code.putIns (spv::OpImageSparseTexelsResident, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(residentCode);

    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opSampledImage(
          uint32_t                resultType,
          uint32_t                image,
          uint32_t                sampler) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpSampledImage, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(image);
    m_code.putWord(sampler);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opImageTexelPointer(
          uint32_t                resultType,
          uint32_t                image,
          uint32_t                coordinates,
          uint32_t                sample) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpImageTexelPointer, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(image);
    m_code.putWord(coordinates);
    m_code.putWord(sample);
    return resultId;
  }
  
    
    API uint32_t APIENTRY SpirvModule::opImageQuerySizeLod(
          uint32_t                resultType,
          uint32_t                image,
          uint32_t                lod) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpImageQuerySizeLod, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(image);
    m_code.putWord(lod);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opImageQuerySize(
          uint32_t                resultType,
          uint32_t                image) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpImageQuerySize, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(image);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opImageQueryLevels(
          uint32_t                resultType,
          uint32_t                image) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpImageQueryLevels, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(image);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opImageQueryLod(
          uint32_t                resultType,
          uint32_t                sampledImage,
          uint32_t                coordinates) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpImageQueryLod, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(sampledImage);
    m_code.putWord(coordinates);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opImageQuerySamples(
          uint32_t                resultType,
          uint32_t                image) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpImageQuerySamples, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(image);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opImageFetch(
          uint32_t                resultType,
          uint32_t                image,
          uint32_t                coordinates,
    const SpirvImageOperands&     operands) {
    uint32_t resultId = this->allocateId();

    spv::Op op = operands.sparse
      ? spv::OpImageSparseFetch
      : spv::OpImageFetch;

    m_code.putIns(op, 5 + getImageOperandWordCount(operands));
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(image);
    m_code.putWord(coordinates);
    
    putImageOperands(operands);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opImageGather(
          uint32_t                resultType,
          uint32_t                sampledImage,
          uint32_t                coordinates,
          uint32_t                component,
    const SpirvImageOperands&     operands) {
    uint32_t resultId = this->allocateId();
    
    spv::Op op = operands.sparse
      ? spv::OpImageSparseGather
      : spv::OpImageGather;

    m_code.putIns(op, 6 + getImageOperandWordCount(operands));
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(sampledImage);
    m_code.putWord(coordinates);
    m_code.putWord(component);
    
    putImageOperands(operands);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opImageDrefGather(
          uint32_t                resultType,
          uint32_t                sampledImage,
          uint32_t                coordinates,
          uint32_t                reference,
    const SpirvImageOperands&     operands) {
    uint32_t resultId = this->allocateId();
    
    spv::Op op = operands.sparse
      ? spv::OpImageSparseDrefGather
      : spv::OpImageDrefGather;

    m_code.putIns(op, 6 + getImageOperandWordCount(operands));
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(sampledImage);
    m_code.putWord(coordinates);
    m_code.putWord(reference);
    
    putImageOperands(operands);
    return resultId;
  }
  
    
    API uint32_t APIENTRY SpirvModule::opImageSampleImplicitLod(
          uint32_t                resultType,
          uint32_t                sampledImage,
          uint32_t                coordinates,
    const SpirvImageOperands&     operands) {
    uint32_t resultId = this->allocateId();
    
    spv::Op op = operands.sparse
      ? spv::OpImageSparseSampleImplicitLod
      : spv::OpImageSampleImplicitLod;

    m_code.putIns(op, 5 + getImageOperandWordCount(operands));
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(sampledImage);
    m_code.putWord(coordinates);
    
    putImageOperands(operands);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opImageSampleExplicitLod(
          uint32_t                resultType,
          uint32_t                sampledImage,
          uint32_t                coordinates,
    const SpirvImageOperands&     operands) {
    uint32_t resultId = this->allocateId();
    
    spv::Op op = operands.sparse
      ? spv::OpImageSparseSampleExplicitLod
      : spv::OpImageSampleExplicitLod;

    m_code.putIns(op, 5 + getImageOperandWordCount(operands));
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(sampledImage);
    m_code.putWord(coordinates);
    
    putImageOperands(operands);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opImageSampleProjImplicitLod(
          uint32_t                resultType,
          uint32_t                sampledImage,
          uint32_t                coordinates,
    const SpirvImageOperands&     operands) {
    uint32_t resultId = this->allocateId();
    
    spv::Op op = operands.sparse
      ? spv::OpImageSparseSampleProjImplicitLod
      : spv::OpImageSampleProjImplicitLod;

    m_code.putIns(op, 5 + getImageOperandWordCount(operands));
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(sampledImage);
    m_code.putWord(coordinates);
    
    putImageOperands(operands);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opImageSampleProjExplicitLod(
          uint32_t                resultType,
          uint32_t                sampledImage,
          uint32_t                coordinates,
    const SpirvImageOperands&     operands) {
    uint32_t resultId = this->allocateId();
    
    spv::Op op = operands.sparse
      ? spv::OpImageSparseSampleProjExplicitLod
      : spv::OpImageSampleProjExplicitLod;

    m_code.putIns(op, 5 + getImageOperandWordCount(operands));
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(sampledImage);
    m_code.putWord(coordinates);
    
    putImageOperands(operands);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opImageSampleDrefImplicitLod(
          uint32_t                resultType,
          uint32_t                sampledImage,
          uint32_t                coordinates,
          uint32_t                reference,
    const SpirvImageOperands&     operands) {
    uint32_t resultId = this->allocateId();
    
    spv::Op op = operands.sparse
      ? spv::OpImageSparseSampleDrefImplicitLod
      : spv::OpImageSampleDrefImplicitLod;

    m_code.putIns(op, 6 + getImageOperandWordCount(operands));
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(sampledImage);
    m_code.putWord(coordinates);
    m_code.putWord(reference);
    
    putImageOperands(operands);
    return resultId;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opImageSampleDrefExplicitLod(
          uint32_t                resultType,
          uint32_t                sampledImage,
          uint32_t                coordinates,
          uint32_t                reference,
    const SpirvImageOperands&     operands) {
    uint32_t resultId = this->allocateId();
    
    spv::Op op = operands.sparse
      ? spv::OpImageSparseSampleDrefExplicitLod
      : spv::OpImageSampleDrefExplicitLod;

    m_code.putIns(op, 6 + getImageOperandWordCount(operands));
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(sampledImage);
    m_code.putWord(coordinates);
    m_code.putWord(reference);
    
    putImageOperands(operands);
    return resultId;
  }
  

    API uint32_t APIENTRY SpirvModule::opImageSampleProjDrefImplicitLod(
          uint32_t                resultType,
          uint32_t                sampledImage,
          uint32_t                coordinates,
          uint32_t                reference,
    const SpirvImageOperands&     operands) {
    uint32_t resultId = this->allocateId();
    
    spv::Op op = operands.sparse
      ? spv::OpImageSparseSampleProjDrefImplicitLod
      : spv::OpImageSampleProjDrefImplicitLod;

    m_code.putIns(op, 6 + getImageOperandWordCount(operands));
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(sampledImage);
    m_code.putWord(coordinates);
    m_code.putWord(reference);
    
    putImageOperands(operands);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opImageSampleProjDrefExplicitLod(
          uint32_t                resultType,
          uint32_t                sampledImage,
          uint32_t                coordinates,
          uint32_t                reference,
    const SpirvImageOperands&     operands) {
    uint32_t resultId = this->allocateId();
    
    spv::Op op = operands.sparse
      ? spv::OpImageSparseSampleProjDrefExplicitLod
      : spv::OpImageSampleProjDrefExplicitLod;

    m_code.putIns(op, 6 + getImageOperandWordCount(operands));
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(sampledImage);
    m_code.putWord(coordinates);
    m_code.putWord(reference);
    
    putImageOperands(operands);
    return resultId;
  }

  
    API uint32_t APIENTRY SpirvModule::opGroupNonUniformBallot(
          uint32_t                resultType,
          uint32_t                execution,
          uint32_t                predicate) {
    uint32_t resultId = this->allocateId();

    m_code.putIns(spv::OpGroupNonUniformBallot, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(execution);
    m_code.putWord(predicate);
    return resultId;
  }

  
    API uint32_t APIENTRY SpirvModule::opGroupNonUniformBallotBitCount(
          uint32_t                resultType,
          uint32_t                execution,
          uint32_t                operation,
          uint32_t                ballot) {
    uint32_t resultId = this->allocateId();

    m_code.putIns(spv::OpGroupNonUniformBallotBitCount, 6);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(execution);
    m_code.putWord(operation);
    m_code.putWord(ballot);
    return resultId;
  }


    API uint32_t APIENTRY SpirvModule::opGroupNonUniformElect(
          uint32_t                resultType,
          uint32_t                execution) {
    uint32_t resultId = this->allocateId();

    m_code.putIns(spv::OpGroupNonUniformElect, 4);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(execution);
    return resultId;
  }

  
    API uint32_t APIENTRY SpirvModule::opGroupNonUniformBroadcastFirst(
          uint32_t                resultType,
          uint32_t                execution,
          uint32_t                value) {
    uint32_t resultId = this->allocateId();

    m_code.putIns(spv::OpGroupNonUniformBroadcastFirst, 5);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    m_code.putWord(execution);
    m_code.putWord(value);
    return resultId;
  }


  API void APIENTRY SpirvModule::opControlBarrier(
          uint32_t                execution,
          uint32_t                memory,
          uint32_t                semantics) {
    m_code.putIns (spv::OpControlBarrier, 4);
    m_code.putWord(execution);
    m_code.putWord(memory);
    m_code.putWord(semantics);
  }
  
  
  API void APIENTRY SpirvModule::opMemoryBarrier(
          uint32_t                memory,
          uint32_t                semantics) {
    m_code.putIns (spv::OpMemoryBarrier, 3);
    m_code.putWord(memory);
    m_code.putWord(semantics);
  }
  
  
  API void APIENTRY SpirvModule::opLoopMerge(
          uint32_t                mergeBlock,
          uint32_t                continueTarget,
          uint32_t                loopControl) {
    m_code.putIns (spv::OpLoopMerge, 4);
    m_code.putWord(mergeBlock);
    m_code.putWord(continueTarget);
    m_code.putWord(loopControl);
  }
  
  
  API void APIENTRY SpirvModule::opSelectionMerge(
          uint32_t                mergeBlock,
          uint32_t                selectionControl) {
    m_code.putIns (spv::OpSelectionMerge, 3);
    m_code.putWord(mergeBlock);
    m_code.putWord(selectionControl);
  }
  
  
  API void APIENTRY SpirvModule::opBranch(
          uint32_t                label) {
    m_code.putIns (spv::OpBranch, 2);
    m_code.putWord(label);

    m_blockId = 0;
  }
  
  
  API void APIENTRY SpirvModule::opBranchConditional(
          uint32_t                condition,
          uint32_t                trueLabel,
          uint32_t                falseLabel) {
    m_code.putIns (spv::OpBranchConditional, 4);
    m_code.putWord(condition);
    m_code.putWord(trueLabel);
    m_code.putWord(falseLabel);

    m_blockId = 0;
  }
  
  
  API void APIENTRY SpirvModule::opSwitch(
          uint32_t                selector,
          uint32_t                jumpDefault,
          uint32_t                caseCount,
    const SpirvSwitchCaseLabel*   caseLabels) {
    m_code.putIns (spv::OpSwitch, 3 + 2 * caseCount);
    m_code.putWord(selector);
    m_code.putWord(jumpDefault);
    
    for (uint32_t i = 0; i < caseCount; i++) {
      m_code.putWord(caseLabels[i].literal);
      m_code.putWord(caseLabels[i].labelId);
    }

    m_blockId = 0;
  }
  
  
    API uint32_t APIENTRY SpirvModule::opPhi(
          uint32_t                resultType,
          uint32_t                sourceCount,
    const SpirvPhiLabel*          sourceLabels) {
    uint32_t resultId = this->allocateId();
    
    m_code.putIns (spv::OpPhi, 3 + 2 * sourceCount);
    m_code.putWord(resultType);
    m_code.putWord(resultId);
    
    for (uint32_t i = 0; i < sourceCount; i++) {
      m_code.putWord(sourceLabels[i].varId);
      m_code.putWord(sourceLabels[i].labelId);
    }
    
    return resultId;
  }
  
    
  API void APIENTRY SpirvModule::opReturn() {
    m_code.putIns (spv::OpReturn, 1);
    m_blockId = 0;
  }
  
  
  API void APIENTRY SpirvModule::opDemoteToHelperInvocation() {
    m_code.putIns (spv::OpDemoteToHelperInvocation, 1);
  }
  
  
  API void APIENTRY SpirvModule::opEmitVertex(
          uint32_t                streamId) {
    if (streamId == 0) {
      m_code.putIns (spv::OpEmitVertex, 1);
    } else {
      m_code.putIns (spv::OpEmitStreamVertex, 2);
      m_code.putWord(streamId);
    }
  }
  
  
  API void APIENTRY SpirvModule::opEndPrimitive(
          uint32_t                streamId) {
    if (streamId == 0) {
      m_code.putIns (spv::OpEndPrimitive, 1);
    } else {
      m_code.putIns (spv::OpEndStreamPrimitive, 2);
      m_code.putWord(streamId);
    }
  }
  
  
  API void APIENTRY SpirvModule::opBeginInvocationInterlock() {
    m_code.putIns(spv::OpBeginInvocationInterlockEXT, 1);
  }


  API void APIENTRY SpirvModule::opEndInvocationInterlock() {
    m_code.putIns(spv::OpEndInvocationInterlockEXT, 1);
  }


  API uint32_t APIENTRY SpirvModule::defType(
          spv::Op                 op, 
          uint32_t                argCount,
    const uint32_t*               argIds) {
    // Since the type info is stored in the code buffer,
    // we can use the code buffer to look up type IDs as
    // well. Result IDs are always stored as argument 1.
    for (auto ins : m_typeConstDefs) {
      bool match = ins.opCode() == op
                && ins.length() == 2 + argCount;
      
      for (uint32_t i = 0; i < argCount && match; i++)
        match &= ins.arg(2 + i) == argIds[i];
      
      if (match)
        return ins.arg(1);
    }
    
    // Type not yet declared, create a new one.
    uint32_t resultId = this->allocateId();
    m_typeConstDefs.putIns (op, 2 + argCount);
    m_typeConstDefs.putWord(resultId);
    
    for (uint32_t i = 0; i < argCount; i++)
      m_typeConstDefs.putWord(argIds[i]);
    return resultId;
  }
  
  
  API uint32_t APIENTRY SpirvModule::defConst(
          spv::Op                 op,
          uint32_t                typeId,
          uint32_t                argCount,
    const uint32_t*               argIds) {
    // Avoid declaring constants multiple times
    for (auto ins : m_typeConstDefs) {
      bool match = ins.opCode() == op
                && ins.length() == 3 + argCount
                && ins.arg(1)   == typeId;
      
      for (uint32_t i = 0; i < argCount && match; i++)
        match &= ins.arg(3 + i) == argIds[i];
      
      if (!match)
        continue;
      
      uint32_t id = ins.arg(2);

      if (m_lateConsts.find(id) == m_lateConsts.end())
        return id;
    }
    
    // Constant not yet declared, make a new one
    uint32_t resultId = this->allocateId();
    m_typeConstDefs.putIns (op, 3 + argCount);
    m_typeConstDefs.putWord(typeId);
    m_typeConstDefs.putWord(resultId);
    
    for (uint32_t i = 0; i < argCount; i++)
      m_typeConstDefs.putWord(argIds[i]);
    return resultId;
  }
  
  
  API void APIENTRY SpirvModule::instImportGlsl450() {
    m_instExtGlsl450 = this->allocateId();
    const char* name = "GLSL.std.450";
    
    m_instExt.putIns (spv::OpExtInstImport, 2 + m_instExt.strLen(name));
    m_instExt.putWord(m_instExtGlsl450);
    m_instExt.putStr (name);
  }
  
  
  API uint32_t APIENTRY SpirvModule::getMemoryOperandWordCount(
    const SpirvMemoryOperands&    op) const {
    const uint32_t result
      = ((op.flags & spv::MemoryAccessAlignedMask)              ? 1 : 0)
      + ((op.flags & spv::MemoryAccessMakePointerAvailableMask) ? 1 : 0)
      + ((op.flags & spv::MemoryAccessMakePointerVisibleMask)   ? 1 : 0);

    return op.flags ? result + 1 : 0;
  }


  API void APIENTRY SpirvModule::putMemoryOperands(
    const SpirvMemoryOperands&    op) {
    if (op.flags) {
      m_code.putWord(op.flags);

      if (op.flags & spv::MemoryAccessAlignedMask)
        m_code.putWord(op.alignment);

      if (op.flags & spv::MemoryAccessMakePointerAvailableMask)
        m_code.putWord(op.makeAvailable);

      if (op.flags & spv::MemoryAccessMakePointerVisibleMask)
        m_code.putWord(op.makeVisible);
    }
  }


  API uint32_t APIENTRY SpirvModule::getImageOperandWordCount(const SpirvImageOperands& op) const {
    // Each flag may add one or more operands
    const uint32_t result
      = ((op.flags & spv::ImageOperandsBiasMask)                ? 1 : 0)
      + ((op.flags & spv::ImageOperandsLodMask)                 ? 1 : 0)
      + ((op.flags & spv::ImageOperandsConstOffsetMask)         ? 1 : 0)
      + ((op.flags & spv::ImageOperandsGradMask)                ? 2 : 0)
      + ((op.flags & spv::ImageOperandsOffsetMask)              ? 1 : 0)
      + ((op.flags & spv::ImageOperandsConstOffsetsMask)        ? 1 : 0)
      + ((op.flags & spv::ImageOperandsSampleMask)              ? 1 : 0)
      + ((op.flags & spv::ImageOperandsMinLodMask)              ? 1 : 0)
      + ((op.flags & spv::ImageOperandsMakeTexelAvailableMask)  ? 1 : 0)
      + ((op.flags & spv::ImageOperandsMakeTexelVisibleMask)    ? 1 : 0);
    
    // Add a DWORD for the operand mask if it is non-zero
    return op.flags ? result + 1 : 0;
  }
  
  
  API void APIENTRY SpirvModule::putImageOperands(const SpirvImageOperands& op) {
    if (op.flags) {
      m_code.putWord(op.flags);
      
      if (op.flags & spv::ImageOperandsBiasMask)
        m_code.putWord(op.sLodBias);
      
      if (op.flags & spv::ImageOperandsLodMask)
        m_code.putWord(op.sLod);
      
      if (op.flags & spv::ImageOperandsConstOffsetMask)
        m_code.putWord(op.sConstOffset);
      
      if (op.flags & spv::ImageOperandsGradMask) {
        m_code.putWord(op.sGradX);
        m_code.putWord(op.sGradY);
      }
      
      if (op.flags & spv::ImageOperandsOffsetMask)
        m_code.putWord(op.gOffset);
      
      if (op.flags & spv::ImageOperandsConstOffsetsMask)
        m_code.putWord(op.gConstOffsets);
      
      if (op.flags & spv::ImageOperandsSampleMask)
        m_code.putWord(op.sSampleId);
      
      if (op.flags & spv::ImageOperandsMinLodMask)
        m_code.putWord(op.sMinLod);

      if (op.flags & spv::ImageOperandsMakeTexelAvailableMask)
        m_code.putWord(op.makeAvailable);

      if (op.flags & spv::ImageOperandsMakeTexelVisibleMask)
        m_code.putWord(op.makeVisible);
    }
  }
  

  bool SpirvModule::isInterfaceVar(
          spv::StorageClass       sclass) const {
    if (m_version < spvVersion(1, 4)) {
      return sclass == spv::StorageClassInput
          || sclass == spv::StorageClassOutput;
    } else {
      // All global variables need to be declared
      return sclass != spv::StorageClassFunction;
    }
  }

}