/* mpn_lshift -- Shift left low level for Cray vector processors.

Copyright (C) 2000 Free Software Foundation, Inc.

This file is part of the GNU MP Library.

The GNU MP Library is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at your
option) any later version.

The GNU MP Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with the GNU MP Library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA 02110-1301, USA. */

#include <intrinsics.h>
#include "mpir.h"
#include "gmp-impl.h"

mp_limb_t
mpn_lshift (mp_ptr wp, mp_srcptr up, mp_size_t n, unsigned int cnt)
{
  unsigned sh_1, sh_2;
  mp_size_t i;
  mp_limb_t retval;

  sh_1 = cnt;
  sh_2 = BITS_PER_MP_LIMB - sh_1;
  retval = up[n - 1] >> sh_2;

#pragma _CRI ivdep
  for (i = n - 1; i > 0; i--)
    {
#if 1
      wp[i] = (up[i] << sh_1) | (up[i - 1] >> sh_2);
#else
      /* This is the recommended way, but at least on SV1 it is slower.  */
      wp[i] = _dshiftl (up[i], up[i - 1], sh_1);
#endif
    }

  wp[0] = up[0] << sh_1;
  return retval;
}
