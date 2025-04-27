#pragma once

#include <unordered_set>

#include "spirv_code_buffer.h"

#if defined(_WIN32)
    #define API __declspec(dllexport)
#else
    #define APIENTRY
    #define API
#endif

namespace dxvk {
  
  struct SpirvPhiLabel {
    uint32_t varId         = 0;
    uint32_t labelId       = 0;
  };
  
  struct SpirvSwitchCaseLabel {
    uint32_t literal       = 0;
    uint32_t labelId       = 0;
  };

  struct SpirvMemoryOperands {
    uint32_t flags         = 0;
    uint32_t alignment     = 0;
    uint32_t makeAvailable = 0;
    uint32_t makeVisible   = 0;
  };
  
  struct SpirvImageOperands {
    uint32_t flags         = 0;
    uint32_t sLodBias      = 0;
    uint32_t sLod          = 0;
    uint32_t sConstOffset  = 0;
    uint32_t sGradX        = 0;
    uint32_t sGradY        = 0;
    uint32_t gOffset       = 0;
    uint32_t gConstOffsets = 0;
    uint32_t sSampleId     = 0;
    uint32_t sMinLod       = 0;
    uint32_t makeAvailable = 0;
    uint32_t makeVisible   = 0;
    bool     sparse        = false;
  };

  constexpr uint32_t spvVersion(uint32_t major, uint32_t minor) {
    return (major << 16) | (minor << 8);
  }
  
  /**
   * \brief SPIR-V module
   * 
   * This class generates a code buffer containing a full
   * SPIR-V shader module. Ensures that the module layout
   * is valid, as defined in the SPIR-V 1.0 specification,
   * section 2.4 "Logical Layout of a Module".
   */
  class API SpirvModule {
    
  public:
    
    explicit SpirvModule(uint32_t version);

    ~SpirvModule();
    
    SpirvCodeBuffer compile() const;

    size_t getInsertionPtr() {
      return m_code.getInsertionPtr();
    }
    
    void beginInsertion(size_t ptr) {
      m_code.beginInsertion(ptr);
    }
    
    void endInsertion() {
      m_code.endInsertion();
    }
    
    uint32_t getBlockId() const {
      return m_blockId;
    }
 
    API uint32_t APIENTRY allocateId();
    
    API bool APIENTRY hasCapability(
            spv::Capability         capability);

    API void APIENTRY enableCapability(
            spv::Capability         capability);
    
    API void APIENTRY enableExtension(
      const char*                   extensionName);
    
    API void APIENTRY addEntryPoint(
            uint32_t                entryPointId,
            spv::ExecutionModel     executionModel,
      const char*                   name);
    
    API void APIENTRY setMemoryModel(
            spv::AddressingModel    addressModel,
            spv::MemoryModel        memoryModel);
    
    API void APIENTRY setExecutionMode(
            uint32_t                entryPointId,
            spv::ExecutionMode      executionMode);
    
    API void APIENTRY setExecutionMode(
            uint32_t                entryPointId,
            spv::ExecutionMode      executionMode,
            uint32_t                argCount,
      const uint32_t*               args);
    
    API void APIENTRY setInvocations(
            uint32_t                entryPointId,
            uint32_t                invocations);
    
    API void APIENTRY setLocalSize(
            uint32_t                entryPointId,
            uint32_t                x,
            uint32_t                y,
            uint32_t                z);
    
    API void APIENTRY setOutputVertices(
            uint32_t                entryPointId,
            uint32_t                vertexCount);
    
    API uint32_t APIENTRY addDebugString(
      const char*                   string);
    
    API void APIENTRY setDebugSource(
            spv::SourceLanguage     language,
            uint32_t                version,
            uint32_t                file,
      const char*                   source);
    
    API void APIENTRY setDebugName(
            uint32_t                expressionId,
      const char*                   debugName);
    
    API void APIENTRY setDebugMemberName(
            uint32_t                structId,
            uint32_t                memberId,
      const char*                   debugName);
    
    API uint32_t APIENTRY constBool(
            bool                    v);
    
    API uint32_t APIENTRY consti32(
            int32_t                 v);
    
    API uint32_t APIENTRY consti64(
            int64_t                 v);
    
    API uint32_t APIENTRY constu32(
            uint32_t                v);
    
    API uint32_t APIENTRY constu64(
            uint64_t                v);
    
    API uint32_t APIENTRY constf32(
            float                   v);
    
    API uint32_t APIENTRY constf64(
            double                  v);
    
    API uint32_t APIENTRY constvec4i32(
            int32_t                 x,
            int32_t                 y,
            int32_t                 z,
            int32_t                 w);

    API uint32_t APIENTRY constvec4b32(
            bool                    x,
            bool                    y,
            bool                    z,
            bool                    w);
    
    API uint32_t APIENTRY constvec4u32(
            uint32_t                x,
            uint32_t                y,
            uint32_t                z,
            uint32_t                w);

    API uint32_t APIENTRY constvec2f32(
            float                   x,
            float                   y);

    API uint32_t APIENTRY constvec3f32(
            float                   x,
            float                   y,
            float                   z);

    API uint32_t APIENTRY constvec4f32(
            float                   x,
            float                   y,
            float                   z,
            float                   w);

    API uint32_t APIENTRY constfReplicant(
            float                   replicant,
            uint32_t                count);

    API uint32_t APIENTRY constbReplicant(
            bool                    replicant,
            uint32_t                count);

    API uint32_t APIENTRY constiReplicant(
            int32_t                 replicant,
            uint32_t                count);

    API uint32_t APIENTRY constuReplicant(
            int32_t                 replicant,
            uint32_t                count);
    
    API uint32_t APIENTRY constComposite(
            uint32_t                typeId,
            uint32_t                constCount,
      const uint32_t*               constIds);
    
    API uint32_t APIENTRY constUndef(
            uint32_t                typeId);
    
    API uint32_t APIENTRY lateConst32(
            uint32_t                typeId);

    API void APIENTRY setLateConst(
            uint32_t                constId,
      const uint32_t*               argIds);

    API uint32_t APIENTRY specConstBool(
            bool                    v);
    
    API uint32_t APIENTRY specConst32(
            uint32_t                typeId,
            uint32_t                value);
    
    API void APIENTRY decorate(
            uint32_t                object,
            spv::Decoration         decoration);
    
    API void APIENTRY decorateArrayStride(
            uint32_t                object,
            uint32_t                stride);
    
    API void APIENTRY decorateBinding(
            uint32_t                object,
            uint32_t                binding);
    
    API void APIENTRY decorateBlock(
            uint32_t                object);
    
    API void APIENTRY decorateBuiltIn(
            uint32_t                object,
            spv::BuiltIn            builtIn);
    
    API void APIENTRY decorateComponent(
            uint32_t                object,
            uint32_t                location);
    
    API void APIENTRY decorateDescriptorSet(
            uint32_t                object,
            uint32_t                set);
    
    API void APIENTRY decorateIndex(
            uint32_t                object,
            uint32_t                index);
    
    API void APIENTRY decorateLocation(
            uint32_t                object,
            uint32_t                location);
    
    API void APIENTRY decorateSpecId(
            uint32_t                object,
            uint32_t                specId);
    
    API void APIENTRY decorateXfb(
            uint32_t                object,
            uint32_t                streamId,
            uint32_t                bufferId,
            uint32_t                offset,
            uint32_t                stride);
    
    API void APIENTRY memberDecorateBuiltIn(
            uint32_t                structId,
            uint32_t                memberId,
            spv::BuiltIn            builtIn);

    API void APIENTRY memberDecorate(
            uint32_t                structId,
            uint32_t                memberId,
            spv::Decoration         decoration);

    API void APIENTRY memberDecorateMatrixStride(
            uint32_t                structId,
            uint32_t                memberId,
            uint32_t                stride);
    
    API void APIENTRY memberDecorateOffset(
            uint32_t                structId,
            uint32_t                memberId,
            uint32_t                offset);
    
    API uint32_t APIENTRY defVoidType();
    
    API uint32_t APIENTRY defBoolType();
    
    API uint32_t APIENTRY defIntType(
            uint32_t                width,
            uint32_t                isSigned);
    
    API uint32_t APIENTRY defFloatType(
            uint32_t                width);
    
    API uint32_t APIENTRY defVectorType(
            uint32_t                elementType,
            uint32_t                elementCount);
    
    API uint32_t APIENTRY defMatrixType(
            uint32_t                columnType,
            uint32_t                columnCount);
    
    API uint32_t APIENTRY defArrayType(
            uint32_t                typeId,
            uint32_t                length);
    
    API uint32_t APIENTRY defArrayTypeUnique(
            uint32_t                typeId,
            uint32_t                length);
    
    API uint32_t APIENTRY defRuntimeArrayType(
            uint32_t                typeId);
    
    API uint32_t APIENTRY defRuntimeArrayTypeUnique(
            uint32_t                typeId);
    
    API uint32_t APIENTRY defFunctionType(
            uint32_t                returnType,
            uint32_t                argCount,
      const uint32_t*               argTypes);
    
    API uint32_t APIENTRY defStructType(
            uint32_t                memberCount,
      const uint32_t*               memberTypes);
    
    API uint32_t APIENTRY defStructTypeUnique(
            uint32_t                memberCount,
      const uint32_t*               memberTypes);
    
    API uint32_t APIENTRY defPointerType(
            uint32_t                variableType,
            spv::StorageClass       storageClass);
    
    API uint32_t APIENTRY defSamplerType();
    
    API uint32_t APIENTRY defImageType(
            uint32_t                sampledType,
            spv::Dim                dimensionality,
            uint32_t                depth,
            uint32_t                arrayed,
            uint32_t                multisample,
            uint32_t                sampled,
            spv::ImageFormat        format);
    
    API uint32_t APIENTRY defSampledImageType(
            uint32_t                imageType);
    
    API uint32_t APIENTRY newVar(
            uint32_t                pointerType,
            spv::StorageClass       storageClass);
    
    API uint32_t APIENTRY newVarInit(
            uint32_t                pointerType,
            spv::StorageClass       storageClass,
            uint32_t                initialValue);
    
    API void APIENTRY functionBegin(
            uint32_t                returnType,
            uint32_t                functionId,
            uint32_t                functionType,
      spv::FunctionControlMask      functionControl);
    
    API uint32_t APIENTRY functionParameter(
            uint32_t                parameterType);
    
    API void APIENTRY functionEnd();

    API uint32_t APIENTRY opAccessChain(
            uint32_t                resultType,
            uint32_t                composite,
            uint32_t                indexCount,
      const uint32_t*               indexArray);
    
    API uint32_t APIENTRY opArrayLength(
            uint32_t                resultType,
            uint32_t                structure,
            uint32_t                memberId);
    
    API uint32_t APIENTRY opAny(
            uint32_t                resultType,
            uint32_t                vector);
    
    API uint32_t APIENTRY opAll(
            uint32_t                resultType,
            uint32_t                vector);
    
    API uint32_t APIENTRY opAtomicLoad(
            uint32_t                resultType,
            uint32_t                pointer,
            uint32_t                scope,
            uint32_t                semantics);
            
    API void APIENTRY opAtomicStore(
            uint32_t                pointer,
            uint32_t                scope,
            uint32_t                semantics,
            uint32_t                value);
            
    API uint32_t APIENTRY opAtomicExchange(
            uint32_t                resultType,
            uint32_t                pointer,
            uint32_t                scope,
            uint32_t                semantics,
            uint32_t                value);
            
    API uint32_t APIENTRY opAtomicCompareExchange(
            uint32_t                resultType,
            uint32_t                pointer,
            uint32_t                scope,
            uint32_t                equal,
            uint32_t                unequal,
            uint32_t                value,
            uint32_t                comparator);
            
    API uint32_t APIENTRY opAtomicIIncrement(
            uint32_t                resultType,
            uint32_t                pointer,
            uint32_t                scope,
            uint32_t                semantics);
            
    API uint32_t APIENTRY opAtomicIDecrement(
            uint32_t                resultType,
            uint32_t                pointer,
            uint32_t                scope,
            uint32_t                semantics);
            
    API uint32_t APIENTRY opAtomicIAdd(
            uint32_t                resultType,
            uint32_t                pointer,
            uint32_t                scope,
            uint32_t                semantics,
            uint32_t                value);
    
    API uint32_t APIENTRY opAtomicISub(
            uint32_t                resultType,
            uint32_t                pointer,
            uint32_t                scope,
            uint32_t                semantics,
            uint32_t                value);
    
    API uint32_t APIENTRY opAtomicSMin(
            uint32_t                resultType,
            uint32_t                pointer,
            uint32_t                scope,
            uint32_t                semantics,
            uint32_t                value);
    
    API uint32_t APIENTRY opAtomicSMax(
            uint32_t                resultType,
            uint32_t                pointer,
            uint32_t                scope,
            uint32_t                semantics,
            uint32_t                value);
    
    API uint32_t APIENTRY opAtomicUMin(
            uint32_t                resultType,
            uint32_t                pointer,
            uint32_t                scope,
            uint32_t                semantics,
            uint32_t                value);
    
    API uint32_t APIENTRY opAtomicUMax(
            uint32_t                resultType,
            uint32_t                pointer,
            uint32_t                scope,
            uint32_t                semantics,
            uint32_t                value);
    
    API uint32_t APIENTRY opAtomicAnd(
            uint32_t                resultType,
            uint32_t                pointer,
            uint32_t                scope,
            uint32_t                semantics,
            uint32_t                value);
    
    API uint32_t APIENTRY opAtomicOr(
            uint32_t                resultType,
            uint32_t                pointer,
            uint32_t                scope,
            uint32_t                semantics,
            uint32_t                value);
    
    API uint32_t APIENTRY opAtomicXor(
            uint32_t                resultType,
            uint32_t                pointer,
            uint32_t                scope,
            uint32_t                semantics,
            uint32_t                value);
    
    API uint32_t APIENTRY opBitcast(
            uint32_t                resultType,
            uint32_t                operand);
            
    API uint32_t APIENTRY opBitCount(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opBitReverse(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opFindILsb(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opFindUMsb(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opFindSMsb(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opBitFieldInsert(
            uint32_t                resultType,
            uint32_t                base,
            uint32_t                insert,
            uint32_t                offset,
            uint32_t                count);
    
    API uint32_t APIENTRY opBitFieldSExtract(
            uint32_t                resultType,
            uint32_t                base,
            uint32_t                offset,
            uint32_t                count);
    
    API uint32_t APIENTRY opBitFieldUExtract(
            uint32_t                resultType,
            uint32_t                base,
            uint32_t                offset,
            uint32_t                count);
    
    API uint32_t APIENTRY opBitwiseAnd(
            uint32_t                resultType,
            uint32_t                operand1,
            uint32_t                operand2);
    
    API uint32_t APIENTRY opBitwiseOr(
            uint32_t                resultType,
            uint32_t                operand1,
            uint32_t                operand2);
    
    API uint32_t APIENTRY opBitwiseXor(
            uint32_t                resultType,
            uint32_t                operand1,
            uint32_t                operand2);
    
    API uint32_t APIENTRY opNot(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opShiftLeftLogical(
            uint32_t                resultType,
            uint32_t                base,
            uint32_t                shift);
    
    API uint32_t APIENTRY opShiftRightArithmetic(
            uint32_t                resultType,
            uint32_t                base,
            uint32_t                shift);
    
    API uint32_t APIENTRY opShiftRightLogical(
            uint32_t                resultType,
            uint32_t                base,
            uint32_t                shift);
    
    API uint32_t APIENTRY opConvertFtoS(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opConvertFtoU(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opConvertStoF(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opConvertUtoF(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opCompositeConstruct(
            uint32_t                resultType,
            uint32_t                valueCount,
      const uint32_t*               valueArray);
    
    API uint32_t APIENTRY opCompositeExtract(
            uint32_t                resultType,
            uint32_t                composite,
            uint32_t                indexCount,
      const uint32_t*               indexArray);
    
    API uint32_t APIENTRY opCompositeInsert(
            uint32_t                resultType,
            uint32_t                object,
            uint32_t                composite,
            uint32_t                indexCount,
      const uint32_t*               indexArray);
    
    API uint32_t APIENTRY opDpdx(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opDpdy(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opDpdxCoarse(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opDpdyCoarse(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opDpdxFine(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opDpdyFine(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opVectorExtractDynamic(
            uint32_t                resultType,
            uint32_t                vector,
            uint32_t                index);
    
    API uint32_t APIENTRY opVectorShuffle(
            uint32_t                resultType,
            uint32_t                vectorLeft,
            uint32_t                vectorRight,
            uint32_t                indexCount,
      const uint32_t*               indexArray);
    
    API uint32_t APIENTRY opSNegate(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opFNegate(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opSAbs(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opFAbs(
            uint32_t                resultType,
            uint32_t                operand);

    API uint32_t APIENTRY opFSign(
            uint32_t                resultType,
            uint32_t                operand);

    API uint32_t APIENTRY opFMix(
            uint32_t                resultType,
            uint32_t                x,
            uint32_t                y,
            uint32_t                a);

    API uint32_t APIENTRY opCross(
            uint32_t                resultType,
            uint32_t                x,
            uint32_t                y);
    
    API uint32_t APIENTRY opIAdd(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opISub(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opFAdd(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opFSub(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opSDiv(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opUDiv(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opSRem(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opUMod(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opFDiv(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opIMul(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opFMul(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);

    API uint32_t APIENTRY opVectorTimesScalar(
            uint32_t                resultType,
            uint32_t                vector,
            uint32_t                scalar);

    API uint32_t APIENTRY opMatrixTimesMatrix(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);

    API uint32_t APIENTRY opMatrixTimesVector(
            uint32_t                resultType,
            uint32_t                matrix,
            uint32_t                vector);

    API uint32_t APIENTRY opVectorTimesMatrix(
            uint32_t                resultType,
            uint32_t                vector,
            uint32_t                matrix);

    API uint32_t APIENTRY opTranspose(
            uint32_t                resultType,
            uint32_t                matrix);

    API uint32_t APIENTRY opInverse(
            uint32_t                resultType,
            uint32_t                matrix);
    
    API uint32_t APIENTRY opFFma(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b,
            uint32_t                c);
    
    API uint32_t APIENTRY opFMax(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opFMin(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opNMax(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opNMin(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opSMax(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opSMin(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opUMax(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opUMin(
            uint32_t                resultType,
            uint32_t                a,
            uint32_t                b);
    
    API uint32_t APIENTRY opFClamp(
            uint32_t                resultType,
            uint32_t                x,
            uint32_t                minVal,
            uint32_t                maxVal);
    
    API uint32_t APIENTRY opNClamp(
            uint32_t                resultType,
            uint32_t                x,
            uint32_t                minVal,
            uint32_t                maxVal);
    
    API uint32_t APIENTRY opIEqual(
            uint32_t                resultType,
            uint32_t                vector1,
            uint32_t                vector2);
    
    API uint32_t APIENTRY opINotEqual(
            uint32_t                resultType,
            uint32_t                vector1,
            uint32_t                vector2);
    
    API uint32_t APIENTRY opSLessThan(
            uint32_t                resultType,
            uint32_t                vector1,
            uint32_t                vector2);
    
    API uint32_t APIENTRY opSLessThanEqual(
            uint32_t                resultType,
            uint32_t                vector1,
            uint32_t                vector2);
    
    API uint32_t APIENTRY opSGreaterThan(
            uint32_t                resultType,
            uint32_t                vector1,
            uint32_t                vector2);
    
    API uint32_t APIENTRY opSGreaterThanEqual(
            uint32_t                resultType,
            uint32_t                vector1,
            uint32_t                vector2);
    
    API uint32_t APIENTRY opULessThan(
            uint32_t                resultType,
            uint32_t                vector1,
            uint32_t                vector2);
    
    API uint32_t APIENTRY opULessThanEqual(
            uint32_t                resultType,
            uint32_t                vector1,
            uint32_t                vector2);
    
    API uint32_t APIENTRY opUGreaterThan(
            uint32_t                resultType,
            uint32_t                vector1,
            uint32_t                vector2);
    
    API uint32_t APIENTRY opUGreaterThanEqual(
            uint32_t                resultType,
            uint32_t                vector1,
            uint32_t                vector2);
    
    API uint32_t APIENTRY opFOrdEqual(
            uint32_t                resultType,
            uint32_t                vector1,
            uint32_t                vector2);
    
    API uint32_t APIENTRY opFOrdNotEqual(
            uint32_t                resultType,
            uint32_t                vector1,
            uint32_t                vector2);
    
    API uint32_t APIENTRY opFOrdLessThan(
            uint32_t                resultType,
            uint32_t                vector1,
            uint32_t                vector2);
    
    API uint32_t APIENTRY opFOrdLessThanEqual(
            uint32_t                resultType,
            uint32_t                vector1,
            uint32_t                vector2);
    
    API uint32_t APIENTRY opFOrdGreaterThan(
            uint32_t                resultType,
            uint32_t                vector1,
            uint32_t                vector2);
    
    API uint32_t APIENTRY opFOrdGreaterThanEqual(
            uint32_t                resultType,
            uint32_t                vector1,
            uint32_t                vector2);
    
    API uint32_t APIENTRY opLogicalEqual(
            uint32_t                resultType,
            uint32_t                operand1,
            uint32_t                operand2);
    
    API uint32_t APIENTRY opLogicalNotEqual(
            uint32_t                resultType,
            uint32_t                operand1,
            uint32_t                operand2);
    
    API uint32_t APIENTRY opLogicalAnd(
            uint32_t                resultType,
            uint32_t                operand1,
            uint32_t                operand2);
    
    API uint32_t APIENTRY opLogicalOr(
            uint32_t                resultType,
            uint32_t                operand1,
            uint32_t                operand2);
    
    API uint32_t APIENTRY opLogicalNot(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opDot(
            uint32_t                resultType,
            uint32_t                vector1,
            uint32_t                vector2);
    
    API uint32_t APIENTRY opSin(
            uint32_t                resultType,
            uint32_t                vector);
    
    API uint32_t APIENTRY opCos(
            uint32_t                resultType,
            uint32_t                vector);
    
    API uint32_t APIENTRY opSqrt(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opInverseSqrt(
            uint32_t                resultType,
            uint32_t                operand);

    API uint32_t APIENTRY opNormalize(
            uint32_t                resultType,
            uint32_t                operand);

    API uint32_t APIENTRY opReflect(
            uint32_t                resultType,
            uint32_t                incident,
            uint32_t                normal);

    API uint32_t APIENTRY opLength(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opExp2(
            uint32_t                resultType,
            uint32_t                operand);

    API uint32_t APIENTRY opExp(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opLog2(
            uint32_t                resultType,
            uint32_t                operand);

    API uint32_t APIENTRY opPow(
            uint32_t                resultType,
            uint32_t                base,
            uint32_t                exponent);
    
    API uint32_t APIENTRY opFract(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opCeil(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opFloor(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opRound(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opRoundEven(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opTrunc(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opFConvert(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opPackHalf2x16(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opUnpackHalf2x16(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opSelect(
            uint32_t                resultType,
            uint32_t                condition,
            uint32_t                operand1,
            uint32_t                operand2);

    API uint32_t APIENTRY opIsNan(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opIsInf(
            uint32_t                resultType,
            uint32_t                operand);
    
    API uint32_t APIENTRY opFunctionCall(
            uint32_t                resultType,
            uint32_t                functionId,
            uint32_t                argCount,
      const uint32_t*               argIds);
    
    API void APIENTRY opLabel(
            uint32_t                labelId);
    
    API uint32_t APIENTRY opLoad(
            uint32_t                typeId,
            uint32_t                pointerId);

    API uint32_t APIENTRY opLoad(
            uint32_t                typeId,
            uint32_t                pointerId,
      const SpirvMemoryOperands&    operands);

    API void APIENTRY opStore(
            uint32_t                pointerId,
            uint32_t                valueId);

    API void APIENTRY opStore(
            uint32_t                pointerId,
            uint32_t                valueId,
      const SpirvMemoryOperands&    operands);

    API uint32_t APIENTRY opInterpolateAtCentroid(
            uint32_t                resultType,
            uint32_t                interpolant);
    
    API uint32_t APIENTRY opInterpolateAtSample(
            uint32_t                resultType,
            uint32_t                interpolant,
            uint32_t                sample);
    
    API uint32_t APIENTRY opInterpolateAtOffset(
            uint32_t                resultType,
            uint32_t                interpolant,
            uint32_t                offset);

    API uint32_t APIENTRY opImage(
            uint32_t                resultType,
            uint32_t                sampledImage);
    
    API uint32_t APIENTRY opImageRead(
            uint32_t                resultType,
            uint32_t                image,
            uint32_t                coordinates,
      const SpirvImageOperands&     operands);
    
    API void APIENTRY opImageWrite(
            uint32_t                image,
            uint32_t                coordinates,
            uint32_t                texel,
      const SpirvImageOperands&     operands);

    API uint32_t APIENTRY opImageSparseTexelsResident(
            uint32_t                resultType,
            uint32_t                residentCode);

    API uint32_t APIENTRY opImageTexelPointer(
            uint32_t                resultType,
            uint32_t                image,
            uint32_t                coordinates,
            uint32_t                sample);
    
    API uint32_t APIENTRY opSampledImage(
            uint32_t                resultType,
            uint32_t                image,
            uint32_t                sampler);
    
    API uint32_t APIENTRY opImageQuerySizeLod(
            uint32_t                resultType,
            uint32_t                image,
            uint32_t                lod);
    
    API uint32_t APIENTRY opImageQuerySize(
            uint32_t                resultType,
            uint32_t                image);
    
    API uint32_t APIENTRY opImageQueryLevels(
            uint32_t                resultType,
            uint32_t                image);
    
    API uint32_t APIENTRY opImageQueryLod(
            uint32_t                resultType,
            uint32_t                sampledImage,
            uint32_t                coordinates);
    
    API uint32_t APIENTRY opImageQuerySamples(
            uint32_t                resultType,
            uint32_t                image);
    
    API uint32_t APIENTRY opImageFetch(
            uint32_t                resultType,
            uint32_t                image,
            uint32_t                coordinates,
      const SpirvImageOperands&     operands);
    
    API uint32_t APIENTRY opImageGather(
            uint32_t                resultType,
            uint32_t                sampledImage,
            uint32_t                coordinates,
            uint32_t                component,
      const SpirvImageOperands&     operands);
    
    API uint32_t APIENTRY opImageDrefGather(
            uint32_t                resultType,
            uint32_t                sampledImage,
            uint32_t                coordinates,
            uint32_t                reference,
      const SpirvImageOperands&     operands);
    
    API uint32_t APIENTRY opImageSampleImplicitLod(
            uint32_t                resultType,
            uint32_t                sampledImage,
            uint32_t                coordinates,
      const SpirvImageOperands&     operands);
    
    API uint32_t APIENTRY opImageSampleExplicitLod(
            uint32_t                resultType,
            uint32_t                sampledImage,
            uint32_t                coordinates,
      const SpirvImageOperands&     operands);

    API uint32_t APIENTRY opImageSampleProjImplicitLod(
            uint32_t                resultType,
            uint32_t                sampledImage,
            uint32_t                coordinates,
      const SpirvImageOperands&     operands);

    API uint32_t APIENTRY opImageSampleProjExplicitLod(
            uint32_t                resultType,
            uint32_t                sampledImage,
            uint32_t                coordinates,
      const SpirvImageOperands&     operands);
    
    API uint32_t APIENTRY opImageSampleDrefImplicitLod(
            uint32_t                resultType,
            uint32_t                sampledImage,
            uint32_t                coordinates,
            uint32_t                reference,
      const SpirvImageOperands&     operands);
    
    API uint32_t APIENTRY opImageSampleDrefExplicitLod(
            uint32_t                resultType,
            uint32_t                sampledImage,
            uint32_t                coordinates,
            uint32_t                reference,
      const SpirvImageOperands&     operands);

    API uint32_t APIENTRY opImageSampleProjDrefImplicitLod(
            uint32_t                resultType,
            uint32_t                sampledImage,
            uint32_t                coordinates,
            uint32_t                reference,
      const SpirvImageOperands&     operands);

    API uint32_t APIENTRY opImageSampleProjDrefExplicitLod(
            uint32_t                resultType,
            uint32_t                sampledImage,
            uint32_t                coordinates,
            uint32_t                reference,
      const SpirvImageOperands&     operands);

    API uint32_t APIENTRY opGroupNonUniformBallot(
            uint32_t                resultType,
            uint32_t                execution,
            uint32_t                predicate);
    
    API uint32_t APIENTRY opGroupNonUniformBallotBitCount(
            uint32_t                resultType,
            uint32_t                execution,
            uint32_t                operation,
            uint32_t                ballot);
    
    API uint32_t APIENTRY opGroupNonUniformElect(
            uint32_t                resultType,
            uint32_t                execution);
    
    API uint32_t APIENTRY opGroupNonUniformBroadcastFirst(
            uint32_t                resultType,
            uint32_t                execution,
            uint32_t                value);
    
    API void APIENTRY opControlBarrier(
            uint32_t                execution,
            uint32_t                memory,
            uint32_t                semantics);
    
    API void APIENTRY opMemoryBarrier(
            uint32_t                memory,
            uint32_t                semantics);
    
    API void APIENTRY opLoopMerge(
            uint32_t                mergeBlock,
            uint32_t                continueTarget,
            uint32_t                loopControl);
    
    API void APIENTRY opSelectionMerge(
            uint32_t                mergeBlock,
            uint32_t                selectionControl);
    
    API void APIENTRY opBranch(
            uint32_t                label);
    
    API void APIENTRY opBranchConditional(
            uint32_t                condition,
            uint32_t                trueLabel,
            uint32_t                falseLabel);
    
    API void APIENTRY opSwitch(
            uint32_t                selector,
            uint32_t                jumpDefault,
            uint32_t                caseCount,
      const SpirvSwitchCaseLabel*   caseLabels);
    
    API uint32_t APIENTRY opPhi(
            uint32_t                resultType,
            uint32_t                sourceCount,
      const SpirvPhiLabel*          sourceLabels);
    
    API void APIENTRY opReturn();
    
    API void APIENTRY opDemoteToHelperInvocation();
    
    API void APIENTRY opEmitVertex(
            uint32_t                streamId);
    
    API void APIENTRY opEndPrimitive(
            uint32_t                streamId);

    API void APIENTRY opBeginInvocationInterlock();

    API void APIENTRY opEndInvocationInterlock();

  private:
    
    uint32_t m_version;
    uint32_t m_id             = 1;
    uint32_t m_instExtGlsl450 = 0;
    uint32_t m_blockId        = 0;
    
    SpirvCodeBuffer m_capabilities;
    SpirvCodeBuffer m_extensions;
    SpirvCodeBuffer m_instExt;
    SpirvCodeBuffer m_memoryModel;
    SpirvCodeBuffer m_entryPoints;
    SpirvCodeBuffer m_execModeInfo;
    SpirvCodeBuffer m_debugNames;
    SpirvCodeBuffer m_annotations;
    SpirvCodeBuffer m_typeConstDefs;
    SpirvCodeBuffer m_variables;
    SpirvCodeBuffer m_code;

    std::unordered_set<uint32_t> m_lateConsts;

    std::vector<uint32_t> m_interfaceVars;

    API uint32_t APIENTRY defType(
            spv::Op                 op, 
            uint32_t                argCount,
      const uint32_t*               argIds);
    
    API uint32_t APIENTRY defConst(
            spv::Op                 op,
            uint32_t                typeId,
            uint32_t                argCount,
      const uint32_t*               argIds);
    
    API void APIENTRY instImportGlsl450();
    
    API uint32_t APIENTRY getMemoryOperandWordCount(
      const SpirvMemoryOperands&    op) const;

    API void APIENTRY putMemoryOperands(
      const SpirvMemoryOperands&    op);

    API uint32_t APIENTRY getImageOperandWordCount(
      const SpirvImageOperands&     op) const;
    
    API void APIENTRY putImageOperands(
      const SpirvImageOperands&     op);

    API bool APIENTRY isInterfaceVar(
            spv::StorageClass       sclass) const;

  };
  
}