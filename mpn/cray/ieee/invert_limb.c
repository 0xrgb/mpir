/* mpn_invert_limb -- Invert a normalized limb.

Copyright 1991, 2000, 2001 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published
by the Free Software Foundation; either version 2.1 of the License, or (at
your option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA. */

#include "mpir.h"
#include "gmp-impl.h"
#include "longlong.h"

/*
  This is needed to make configure define HAVE_NATIVE_mpn_invert_limb:
  PROLOGUE(mpn_invert_limb)
*/

static const unsigned short int approx_tab[0x100] =
{
  /* 0x400, */
  0x3ff,
         0x3fc, 0x3f8, 0x3f4, 0x3f0, 0x3ec, 0x3e8, 0x3e4,
  0x3e0, 0x3dd, 0x3d9, 0x3d5, 0x3d2, 0x3ce, 0x3ca, 0x3c7,
  0x3c3, 0x3c0, 0x3bc, 0x3b9, 0x3b5, 0x3b2, 0x3ae, 0x3ab,
  0x3a8, 0x3a4, 0x3a1, 0x39e, 0x39b, 0x397, 0x394, 0x391,
  0x38e, 0x38b, 0x387, 0x384, 0x381, 0x37e, 0x37b, 0x378,
  0x375, 0x372, 0x36f, 0x36c, 0x369, 0x366, 0x364, 0x361,
  0x35e, 0x35b, 0x358, 0x355, 0x353, 0x350, 0x34d, 0x34a,
  0x348, 0x345, 0x342, 0x340, 0x33d, 0x33a, 0x338, 0x335,
  0x333, 0x330, 0x32e, 0x32b, 0x329, 0x326, 0x324, 0x321,
  0x31f, 0x31c, 0x31a, 0x317, 0x315, 0x313, 0x310, 0x30e,
  0x30c, 0x309, 0x307, 0x305, 0x303, 0x300, 0x2fe, 0x2fc,
  0x2fa, 0x2f7, 0x2f5, 0x2f3, 0x2f1, 0x2ef, 0x2ec, 0x2ea,
  0x2e8, 0x2e6, 0x2e4, 0x2e2, 0x2e0, 0x2de, 0x2dc, 0x2da,
  0x2d8, 0x2d6, 0x2d4, 0x2d2, 0x2d0, 0x2ce, 0x2cc, 0x2ca,
  0x2c8, 0x2c6, 0x2c4, 0x2c2, 0x2c0, 0x2be, 0x2bc, 0x2bb,
  0x2b9, 0x2b7, 0x2b5, 0x2b3, 0x2b1, 0x2b0, 0x2ae, 0x2ac,
  0x2aa, 0x2a8, 0x2a7, 0x2a5, 0x2a3, 0x2a1, 0x2a0, 0x29e,
  0x29c, 0x29b, 0x299, 0x297, 0x295, 0x294, 0x292, 0x291,
  0x28f, 0x28d, 0x28c, 0x28a, 0x288, 0x287, 0x285, 0x284,
  0x282, 0x280, 0x27f, 0x27d, 0x27c, 0x27a, 0x279, 0x277,
  0x276, 0x274, 0x273, 0x271, 0x270, 0x26e, 0x26d, 0x26b,
  0x26a, 0x268, 0x267, 0x265, 0x264, 0x263, 0x261, 0x260,
  0x25e, 0x25d, 0x25c, 0x25a, 0x259, 0x257, 0x256, 0x255,
  0x253, 0x252, 0x251, 0x24f, 0x24e, 0x24d, 0x24b, 0x24a,
  0x249, 0x247, 0x246, 0x245, 0x243, 0x242, 0x241, 0x240,
  0x23e, 0x23d, 0x23c, 0x23b, 0x239, 0x238, 0x237, 0x236,
  0x234, 0x233, 0x232, 0x231, 0x230, 0x22e, 0x22d, 0x22c,
  0x22b, 0x22a, 0x229, 0x227, 0x226, 0x225, 0x224, 0x223,
  0x222, 0x220, 0x21f, 0x21e, 0x21d, 0x21c, 0x21b, 0x21a,
  0x219, 0x218, 0x216, 0x215, 0x214, 0x213, 0x212, 0x211,
  0x210, 0x20f, 0x20e, 0x20d, 0x20c, 0x20b, 0x20a, 0x209,
  0x208, 0x207, 0x206, 0x205, 0x204, 0x203, 0x202, 0x201,
};

/* iteration: z = 2z-(z**2)d */

mp_limb_t
mpn_invert_limb (mp_limb_t d)
{
  mp_limb_t z, z2l, z2h, tl, th;
  mp_limb_t xh, xl;
  mp_limb_t zh, zl;

#if BITS_PER_MP_LIMB == 32
  z = approx_tab[(d >> 23) - 0x100] << 6;	/* z < 2^16 */

  z2l = z * z;					/* z2l < 2^32 */
  umul_ppmm (th, tl, z2l, d);
  z = (z << 17) - (th << 1);
#endif
#if BITS_PER_MP_LIMB == 64
  z = approx_tab[(d >> 55) - 0x100] << 6;	/* z < 2^16 */

  z2l = z * z;					/* z2l < 2^32 */
  th = z2l * (d >> 32);				/* th < 2^64 */
  z = (z << 17) - (th >> 31);			/* z < 2^32 */

  z2l = z * z;
  umul_ppmm (th, tl, z2l, d);
  z = (z << 33) - (th << 1);
#endif

  umul_ppmm (z2h, z2l, z, z);
  umul_ppmm (th, tl, z2h, d);
  umul_ppmm (xh, xl, z2l, d);
  tl += xh;
  th += tl < xh;
  th = (th << 2) | (tl >> BITS_PER_MP_LIMB - 2);
  tl = tl << 2;
  sub_ddmmss (zh, zl, z << 2, 0, th, tl);

  umul_ppmm (xh, xl, d, zh);
  xh += d;		/* add_ssaaaa (xh, xl, xh, xl, d, 0); */
  if (~xh != 0)
    {
      add_ssaaaa (xh, xl, xh, xl, 0, d);
      zh++;
    }

  add_ssaaaa (xh, xl, xh, xl, 0, d);
  if (xh != 0)
    zh++;

  return zh;
}
