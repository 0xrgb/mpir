
;  Copyright 2006  Jason Worth Martin <jason.worth.martin@gmail.com>
;
;  Copyright 2008, 2009 Brian Gladman
;
;  This file is part of the MPIR Library.
;
;  The MPIR Library is free software; you can redistribute it and/or
;  modify it under the terms of the GNU Lesser General Public License as
;  published by the Free Software Foundation; either version 2.1 of the
;  License, or (at your option) any later version.
;
;  The MPIR Library is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
;  Lesser General Public License for more details.
;
;  You should have received a copy of the GNU Lesser General Public
;  License along with the MPIR Library; see the file COPYING.LIB.  If
;  not, write to the Free Software Foundation, Inc., 51 Franklin Street,
;  Fifth Floor, Boston, MA 02110-1301, USA.
;
; CREDITS
;
; The code used here is derived from that provided by ct35z at:
;
;    http://www.geocities.jp/ct35z/gmp-core2-en.html
;
; This code is based largely on Pierrick Gaudry's excellent assembly
; support for the AMD64 architecture.  (Note that Intel64 and AMD64,
; while using the same instruction set, have very different
; microarchitectures.  So, this code performs very poorly on AMD64
; machines even though it is near-optimal on Intel64.)
;
; Roger Golliver works for Intel and provided insightful improvements
; particularly in using the "lea" instruction to perform additions
; and register-to-register moves.
;
; Jason Worth Martin's excellent assembly support for the Intel64
; architecture has been used where appropriate.
;
; Eric Bainville has a brilliant exposition of optimizing arithmetic for
; AMD64 (http://www.bealto.it).  I adapted many of the ideas he
; describes to Intel64.
;
; Agner Fog is a demigod in the x86 world.  If you are reading assembly
; code files and you haven't heard of Agner Fog, then take a minute to
; look over his software optimization manuals (http://www.agner.org/).
; They are superb.
;
; Adapted for use with VC++ and YASM using a special mode in which NASM
; preprocessing is used with AT&T assembler syntax. I am very grateful
; for the support that Peter Johnson (one of the authors of YASM) has
; provided in getting this special YASM mode working.  Without his
; support this port would have been a great deal more difficult.
;
; The principle issues that I have had to address is the difference
; between GCC and MSVC in their register saving and parameter passing
; conventions.  Registers that have to be preserved across function
; calls are:
;
; GCC:             rbx, rbp, r12..r15
; MSVC:  rsi, rdi, rbx, rbp, r12..r15 xmm6..xmm15
;
; Parameter passing conventions for non floating point parameters:
;
;   function(   GCC     MSVC
;       p1,     rdi      rcx
;       p2,     rsi      rdx
;       p3,     rdx       r8
;       p4,     rcx       r9
;       p5,      r8 [rsp+40]
;       p6,      r9 [rsp+48]
;
; Care must be taken with 32-bit values in 64-bit register or on the
; stack because the upper 32-bits of such parameters are undefined.
;
;       Brian Gladman
;

; Intel64 mpn_addmul_1 -- Multiply a limb vector with a limb and
; add the result to a second limb vector.
;
;  Calling interface:
;
;  mp_limb_t __gmpn_<op>mul_1(          <op> = add or sub
;     mp_ptr dst,               rcx
;     mp_srcptr src,            rdx
;     mp_size_t size,            r8
;     mp_limb_t mult             r9
;  )
;
;  mp_limb_t __gmpn_<op>mul_1c(
;     mp_ptr dst,               rcx
;     mp_srcptr src,            rdx
;     mp_size_t size,            r8
;     mp_limb_t mult,            r9
;     mp_limb_t carry    [rsp+0x28]
;  )
;
; Calculate src[size] multiplied by mult[1] and add to /subtract from dst[size] and
; return the carry or borrow from the top of the result
;
; BPL is bytes per limb, which is 8 in the 64-bit code here

%define BPL                 8
%define UNROLL_EXPONENT     4
%define UNROLL_SIZE         (1 << UNROLL_EXPONENT)
%define UNROLL_MASK         (UNROLL_SIZE - 1)
%define ADDR(p,i,d)         (d*BPL)(p, i, BPL)

; Register  Usage
; --------  -----
; rax    low word from mul
; rbx
; rcx    s2limb
; rdx    high word from mul
; rsi    s1p
; rdi    rp
; rbp    Base Pointer
; rsp    Stack Pointer
; r8     A_x
; r9     A_y
; r10    A_z
; r11    B_x
; r12    B_y
; r13    B_z
; r14    temp
; r15    index

%include "..\x86_64_asm.inc"

%define reg_save_list       rsi, rdi, r12, r13, r14, r15

%define s2limb  rcx
%define s1p     rsi
%define rp      rdi
%define a_x      r8
%define a_y      r9
%define a_z     r10
%define b_x     r11
%define b_y     r12
%define b_z     r13
%define temp    r14
%define index   r15

%macro func 1
    global  __gmpn_%1mul_1
    global  __gmpn_%1mul_1c

%ifdef DLL
    export  __gmpn_%1mul_1
    export  __gmpn_%1mul_1c
%endif

__gmpn_%1mul_1:
    xor     a_z, a_z
    jmp     %%1

__gmpn_%1mul_1c:
    mov     a_z, [rsp+0x28]

prologue    %%1, 0, reg_save_list
    mov     rdi, rcx
    mov     rsi, rdx
    xor     rdx, rdx
    mov     edx, r8d
    mov     rcx, r9

    lea     s1p, [s1p+rdx*8]
    lea     rp, [rp+rdx*8]
    xor     index, index
    sub     index, rdx
    cmp     rdx, 4
    jge     %%7
    lea     rax, [rel %%2]
    add     rax, [rax+rdx*8]
    jmp     rax

    align   8
%%2:
    dq      %%3 - %%2
    dq      %%4 - %%2
    dq      %%5 - %%2
    dq      %%6 - %%2
%%3:
    mov     rax, a_z
    jmp     %%13
%%4:
    mov     rax, [s1p+index*8]
    mul     s2limb
    add     rax, a_z
    adc     rdx, 0
    mov     a_z, [rp+index*8]
    %1      a_z, rax
    mov     rax, 0
    mov     [rp+index*8], a_z
    adc     rax, rdx
    jmp     %%13
%%5:
    mov     rax, [s1p+index*8]
    mul     s2limb
    add     rax, a_z
    adc     rdx, 0
    mov     a_z, [rp+index*8]
    mov     a_x, rax
    mov     a_y, rdx

    mov     rax, [s1p+index*8+8]
    mul     s2limb
    mov     b_z, [rp+index*8+8]
    %1      a_z, a_x
    adc     rax, a_y
    mov     [rp+index*8], a_z
    adc     rdx, 0
    %1      b_z, rax
    mov     rax, 0
    mov     [rp+index*8+8], b_z
    adc     rax, rdx
    jmp     %%13
%%6:
    mov     rax, [s1p+index*8]
    mul     s2limb
    add     rax, a_z
    adc     rdx, 0
    mov     a_z, [rp+index*8]
    mov     a_x, rax
    mov     a_y, rdx
    mov     rax, [s1p+index*8+8]
    mul     s2limb
    mov     b_z, [rp+index*8+8]
    mov     b_x, rax
    mov     b_y, rdx
    mov     rax, [s1p+index*8+16]
    mul     s2limb
    %1      a_z, a_x
    adc     b_x, a_y
    mov     [rp+index*8], a_z
    mov     a_z, [rp+index*8+16]
    adc     b_y, 0
    %1      b_z, b_x
    adc     rax, b_y
    mov     [rp+index*8+8], b_z
    adc     rdx, 0
    %1      a_z, rax
    mov     rax, 0
    mov     [rp+index*8+16], a_z
    adc     rax, rdx
    jmp     %%13
%%7:
    mov     temp, rdx
    test    rdx, 1
    jz      %%8
    mov     rax, [s1p+index*8]
    mul     s2limb
    add     rax, a_z
    adc     rdx, 0
    mov     a_z, [rp+index*8]
    mov     a_x, rax
    mov     a_y, rdx
    mov     rax, [s1p+index*8+8]
    mul     s2limb
    mov     b_z, [rp+index*8+8]
    mov     b_x, rax
    mov     b_y, rdx
    jmp     %%9
%%8:
    mov     rax, [s1p+index*8]
    mul     s2limb
    add     rax, a_z
    adc     rdx, 0
    mov     b_z, [rp+index*8]
    mov     b_x, rax
    mov     b_y, rdx
    mov     rax, [s1p+index*8+8]
    mul     s2limb
    mov     a_z, [rp+index*8+8]
    mov     a_x, rax
    mov     a_y, rdx
%%9:
    sub     temp, 4
    and     temp, UNROLL_MASK
    inc     temp
    mov     rax, (%%11 - %%10) >> UNROLL_EXPONENT
    mul     temp
    lea     rdx, [rel %%11]
    sub     rdx, rax
    mov     rax, [s1p+index*8+16]
    lea     index, [index+temp+3-UNROLL_SIZE]
    jmp     rdx

%macro seq_1 7
    mul     s2limb
    %7      %3, %1
    lea     %1, [rax]
    mov     rax, [byte s1p+index*8+8*%6]
    adc     %4, %2
    mov     [byte rp+index*8+8*(%6-3)], %3
    mov     %3, [byte rp+index*8+8*(%6-1)]
    lea     %2, [rdx]
    adc     %5, 0
%endmacro

   align 16
%%10:
%assign i 0
%rep    16
    %if (i & 1)
        seq_1   b_x, b_y, b_z, a_x, a_y, i, %1
    %else
        seq_1   a_x, a_y, a_z, b_x, b_y, i, %1
    %endif
%assign i i + 1
%endrep
%%11:
    add     index, UNROLL_SIZE
    jnz     %%10
%%12:
    mul     s2limb
    %1      a_z, a_x
    mov     [rp+index*8-24], a_z
    mov     a_z, [rp+index*8-8]
    adc     b_x, a_y
    adc     b_y, 0
    %1      b_z, b_x
    mov     [rp+index*8-16], b_z
    adc     rax, b_y
    adc     rdx, 0
    %1      a_z, rax
    mov     rax, 0
    mov     [rp+index*8-8], a_z
    adc     rax, rdx
%%13:
    epilogue    reg_save_list
%endmacro

    bits    64
    section .text
    align   16

    func    add
    func    sub

    end
