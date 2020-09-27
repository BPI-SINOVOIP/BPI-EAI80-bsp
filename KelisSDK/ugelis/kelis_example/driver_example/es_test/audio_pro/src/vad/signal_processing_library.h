/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */


/*
 * This header file includes all of the fix point signal processing library (SPL) function
 * descriptions and declarations.
 * For specific function calls, see bottom of file.
 */

#ifndef WEBRTC_SPL_SIGNAL_PROCESSING_LIBRARY_H_
#define WEBRTC_SPL_SIGNAL_PROCESSING_LIBRARY_H_

#include <string.h>
#include "vad_core.h"
//#include "webrtc/typedefs.h"
#define WEBRTC_SPL_MUL(a, b) \
    ((int32_t) ((int32_t)(a) * (int32_t)(b)))

#define WEBRTC_SPL_LSHIFT_W32(x, c)     ((x) << (c))



int16_t WebRtcSpl_GetScalingSquare(int16_t *in_vector,
                                   size_t in_vector_length,
                                   size_t times);

int32_t WebRtcSpl_DivW32W16(int32_t num, int16_t den);

//
// WebRtcSpl_AddSatW16(...)
// WebRtcSpl_AddSatW32(...)
//
// Returns the result of a saturated 16-bit, respectively 32-bit, addition of
// the numbers specified by the |var1| and |var2| parameters.
//
// Input:
//      - var1      : Input variable 1
//      - var2      : Input variable 2
//
// Return value     : Added and saturated value
//

//
// WebRtcSpl_SubSatW16(...)
// WebRtcSpl_SubSatW32(...)
//
// Returns the result of a saturated 16-bit, respectively 32-bit, subtraction
// of the numbers specified by the |var1| and |var2| parameters.
//
// Input:
//      - var1      : Input variable 1
//      - var2      : Input variable 2
//
// Returned value   : Subtracted and saturated value
//

//
// WebRtcSpl_GetSizeInBits(...)
//
// Returns the # of bits that are needed at the most to represent the number
// specified by the |value| parameter.
//
// Input:
//      - value     : Input value
//
// Return value     : Number of bits needed to represent |value|
//

//
// WebRtcSpl_NormW32(...)
//
// Norm returns the # of left shifts required to 32-bit normalize the 32-bit
// signed number specified by the |value| parameter.
//
// Input:
//      - value     : Input value
//
// Return value     : Number of bit shifts needed to 32-bit normalize |value|
//

//
// WebRtcSpl_NormW16(...)
//
// Norm returns the # of left shifts required to 16-bit normalize the 16-bit
// signed number specified by the |value| parameter.
//
// Input:
//      - value     : Input value
//
// Return value     : Number of bit shifts needed to 32-bit normalize |value|
//

//
// WebRtcSpl_NormU32(...)
//
// Norm returns the # of left shifts required to 32-bit normalize the unsigned
// 32-bit number specified by the |value| parameter.
//
// Input:
//      - value     : Input value
//
// Return value     : Number of bit shifts needed to 32-bit normalize |value|
//

//
// WebRtcSpl_GetScalingSquare(...)
//
// Returns the # of bits required to scale the samples specified in the
// |in_vector| parameter so that, if the squares of the samples are added the
// # of times specified by the |times| parameter, the 32-bit addition will not
// overflow (result in int32_t).
//
// Input:
//      - in_vector         : Input vector to check scaling on
//      - in_vector_length  : Samples in |in_vector|
//      - times             : Number of additions to be performed
//
// Return value             : Number of right bit shifts needed to avoid
//                            overflow in the addition calculation
//

//
// WebRtcSpl_MemSetW16(...)
//
// Sets all the values in the int16_t vector |vector| of length
// |vector_length| to the specified value |set_value|
//
// Input:
//      - vector        : Pointer to the int16_t vector
//      - set_value     : Value specified
//      - vector_length : Length of vector
//

//
// WebRtcSpl_MemSetW32(...)
//
// Sets all the values in the int32_t vector |vector| of length
// |vector_length| to the specified value |set_value|
//
// Input:
//      - vector        : Pointer to the int16_t vector
//      - set_value     : Value specified
//      - vector_length : Length of vector
//

//
// WebRtcSpl_MemCpyReversedOrder(...)
//
// Copies all the values from the source int16_t vector |in_vector| to a
// destination int16_t vector |out_vector|. It is done in reversed order,
// meaning that the first sample of |in_vector| is copied to the last sample of
// the |out_vector|. The procedure continues until the last sample of
// |in_vector| has been copied to the first sample of |out_vector|. This
// creates a reversed vector. Used in e.g. prediction in iLBC.
//
// Input:
//      - in_vector     : Pointer to the first sample in a int16_t vector
//                        of length |length|
//      - vector_length : Number of elements to copy
//
// Output:
//      - out_vector    : Pointer to the last sample in a int16_t vector
//                        of length |length|
//

//
// WebRtcSpl_CopyFromEndW16(...)
//
// Copies the rightmost |samples| of |in_vector| (of length |in_vector_length|)
// to the vector |out_vector|.
//
// Input:
//      - in_vector         : Input vector
//      - in_vector_length  : Number of samples in |in_vector|
//      - samples           : Number of samples to extract (from right side)
//                            from |in_vector|
//
// Output:
//      - out_vector        : Vector with the requested samples
//

//
// WebRtcSpl_ZerosArrayW16(...)
// WebRtcSpl_ZerosArrayW32(...)
//
// Inserts the value "zero" in all positions of a w16 and a w32 vector
// respectively.
//
// Input:
//      - vector_length : Number of samples in vector
//
// Output:
//      - vector        : Vector containing all zeros
//

//
// WebRtcSpl_VectorBitShiftW16(...)
// WebRtcSpl_VectorBitShiftW32(...)
//
// Bit shifts all the values in a vector up or downwards. Different calls for
// int16_t and int32_t vectors respectively.
//
// Input:
//      - vector_length : Length of vector
//      - in_vector     : Pointer to the vector that should be bit shifted
//      - right_shifts  : Number of right bit shifts (negative value gives left
//                        shifts)
//
// Output:
//      - out_vector    : Pointer to the result vector (can be the same as
//                        |in_vector|)
//

//
// WebRtcSpl_VectorBitShiftW32ToW16(...)
//
// Bit shifts all the values in a int32_t vector up or downwards and
// stores the result as an int16_t vector. The function will saturate the
// signal if needed, before storing in the output vector.
//
// Input:
//      - vector_length : Length of vector
//      - in_vector     : Pointer to the vector that should be bit shifted
//      - right_shifts  : Number of right bit shifts (negative value gives left
//                        shifts)
//
// Output:
//      - out_vector    : Pointer to the result vector (can be the same as
//                        |in_vector|)
//

//
// WebRtcSpl_ScaleVector(...)
//
// Performs the vector operation:
//  out_vector[k] = (gain*in_vector[k])>>right_shifts
//
// Input:
//      - in_vector     : Input vector
//      - gain          : Scaling gain
//      - vector_length : Elements in the |in_vector|
//      - right_shifts  : Number of right bit shifts applied
//
// Output:
//      - out_vector    : Output vector (can be the same as |in_vector|)
//

//
// WebRtcSpl_ScaleVectorWithSat(...)
//
// Performs the vector operation:
//  out_vector[k] = SATURATE( (gain*in_vector[k])>>right_shifts )
//
// Input:
//      - in_vector     : Input vector
//      - gain          : Scaling gain
//      - vector_length : Elements in the |in_vector|
//      - right_shifts  : Number of right bit shifts applied
//
// Output:
//      - out_vector    : Output vector (can be the same as |in_vector|)
//

//
// WebRtcSpl_ScaleAndAddVectors(...)
//
// Performs the vector operation:
//  out_vector[k] = (gain1*in_vector1[k])>>right_shifts1
//                  + (gain2*in_vector2[k])>>right_shifts2
//
// Input:
//      - in_vector1    : Input vector 1
//      - gain1         : Gain to be used for vector 1
//      - right_shifts1 : Right bit shift to be used for vector 1
//      - in_vector2    : Input vector 2
//      - gain2         : Gain to be used for vector 2
//      - right_shifts2 : Right bit shift to be used for vector 2
//      - vector_length : Elements in the input vectors
//
// Output:
//      - out_vector    : Output vector
//

//
// WebRtcSpl_ReverseOrderMultArrayElements(...)
//
// Performs the vector operation:
//  out_vector[n] = (in_vector[n]*window[-n])>>right_shifts
//
// Input:
//      - in_vector     : Input vector
//      - window        : Window vector (should be reversed). The pointer
//                        should be set to the last value in the vector
//      - right_shifts  : Number of right bit shift to be applied after the
//                        multiplication
//      - vector_length : Number of elements in |in_vector|
//
// Output:
//      - out_vector    : Output vector (can be same as |in_vector|)
//

//
// WebRtcSpl_ElementwiseVectorMult(...)
//
// Performs the vector operation:
//  out_vector[n] = (in_vector[n]*window[n])>>right_shifts
//
// Input:
//      - in_vector     : Input vector
//      - window        : Window vector.
//      - right_shifts  : Number of right bit shift to be applied after the
//                        multiplication
//      - vector_length : Number of elements in |in_vector|
//
// Output:
//      - out_vector    : Output vector (can be same as |in_vector|)
//

//
// WebRtcSpl_AddVectorsAndShift(...)
//
// Performs the vector operation:
//  out_vector[k] = (in_vector1[k] + in_vector2[k])>>right_shifts
//
// Input:
//      - in_vector1    : Input vector 1
//      - in_vector2    : Input vector 2
//      - right_shifts  : Number of right bit shift to be applied after the
//                        multiplication
//      - vector_length : Number of elements in |in_vector1| and |in_vector2|
//
// Output:
//      - out_vector    : Output vector (can be same as |in_vector1|)
//

//
// WebRtcSpl_AddAffineVectorToVector(...)
//
// Adds an affine transformed vector to another vector |out_vector|, i.e,
// performs
//  out_vector[k] += (in_vector[k]*gain+add_constant)>>right_shifts
//
// Input:
//      - in_vector     : Input vector
//      - gain          : Gain value, used to multiply the in vector with
//      - add_constant  : Constant value to add (usually 1<<(right_shifts-1),
//                        but others can be used as well
//      - right_shifts  : Number of right bit shifts (0-16)
//      - vector_length : Number of samples in |in_vector| and |out_vector|
//
// Output:
//      - out_vector    : Vector with the output
//

//
// WebRtcSpl_AffineTransformVector(...)
//
// Affine transforms a vector, i.e, performs
//  out_vector[k] = (in_vector[k]*gain+add_constant)>>right_shifts
//
// Input:
//      - in_vector     : Input vector
//      - gain          : Gain value, used to multiply the in vector with
//      - add_constant  : Constant value to add (usually 1<<(right_shifts-1),
//                        but others can be used as well
//      - right_shifts  : Number of right bit shifts (0-16)
//      - vector_length : Number of samples in |in_vector| and |out_vector|
//
// Output:
//      - out_vector    : Vector with the output
//

//
// WebRtcSpl_IncreaseSeed(...)
//
// Increases the seed (and returns the new value)
//
// Input:
//      - seed      : Seed for random calculation
//
// Output:
//      - seed      : Updated seed value
//
// Return value     : The new seed value
//

//
// WebRtcSpl_RandU(...)
//
// Produces a uniformly distributed value in the int16_t range
//
// Input:
//      - seed      : Seed for random calculation
//
// Output:
//      - seed      : Updated seed value
//
// Return value     : Uniformly distributed value in the range
//                    [Word16_MIN...Word16_MAX]
//

//
// WebRtcSpl_RandN(...)
//
// Produces a normal distributed value in the int16_t range
//
// Input:
//      - seed      : Seed for random calculation
//
// Output:
//      - seed      : Updated seed value
//
// Return value     : N(0,1) value in the Q13 domain
//

//
// WebRtcSpl_RandUArray(...)
//
// Produces a uniformly distributed vector with elements in the int16_t
// range
//
// Input:
//      - vector_length : Samples wanted in the vector
//      - seed          : Seed for random calculation
//
// Output:
//      - vector        : Vector with the uniform values
//      - seed          : Updated seed value
//
// Return value         : Number of samples in vector, i.e., |vector_length|
//

//
// WebRtcSpl_Sqrt(...)
//
// Returns the square root of the input value |value|. The precision of this
// function is integer precision, i.e., sqrt(8) gives 2 as answer.
// If |value| is a negative number then 0 is returned.
//
// Algorithm:
//
// A sixth order Taylor Series expansion is used here to compute the square
// root of a number y^0.5 = (1+x)^0.5
// where
// x = y-1
//   = 1+(x/2)-0.5*((x/2)^2+0.5*((x/2)^3-0.625*((x/2)^4+0.875*((x/2)^5)
// 0.5 <= x < 1
//
// Input:
//      - value     : Value to calculate sqrt of
//
// Return value     : Result of the sqrt calculation
//

//
// WebRtcSpl_SqrtFloor(...)
//
// Returns the square root of the input value |value|. The precision of this
// function is rounding down integer precision, i.e., sqrt(8) gives 2 as answer.
// If |value| is a negative number then 0 is returned.
//
// Algorithm:
//
// An iterative 4 cylce/bit routine
//
// Input:
//      - value     : Value to calculate sqrt of
//
// Return value     : Result of the sqrt calculation
//

//
// WebRtcSpl_DivU32U16(...)
//
// Divides a uint32_t |num| by a uint16_t |den|.
//
// If |den|==0, (uint32_t)0xFFFFFFFF is returned.
//
// Input:
//      - num       : Numerator
//      - den       : Denominator
//
// Return value     : Result of the division (as a uint32_t), i.e., the
//                    integer part of num/den.
//

//
// WebRtcSpl_DivW32W16(...)
//
// Divides a int32_t |num| by a int16_t |den|.
//
// If |den|==0, (int32_t)0x7FFFFFFF is returned.
//
// Input:
//      - num       : Numerator
//      - den       : Denominator
//
// Return value     : Result of the division (as a int32_t), i.e., the
//                    integer part of num/den.
//

//
// WebRtcSpl_DivW32W16ResW16(...)
//
// Divides a int32_t |num| by a int16_t |den|, assuming that the
// result is less than 32768, otherwise an unpredictable result will occur.
//
// If |den|==0, (int16_t)0x7FFF is returned.
//
// Input:
//      - num       : Numerator
//      - den       : Denominator
//
// Return value     : Result of the division (as a int16_t), i.e., the
//                    integer part of num/den.
//

//
// WebRtcSpl_DivResultInQ31(...)
//
// Divides a int32_t |num| by a int16_t |den|, assuming that the
// absolute value of the denominator is larger than the numerator, otherwise
// an unpredictable result will occur.
//
// Input:
//      - num       : Numerator
//      - den       : Denominator
//
// Return value     : Result of the division in Q31.
//

//
// WebRtcSpl_DivW32HiLow(...)
//
// Divides a int32_t |num| by a denominator in hi, low format. The
// absolute value of the denominator has to be larger (or equal to) the
// numerator.
//
// Input:
//      - num       : Numerator
//      - den_hi    : High part of denominator
//      - den_low   : Low part of denominator
//
// Return value     : Divided value in Q31
//

//
// WebRtcSpl_Energy(...)
//
// Calculates the energy of a vector
//
// Input:
//      - vector        : Vector which the energy should be calculated on
//      - vector_length : Number of samples in vector
//
// Output:
//      - scale_factor  : Number of left bit shifts needed to get the physical
//                        energy value, i.e, to get the Q0 value
//
// Return value         : Energy value in Q(-|scale_factor|)
//

//
// WebRtcSpl_FilterAR(...)
//
// Performs a 32-bit AR filtering on a vector in Q12
//
// Input:
//  - ar_coef                   : AR-coefficient vector (values in Q12),
//                                ar_coef[0] must be 4096.
//  - ar_coef_length            : Number of coefficients in |ar_coef|.
//  - in_vector                 : Vector to be filtered.
//  - in_vector_length          : Number of samples in |in_vector|.
//  - filter_state              : Current state (higher part) of the filter.
//  - filter_state_length       : Length (in samples) of |filter_state|.
//  - filter_state_low          : Current state (lower part) of the filter.
//  - filter_state_low_length   : Length (in samples) of |filter_state_low|.
//  - out_vector_low_length     : Maximum length (in samples) of
//                                |out_vector_low|.
//
// Output:
//  - filter_state              : Updated state (upper part) vector.
//  - filter_state_low          : Updated state (lower part) vector.
//  - out_vector                : Vector containing the upper part of the
//                                filtered values.
//  - out_vector_low            : Vector containing the lower part of the
//                                filtered values.
//
// Return value                 : Number of samples in the |out_vector|.
//

//
// WebRtcSpl_ComplexIFFT(...)
//
// Complex Inverse FFT
//
// Computes an inverse complex 2^|stages|-point FFT on the input vector, which
// is in bit-reversed order. The original content of the vector is destroyed in
// the process, since the input is overwritten by the output, normal-ordered,
// FFT vector. With X as the input complex vector, y as the output complex
// vector and with M = 2^|stages|, the following is computed:
//
//        M-1
// y(k) = sum[X(i)*[cos(2*pi*i*k/M) + j*sin(2*pi*i*k/M)]]
//        i=0
//
// The implementations are optimized for speed, not for code size. It uses the
// decimation-in-time algorithm with radix-2 butterfly technique.
//
// Input:
//      - vector    : In pointer to complex vector containing 2^|stages|
//                    real elements interleaved with 2^|stages| imaginary
//                    elements.
//                    [ReImReImReIm....]
//                    The elements are in Q(-scale) domain, see more on Return
//                    Value below.
//
//      - stages    : Number of FFT stages. Must be at least 3 and at most 10,
//                    since the table WebRtcSpl_kSinTable1024[] is 1024
//                    elements long.
//
//      - mode      : This parameter gives the user to choose how the FFT
//                    should work.
//                    mode==0: Low-complexity and Low-accuracy mode
//                    mode==1: High-complexity and High-accuracy mode
//
// Output:
//      - vector    : Out pointer to the FFT vector (the same as input).
//
// Return Value     : The scale value that tells the number of left bit shifts
//                    that the elements in the |vector| should be shifted with
//                    in order to get Q0 values, i.e. the physically correct
//                    values. The scale parameter is always 0 or positive,
//                    except if N>1024 (|stages|>10), which returns a scale
//                    value of -1, indicating error.
//

//
// WebRtcSpl_ComplexFFT(...)
//
// Complex FFT
//
// Computes a complex 2^|stages|-point FFT on the input vector, which is in
// bit-reversed order. The original content of the vector is destroyed in
// the process, since the input is overwritten by the output, normal-ordered,
// FFT vector. With x as the input complex vector, Y as the output complex
// vector and with M = 2^|stages|, the following is computed:
//
//              M-1
// Y(k) = 1/M * sum[x(i)*[cos(2*pi*i*k/M) + j*sin(2*pi*i*k/M)]]
//              i=0
//
// The implementations are optimized for speed, not for code size. It uses the
// decimation-in-time algorithm with radix-2 butterfly technique.
//
// This routine prevents overflow by scaling by 2 before each FFT stage. This is
// a fixed scaling, for proper normalization - there will be log2(n) passes, so
// this results in an overall factor of 1/n, distributed to maximize arithmetic
// accuracy.
//
// Input:
//      - vector    : In pointer to complex vector containing 2^|stages| real
//                    elements interleaved with 2^|stages| imaginary elements.
//                    [ReImReImReIm....]
//                    The output is in the Q0 domain.
//
//      - stages    : Number of FFT stages. Must be at least 3 and at most 10,
//                    since the table WebRtcSpl_kSinTable1024[] is 1024
//                    elements long.
//
//      - mode      : This parameter gives the user to choose how the FFT
//                    should work.
//                    mode==0: Low-complexity and Low-accuracy mode
//                    mode==1: High-complexity and High-accuracy mode
//
// Output:
//      - vector    : The output FFT vector is in the Q0 domain.
//
// Return value     : The scale parameter is always 0, except if N>1024,
//                    which returns a scale value of -1, indicating error.
//

//
// WebRtcSpl_AnalysisQMF(...)
//
// Splits a 0-2*F Hz signal into two sub bands: 0-F Hz and F-2*F Hz. The
// current version has F = 8000, therefore, a super-wideband audio signal is
// split to lower-band 0-8 kHz and upper-band 8-16 kHz.
//
// Input:
//      - in_data       : Wide band speech signal, 320 samples (10 ms)
//
// Input & Output:
//      - filter_state1 : Filter state for first All-pass filter
//      - filter_state2 : Filter state for second All-pass filter
//
// Output:
//      - low_band      : Lower-band signal 0-8 kHz band, 160 samples (10 ms)
//      - high_band     : Upper-band signal 8-16 kHz band (flipped in frequency
//                        domain), 160 samples (10 ms)
//

//
// WebRtcSpl_SynthesisQMF(...)
//
// Combines the two sub bands (0-F and F-2*F Hz) into a signal of 0-2*F
// Hz, (current version has F = 8000 Hz). So the filter combines lower-band
// (0-8 kHz) and upper-band (8-16 kHz) channels to obtain super-wideband 0-16
// kHz audio.
//
// Input:
//      - low_band      : The signal with the 0-8 kHz band, 160 samples (10 ms)
//      - high_band     : The signal with the 8-16 kHz band, 160 samples (10 ms)
//
// Input & Output:
//      - filter_state1 : Filter state for first All-pass filter
//      - filter_state2 : Filter state for second All-pass filter
//
// Output:
//      - out_data      : Super-wideband speech signal, 0-16 kHz
//

// int16_t WebRtcSpl_SatW32ToW16(...)
//
// This function saturates a 32-bit word into a 16-bit word.
//
// Input:
//      - value32   : The value of a 32-bit word.
//
// Output:
//      - out16     : the saturated 16-bit word.
//

// int32_t WebRtc_MulAccumW16(...)
//
// This function multiply a 16-bit word by a 16-bit word, and accumulate this
// value to a 32-bit integer.
//
// Input:
//      - a    : The value of the first 16-bit word.
//      - b    : The value of the second 16-bit word.
//      - c    : The value of an 32-bit integer.
//
// Return Value: The value of a * b + c.
//
#endif
