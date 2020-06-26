/* Type information for dwarf2out.c.
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
gt_ggc_mx_hash_table_addr_hasher_ (void *x_p)
{
  hash_table<addr_hasher> * const x = (hash_table<addr_hasher> *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_mx (x);
    }
}

void
gt_ggc_mx (struct addr_hasher& x_r ATTRIBUTE_UNUSED)
{
  struct addr_hasher * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_ggc_mx_vec_die_arg_entry_va_gc_ (void *x_p)
{
  vec<die_arg_entry,va_gc> * const x = (vec<die_arg_entry,va_gc> *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_mx (x);
    }
}

void
gt_ggc_mx (struct die_arg_entry_struct& x_r ATTRIBUTE_UNUSED)
{
  struct die_arg_entry_struct * ATTRIBUTE_UNUSED x = &x_r;
  gt_ggc_m_10die_struct ((*x).die);
  gt_ggc_m_9tree_node ((*x).arg);
}

void
gt_ggc_mx_vec_macinfo_entry_va_gc_ (void *x_p)
{
  vec<macinfo_entry,va_gc> * const x = (vec<macinfo_entry,va_gc> *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_mx (x);
    }
}

void
gt_ggc_mx (struct macinfo_struct& x_r ATTRIBUTE_UNUSED)
{
  struct macinfo_struct * ATTRIBUTE_UNUSED x = &x_r;
  gt_ggc_m_S ((*x).info);
}

void
gt_ggc_mx_vec_pubname_entry_va_gc_ (void *x_p)
{
  vec<pubname_entry,va_gc> * const x = (vec<pubname_entry,va_gc> *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_mx (x);
    }
}

void
gt_ggc_mx (struct pubname_struct& x_r ATTRIBUTE_UNUSED)
{
  struct pubname_struct * ATTRIBUTE_UNUSED x = &x_r;
  gt_ggc_m_10die_struct ((*x).die);
  gt_ggc_m_S ((*x).name);
}

void
gt_ggc_mx_vec_dw_line_info_table__va_gc_ (void *x_p)
{
  vec<dw_line_info_table*,va_gc> * const x = (vec<dw_line_info_table*,va_gc> *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_mx (x);
    }
}

void
gt_ggc_mx (struct dw_line_info_table *& x)
{
  if (x)
    gt_ggc_mx_dw_line_info_table ((void *) x);
}

void
gt_ggc_mx_hash_table_dw_loc_list_hasher_ (void *x_p)
{
  hash_table<dw_loc_list_hasher> * const x = (hash_table<dw_loc_list_hasher> *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_mx (x);
    }
}

void
gt_ggc_mx (struct dw_loc_list_hasher& x_r ATTRIBUTE_UNUSED)
{
  struct dw_loc_list_hasher * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_ggc_mx_cached_dw_loc_list_def (void *x_p)
{
  struct cached_dw_loc_list_def * const x = (struct cached_dw_loc_list_def *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_m_18dw_loc_list_struct ((*x).loc_list);
    }
}

void
gt_ggc_mx (struct cached_dw_loc_list_def& x_r ATTRIBUTE_UNUSED)
{
  struct cached_dw_loc_list_def * ATTRIBUTE_UNUSED x = &x_r;
  gt_ggc_m_18dw_loc_list_struct ((*x).loc_list);
}

void
gt_ggc_mx (struct cached_dw_loc_list_def *& x)
{
  if (x)
    gt_ggc_mx_cached_dw_loc_list_def ((void *) x);
}

void
gt_ggc_mx_hash_table_decl_loc_hasher_ (void *x_p)
{
  hash_table<decl_loc_hasher> * const x = (hash_table<decl_loc_hasher> *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_mx (x);
    }
}

void
gt_ggc_mx (struct decl_loc_hasher& x_r ATTRIBUTE_UNUSED)
{
  struct decl_loc_hasher * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_ggc_mx_call_arg_loc_node (void *x_p)
{
  struct call_arg_loc_node * x = (struct call_arg_loc_node *)x_p;
  struct call_arg_loc_node * xlimit = x;
  while (ggc_test_and_set_mark (xlimit))
   xlimit = ((*xlimit).next);
  while (x != xlimit)
    {
      gt_ggc_m_7rtx_def ((*x).call_arg_loc_note);
      gt_ggc_m_S ((*x).label);
      gt_ggc_m_9tree_node ((*x).block);
      gt_ggc_m_7rtx_def ((*x).symbol_ref);
      gt_ggc_m_17call_arg_loc_node ((*x).next);
      x = ((*x).next);
    }
}

void
gt_ggc_mx_var_loc_list_def (void *x_p)
{
  struct var_loc_list_def * const x = (struct var_loc_list_def *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_m_12var_loc_node ((*x).first);
    }
}

void
gt_ggc_mx (struct var_loc_list_def& x_r ATTRIBUTE_UNUSED)
{
  struct var_loc_list_def * ATTRIBUTE_UNUSED x = &x_r;
  gt_ggc_m_12var_loc_node ((*x).first);
}

void
gt_ggc_mx (struct var_loc_list_def *& x)
{
  if (x)
    gt_ggc_mx_var_loc_list_def ((void *) x);
}

void
gt_ggc_mx_var_loc_node (void *x_p)
{
  struct var_loc_node * x = (struct var_loc_node *)x_p;
  struct var_loc_node * xlimit = x;
  while (ggc_test_and_set_mark (xlimit))
   xlimit = ((*xlimit).next);
  while (x != xlimit)
    {
      gt_ggc_m_7rtx_def ((*x).loc);
      gt_ggc_m_S ((*x).label);
      gt_ggc_m_12var_loc_node ((*x).next);
      x = ((*x).next);
    }
}

void
gt_ggc_mx_hash_table_block_die_hasher_ (void *x_p)
{
  hash_table<block_die_hasher> * const x = (hash_table<block_die_hasher> *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_mx (x);
    }
}

void
gt_ggc_mx (struct block_die_hasher& x_r ATTRIBUTE_UNUSED)
{
  struct block_die_hasher * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_ggc_mx_hash_table_decl_die_hasher_ (void *x_p)
{
  hash_table<decl_die_hasher> * const x = (hash_table<decl_die_hasher> *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_mx (x);
    }
}

void
gt_ggc_mx (struct decl_die_hasher& x_r ATTRIBUTE_UNUSED)
{
  struct decl_die_hasher * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_ggc_mx_hash_table_dwarf_file_hasher_ (void *x_p)
{
  hash_table<dwarf_file_hasher> * const x = (hash_table<dwarf_file_hasher> *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_mx (x);
    }
}

void
gt_ggc_mx (struct dwarf_file_hasher& x_r ATTRIBUTE_UNUSED)
{
  struct dwarf_file_hasher * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_ggc_mx_limbo_die_struct (void *x_p)
{
  struct limbo_die_struct * const x = (struct limbo_die_struct *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_m_10die_struct ((*x).die);
      gt_ggc_m_9tree_node ((*x).created_for);
      gt_ggc_m_16limbo_die_struct ((*x).next);
    }
}

void
gt_ggc_mx_dw_ranges_by_label (void *x_p)
{
  struct dw_ranges_by_label * const x = (struct dw_ranges_by_label *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_m_S ((*x).begin);
      gt_ggc_m_S ((*x).end);
    }
}

void
gt_ggc_mx_dw_ranges (void *x_p)
{
  struct dw_ranges * const x = (struct dw_ranges *)x_p;
  if (ggc_test_and_set_mark (x))
    {
    }
}

void
gt_ggc_mx_vec_dw_attr_node_va_gc_ (void *x_p)
{
  vec<dw_attr_node,va_gc> * const x = (vec<dw_attr_node,va_gc> *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_mx (x);
    }
}

void
gt_ggc_mx (struct dw_attr_struct& x_r ATTRIBUTE_UNUSED)
{
  struct dw_attr_struct * ATTRIBUTE_UNUSED x = &x_r;
  gt_ggc_m_16addr_table_entry ((*x).dw_attr_val.val_entry);
  switch ((int) (((*x).dw_attr_val).val_class))
    {
    case dw_val_class_addr:
      gt_ggc_m_7rtx_def ((*x).dw_attr_val.v.val_addr);
      break;
    case dw_val_class_offset:
      break;
    case dw_val_class_loc_list:
      gt_ggc_m_18dw_loc_list_struct ((*x).dw_attr_val.v.val_loc_list);
      break;
    case dw_val_class_loc:
      gt_ggc_m_17dw_loc_descr_node ((*x).dw_attr_val.v.val_loc);
      break;
    default:
      break;
    case dw_val_class_unsigned_const:
      break;
    case dw_val_class_const_double:
      break;
    case dw_val_class_wide_int:
      gt_ggc_m_34generic_wide_int_wide_int_storage_ ((*x).dw_attr_val.v.val_wide);
      break;
    case dw_val_class_vec:
      gt_ggc_m_S ((*x).dw_attr_val.v.val_vec.array);
      break;
    case dw_val_class_die_ref:
      gt_ggc_m_10die_struct ((*x).dw_attr_val.v.val_die_ref.die);
      break;
    case dw_val_class_fde_ref:
      break;
    case dw_val_class_str:
      gt_ggc_m_20indirect_string_node ((*x).dw_attr_val.v.val_str);
      break;
    case dw_val_class_lbl_id:
      gt_ggc_m_S ((*x).dw_attr_val.v.val_lbl_id);
      break;
    case dw_val_class_flag:
      break;
    case dw_val_class_file:
      gt_ggc_m_15dwarf_file_data ((*x).dw_attr_val.v.val_file);
      break;
    case dw_val_class_data8:
      break;
    case dw_val_class_decl_ref:
      gt_ggc_m_9tree_node ((*x).dw_attr_val.v.val_decl_ref);
      break;
    case dw_val_class_vms_delta:
      gt_ggc_m_S ((*x).dw_attr_val.v.val_vms_delta.lbl1);
      gt_ggc_m_S ((*x).dw_attr_val.v.val_vms_delta.lbl2);
      break;
    case dw_val_class_discr_value:
      switch ((int) (((*x).dw_attr_val.v.val_discr_value).pos))
        {
        case 0:
          break;
        case 1:
          break;
        default:
          break;
        }
      break;
    case dw_val_class_discr_list:
      gt_ggc_m_18dw_discr_list_node ((*x).dw_attr_val.v.val_discr_list);
      break;
    }
}

void
gt_ggc_mx_dw_line_info_table (void *x_p)
{
  struct dw_line_info_table * const x = (struct dw_line_info_table *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_m_S ((*x).end_label);
      gt_ggc_m_29vec_dw_line_info_entry_va_gc_ ((*x).entries);
    }
}

void
gt_ggc_mx_vec_dw_line_info_entry_va_gc_ (void *x_p)
{
  vec<dw_line_info_entry,va_gc> * const x = (vec<dw_line_info_entry,va_gc> *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_mx (x);
    }
}

void
gt_ggc_mx (struct dw_line_info_struct& x_r ATTRIBUTE_UNUSED)
{
  struct dw_line_info_struct * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_ggc_mx_comdat_type_node (void *x_p)
{
  struct comdat_type_node * const x = (struct comdat_type_node *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_m_10die_struct ((*x).root_die);
      gt_ggc_m_10die_struct ((*x).type_die);
      gt_ggc_m_10die_struct ((*x).skeleton_die);
      gt_ggc_m_16comdat_type_node ((*x).next);
    }
}

void
gt_ggc_mx_hash_table_indirect_string_hasher_ (void *x_p)
{
  hash_table<indirect_string_hasher> * const x = (hash_table<indirect_string_hasher> *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_mx (x);
    }
}

void
gt_ggc_mx (struct indirect_string_hasher& x_r ATTRIBUTE_UNUSED)
{
  struct indirect_string_hasher * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_ggc_mx_vec_dw_fde_ref_va_gc_ (void *x_p)
{
  vec<dw_fde_ref,va_gc> * const x = (vec<dw_fde_ref,va_gc> *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_mx (x);
    }
}

void
gt_ggc_mx (struct dw_fde_node *& x)
{
  if (x)
    gt_ggc_mx_dw_fde_node ((void *) x);
}

void
gt_ggc_mx_dwarf_file_data (void *x_p)
{
  struct dwarf_file_data * const x = (struct dwarf_file_data *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_m_S ((*x).filename);
    }
}

void
gt_ggc_mx (struct dwarf_file_data& x_r ATTRIBUTE_UNUSED)
{
  struct dwarf_file_data * ATTRIBUTE_UNUSED x = &x_r;
  gt_ggc_m_S ((*x).filename);
}

void
gt_ggc_mx (struct dwarf_file_data *& x)
{
  if (x)
    gt_ggc_mx_dwarf_file_data ((void *) x);
}

void
gt_ggc_mx_indirect_string_node (void *x_p)
{
  struct indirect_string_node * const x = (struct indirect_string_node *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_m_S ((*x).str);
      gt_ggc_m_S ((*x).label);
    }
}

void
gt_ggc_mx (struct indirect_string_node& x_r ATTRIBUTE_UNUSED)
{
  struct indirect_string_node * ATTRIBUTE_UNUSED x = &x_r;
  gt_ggc_m_S ((*x).str);
  gt_ggc_m_S ((*x).label);
}

void
gt_ggc_mx (struct indirect_string_node *& x)
{
  if (x)
    gt_ggc_mx_indirect_string_node ((void *) x);
}

void
gt_ggc_mx_addr_table_entry (void *x_p)
{
  struct addr_table_entry * const x = (struct addr_table_entry *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      switch ((int) (((*x)).kind))
        {
        case 0:
          gt_ggc_m_7rtx_def ((*x).addr.rtl);
          break;
        case 1:
          gt_ggc_m_S ((*x).addr.label);
          break;
        default:
          break;
        }
    }
}

void
gt_ggc_mx (struct addr_table_entry& x_r ATTRIBUTE_UNUSED)
{
  struct addr_table_entry * ATTRIBUTE_UNUSED x = &x_r;
  switch ((int) (((*x)).kind))
    {
    case 0:
      gt_ggc_m_7rtx_def ((*x).addr.rtl);
      break;
    case 1:
      gt_ggc_m_S ((*x).addr.label);
      break;
    default:
      break;
    }
}

void
gt_ggc_mx (struct addr_table_entry *& x)
{
  if (x)
    gt_ggc_mx_addr_table_entry ((void *) x);
}

void
gt_ggc_mx_dw_loc_list_struct (void *x_p)
{
  struct dw_loc_list_struct * const x = (struct dw_loc_list_struct *)x_p;
  if (ggc_test_and_set_mark (x))
    {
      gt_ggc_m_18dw_loc_list_struct ((*x).dw_loc_next);
      gt_ggc_m_S ((*x).begin);
      gt_ggc_m_16addr_table_entry ((*x).begin_entry);
      gt_ggc_m_S ((*x).end);
      gt_ggc_m_S ((*x).ll_symbol);
      gt_ggc_m_S ((*x).section);
      gt_ggc_m_17dw_loc_descr_node ((*x).expr);
    }
}

void
gt_ggc_mx_die_struct (void *x_p)
{
  struct die_struct * x = (struct die_struct *)x_p;
  struct die_struct * xlimit = x;
  if (!ggc_test_and_set_mark (xlimit))
    return;
  do
   xlimit = ((*xlimit).die_sib);
  while (ggc_test_and_set_mark (xlimit));
  do
    {
      switch ((int) ((*x).comdat_type_p))
        {
        case 0:
          gt_ggc_m_S ((*x).die_id.die_symbol);
          break;
        case 1:
          gt_ggc_m_16comdat_type_node ((*x).die_id.die_type_node);
          break;
        default:
          break;
        }
      gt_ggc_m_23vec_dw_attr_node_va_gc_ ((*x).die_attr);
      gt_ggc_m_10die_struct ((*x).die_parent);
      gt_ggc_m_10die_struct ((*x).die_child);
      gt_ggc_m_10die_struct ((*x).die_sib);
      gt_ggc_m_10die_struct ((*x).die_definition);
      x = ((*x).die_sib);
    }
  while (x != xlimit);
}

void
gt_ggc_mx (struct die_struct& x_r ATTRIBUTE_UNUSED)
{
  struct die_struct * ATTRIBUTE_UNUSED x = &x_r;
  switch ((int) ((*x).comdat_type_p))
    {
    case 0:
      gt_ggc_m_S ((*x).die_id.die_symbol);
      break;
    case 1:
      gt_ggc_m_16comdat_type_node ((*x).die_id.die_type_node);
      break;
    default:
      break;
    }
  gt_ggc_m_23vec_dw_attr_node_va_gc_ ((*x).die_attr);
  gt_ggc_m_10die_struct ((*x).die_parent);
  gt_ggc_m_10die_struct ((*x).die_child);
  gt_ggc_m_10die_struct ((*x).die_sib);
  gt_ggc_m_10die_struct ((*x).die_definition);
}

void
gt_ggc_mx (struct die_struct *& x)
{
  if (x)
    gt_ggc_mx_die_struct ((void *) x);
}

void
gt_pch_nx_hash_table_addr_hasher_ (void *x_p)
{
  hash_table<addr_hasher> * const x = (hash_table<addr_hasher> *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_23hash_table_addr_hasher_))
    {
      gt_pch_nx (x);
    }
}

void
gt_pch_nx (struct addr_hasher& x_r ATTRIBUTE_UNUSED)
{
  struct addr_hasher * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_pch_nx_vec_die_arg_entry_va_gc_ (void *x_p)
{
  vec<die_arg_entry,va_gc> * const x = (vec<die_arg_entry,va_gc> *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_24vec_die_arg_entry_va_gc_))
    {
      gt_pch_nx (x);
    }
}

void
gt_pch_nx (struct die_arg_entry_struct& x_r ATTRIBUTE_UNUSED)
{
  struct die_arg_entry_struct * ATTRIBUTE_UNUSED x = &x_r;
  gt_pch_n_10die_struct ((*x).die);
  gt_pch_n_9tree_node ((*x).arg);
}

void
gt_pch_nx_vec_macinfo_entry_va_gc_ (void *x_p)
{
  vec<macinfo_entry,va_gc> * const x = (vec<macinfo_entry,va_gc> *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_24vec_macinfo_entry_va_gc_))
    {
      gt_pch_nx (x);
    }
}

void
gt_pch_nx (struct macinfo_struct& x_r ATTRIBUTE_UNUSED)
{
  struct macinfo_struct * ATTRIBUTE_UNUSED x = &x_r;
  gt_pch_n_S ((*x).info);
}

void
gt_pch_nx_vec_pubname_entry_va_gc_ (void *x_p)
{
  vec<pubname_entry,va_gc> * const x = (vec<pubname_entry,va_gc> *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_24vec_pubname_entry_va_gc_))
    {
      gt_pch_nx (x);
    }
}

void
gt_pch_nx (struct pubname_struct& x_r ATTRIBUTE_UNUSED)
{
  struct pubname_struct * ATTRIBUTE_UNUSED x = &x_r;
  gt_pch_n_10die_struct ((*x).die);
  gt_pch_n_S ((*x).name);
}

void
gt_pch_nx_vec_dw_line_info_table__va_gc_ (void *x_p)
{
  vec<dw_line_info_table*,va_gc> * const x = (vec<dw_line_info_table*,va_gc> *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_30vec_dw_line_info_table__va_gc_))
    {
      gt_pch_nx (x);
    }
}

void
gt_pch_nx (struct dw_line_info_table *& x)
{
  if (x)
    gt_pch_nx_dw_line_info_table ((void *) x);
}

void
gt_pch_nx_hash_table_dw_loc_list_hasher_ (void *x_p)
{
  hash_table<dw_loc_list_hasher> * const x = (hash_table<dw_loc_list_hasher> *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_30hash_table_dw_loc_list_hasher_))
    {
      gt_pch_nx (x);
    }
}

void
gt_pch_nx (struct dw_loc_list_hasher& x_r ATTRIBUTE_UNUSED)
{
  struct dw_loc_list_hasher * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_pch_nx_cached_dw_loc_list_def (void *x_p)
{
  struct cached_dw_loc_list_def * const x = (struct cached_dw_loc_list_def *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_22cached_dw_loc_list_def))
    {
      gt_pch_n_18dw_loc_list_struct ((*x).loc_list);
    }
}

void
gt_pch_nx (struct cached_dw_loc_list_def& x_r ATTRIBUTE_UNUSED)
{
  struct cached_dw_loc_list_def * ATTRIBUTE_UNUSED x = &x_r;
  gt_pch_n_18dw_loc_list_struct ((*x).loc_list);
}

void
gt_pch_nx (struct cached_dw_loc_list_def *& x)
{
  if (x)
    gt_pch_nx_cached_dw_loc_list_def ((void *) x);
}

void
gt_pch_nx_hash_table_decl_loc_hasher_ (void *x_p)
{
  hash_table<decl_loc_hasher> * const x = (hash_table<decl_loc_hasher> *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_27hash_table_decl_loc_hasher_))
    {
      gt_pch_nx (x);
    }
}

void
gt_pch_nx (struct decl_loc_hasher& x_r ATTRIBUTE_UNUSED)
{
  struct decl_loc_hasher * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_pch_nx_call_arg_loc_node (void *x_p)
{
  struct call_arg_loc_node * x = (struct call_arg_loc_node *)x_p;
  struct call_arg_loc_node * xlimit = x;
  while (gt_pch_note_object (xlimit, xlimit, gt_pch_p_17call_arg_loc_node))
   xlimit = ((*xlimit).next);
  while (x != xlimit)
    {
      gt_pch_n_7rtx_def ((*x).call_arg_loc_note);
      gt_pch_n_S ((*x).label);
      gt_pch_n_9tree_node ((*x).block);
      gt_pch_n_7rtx_def ((*x).symbol_ref);
      gt_pch_n_17call_arg_loc_node ((*x).next);
      x = ((*x).next);
    }
}

void
gt_pch_nx_var_loc_list_def (void *x_p)
{
  struct var_loc_list_def * const x = (struct var_loc_list_def *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_16var_loc_list_def))
    {
      gt_pch_n_12var_loc_node ((*x).first);
    }
}

void
gt_pch_nx (struct var_loc_list_def& x_r ATTRIBUTE_UNUSED)
{
  struct var_loc_list_def * ATTRIBUTE_UNUSED x = &x_r;
  gt_pch_n_12var_loc_node ((*x).first);
}

void
gt_pch_nx (struct var_loc_list_def *& x)
{
  if (x)
    gt_pch_nx_var_loc_list_def ((void *) x);
}

void
gt_pch_nx_var_loc_node (void *x_p)
{
  struct var_loc_node * x = (struct var_loc_node *)x_p;
  struct var_loc_node * xlimit = x;
  while (gt_pch_note_object (xlimit, xlimit, gt_pch_p_12var_loc_node))
   xlimit = ((*xlimit).next);
  while (x != xlimit)
    {
      gt_pch_n_7rtx_def ((*x).loc);
      gt_pch_n_S ((*x).label);
      gt_pch_n_12var_loc_node ((*x).next);
      x = ((*x).next);
    }
}

void
gt_pch_nx_hash_table_block_die_hasher_ (void *x_p)
{
  hash_table<block_die_hasher> * const x = (hash_table<block_die_hasher> *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_28hash_table_block_die_hasher_))
    {
      gt_pch_nx (x);
    }
}

void
gt_pch_nx (struct block_die_hasher& x_r ATTRIBUTE_UNUSED)
{
  struct block_die_hasher * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_pch_nx_hash_table_decl_die_hasher_ (void *x_p)
{
  hash_table<decl_die_hasher> * const x = (hash_table<decl_die_hasher> *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_27hash_table_decl_die_hasher_))
    {
      gt_pch_nx (x);
    }
}

void
gt_pch_nx (struct decl_die_hasher& x_r ATTRIBUTE_UNUSED)
{
  struct decl_die_hasher * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_pch_nx_hash_table_dwarf_file_hasher_ (void *x_p)
{
  hash_table<dwarf_file_hasher> * const x = (hash_table<dwarf_file_hasher> *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_29hash_table_dwarf_file_hasher_))
    {
      gt_pch_nx (x);
    }
}

void
gt_pch_nx (struct dwarf_file_hasher& x_r ATTRIBUTE_UNUSED)
{
  struct dwarf_file_hasher * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_pch_nx_limbo_die_struct (void *x_p)
{
  struct limbo_die_struct * const x = (struct limbo_die_struct *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_16limbo_die_struct))
    {
      gt_pch_n_10die_struct ((*x).die);
      gt_pch_n_9tree_node ((*x).created_for);
      gt_pch_n_16limbo_die_struct ((*x).next);
    }
}

void
gt_pch_nx_dw_ranges_by_label (void *x_p)
{
  struct dw_ranges_by_label * const x = (struct dw_ranges_by_label *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_18dw_ranges_by_label))
    {
      gt_pch_n_S ((*x).begin);
      gt_pch_n_S ((*x).end);
    }
}

void
gt_pch_nx_dw_ranges (void *x_p)
{
  struct dw_ranges * const x = (struct dw_ranges *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_9dw_ranges))
    {
    }
}

void
gt_pch_nx_vec_dw_attr_node_va_gc_ (void *x_p)
{
  vec<dw_attr_node,va_gc> * const x = (vec<dw_attr_node,va_gc> *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_23vec_dw_attr_node_va_gc_))
    {
      gt_pch_nx (x);
    }
}

void
gt_pch_nx (struct dw_attr_struct& x_r ATTRIBUTE_UNUSED)
{
  struct dw_attr_struct * ATTRIBUTE_UNUSED x = &x_r;
  gt_pch_n_16addr_table_entry ((*x).dw_attr_val.val_entry);
  switch ((int) (((*x).dw_attr_val).val_class))
    {
    case dw_val_class_addr:
      gt_pch_n_7rtx_def ((*x).dw_attr_val.v.val_addr);
      break;
    case dw_val_class_offset:
      break;
    case dw_val_class_loc_list:
      gt_pch_n_18dw_loc_list_struct ((*x).dw_attr_val.v.val_loc_list);
      break;
    case dw_val_class_loc:
      gt_pch_n_17dw_loc_descr_node ((*x).dw_attr_val.v.val_loc);
      break;
    default:
      break;
    case dw_val_class_unsigned_const:
      break;
    case dw_val_class_const_double:
      break;
    case dw_val_class_wide_int:
      gt_pch_n_34generic_wide_int_wide_int_storage_ ((*x).dw_attr_val.v.val_wide);
      break;
    case dw_val_class_vec:
      gt_pch_n_S ((*x).dw_attr_val.v.val_vec.array);
      break;
    case dw_val_class_die_ref:
      gt_pch_n_10die_struct ((*x).dw_attr_val.v.val_die_ref.die);
      break;
    case dw_val_class_fde_ref:
      break;
    case dw_val_class_str:
      gt_pch_n_20indirect_string_node ((*x).dw_attr_val.v.val_str);
      break;
    case dw_val_class_lbl_id:
      gt_pch_n_S ((*x).dw_attr_val.v.val_lbl_id);
      break;
    case dw_val_class_flag:
      break;
    case dw_val_class_file:
      gt_pch_n_15dwarf_file_data ((*x).dw_attr_val.v.val_file);
      break;
    case dw_val_class_data8:
      break;
    case dw_val_class_decl_ref:
      gt_pch_n_9tree_node ((*x).dw_attr_val.v.val_decl_ref);
      break;
    case dw_val_class_vms_delta:
      gt_pch_n_S ((*x).dw_attr_val.v.val_vms_delta.lbl1);
      gt_pch_n_S ((*x).dw_attr_val.v.val_vms_delta.lbl2);
      break;
    case dw_val_class_discr_value:
      switch ((int) (((*x).dw_attr_val.v.val_discr_value).pos))
        {
        case 0:
          break;
        case 1:
          break;
        default:
          break;
        }
      break;
    case dw_val_class_discr_list:
      gt_pch_n_18dw_discr_list_node ((*x).dw_attr_val.v.val_discr_list);
      break;
    }
}

void
gt_pch_nx_dw_line_info_table (void *x_p)
{
  struct dw_line_info_table * const x = (struct dw_line_info_table *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_18dw_line_info_table))
    {
      gt_pch_n_S ((*x).end_label);
      gt_pch_n_29vec_dw_line_info_entry_va_gc_ ((*x).entries);
    }
}

void
gt_pch_nx_vec_dw_line_info_entry_va_gc_ (void *x_p)
{
  vec<dw_line_info_entry,va_gc> * const x = (vec<dw_line_info_entry,va_gc> *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_29vec_dw_line_info_entry_va_gc_))
    {
      gt_pch_nx (x);
    }
}

void
gt_pch_nx (struct dw_line_info_struct& x_r ATTRIBUTE_UNUSED)
{
  struct dw_line_info_struct * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_pch_nx_comdat_type_node (void *x_p)
{
  struct comdat_type_node * const x = (struct comdat_type_node *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_16comdat_type_node))
    {
      gt_pch_n_10die_struct ((*x).root_die);
      gt_pch_n_10die_struct ((*x).type_die);
      gt_pch_n_10die_struct ((*x).skeleton_die);
      gt_pch_n_16comdat_type_node ((*x).next);
    }
}

void
gt_pch_nx_hash_table_indirect_string_hasher_ (void *x_p)
{
  hash_table<indirect_string_hasher> * const x = (hash_table<indirect_string_hasher> *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_34hash_table_indirect_string_hasher_))
    {
      gt_pch_nx (x);
    }
}

void
gt_pch_nx (struct indirect_string_hasher& x_r ATTRIBUTE_UNUSED)
{
  struct indirect_string_hasher * ATTRIBUTE_UNUSED x = &x_r;
}

void
gt_pch_nx_vec_dw_fde_ref_va_gc_ (void *x_p)
{
  vec<dw_fde_ref,va_gc> * const x = (vec<dw_fde_ref,va_gc> *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_21vec_dw_fde_ref_va_gc_))
    {
      gt_pch_nx (x);
    }
}

void
gt_pch_nx (struct dw_fde_node *& x)
{
  if (x)
    gt_pch_nx_dw_fde_node ((void *) x);
}

void
gt_pch_nx_dwarf_file_data (void *x_p)
{
  struct dwarf_file_data * const x = (struct dwarf_file_data *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_15dwarf_file_data))
    {
      gt_pch_n_S ((*x).filename);
    }
}

void
gt_pch_nx (struct dwarf_file_data& x_r ATTRIBUTE_UNUSED)
{
  struct dwarf_file_data * ATTRIBUTE_UNUSED x = &x_r;
  gt_pch_n_S ((*x).filename);
}

void
gt_pch_nx (struct dwarf_file_data *& x)
{
  if (x)
    gt_pch_nx_dwarf_file_data ((void *) x);
}

void
gt_pch_nx_indirect_string_node (void *x_p)
{
  struct indirect_string_node * const x = (struct indirect_string_node *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_20indirect_string_node))
    {
      gt_pch_n_S ((*x).str);
      gt_pch_n_S ((*x).label);
    }
}

void
gt_pch_nx (struct indirect_string_node& x_r ATTRIBUTE_UNUSED)
{
  struct indirect_string_node * ATTRIBUTE_UNUSED x = &x_r;
  gt_pch_n_S ((*x).str);
  gt_pch_n_S ((*x).label);
}

void
gt_pch_nx (struct indirect_string_node *& x)
{
  if (x)
    gt_pch_nx_indirect_string_node ((void *) x);
}

void
gt_pch_nx_addr_table_entry (void *x_p)
{
  struct addr_table_entry * const x = (struct addr_table_entry *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_16addr_table_entry))
    {
      switch ((int) (((*x)).kind))
        {
        case 0:
          gt_pch_n_7rtx_def ((*x).addr.rtl);
          break;
        case 1:
          gt_pch_n_S ((*x).addr.label);
          break;
        default:
          break;
        }
    }
}

void
gt_pch_nx (struct addr_table_entry& x_r ATTRIBUTE_UNUSED)
{
  struct addr_table_entry * ATTRIBUTE_UNUSED x = &x_r;
  switch ((int) (((*x)).kind))
    {
    case 0:
      gt_pch_n_7rtx_def ((*x).addr.rtl);
      break;
    case 1:
      gt_pch_n_S ((*x).addr.label);
      break;
    default:
      break;
    }
}

void
gt_pch_nx (struct addr_table_entry *& x)
{
  if (x)
    gt_pch_nx_addr_table_entry ((void *) x);
}

void
gt_pch_nx_dw_loc_list_struct (void *x_p)
{
  struct dw_loc_list_struct * const x = (struct dw_loc_list_struct *)x_p;
  if (gt_pch_note_object (x, x, gt_pch_p_18dw_loc_list_struct))
    {
      gt_pch_n_18dw_loc_list_struct ((*x).dw_loc_next);
      gt_pch_n_S ((*x).begin);
      gt_pch_n_16addr_table_entry ((*x).begin_entry);
      gt_pch_n_S ((*x).end);
      gt_pch_n_S ((*x).ll_symbol);
      gt_pch_n_S ((*x).section);
      gt_pch_n_17dw_loc_descr_node ((*x).expr);
    }
}

void
gt_pch_nx_die_struct (void *x_p)
{
  struct die_struct * x = (struct die_struct *)x_p;
  struct die_struct * xlimit = x;
  if (!gt_pch_note_object (xlimit, xlimit, gt_pch_p_10die_struct))
    return;
  do
   xlimit = ((*xlimit).die_sib);
  while (gt_pch_note_object (xlimit, xlimit, gt_pch_p_10die_struct));
  do
    {
      switch ((int) ((*x).comdat_type_p))
        {
        case 0:
          gt_pch_n_S ((*x).die_id.die_symbol);
          break;
        case 1:
          gt_pch_n_16comdat_type_node ((*x).die_id.die_type_node);
          break;
        default:
          break;
        }
      gt_pch_n_23vec_dw_attr_node_va_gc_ ((*x).die_attr);
      gt_pch_n_10die_struct ((*x).die_parent);
      gt_pch_n_10die_struct ((*x).die_child);
      gt_pch_n_10die_struct ((*x).die_sib);
      gt_pch_n_10die_struct ((*x).die_definition);
      x = ((*x).die_sib);
    }
  while (x != xlimit);
}

void
gt_pch_nx (struct die_struct& x_r ATTRIBUTE_UNUSED)
{
  struct die_struct * ATTRIBUTE_UNUSED x = &x_r;
  switch ((int) ((*x).comdat_type_p))
    {
    case 0:
      gt_pch_n_S ((*x).die_id.die_symbol);
      break;
    case 1:
      gt_pch_n_16comdat_type_node ((*x).die_id.die_type_node);
      break;
    default:
      break;
    }
  gt_pch_n_23vec_dw_attr_node_va_gc_ ((*x).die_attr);
  gt_pch_n_10die_struct ((*x).die_parent);
  gt_pch_n_10die_struct ((*x).die_child);
  gt_pch_n_10die_struct ((*x).die_sib);
  gt_pch_n_10die_struct ((*x).die_definition);
}

void
gt_pch_nx (struct die_struct *& x)
{
  if (x)
    gt_pch_nx_die_struct ((void *) x);
}

void
gt_pch_p_23hash_table_addr_hasher_ (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct hash_table<addr_hasher> * x ATTRIBUTE_UNUSED = (struct hash_table<addr_hasher> *)x_p;
  if ((void *)(x) == this_obj)
    gt_pch_nx (&((*x)), op, cookie);
}

void
gt_pch_nx (struct addr_hasher* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
}

void
gt_pch_p_24vec_die_arg_entry_va_gc_ (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct vec<die_arg_entry,va_gc> * x ATTRIBUTE_UNUSED = (struct vec<die_arg_entry,va_gc> *)x_p;
  if ((void *)(x) == this_obj)
    gt_pch_nx (&((*x)), op, cookie);
}

void
gt_pch_nx (struct die_arg_entry_struct* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
    op (&((*x).die), cookie);
    op (&((*x).arg), cookie);
}

void
gt_pch_p_24vec_macinfo_entry_va_gc_ (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct vec<macinfo_entry,va_gc> * x ATTRIBUTE_UNUSED = (struct vec<macinfo_entry,va_gc> *)x_p;
  if ((void *)(x) == this_obj)
    gt_pch_nx (&((*x)), op, cookie);
}

void
gt_pch_nx (struct macinfo_struct* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
    op (&((*x).info), cookie);
}

void
gt_pch_p_24vec_pubname_entry_va_gc_ (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct vec<pubname_entry,va_gc> * x ATTRIBUTE_UNUSED = (struct vec<pubname_entry,va_gc> *)x_p;
  if ((void *)(x) == this_obj)
    gt_pch_nx (&((*x)), op, cookie);
}

void
gt_pch_nx (struct pubname_struct* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
    op (&((*x).die), cookie);
    op (&((*x).name), cookie);
}

void
gt_pch_p_30vec_dw_line_info_table__va_gc_ (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct vec<dw_line_info_table*,va_gc> * x ATTRIBUTE_UNUSED = (struct vec<dw_line_info_table*,va_gc> *)x_p;
  if ((void *)(x) == this_obj)
    gt_pch_nx (&((*x)), op, cookie);
}

void
gt_pch_p_30hash_table_dw_loc_list_hasher_ (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct hash_table<dw_loc_list_hasher> * x ATTRIBUTE_UNUSED = (struct hash_table<dw_loc_list_hasher> *)x_p;
  if ((void *)(x) == this_obj)
    gt_pch_nx (&((*x)), op, cookie);
}

void
gt_pch_nx (struct dw_loc_list_hasher* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
}

void
gt_pch_p_22cached_dw_loc_list_def (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct cached_dw_loc_list_def * x ATTRIBUTE_UNUSED = (struct cached_dw_loc_list_def *)x_p;
  if ((void *)(x) == this_obj)
    op (&((*x).loc_list), cookie);
}

void
gt_pch_nx (struct cached_dw_loc_list_def* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
    op (&((*x).loc_list), cookie);
}

void
gt_pch_p_27hash_table_decl_loc_hasher_ (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct hash_table<decl_loc_hasher> * x ATTRIBUTE_UNUSED = (struct hash_table<decl_loc_hasher> *)x_p;
  if ((void *)(x) == this_obj)
    gt_pch_nx (&((*x)), op, cookie);
}

void
gt_pch_nx (struct decl_loc_hasher* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
}

void
gt_pch_p_17call_arg_loc_node (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct call_arg_loc_node * x ATTRIBUTE_UNUSED = (struct call_arg_loc_node *)x_p;
  if ((void *)(x) == this_obj)
    op (&((*x).call_arg_loc_note), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).label), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).block), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).symbol_ref), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).next), cookie);
}

void
gt_pch_p_16var_loc_list_def (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct var_loc_list_def * x ATTRIBUTE_UNUSED = (struct var_loc_list_def *)x_p;
  if ((void *)(x) == this_obj)
    op (&((*x).first), cookie);
}

void
gt_pch_nx (struct var_loc_list_def* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
    op (&((*x).first), cookie);
}

void
gt_pch_p_12var_loc_node (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct var_loc_node * x ATTRIBUTE_UNUSED = (struct var_loc_node *)x_p;
  if ((void *)(x) == this_obj)
    op (&((*x).loc), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).label), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).next), cookie);
}

void
gt_pch_p_28hash_table_block_die_hasher_ (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct hash_table<block_die_hasher> * x ATTRIBUTE_UNUSED = (struct hash_table<block_die_hasher> *)x_p;
  if ((void *)(x) == this_obj)
    gt_pch_nx (&((*x)), op, cookie);
}

void
gt_pch_nx (struct block_die_hasher* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
}

void
gt_pch_p_27hash_table_decl_die_hasher_ (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct hash_table<decl_die_hasher> * x ATTRIBUTE_UNUSED = (struct hash_table<decl_die_hasher> *)x_p;
  if ((void *)(x) == this_obj)
    gt_pch_nx (&((*x)), op, cookie);
}

void
gt_pch_nx (struct decl_die_hasher* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
}

void
gt_pch_p_29hash_table_dwarf_file_hasher_ (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct hash_table<dwarf_file_hasher> * x ATTRIBUTE_UNUSED = (struct hash_table<dwarf_file_hasher> *)x_p;
  if ((void *)(x) == this_obj)
    gt_pch_nx (&((*x)), op, cookie);
}

void
gt_pch_nx (struct dwarf_file_hasher* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
}

void
gt_pch_p_16limbo_die_struct (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct limbo_die_struct * x ATTRIBUTE_UNUSED = (struct limbo_die_struct *)x_p;
  if ((void *)(x) == this_obj)
    op (&((*x).die), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).created_for), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).next), cookie);
}

void
gt_pch_p_18dw_ranges_by_label (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct dw_ranges_by_label * x ATTRIBUTE_UNUSED = (struct dw_ranges_by_label *)x_p;
  if ((void *)(x) == this_obj)
    op (&((*x).begin), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).end), cookie);
}

void
gt_pch_p_9dw_ranges (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct dw_ranges * x ATTRIBUTE_UNUSED = (struct dw_ranges *)x_p;
}

void
gt_pch_p_23vec_dw_attr_node_va_gc_ (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct vec<dw_attr_node,va_gc> * x ATTRIBUTE_UNUSED = (struct vec<dw_attr_node,va_gc> *)x_p;
  if ((void *)(x) == this_obj)
    gt_pch_nx (&((*x)), op, cookie);
}

void
gt_pch_nx (struct dw_attr_struct* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
    op (&((*x).dw_attr_val.val_entry), cookie);
  switch ((int) (((*x).dw_attr_val).val_class))
    {
    case dw_val_class_addr:
        op (&((*x).dw_attr_val.v.val_addr), cookie);
      break;
    case dw_val_class_offset:
      break;
    case dw_val_class_loc_list:
        op (&((*x).dw_attr_val.v.val_loc_list), cookie);
      break;
    case dw_val_class_loc:
        op (&((*x).dw_attr_val.v.val_loc), cookie);
      break;
    default:
      break;
    case dw_val_class_unsigned_const:
      break;
    case dw_val_class_const_double:
      break;
    case dw_val_class_wide_int:
        op (&((*x).dw_attr_val.v.val_wide), cookie);
      break;
    case dw_val_class_vec:
        op (&((*x).dw_attr_val.v.val_vec.array), cookie);
      break;
    case dw_val_class_die_ref:
        op (&((*x).dw_attr_val.v.val_die_ref.die), cookie);
      break;
    case dw_val_class_fde_ref:
      break;
    case dw_val_class_str:
        op (&((*x).dw_attr_val.v.val_str), cookie);
      break;
    case dw_val_class_lbl_id:
        op (&((*x).dw_attr_val.v.val_lbl_id), cookie);
      break;
    case dw_val_class_flag:
      break;
    case dw_val_class_file:
        op (&((*x).dw_attr_val.v.val_file), cookie);
      break;
    case dw_val_class_data8:
      break;
    case dw_val_class_decl_ref:
        op (&((*x).dw_attr_val.v.val_decl_ref), cookie);
      break;
    case dw_val_class_vms_delta:
        op (&((*x).dw_attr_val.v.val_vms_delta.lbl1), cookie);
        op (&((*x).dw_attr_val.v.val_vms_delta.lbl2), cookie);
      break;
    case dw_val_class_discr_value:
      switch ((int) (((*x).dw_attr_val.v.val_discr_value).pos))
        {
        case 0:
          break;
        case 1:
          break;
        default:
          break;
        }
      break;
    case dw_val_class_discr_list:
        op (&((*x).dw_attr_val.v.val_discr_list), cookie);
      break;
    }
}

void
gt_pch_p_18dw_line_info_table (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct dw_line_info_table * x ATTRIBUTE_UNUSED = (struct dw_line_info_table *)x_p;
  if ((void *)(x) == this_obj)
    op (&((*x).end_label), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).entries), cookie);
}

void
gt_pch_p_29vec_dw_line_info_entry_va_gc_ (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct vec<dw_line_info_entry,va_gc> * x ATTRIBUTE_UNUSED = (struct vec<dw_line_info_entry,va_gc> *)x_p;
  if ((void *)(x) == this_obj)
    gt_pch_nx (&((*x)), op, cookie);
}

void
gt_pch_nx (struct dw_line_info_struct* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
}

void
gt_pch_p_16comdat_type_node (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct comdat_type_node * x ATTRIBUTE_UNUSED = (struct comdat_type_node *)x_p;
  if ((void *)(x) == this_obj)
    op (&((*x).root_die), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).type_die), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).skeleton_die), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).next), cookie);
}

void
gt_pch_p_34hash_table_indirect_string_hasher_ (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct hash_table<indirect_string_hasher> * x ATTRIBUTE_UNUSED = (struct hash_table<indirect_string_hasher> *)x_p;
  if ((void *)(x) == this_obj)
    gt_pch_nx (&((*x)), op, cookie);
}

void
gt_pch_nx (struct indirect_string_hasher* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
}

void
gt_pch_p_21vec_dw_fde_ref_va_gc_ (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct vec<dw_fde_ref,va_gc> * x ATTRIBUTE_UNUSED = (struct vec<dw_fde_ref,va_gc> *)x_p;
  if ((void *)(x) == this_obj)
    gt_pch_nx (&((*x)), op, cookie);
}

void
gt_pch_p_15dwarf_file_data (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct dwarf_file_data * x ATTRIBUTE_UNUSED = (struct dwarf_file_data *)x_p;
  if ((void *)(x) == this_obj)
    op (&((*x).filename), cookie);
}

void
gt_pch_nx (struct dwarf_file_data* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
    op (&((*x).filename), cookie);
}

void
gt_pch_p_20indirect_string_node (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct indirect_string_node * x ATTRIBUTE_UNUSED = (struct indirect_string_node *)x_p;
  if ((void *)(x) == this_obj)
    op (&((*x).str), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).label), cookie);
}

void
gt_pch_nx (struct indirect_string_node* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
    op (&((*x).str), cookie);
    op (&((*x).label), cookie);
}

void
gt_pch_p_16addr_table_entry (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct addr_table_entry * x ATTRIBUTE_UNUSED = (struct addr_table_entry *)x_p;
  switch ((int) (((*x)).kind))
    {
    case 0:
      if ((void *)(x) == this_obj)
        op (&((*x).addr.rtl), cookie);
      break;
    case 1:
      if ((void *)(x) == this_obj)
        op (&((*x).addr.label), cookie);
      break;
    default:
      break;
    }
}

void
gt_pch_nx (struct addr_table_entry* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  switch ((int) (((*x)).kind))
    {
    case 0:
        op (&((*x).addr.rtl), cookie);
      break;
    case 1:
        op (&((*x).addr.label), cookie);
      break;
    default:
      break;
    }
}

void
gt_pch_p_18dw_loc_list_struct (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct dw_loc_list_struct * x ATTRIBUTE_UNUSED = (struct dw_loc_list_struct *)x_p;
  if ((void *)(x) == this_obj)
    op (&((*x).dw_loc_next), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).begin), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).begin_entry), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).end), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).ll_symbol), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).section), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).expr), cookie);
}

void
gt_pch_p_10die_struct (ATTRIBUTE_UNUSED void *this_obj,
	void *x_p,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  struct die_struct * x ATTRIBUTE_UNUSED = (struct die_struct *)x_p;
  switch ((int) ((*x).comdat_type_p))
    {
    case 0:
      if ((void *)(x) == this_obj)
        op (&((*x).die_id.die_symbol), cookie);
      break;
    case 1:
      if ((void *)(x) == this_obj)
        op (&((*x).die_id.die_type_node), cookie);
      break;
    default:
      break;
    }
  if ((void *)(x) == this_obj)
    op (&((*x).die_attr), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).die_parent), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).die_child), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).die_sib), cookie);
  if ((void *)(x) == this_obj)
    op (&((*x).die_definition), cookie);
}

void
gt_pch_nx (struct die_struct* x ATTRIBUTE_UNUSED,
	ATTRIBUTE_UNUSED gt_pointer_operator op,
	ATTRIBUTE_UNUSED void *cookie)
{
  switch ((int) ((*x).comdat_type_p))
    {
    case 0:
        op (&((*x).die_id.die_symbol), cookie);
      break;
    case 1:
        op (&((*x).die_id.die_type_node), cookie);
      break;
    default:
      break;
    }
    op (&((*x).die_attr), cookie);
    op (&((*x).die_parent), cookie);
    op (&((*x).die_child), cookie);
    op (&((*x).die_sib), cookie);
    op (&((*x).die_definition), cookie);
}

/* GC roots.  */

static void gt_ggc_ma_ranges_by_label (void *);
static void
gt_ggc_ma_ranges_by_label (ATTRIBUTE_UNUSED void *x_p)
{
  if (ranges_by_label != NULL) {
    size_t i0;
    for (i0 = 0; i0 != (size_t)(ranges_by_label_allocated); i0++) {
      gt_ggc_m_S (ranges_by_label[i0].begin);
      gt_ggc_m_S (ranges_by_label[i0].end);
    }
    ggc_mark (ranges_by_label);
  }
}

static void gt_pch_pa_ranges_by_label
    (void *, void *, gt_pointer_operator, void *);
static void gt_pch_pa_ranges_by_label (ATTRIBUTE_UNUSED void *this_obj,
      ATTRIBUTE_UNUSED void *x_p,
      ATTRIBUTE_UNUSED gt_pointer_operator op,
      ATTRIBUTE_UNUSED void * cookie)
{
  if (ranges_by_label != NULL) {
    size_t i0;
    for (i0 = 0; i0 != (size_t)(ranges_by_label_allocated) && ((void *)ranges_by_label == this_obj); i0++) {
      if ((void *)(ranges_by_label) == this_obj)
        op (&(ranges_by_label[i0].begin), cookie);
      if ((void *)(ranges_by_label) == this_obj)
        op (&(ranges_by_label[i0].end), cookie);
    }
    if ((void *)(&ranges_by_label) == this_obj)
      op (&(ranges_by_label), cookie);
  }
}

static void gt_pch_na_ranges_by_label (void *);
static void
gt_pch_na_ranges_by_label (ATTRIBUTE_UNUSED void *x_p)
{
  if (ranges_by_label != NULL) {
    size_t i0;
    for (i0 = 0; i0 != (size_t)(ranges_by_label_allocated); i0++) {
      gt_pch_n_S (ranges_by_label[i0].begin);
      gt_pch_n_S (ranges_by_label[i0].end);
    }
    gt_pch_note_object (ranges_by_label, &ranges_by_label, gt_pch_pa_ranges_by_label);
  }
}

static void gt_ggc_ma_ranges_table (void *);
static void
gt_ggc_ma_ranges_table (ATTRIBUTE_UNUSED void *x_p)
{
  if (ranges_table != NULL) {
    size_t i0;
    for (i0 = 0; i0 != (size_t)(ranges_table_allocated); i0++) {
    }
    ggc_mark (ranges_table);
  }
}

static void gt_pch_pa_ranges_table
    (void *, void *, gt_pointer_operator, void *);
static void gt_pch_pa_ranges_table (ATTRIBUTE_UNUSED void *this_obj,
      ATTRIBUTE_UNUSED void *x_p,
      ATTRIBUTE_UNUSED gt_pointer_operator op,
      ATTRIBUTE_UNUSED void * cookie)
{
  if (ranges_table != NULL) {
    size_t i0;
    for (i0 = 0; i0 != (size_t)(ranges_table_allocated) && ((void *)ranges_table == this_obj); i0++) {
    }
    if ((void *)(&ranges_table) == this_obj)
      op (&(ranges_table), cookie);
  }
}

static void gt_pch_na_ranges_table (void *);
static void
gt_pch_na_ranges_table (ATTRIBUTE_UNUSED void *x_p)
{
  if (ranges_table != NULL) {
    size_t i0;
    for (i0 = 0; i0 != (size_t)(ranges_table_allocated); i0++) {
    }
    gt_pch_note_object (ranges_table, &ranges_table, gt_pch_pa_ranges_table);
  }
}

static void gt_ggc_ma_abbrev_die_table (void *);
static void
gt_ggc_ma_abbrev_die_table (ATTRIBUTE_UNUSED void *x_p)
{
  if (abbrev_die_table != NULL) {
    size_t i0;
    for (i0 = 0; i0 != (size_t)(abbrev_die_table_allocated); i0++) {
      gt_ggc_m_10die_struct (abbrev_die_table[i0]);
    }
    ggc_mark (abbrev_die_table);
  }
}

static void gt_pch_pa_abbrev_die_table
    (void *, void *, gt_pointer_operator, void *);
static void gt_pch_pa_abbrev_die_table (ATTRIBUTE_UNUSED void *this_obj,
      ATTRIBUTE_UNUSED void *x_p,
      ATTRIBUTE_UNUSED gt_pointer_operator op,
      ATTRIBUTE_UNUSED void * cookie)
{
  if (abbrev_die_table != NULL) {
    size_t i0;
    for (i0 = 0; i0 != (size_t)(abbrev_die_table_allocated) && ((void *)abbrev_die_table == this_obj); i0++) {
      if ((void *)(abbrev_die_table) == this_obj)
        op (&(abbrev_die_table[i0]), cookie);
    }
    if ((void *)(&abbrev_die_table) == this_obj)
      op (&(abbrev_die_table), cookie);
  }
}

static void gt_pch_na_abbrev_die_table (void *);
static void
gt_pch_na_abbrev_die_table (ATTRIBUTE_UNUSED void *x_p)
{
  if (abbrev_die_table != NULL) {
    size_t i0;
    for (i0 = 0; i0 != (size_t)(abbrev_die_table_allocated); i0++) {
      gt_pch_n_10die_struct (abbrev_die_table[i0]);
    }
    gt_pch_note_object (abbrev_die_table, &abbrev_die_table, gt_pch_pa_abbrev_die_table);
  }
}

EXPORTED_CONST struct ggc_root_tab gt_ggc_r_gt_dwarf2out_h[] = {
  {
    &addr_index_table,
    1,
    sizeof (addr_index_table),
    &gt_ggc_mx_hash_table_addr_hasher_,
    &gt_pch_nx_hash_table_addr_hasher_
  },
  {
    &generic_type_instances,
    1,
    sizeof (generic_type_instances),
    &gt_ggc_mx_vec_tree_va_gc_,
    &gt_pch_nx_vec_tree_va_gc_
  },
  {
    &tmpl_value_parm_die_table,
    1,
    sizeof (tmpl_value_parm_die_table),
    &gt_ggc_mx_vec_die_arg_entry_va_gc_,
    &gt_pch_nx_vec_die_arg_entry_va_gc_
  },
  {
    &last_emitted_file,
    1,
    sizeof (last_emitted_file),
    &gt_ggc_mx_dwarf_file_data,
    &gt_pch_nx_dwarf_file_data
  },
  {
    &ranges_by_label,
    1,
    sizeof (ranges_by_label),
    &gt_ggc_ma_ranges_by_label,
    &gt_pch_na_ranges_by_label
  },
  {
    &ranges_table,
    1,
    sizeof (ranges_table),
    &gt_ggc_ma_ranges_table,
    &gt_pch_na_ranges_table
  },
  {
    &macinfo_table,
    1,
    sizeof (macinfo_table),
    &gt_ggc_mx_vec_macinfo_entry_va_gc_,
    &gt_pch_nx_vec_macinfo_entry_va_gc_
  },
  {
    &pubtype_table,
    1,
    sizeof (pubtype_table),
    &gt_ggc_mx_vec_pubname_entry_va_gc_,
    &gt_pch_nx_vec_pubname_entry_va_gc_
  },
  {
    &pubname_table,
    1,
    sizeof (pubname_table),
    &gt_ggc_mx_vec_pubname_entry_va_gc_,
    &gt_pch_nx_vec_pubname_entry_va_gc_
  },
  {
    &separate_line_info,
    1,
    sizeof (separate_line_info),
    &gt_ggc_mx_vec_dw_line_info_table__va_gc_,
    &gt_pch_nx_vec_dw_line_info_table__va_gc_
  },
  {
    &cold_text_section_line_info,
    1,
    sizeof (cold_text_section_line_info),
    &gt_ggc_mx_dw_line_info_table,
    &gt_pch_nx_dw_line_info_table
  },
  {
    &text_section_line_info,
    1,
    sizeof (text_section_line_info),
    &gt_ggc_mx_dw_line_info_table,
    &gt_pch_nx_dw_line_info_table
  },
  {
    &abbrev_die_table,
    1,
    sizeof (abbrev_die_table),
    &gt_ggc_ma_abbrev_die_table,
    &gt_pch_na_abbrev_die_table
  },
  {
    &cached_dw_loc_list_table,
    1,
    sizeof (cached_dw_loc_list_table),
    &gt_ggc_mx_hash_table_dw_loc_list_hasher_,
    &gt_pch_nx_hash_table_dw_loc_list_hasher_
  },
  {
    &call_arg_locations,
    1,
    sizeof (call_arg_locations),
    &gt_ggc_mx_call_arg_loc_node,
    &gt_pch_nx_call_arg_loc_node
  },
  {
    &decl_loc_table,
    1,
    sizeof (decl_loc_table),
    &gt_ggc_mx_hash_table_decl_loc_hasher_,
    &gt_pch_nx_hash_table_decl_loc_hasher_
  },
  {
    &common_block_die_table,
    1,
    sizeof (common_block_die_table),
    &gt_ggc_mx_hash_table_block_die_hasher_,
    &gt_pch_nx_hash_table_block_die_hasher_
  },
  {
    &decl_die_table,
    1,
    sizeof (decl_die_table),
    &gt_ggc_mx_hash_table_decl_die_hasher_,
    &gt_pch_nx_hash_table_decl_die_hasher_
  },
  {
    &file_table,
    1,
    sizeof (file_table),
    &gt_ggc_mx_hash_table_dwarf_file_hasher_,
    &gt_pch_nx_hash_table_dwarf_file_hasher_
  },
  {
    &deferred_asm_name,
    1,
    sizeof (deferred_asm_name),
    &gt_ggc_mx_limbo_die_struct,
    &gt_pch_nx_limbo_die_struct
  },
  {
    &limbo_die_list,
    1,
    sizeof (limbo_die_list),
    &gt_ggc_mx_limbo_die_struct,
    &gt_pch_nx_limbo_die_struct
  },
  {
    &comdat_type_list,
    1,
    sizeof (comdat_type_list),
    &gt_ggc_mx_comdat_type_node,
    &gt_pch_nx_comdat_type_node
  },
  {
    &single_comp_unit_die,
    1,
    sizeof (single_comp_unit_die),
    &gt_ggc_mx_die_struct,
    &gt_pch_nx_die_struct
  },
  {
    &current_unit_personality,
    1,
    sizeof (current_unit_personality),
    &gt_ggc_mx_rtx_def,
    &gt_pch_nx_rtx_def
  },
  {
    &decltype_auto_die,
    1,
    sizeof (decltype_auto_die),
    &gt_ggc_mx_die_struct,
    &gt_pch_nx_die_struct
  },
  {
    &auto_die,
    1,
    sizeof (auto_die),
    &gt_ggc_mx_die_struct,
    &gt_pch_nx_die_struct
  },
  {
    &cold_text_section,
    1,
    sizeof (cold_text_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &skeleton_debug_str_hash,
    1,
    sizeof (skeleton_debug_str_hash),
    &gt_ggc_mx_hash_table_indirect_string_hasher_,
    &gt_pch_nx_hash_table_indirect_string_hasher_
  },
  {
    &debug_str_hash,
    1,
    sizeof (debug_str_hash),
    &gt_ggc_mx_hash_table_indirect_string_hasher_,
    &gt_pch_nx_hash_table_indirect_string_hasher_
  },
  {
    &fde_vec,
    1,
    sizeof (fde_vec),
    &gt_ggc_mx_vec_dw_fde_ref_va_gc_,
    &gt_pch_nx_vec_dw_fde_ref_va_gc_
  },
  {
    &debug_frame_section,
    1,
    sizeof (debug_frame_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &debug_ranges_section,
    1,
    sizeof (debug_ranges_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &debug_str_offsets_section,
    1,
    sizeof (debug_str_offsets_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &debug_str_dwo_section,
    1,
    sizeof (debug_str_dwo_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &debug_str_section,
    1,
    sizeof (debug_str_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &debug_pubtypes_section,
    1,
    sizeof (debug_pubtypes_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &debug_pubnames_section,
    1,
    sizeof (debug_pubnames_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &debug_loc_section,
    1,
    sizeof (debug_loc_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &debug_skeleton_line_section,
    1,
    sizeof (debug_skeleton_line_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &debug_line_section,
    1,
    sizeof (debug_line_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &debug_macinfo_section,
    1,
    sizeof (debug_macinfo_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &debug_addr_section,
    1,
    sizeof (debug_addr_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &debug_aranges_section,
    1,
    sizeof (debug_aranges_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &debug_skeleton_abbrev_section,
    1,
    sizeof (debug_skeleton_abbrev_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &debug_abbrev_section,
    1,
    sizeof (debug_abbrev_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &debug_skeleton_info_section,
    1,
    sizeof (debug_skeleton_info_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &debug_info_section,
    1,
    sizeof (debug_info_section),
    &gt_ggc_mx_section,
    &gt_pch_nx_section
  },
  {
    &decl_scope_table,
    1,
    sizeof (decl_scope_table),
    &gt_ggc_mx_vec_tree_va_gc_,
    &gt_pch_nx_vec_tree_va_gc_
  },
  {
    &incomplete_types,
    1,
    sizeof (incomplete_types),
    &gt_ggc_mx_vec_tree_va_gc_,
    &gt_pch_nx_vec_tree_va_gc_
  },
  {
    &used_rtx_array,
    1,
    sizeof (used_rtx_array),
    &gt_ggc_mx_vec_rtx_va_gc_,
    &gt_pch_nx_vec_rtx_va_gc_
  },
  LAST_GGC_ROOT_TAB
};

EXPORTED_CONST struct ggc_root_tab gt_pch_rs_gt_dwarf2out_h[] = {
  { &label_num, 1, sizeof (label_num), NULL, NULL },
  { &poc_label_num, 1, sizeof (poc_label_num), NULL, NULL },
  { &loclabel_num, 1, sizeof (loclabel_num), NULL, NULL },
  { &have_location_lists, 1, sizeof (have_location_lists), NULL, NULL },
  { &ranges_by_label_in_use, 1, sizeof (ranges_by_label_in_use), NULL, NULL },
  { &ranges_by_label_allocated, 1, sizeof (ranges_by_label_allocated), NULL, NULL },
  { &ranges_table_in_use, 1, sizeof (ranges_table_in_use), NULL, NULL },
  { &ranges_table_allocated, 1, sizeof (ranges_table_allocated), NULL, NULL },
  { &abbrev_die_table_in_use, 1, sizeof (abbrev_die_table_in_use), NULL, NULL },
  { &abbrev_die_table_allocated, 1, sizeof (abbrev_die_table_allocated), NULL, NULL },
  { &cold_text_section_used, 1, sizeof (cold_text_section_used), NULL, NULL },
  { &text_section_used, 1, sizeof (text_section_used), NULL, NULL },
  { &have_multiple_function_sections, 1, sizeof (have_multiple_function_sections), NULL, NULL },
  { &dw2_string_counter, 1, sizeof (dw2_string_counter), NULL, NULL },
  LAST_GGC_ROOT_TAB
};

