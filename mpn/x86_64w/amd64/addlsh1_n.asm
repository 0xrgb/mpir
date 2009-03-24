
;  AMD64 mpn_addlsh1_n
;  Version 1.0.3
;
;  Copyright 2008 Jason Moxham
;
;  Windows Conversion Copyright 2008 Brian Gladman
;
;  This file is part of the MPIR Library.
;  The MPIR Library is free software; you can redistribute it and/or modify
;  it under the terms of the GNU Lesser General Public License as published
;  by the Free Software Foundation; either version 2.1 of the License, or (at
;  your option) any later version.
;  The MPIR Library is distributed in the hope that it will be useful, but
;  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
;  License for more details.
;  You should have received a copy of the GNU Lesser General Public License
;  along with the MPIR Library; see the file COPYING.LIB.  If not, write
;  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
;  Boston, MA 02110-1301, USA.
;
;  Calling interface:
;
;  rcx[r9] = rdx[r9] + 2 * r8[r9]
;
;  void __gmpn_addlsh1_n(
;     mp_ptr rp,          rcx
;     mp_srcptr xp,       rdx
;     mp_srcptr yp,        r8
;     mp_size_t  n,        r9
;  )
;
; This is an SEH frame function

%include "..\yasm_mac.inc"

%define reg_save_list rbx, rsi, rdi

    CPU  Athlon64
    BITS 64

    FRAME_PROC mpn_addlsh1_n, 0, reg_save_list
    movsxd  r9, r9d

    lea     rdx, [rdx+r9*8]
    lea     r8, [r8+r9*8]
    lea     rcx, [rcx+r9*8]
    neg     r9
    xor     rsi, rsi
    xor     rax, rax
    test    r9, 3
    jz      .2

.1: mov     rdi, [r8+r9*8]
    add     rsi, 1
    adc     rdi, rdi
    sbb     rsi, rsi
    add     rax, 1
    adc     rdi, [rdx+r9*8]
    sbb     rax, rax
    mov     [rcx+r9*8], rdi
    inc     r9
    test    r9, 3
    jnz     .1

.2: cmp     r9, 0
    jz      .4

    xalign  16
.3: mov     rdi, [r8+r9*8]
    mov     rbx, [r8+r9*8+8]
    mov     r10, [r8+r9*8+16]
    mov     r11, [r8+r9*8+24]
    add     rsi, 1
    adc     rdi, rdi
    adc     rbx, rbx
    adc     r10, r10
    adc     r11, r11
    sbb     rsi, rsi
    add     rax, 1
    adc     rdi, [rdx+r9*8]
    adc     rbx, [rdx+r9*8+8]
    adc     r10, [rdx+r9*8+16]
    adc     r11, [rdx+r9*8+24]
    sbb     rax, rax
    mov     [rcx+r9*8], rdi
    mov     [rcx+r9*8+8], rbx
    mov     [rcx+r9*8+16], r10
    mov     [rcx+r9*8+24], r11
    add     r9, 4
    jnz     .3

.4: add     rax, rsi
    neg     rax
    END_PROC reg_save_list

    end
