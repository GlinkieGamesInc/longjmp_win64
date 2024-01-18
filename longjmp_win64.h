/**
 * longjmp_win64.h - a replacement for setjmp/longjmp on win64 to allow the kind of abuse necessary for coroutines.
 *
 *
 * Written by Paul Glinker - Glinkie Games Inc.
 *
 * Copyright 2017 Glinkie Games Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 **/

#ifndef longjmp_win64_h
#define longjmp_win64_h

#ifdef __cplusplus
#include <string.h> // for memset function.  not used in C implementation, in which you are responsible for clearing any JMP_BUF_WIN64 yourself if necessary. 
extern "C"
{
#endif // __cplusplus


#ifndef _WIN64

	// This struct has room for all of the registers and return addresses that we require.
	typedef struct _JMP_BUF_WIN64
	{
		unsigned long Ebp;
		unsigned long Ebx;
		unsigned long Edi;
		unsigned long Esi;
		unsigned long Esp;
		unsigned long Eip;
		unsigned long RetAddr;
	
#ifdef __cplusplus
		inline _JMP_BUF_WIN64(void)
		{
			memset(this, 0, sizeof(_JMP_BUF_WIN64));
		}
#endif // __cplusplus

	} JMP_BUF_WIN64[1];


	// Analogous to setjmp.
	int setjmp_win64(JMP_BUF_WIN64 buff);

	// Analogous to longjmp.
	int longjmp_win64(JMP_BUF_WIN64 buff, int iReturnValue);


#else // _WIN64

	// This struct defines the type necessary to store an SE register.
	typedef struct __declspec(align(16)) JMPBUF_FLOAT128
	{
		unsigned __int64 m_uPart[2];
	} JMPBUF_FLOAT128;

	// This struct has room for all of the registers and return addresses that we require.
	typedef struct _JMP_BUF_WIN64
	{									// <offset>	<description>
		unsigned __int64 rip_getjmp;	//   0x000	address that we return to within the getjmp function, when calling longjmp_win64.
		unsigned __int64 rsp_getjmp;	//   0x008	the stack pointer that was saved from within the getjmp function.
		unsigned __int64 rip_caller;	//   0x010	address that we return to when leaving the getjmp function, as part of the longjmp_win64.
										     
		unsigned __int64 rbx;			//   0x018	the non-volatile registers that must be preserved...
		unsigned __int64 rbp;			//   0x020
		unsigned __int64 rdi;			//   0x028
		unsigned __int64 rsi;			//   0x030
		unsigned __int64 r12;			//   0x038
		unsigned __int64 r13;			//   0x040
		unsigned __int64 r14;			//   0x048
		unsigned __int64 r15;			//   0x050
		unsigned __int64 mxcsr;			//   0x058
		unsigned __int64 fpcsr;			//   0x060
										     
		unsigned __int64 uPadding;		//   0x068	fills in the gap that would be implicitly created by the BIGJMP_FLOAT128's need for 16 byte alignment.
										     
		JMPBUF_FLOAT128 xmm6;			//   0x070	the SSE registers that must be preserved...
		JMPBUF_FLOAT128 xmm7;			//   0x080
		JMPBUF_FLOAT128 xmm8;			//   0x090
		JMPBUF_FLOAT128 xmm9;			//   0x0a0
		JMPBUF_FLOAT128 xmm10;			//   0x0b0
		JMPBUF_FLOAT128 xmm11;			//   0x0c0
		JMPBUF_FLOAT128 xmm12;			//   0x0d0
		JMPBUF_FLOAT128 xmm13;			//   0x0e0
		JMPBUF_FLOAT128 xmm14;			//   0x0f0
		JMPBUF_FLOAT128 xmm15;			//   0x100

#ifdef __cplusplus
		inline _JMP_BUF_WIN64(void)
		{
			memset(this, 0, sizeof(_JMP_BUF_WIN64));
		}
#endif // __cplusplus

	} JMP_BUF_WIN64[1];

	// Analogous to setjmp.
	//   - Returns 0 if it populated the BIGJMP_BUFFER.
	//   - Returns 1 if it returned from somewhere else, due to a call to longjmp_win64.
	__int64 setjmp_win64(JMP_BUF_WIN64 buff);

	// Analogous to longjmp.
	//   - Jumps back to the location/state stored in pBuff.
	//   - never actually returns.
	//   - warning: if you call this with an uninitialized buffer, your program will crash.
	__int64 longjmp_win64(JMP_BUF_WIN64 buff, __int64 iReturnValue);

#endif // _WIN64

#ifdef __cplusplus
} // closing the extern "C"
#endif // __cplusplus

#endif // longjmp_win64_h
