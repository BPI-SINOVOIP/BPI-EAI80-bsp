/* Type information for sdbout.c.
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

/* GC roots.  */

EXPORTED_CONST struct ggc_root_tab gt_ggc_r_gt_sdbout_h[] = {
  {
    &preinit_symbols,
    1,
    sizeof (preinit_symbols),
    &gt_ggc_mx_tree_node,
    &gt_pch_nx_tree_node
  },
  {
    &deferred_global_decls,
    1,
    sizeof (deferred_global_decls),
    &gt_ggc_mx_vec_tree_va_gc_,
    &gt_pch_nx_vec_tree_va_gc_
  },
  {
    &anonymous_types,
    1,
    sizeof (anonymous_types),
    &gt_ggc_mx_tree_node,
    &gt_pch_nx_tree_node
  },
  LAST_GGC_ROOT_TAB
};

EXPORTED_CONST struct ggc_root_tab gt_pch_rs_gt_sdbout_h[] = {
  { &sdbout_initialized, 1, sizeof (sdbout_initialized), NULL, NULL },
  { &unnamed_struct_number, 1, sizeof (unnamed_struct_number), NULL, NULL },
  LAST_GGC_ROOT_TAB
};

