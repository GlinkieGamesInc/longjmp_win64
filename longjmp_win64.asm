TITLE longjmp_win64 - a replacement for setjmp/longjmp on win64 to allow the kind of abuse necessary for coroutines.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; For reference... (C calling convention for win64)
;< Parameter type >								< How passed >
;Floating point									First 4 parameters – XMM0 through XMM3. Others passed on stack.
;Integer										First 4 parameters – RCX, RDX, R8, R9. Others passed on stack.
;Aggregates (8, 16, 32, or 64 bits) and __m64	First 4 parameters – RCX, RDX, R8, R9. Others passed on stack.
;Aggregates (other)								By pointer. First 4 parameters passed as pointers in RCX, RDX, R8, and R9
;__m128											By pointer. First 4 parameters passed as pointers in RCX, RDX, R8, and R9
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; Written by Paul Glinker - Glinkie Games Inc.
;
; Copyright 2017 Glinkie Games Inc.
;
; Licensed under the Apache License, Version 2.0 (the "License");
; you may not use this file except in compliance with the License.
; You may obtain a copy of the License at
;
;     http://www.apache.org/licenses/LICENSE-2.0
;
; Unless required by applicable law or agreed to in writing, software
; distributed under the License is distributed on an "AS IS" BASIS,
; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; See the License for the specific language governing permissions and
; limitations under the License.



IFDEF RAX

.code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; x64

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Internal proc that gets the instruction pointer and puts it into rax.
;	note: this is a proc, so the instruction pointer was pushed onto
;         the stack before calling it. that's how we get it.
get_rip PROC
	mov rax, [rsp]
	ret
get_rip ENDP


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern "C" __int64 setjmp_win64(JMP_BUF_WIN64 buff);
;   addr must have 3 or more elements.
;   addr is populated with: rsp, [rsp], and an internal address within this proc.
;
PUBLIC setjmp_win64
setjmp_win64 PROC
	mov [rcx+008h], rsp	; save stack pointer register

	mov rax, [rsp]		; save what the stack pointer is pointing to. (return address)
	mov [rcx+010h], rax
	
	call get_rip		; get a return address into this proc, store it in rax.
	test rax, rax
	je setjmp_return	; this will jump if we're returning from a long jump

	mov [rcx], rax		; save return address into this proc.

	mov     [rcx+018h], rbx		; save all the registers as necessary
	mov     [rcx+020h], rbp
	mov     [rcx+028h], rdi
	mov     [rcx+030h], rsi
	mov     [rcx+038h], r12
	mov     [rcx+040h], r13
	mov     [rcx+048h], r14
	mov     [rcx+050h], r15
	stmxcsr	dword ptr [rcx+058h]  
	fnstcw  word ptr [rcx+060h]  
	movdqa  xmmword ptr [rcx+070h],xmm6  
	movdqa  xmmword ptr [rcx+080h],xmm7  
	movdqa  xmmword ptr [rcx+090h],xmm8  
	movdqa  xmmword ptr [rcx+0a0h],xmm9  
	movdqa  xmmword ptr [rcx+0b0h],xmm10  
	movdqa  xmmword ptr [rcx+0c0h],xmm11  
	movdqa  xmmword ptr [rcx+0d0h],xmm12  
	movdqa  xmmword ptr [rcx+0e0h],xmm13  
	movdqa  xmmword ptr [rcx+0f0h],xmm14  
	movdqa  xmmword ptr [rcx+100h],xmm15 

	mov rax, 0	; return 0 to indicate we're not returning from a long jump
	ret

setjmp_return:
	mov rax, [rcx+010h]	; restore address that we're returning to, and the registers as well.
	mov [rsp], rax

	mov rax, rdx		; set return value (C expects return value in rax. rdx was set to return value by call to longjmp_win64)

	mov     rbx, [rcx+018h]		; restore all the registers as necessary
	mov     rbp, [rcx+020h]
	mov     rdi, [rcx+028h]
	mov     rsi, [rcx+030h]
	mov     r12, [rcx+038h]
	mov     r13, [rcx+040h]
	mov     r14, [rcx+048h]
	mov     r15, [rcx+050h]
	ldmxcsr	dword ptr [rcx+058h]
	fnclex
	fldcw	word ptr [rcx+060h]  
	movdqa  xmm6, xmmword ptr [rcx+070h]
	movdqa  xmm7, xmmword ptr [rcx+080h]
	movdqa  xmm8, xmmword ptr [rcx+090h]
	movdqa  xmm9, xmmword ptr [rcx+0a0h]
	movdqa  xmm10, xmmword ptr [rcx+0b0h]
	movdqa  xmm11, xmmword ptr [rcx+0c0h]
	movdqa  xmm12, xmmword ptr [rcx+0d0h]
	movdqa  xmm13, xmmword ptr [rcx+0e0h]
	movdqa  xmm14, xmmword ptr [rcx+0f0h]
	movdqa  xmm15, xmmword ptr [rcx+100h]

	ret

setjmp_win64 ENDP


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern "C" __int64 longjmp_win64(JMP_BUF_WIN64 buff, __int64 iReturnValue);
;   addr must have 3 or more elements, and must have been populated by
;   a previous call to getjmp.  When you call this from C++, it never
;	returns.
;
PUBLIC longjmp_win64
longjmp_win64 PROC		; rcx has the address of the JMP_BUF_WIN64.  rdx has the return value.
	mov r8, [rcx]
	mov r9, [rcx+8]
	mov rsp, r9
	mov [rsp], r8
	mov rax, 0
	jmp r8
longjmp_win64 ENDP


ELSE	

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; x86

.386
.MODEL flat, C
.code

get_eip PROC
	mov eax, [esp]
	ret
get_eip ENDP


PUBLIC setjmp_win64
setjmp_win64 PROC

	mov         edx,dword ptr [esp+4]  
	mov			eax,[esp]
	mov			dword ptr [edx+18h],eax			; Save return address
	mov         dword ptr [edx],ebp  
	mov         dword ptr [edx+4],ebx  
	mov         dword ptr [edx+8],edi  
	mov         dword ptr [edx+0Ch],esi  
	mov         dword ptr [edx+10h],esp  
	call		get_eip
	test		eax,eax
	je			setjmp_return					; this will jump if we're returning from a long jump
	mov         dword ptr [edx+14h],eax			; eip
	xor			eax,eax
	ret

setjmp_return:
	mov         ebp, dword ptr [edx]
	mov         ebx, dword ptr [edx+4]
	mov         edi, dword ptr [edx+8]
	mov         esi, dword ptr [edx+0Ch]
	mov         esp, dword ptr [edx+10h]
	mov			edx, dword ptr [edx+18h]
	mov			[esp],edx
	mov			eax, ecx
	ret
	

setjmp_win64 ENDP


PUBLIC longjmp_win64
longjmp_win64 PROC
	mov         edx,dword ptr [esp+4]  
	mov			ecx,dword ptr [esp+8]
	mov         ebx,dword ptr [edx+14h]
	xor			eax,eax
	jmp			ebx
longjmp_win64 ENDP

ENDIF

END

