/* Type information for config/mips/mips.c.
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
gt_ggc_mx_hash_map_nofree_string_hash_rtx_ (void *x_p)
{
  hash_map<nofree_string_hash,rtx> * const x = (hash_map<nofree_string_hash,rtx> *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_mx (x);
    }
}

void
gt_ggc_mx (struct rtx_def *& x)
{
  if (x)
    gt_ggc_mx_rtx_def ((void *) x);
}

void
gt_ggc_mx_hash_map_nofree_string_hash_bool_ (void *x_p)
{
  hash_map<nofree_string_hash,bool> * const x = (hash_map<nofree_string_hash,bool> *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_mx (x);
    }
}

void
gt_pch_nx_hash_map_nofree_string_hash_rtx_ (void *x_p)
{
  hash_map<nofree_string_hash,rtx> * const x = (hash_map<nofree_string_hash,rtx> *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_32hash_map_nofree_string_hash_rtx_))
    {
      gt_pch_nx (x);
    }
}

void
gt_pch_nx (struct rtx_def *& x)
{
  if (x)
    gt_pch_nx_rtx_def ((void *) x);
}

void
gt_pch_nx_hash_map_nofree_string_hash_bool_ (void *x_p)
{
  hash_map<nofree_string_hash,bool> * const x = (hash_map<nofree_string_hash,bool> *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_33hash_map_nofree_string_hash_bool_))
    {
      gt_pch_nx (x);
    }
}

void
gt_pch_p_32hash_map_nofree_string_hash_rtx_ (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct hash_map<nofree_string_hash,rtx> * x ATTRIBUTE_UNUSED = (struct hash_map<nofree_string_hash,rtx> *)x_p;
  if ((void *)(x) == this_obj)
    gt_pch_nx (&((*x)), op, cookie);
}

void
gt_pch_p_33hash_map_nofree_string_hash_bool_ (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct hash_map<nofree_string_hash,bool> * x ATTRIBUTE_UNUSED = (struct hash_map<nofree_string_hash,bool> *)x_p;
  if ((void *)(x) == this_obj)
    gt_pch_nx (&((*x)), op, cookie);
}

/* GC roots.  */

EXPORTED_CONST struct ggc_root_tab gt_ggc_r_gt_mips_h[] = {
  {
    &mips_builtin_decls[0],
    1 * (ARRAY_SIZE (mips_builtins)),
    sizeof (mips_builtin_decls[0]),
    &gt_ggc_mx_tree_node,
    &gt_pch_nx_tree_node
  },
  {
    &mips_gnu_local_gp,
    1,
    sizeof (mips_gnu_local_gp),
    &gt_ggc_mx_rtx_def,
    &gt_pch_nx_rtx_def
  },
  {
    &mips16_local_aliases,
    1,
    sizeof (mips16_local_aliases),
    &gt_ggc_mx_hash_map_nofree_string_hash_rtx_,
    &gt_pch_nx_hash_map_nofree_string_hash_rtx_
  },
  {
    &mips_tls_symbol,
    1,
    sizeof (mips_tls_symbol),
    &gt_ggc_mx_rtx_def,
    &gt_pch_nx_rtx_def
  },
  {
    &mflip_mips16_htab,
    1,
    sizeof (mflip_mips16_htab),
    &gt_ggc_mx_hash_map_nofree_string_hash_bool_,
    &gt_pch_nx_hash_map_nofree_string_hash_bool_
  },
  LAST_GGC_ROOT_TAB
};

EXPORTED_CONST struct ggc_root_tab gt_pch_rs_gt_mips_h[] = {
  { &mips16_flipper, 1, sizeof (mips16_flipper), NULL, NULL },
  { &mips_output_filename_first_time, 1, sizeof (mips_output_filename_first_time), NULL, NULL },
  LAST_GGC_ROOT_TAB
};

