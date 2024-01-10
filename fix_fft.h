#ifndef _FIX_FFT_H
#define _FIX_FFT_H

//
//adopted from 
//  http://web.mit.edu/ghudson/dev/nokrb/third/gst-plugins/gst/spectrum/fix_fft.c
//
//
//three routines: most applications use fix_fftr() to produce fft spectrum output
// fix_fftr(): performs fft with real numbers
// fix_fftr_init(): produces data to test fix_fftr(). Only for debugging, NOT needed for user application.
// fix_fft():  performs fft with real / imaginary numbers
//
//One user-specified parameter:
// FFT_log2N:  specifies number of fft data points, in power of 2. you want to have 128+ data points (N=8). Max of 2048 points (N=11)
//
//One macro:
// FFT_SIZE:   specifies the size of the data buffer (=2^FFT_log2N). 
//
//application example
// int16_t fr[FFT_SIZE];			//define data buffer
// fix_fftr_init(fr, FFT_log2N);	//generate simulation data, user defined
// fix_fftr(fr, FFT_log2N, 0);		//forward fix_fft -> fr[] now contains the spectrum coefficients
//
//***** tips *****
//- FIX_MPY() should be optimized by user on the target system.
//- Using target specific multiplification instructions can gratly speed up the execution
//- Should inline FIX_MPY() if possible
//****************
//
//
/* fix_fft.c - Fixed-point in-place Fast Fourier Transform  */
/*
  All data are fixed-point short integers, in which -32768
  to +32768 represent -1.0 to +1.0 respectively. Integer
  arithmetic is used for speed, instead of the more natural
  floating-point.

  For the forward FFT (time -> freq), fixed scaling is
  performed to prevent arithmetic overflow, and to map a 0dB
  sine/cosine wave (i.e. amplitude = 32767) to two -6dB freq
  coefficients. The return value is always 0.

  For the inverse FFT (freq -> time), fixed scaling cannot be
  done, as two 0dB coefficients would sum to a peak amplitude
  of 64K, overflowing the 32k range of the fixed-point integers.
  Thus, the fix_fft() routine performs variable scaling, and
  returns a value which is the number of bits LEFT by which
  the output must be shifted to get the actual amplitude
  (i.e. if fix_fft() returns 3, each value of fr[] and fi[]
  must be multiplied by 8 (2**3) for proper scaling.
  Clearly, this cannot be done within fixed-point short
  integers. In practice, if the result is to be used as a
  filter, the scale_shift can usually be ignored, as the
  result will be approximately correctly normalized as is.

  Written by:  Tom Roberts  11/8/89
  Made portable:  Malcolm Slaney 12/15/94 malcolm@interval.com
  Enhanced:  Dimitrios P. Bouras  14 Jun 2006 dbouras@ieee.org
*/

#include <stdint.h>

//configuration
#define FFT_log2N   	8							//2^log2FFT: number of real data points. max [1..LOG2_N_WAVE+1]
//end configuration

//global defines
#define FFT_SIZE 		(1ul<<FFT_log2N)			//(2*(1ul<<log2FFT))		//data buffer
//#define log2N     	(log2FFT + 1)
//#define FFT_log2		(FFT_log2N - 1)				//number of real data points = 2^

/*
  Henceforth "short" implies 16-bit word. If this is not
  the case in your architecture, please replace "short"
  with a type definition which *is* a 16-bit word.
*/

/*
  fix_fft() - perform forward/inverse fast Fourier transform.
  fr[n],fi[n] are real and imaginary arrays, both INPUT AND
  RESULT (in-place FFT), with 0 <= n < 2**m; set inverse to
  0 for forward transform (FFT), or 1 for iFFT.
*/
int32_t fix_fft(int16_t fr[], int16_t fi[], int32_t m, int32_t inverse);
/*
  fix_fftr() - forward/inverse FFT on array of real numbers.
  Real FFT/iFFT using half-size complex FFT by distributing
  even/odd samples into real/imaginary arrays respectively.
  In order to save data space (i.e. to avoid two arrays, one
  for real, one for imaginary samples), we proceed in the
  following two steps: a) samples are rearranged in the real
  array so that all even samples are in places 0-(N/2-1) and
  all imaginary samples in places (N/2)-(N-1), and b) fix_fft
  is called with fr and fi pointing to index 0 and index N/2
  respectively in the original array. The above guarantees
  that fix_fft "sees" consecutive real samples as alternating
  real and imaginary samples in the complex array.
*/
int32_t fix_fftr(int16_t f[], int32_t m, int32_t inverse);
//performance figures (cpu cycles) on W801
//FFT_log2N = 6:  8.7K /  64 points, 19-10KB
//FFT_log2N = 7: 19.7K / 128 points, 19-10KB
//FFT_log2N = 8: 43.2K / 256 points, 20-10KB
//FFT_log2N = 9: 91.7K / 512 points, 21-10KB
//FFT_log2N =10: 199.1K/1024 points, 23-10KB
//FFT_log2N =11: 427.4K/2048 points, 27-10KB (max)

//generating data for testing purposes only
//user to configure
int fix_fftr_init(int16_t fr[], int32_t m);

#endif	//fix_fft
