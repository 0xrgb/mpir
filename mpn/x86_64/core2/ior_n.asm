dnl  core2 mpn_ior_n

dnl  Copyright 2009 Jason Moxham

dnl  This file is part of the MPIR Library.

dnl  The MPIR Library is free software; you can redistribute it and/or modify
dnl  it under the terms of the GNU Lesser General Public License as published
dnl  by the Free Software Foundation; either version 2.1 of the License, or (at
dnl  your option) any later version.

dnl  The MPIR Library is distributed in the hope that it will be useful, but
dnl  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
dnl  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
dnl  License for more details.

dnl  You should have received a copy of the GNU Lesser General Public License
dnl  along with the MPIR Library; see the file COPYING.LIB.  If not, write
dnl  to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
dnl  Boston, MA 02110-1301, USA.

include(`../config.m4')

C	(rdi,rcx)=(rsi,rcx)|(rdx,rcx)

ASM_START()
PROLOGUE(mpn_ior_n)
# Version 1.0.4
mov	%rcx,%rax
and	$3,%rax
shr	$2,%rcx
jz	skiploop
ALIGN(8)
loop:
	mov	(%rsi),%r11
	mov	8(%rsi),%r8
	lea	32(%rsi),%rsi
	or	(%rdx),%r11
	or	8(%rdx),%r8
	lea	32(%rdx),%rdx
	mov	%r11,(%rdi)
	mov	%r8,8(%rdi)
	lea	32(%rdi),%rdi
	mov	-16(%rsi),%r9
	mov	-8(%rsi),%r10
	or	-16(%rdx),%r9
	or	-8(%rdx),%r10
	mov	%r9,-16(%rdi)
	#dec	%rcx
	sub	$1,%rcx
	mov	%r10,-8(%rdi)
	jnz	loop
skiploop:
#inc	%rax
#dec	%rax
cmp	$0,%rax
jz	end
mov	(%rsi),%r11
or	(%rdx),%r11
mov	%r11,(%rdi)
#dec	%rax
sub	$1,%rax
jz	end
mov	8(%rsi),%r11
or	8(%rdx),%r11
mov	%r11,8(%rdi)
#dec	%rax
sub	$1,%rax
jz	end
mov	16(%rsi),%r11
or	16(%rdx),%r11
mov	%r11,16(%rdi)
end:
ret
EPILOGUE()
