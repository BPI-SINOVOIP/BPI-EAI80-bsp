/* Type information for config/xtensa/xtensa.c.
   Copyright (C) 2004-2016 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

/* This file is machine generated.  Do not edit.  */

void
gt_ggc_mx_machine_function (void *x_p)
{
  struct machine_function * const x = (struct machine_function *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_m_7rtx_def ((*x).vararg_a7_copy);
      gt_ggc_m_7rtx_def ((*x).set_frame_ptr_insn);
    }
}

void
gt_pch_nx_machine_function (void *x_p)
{
  struct machine_function * const x = (struct machine_function *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_16machine_function))
    {
      gt_pch_n_7rtx_def ((*x).vararg_a7_copy);
      gt_pch_n_7rtx_def ((*x).set_frame_ptr_insn);
    }
}

void
gt_pch_p_16machine_function (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct machine_function * x ATTRIBUTE_UNUSED = (struct machine_function *)x_p;
  if ((void *)(x) == this_obj)
    op (&((*x).vararg_a7_copy), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).set_frame_ptr_insn), cookie);
}

/* GC roots.  */

EXPORTED_CONST struct ggc_root_tab gt_ggc_r_gt_xtensa_h[] = {
  {
    &xtensa_tls_module_base_symbol,
    1,
    sizeof (xtensa_tls_module_base_symbol),
    &gt_ggc_mx_rtx_def,
    &gt_pch_nx_rtx_def
  },
  LAST_GGC_ROOT_TAB
};

