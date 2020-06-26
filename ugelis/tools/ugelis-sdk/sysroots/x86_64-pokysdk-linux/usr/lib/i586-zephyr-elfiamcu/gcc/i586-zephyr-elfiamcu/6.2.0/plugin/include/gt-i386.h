/* Type information for config/i386/i386.c.
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
gt_ggc_mx_hash_table_dllimport_hasher_ (void *x_p)
{
  hash_table<dllimport_hasher> * const x = (hash_table<dllimport_hasher> *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_mx (x);
    }
}

void
gt_ggc_mx (struct dllimport_hasher& x_r ATTRIBUTE_UNUSED)
{
  struct dllimport_hasher * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_ggc_mx_stack_local_entry (void *x_p)
{
  struct stack_local_entry * const x = (struct stack_local_entry *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_m_7rtx_def ((*x).rtl);
      gt_ggc_m_17stack_local_entry ((*x).next);
    }
}

void
gt_pch_nx_hash_table_dllimport_hasher_ (void *x_p)
{
  hash_table<dllimport_hasher> * const x = (hash_table<dllimport_hasher> *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_28hash_table_dllimport_hasher_))
    {
      gt_pch_nx (x);
    }
}

void
gt_pch_nx (struct dllimport_hasher& x_r ATTRIBUTE_UNUSED)
{
  struct dllimport_hasher * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_pch_nx_stack_local_entry (void *x_p)
{
  struct stack_local_entry * const x = (struct stack_local_entry *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_17stack_local_entry))
    {
      gt_pch_n_7rtx_def ((*x).rtl);
      gt_pch_n_17stack_local_entry ((*x).next);
    }
}

void
gt_pch_p_28hash_table_dllimport_hasher_ (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct hash_table<dllimport_hasher> * x ATTRIBUTE_UNUSED = (struct hash_table<dllimport_hasher> *)x_p;
  if ((void *)(x) == this_obj)
    gt_pch_nx (&((*x)), op, cookie);
}

void
gt_pch_nx (struct dllimport_hasher* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
}

void
gt_pch_p_17stack_local_entry (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct stack_local_entry * x ATTRIBUTE_UNUSED = (struct stack_local_entry *)x_p;
  if ((void *)(x) == this_obj)
    op (&((*x).rtl), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).next), cookie);
}

/* GC roots.  */

EXPORTED_CONST struct ggc_root_tab gt_ggc_r_gt_i386_h[] = {
  {
    &vselect_insn,
    1,
    sizeof (vselect_insn),
    &gt_ggc_mx_rtx_def,
    &gt_pch_nx_rtx_def
  },
  {
    &ix86_builtins[0],
    1 * ((int) IX86_BUILTIN_MAX),
    sizeof (ix86_builtins[0]),
    &gt_ggc_mx_tree_node,
    &gt_pch_nx_tree_node
  },
  {
    &ix86_builtin_func_type_tab[0],
    1 * ((int) IX86_BT_LAST_ALIAS + 1),
    sizeof (ix86_builtin_func_type_tab[0]),
    &gt_ggc_mx_tree_node,
    &gt_pch_nx_tree_node
  },
  {
    &ix86_builtin_type_tab[0],
    1 * ((int) IX86_BT_LAST_CPTR + 1),
    sizeof (ix86_builtin_type_tab[0]),
    &gt_ggc_mx_tree_node,
    &gt_pch_nx_tree_node
  },
  {
    &dllimport_map,
    1,
    sizeof (dllimport_map),
    &gt_ggc_mx_hash_table_dllimport_hasher_,
    &gt_pch_nx_hash_table_dllimport_hasher_
  },
  {
    &ix86_tls_module_base_symbol,
    1,
    sizeof (ix86_tls_module_base_symbol),
    &gt_ggc_mx_rtx_def,
    &gt_pch_nx_rtx_def
  },
  {
    &ix86_tls_symbol,
    1,
    sizeof (ix86_tls_symbol),
    &gt_ggc_mx_rtx_def,
    &gt_pch_nx_rtx_def
  },
  {
    &split_stack_fn_large,
    1,
    sizeof (split_stack_fn_large),
    &gt_ggc_mx_rtx_def,
    &gt_pch_nx_rtx_def
  },
  {
    &split_stack_fn,
    1,
    sizeof (split_stack_fn),
    &gt_ggc_mx_rtx_def,
    &gt_pch_nx_rtx_def
  },
  {
    &queued_cfa_restores,
    1,
    sizeof (queued_cfa_restores),
    &gt_ggc_mx_rtx_def,
    &gt_pch_nx_rtx_def
  },
  {
    &ix86_previous_fndecl,
    1,
    sizeof (ix86_previous_fndecl),
    &gt_ggc_mx_tree_node,
    &gt_pch_nx_tree_node
  },
  {
    &ms_va_list_type_node,
    1,
    sizeof (ms_va_list_type_node),
    &gt_ggc_mx_tree_node,
    &gt_pch_nx_tree_node
  },
  {
    &sysv_va_list_type_node,
    1,
    sizeof (sysv_va_list_type_node),
    &gt_ggc_mx_tree_node,
    &gt_pch_nx_tree_node
  },
  LAST_GGC_ROOT_TAB
};

void
gt_clear_caches_gt_i386_h ()
{
  gt_cleare_cache (dllimport_map);
}

