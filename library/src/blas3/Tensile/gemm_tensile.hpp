/* ************************************************************************
 * Copyright 2016-2021 Advanced Micro Devices, Inc.
 * ************************************************************************ */

#pragma once

#include "check_numerics_matrix.hpp"
#include "handle.hpp"

#ifdef BUILD_WITH_TENSILE
#ifdef USE_TENSILE_HOST

#include "tensile_host.hpp"

#else // USE_TENSILE_HOST

/*******************************************************************************
 * Helper enumeration over different transpose combinations
 ******************************************************************************/
typedef enum
{
    // First letter refers to A, second letter refers to B
    NN,
    NT,
    TN,
    TT,
    NC,
    CN,
    TC,
    CT,
    CC,
} transpose_mode;

constexpr transpose_mode GetTransposeMode(rocblas_operation trans_a, rocblas_operation trans_b)
{
    if(trans_a == rocblas_operation_none)
    {
        if(trans_b == rocblas_operation_none)
            return NN;
        if(trans_b == rocblas_operation_conjugate_transpose)
            return NC;
        return NT;
    }
    else if(trans_a == rocblas_operation_conjugate_transpose)
    {
        if(trans_b == rocblas_operation_none)
            return CN;
        if(trans_b == rocblas_operation_conjugate_transpose)
            return CC;
        return CT;
    }
    else
    {
        if(trans_b == rocblas_operation_none)
            return TN;
        if(trans_b == rocblas_operation_conjugate_transpose)
            return TC;
        return TT;
    }
}

#include "Tensile.h"

/*******************************************************************************
 * Tensile Helper Function call
 ******************************************************************************/
template <typename T>
rocblas_status tensile_helper(const T&          alpha_h,
                              const T&          beta_h,
                              const T*          A,
                              const T*          B,
                              T*                C,
                              rocblas_operation trans_a,
                              rocblas_operation trans_b,
                              rocblas_stride    strideC1,
                              rocblas_stride    strideC2,
                              rocblas_stride    strideA1,
                              rocblas_stride    strideA2,
                              rocblas_stride    strideB1,
                              rocblas_stride    strideB2,
                              rocblas_int       sizeI,
                              rocblas_int       sizeJ,
                              rocblas_int       sizeK,
                              rocblas_int       sizeL,
                              rocblas_handle    handle);

#define TENSILE_ARGS(T)                                                                        \
    (T*)C, (const T*)C, (const T*)A, (const T*)B, *((const T*)&alpha_h), *((const T*)&beta_h), \
        strideC1, strideC2, strideC1, strideC2, strideA1, strideA2, strideB1, strideB2, sizeI, \
        sizeJ, sizeK, sizeL, handle->get_stream(), 0, nullptr, nullptr, nullptr

template <>
inline rocblas_status tensile_helper(const rocblas_half& alpha_h,
                                     const rocblas_half& beta_h,
                                     const rocblas_half* A,
                                     const rocblas_half* B,
                                     rocblas_half*       C,
                                     rocblas_operation   trans_a,
                                     rocblas_operation   trans_b,
                                     rocblas_stride      strideC1,
                                     rocblas_stride      strideC2,
                                     rocblas_stride      strideA1,
                                     rocblas_stride      strideA2,
                                     rocblas_stride      strideB1,
                                     rocblas_stride      strideB2,
                                     rocblas_int         sizeI,
                                     rocblas_int         sizeJ,
                                     rocblas_int         sizeK,
                                     rocblas_int         sizeL,
                                     rocblas_handle      handle)
{
    hipError_t status = hipErrorInvalidValue;

    switch(GetTransposeMode(trans_a, trans_b))
    {
    case NN:
        status = tensile_Cijk_Ailk_Bljk_HB(TENSILE_ARGS(rocblas_half));
        break;
    case NT:
    case NC:
        status = tensile_Cijk_Ailk_Bjlk_HB(TENSILE_ARGS(rocblas_half));
        break;
    case TN:
    case CN:
        status = tensile_Cijk_Alik_Bljk_HB(TENSILE_ARGS(rocblas_half));
        break;
    case TT:
    case TC:
    case CT:
    case CC:
        status = tensile_Cijk_Alik_Bjlk_HB(TENSILE_ARGS(rocblas_half));
        break;
    }

    return get_rocblas_status_for_hip_status(status);
}

template <>
inline rocblas_status tensile_helper(const float&      alpha_h,
                                     const float&      beta_h,
                                     const float*      A,
                                     const float*      B,
                                     float*            C,
                                     rocblas_operation trans_a,
                                     rocblas_operation trans_b,
                                     rocblas_stride    strideC1,
                                     rocblas_stride    strideC2,
                                     rocblas_stride    strideA1,
                                     rocblas_stride    strideA2,
                                     rocblas_stride    strideB1,
                                     rocblas_stride    strideB2,
                                     rocblas_int       sizeI,
                                     rocblas_int       sizeJ,
                                     rocblas_int       sizeK,
                                     rocblas_int       sizeL,
                                     rocblas_handle    handle)
{
    hipError_t status = hipErrorInvalidValue;

    switch(GetTransposeMode(trans_a, trans_b))
    {
    case NN:
        status = tensile_Cijk_Ailk_Bljk_SB(TENSILE_ARGS(float));
        break;
    case NT:
    case NC:
        status = tensile_Cijk_Ailk_Bjlk_SB(TENSILE_ARGS(float));
        break;
    case TN:
    case CN:
        status = tensile_Cijk_Alik_Bljk_SB(TENSILE_ARGS(float));
        break;
    case TT:
    case TC:
    case CT:
    case CC:
        status = tensile_Cijk_Alik_Bjlk_SB(TENSILE_ARGS(float));
        break;
    }

    return get_rocblas_status_for_hip_status(status);
}

template <>
inline rocblas_status tensile_helper(const double&     alpha_h,
                                     const double&     beta_h,
                                     const double*     A,
                                     const double*     B,
                                     double*           C,
                                     rocblas_operation trans_a,
                                     rocblas_operation trans_b,
                                     rocblas_stride    strideC1,
                                     rocblas_stride    strideC2,
                                     rocblas_stride    strideA1,
                                     rocblas_stride    strideA2,
                                     rocblas_stride    strideB1,
                                     rocblas_stride    strideB2,
                                     rocblas_int       sizeI,
                                     rocblas_int       sizeJ,
                                     rocblas_int       sizeK,
                                     rocblas_int       sizeL,
                                     rocblas_handle    handle)
{
    hipError_t status = hipErrorInvalidValue;

    switch(GetTransposeMode(trans_a, trans_b))
    {
    case NN:
        status = tensile_Cijk_Ailk_Bljk_DB(TENSILE_ARGS(double));
        break;
    case NT:
    case NC:
        status = tensile_Cijk_Ailk_Bjlk_DB(TENSILE_ARGS(double));
        break;
    case TN:
    case CN:
        status = tensile_Cijk_Alik_Bljk_DB(TENSILE_ARGS(double));
        break;
    case TT:
    case TC:
    case CT:
    case CC:
        status = tensile_Cijk_Alik_Bjlk_DB(TENSILE_ARGS(double));
        break;
    }

    return get_rocblas_status_for_hip_status(status);
}

template <>
inline rocblas_status tensile_helper(const rocblas_float_complex& alpha_h,
                                     const rocblas_float_complex& beta_h,
                                     const rocblas_float_complex* A,
                                     const rocblas_float_complex* B,
                                     rocblas_float_complex*       C,
                                     rocblas_operation            trans_a,
                                     rocblas_operation            trans_b,
                                     rocblas_stride               strideC1,
                                     rocblas_stride               strideC2,
                                     rocblas_stride               strideA1,
                                     rocblas_stride               strideA2,
                                     rocblas_stride               strideB1,
                                     rocblas_stride               strideB2,
                                     rocblas_int                  sizeI,
                                     rocblas_int                  sizeJ,
                                     rocblas_int                  sizeK,
                                     rocblas_int                  sizeL,
                                     rocblas_handle               handle)
{
    static_assert(std::is_standard_layout<TensileComplexFloat>{},
                  "TensileComplexFloat is not a standard layout type, and thus is "
                  "incompatible with C.");

    static_assert(std::is_trivial<TensileComplexFloat>{},
                  "TensileComplexFloat is not a trivial type, and thus is "
                  "incompatible with C.");

    static_assert(sizeof(rocblas_float_complex) == sizeof(TensileComplexFloat),
                  "TensileComplexFloat does not match rocblas_float_complex");

    hipError_t status = hipErrorInvalidValue;

    switch(GetTransposeMode(trans_a, trans_b))
    {
    case NN:
        status = tensile_Cijk_Ailk_Bljk_CB(TENSILE_ARGS(TensileComplexFloat));
        break;
    case NT:
        status = tensile_Cijk_Ailk_Bjlk_CB(TENSILE_ARGS(TensileComplexFloat));
        break;
    case TN:
        status = tensile_Cijk_Alik_Bljk_CB(TENSILE_ARGS(TensileComplexFloat));
        break;
    case TT:
        status = tensile_Cijk_Alik_Bjlk_CB(TENSILE_ARGS(TensileComplexFloat));
        break;
    case NC:
        status = tensile_Cijk_Ailk_BjlkC_CB(TENSILE_ARGS(TensileComplexFloat));
        break;
    case CN:
        status = tensile_Cijk_AlikC_Bljk_CB(TENSILE_ARGS(TensileComplexFloat));
        break;
    case TC:
        status = tensile_Cijk_Alik_BjlkC_CB(TENSILE_ARGS(TensileComplexFloat));
        break;
    case CT:
        status = tensile_Cijk_AlikC_Bjlk_CB(TENSILE_ARGS(TensileComplexFloat));
        break;
    case CC:
        status = tensile_Cijk_AlikC_BjlkC_CB(TENSILE_ARGS(TensileComplexFloat));
        break;
    }

    return get_rocblas_status_for_hip_status(status);
}

template <>
inline rocblas_status tensile_helper(const rocblas_double_complex& alpha_h,
                                     const rocblas_double_complex& beta_h,
                                     const rocblas_double_complex* A,
                                     const rocblas_double_complex* B,
                                     rocblas_double_complex*       C,
                                     rocblas_operation             trans_a,
                                     rocblas_operation             trans_b,
                                     rocblas_stride                strideC1,
                                     rocblas_stride                strideC2,
                                     rocblas_stride                strideA1,
                                     rocblas_stride                strideA2,
                                     rocblas_stride                strideB1,
                                     rocblas_stride                strideB2,
                                     rocblas_int                   sizeI,
                                     rocblas_int                   sizeJ,
                                     rocblas_int                   sizeK,
                                     rocblas_int                   sizeL,
                                     rocblas_handle                handle)
{
    static_assert(std::is_standard_layout<TensileComplexDouble>{},
                  "TensileComplexDouble is not a standard layout type, and thus is "
                  "incompatible with C.");

    static_assert(std::is_trivial<TensileComplexDouble>{},
                  "TensileComplexDouble is not a trivial type, and thus is "
                  "incompatible with C.");

    static_assert(sizeof(rocblas_double_complex) == sizeof(TensileComplexDouble),
                  "TensileComplexDouble does not match rocblas_double_complex");

    hipError_t status = hipErrorInvalidValue;

    switch(GetTransposeMode(trans_a, trans_b))
    {
    case NN:
        status = tensile_Cijk_Ailk_Bljk_ZB(TENSILE_ARGS(TensileComplexDouble));
        break;
    case NT:
        status = tensile_Cijk_Ailk_Bjlk_ZB(TENSILE_ARGS(TensileComplexDouble));
        break;
    case TN:
        status = tensile_Cijk_Alik_Bljk_ZB(TENSILE_ARGS(TensileComplexDouble));
        break;
    case TT:
        status = tensile_Cijk_Alik_Bjlk_ZB(TENSILE_ARGS(TensileComplexDouble));
        break;
    case NC:
        status = tensile_Cijk_Ailk_BjlkC_ZB(TENSILE_ARGS(TensileComplexDouble));
        break;
    case CN:
        status = tensile_Cijk_AlikC_Bljk_ZB(TENSILE_ARGS(TensileComplexDouble));
        break;
    case TC:
        status = tensile_Cijk_Alik_BjlkC_ZB(TENSILE_ARGS(TensileComplexDouble));
        break;
    case CT:
        status = tensile_Cijk_AlikC_Bjlk_ZB(TENSILE_ARGS(TensileComplexDouble));
        break;
    case CC:
        status = tensile_Cijk_AlikC_BjlkC_ZB(TENSILE_ARGS(TensileComplexDouble));
        break;
    }

    return get_rocblas_status_for_hip_status(status);
}
#undef TENSILE_ARGS

#endif // USE_TENSILE_HOST

/*******************************************************************************
 * Tensile Function call
 ******************************************************************************/
template <typename T>
inline rocblas_status call_tensile(rocblas_handle    handle,
                                   const T*          alpha,
                                   const T*          beta,
                                   const T* const*   batchA,
                                   const T* const*   batchB,
                                   T* const*         batchC,
                                   rocblas_operation trans_a,
                                   rocblas_operation trans_b,
                                   rocblas_int       ld_c,
                                   rocblas_stride    stride_c,
                                   rocblas_int       offset_c,
                                   rocblas_int       ld_a,
                                   rocblas_stride    stride_a,
                                   rocblas_int       offset_a,
                                   rocblas_int       ld_b,
                                   rocblas_stride    stride_b,
                                   rocblas_int       offset_b,
                                   rocblas_int       m,
                                   rocblas_int       n,
                                   rocblas_int       k,
                                   rocblas_int       batch_count = 1)
{

#ifdef USE_TENSILE_HOST
    RocblasContractionProblem<T> problem{handle,   trans_a,
                                         trans_b,  m,
                                         n,        k,
                                         alpha,    nullptr,
                                         batchA,   ld_a,
                                         stride_a, offset_a,
                                         nullptr,  batchB,
                                         ld_b,     stride_b,
                                         offset_b, beta,
                                         nullptr,  batchC,
                                         ld_c,     stride_c,
                                         offset_c, batch_count,
                                         false,    rocblas_gemm_flags_none};

    return runContractionProblem(problem);
#else
    return rocblas_status_not_implemented;
#endif // USE_TENSILE_HOST
}

template <typename T>
inline rocblas_status call_tensile(rocblas_handle    handle,
                                   const T*          alpha,
                                   const T*          beta,
                                   const T*          A,
                                   const T*          B,
                                   T*                C,
                                   rocblas_operation trans_a,
                                   rocblas_operation trans_b,
                                   rocblas_int       ld_c,
                                   rocblas_stride    stride_c,
                                   rocblas_int       offset_c,
                                   rocblas_int       ld_a,
                                   rocblas_stride    stride_a,
                                   rocblas_int       offset_a,
                                   rocblas_int       ld_b,
                                   rocblas_stride    stride_b,
                                   rocblas_int       offset_b,
                                   rocblas_int       m,
                                   rocblas_int       n,
                                   rocblas_int       k,
                                   rocblas_int       batch_count = 1)
{

#ifdef USE_TENSILE_HOST

    RocblasContractionProblem<T> problem{handle,   trans_a,
                                         trans_b,  m,
                                         n,        k,
                                         alpha,    A,
                                         nullptr,  ld_a,
                                         stride_a, offset_a,
                                         B,        nullptr,
                                         ld_b,     stride_b,
                                         offset_b, beta,
                                         C,        nullptr,
                                         ld_c,     stride_c,
                                         offset_c, batch_count,
                                         true,     rocblas_gemm_flags_none};

    return runContractionProblem(problem);

#else // USE_TENSILE_HOST

    return tensile_helper(*alpha,
                          *beta,
                          A,
                          B,
                          C,
                          trans_a,
                          trans_b,
                          ld_c,
                          stride_c,
                          ld_a,
                          stride_a,
                          ld_b,
                          stride_b,
                          m,
                          n,
                          batch_count,
                          k,
                          handle);

#endif // USE_TENSILE_HOST
}

#endif // BUILD_WITH_TENSILE