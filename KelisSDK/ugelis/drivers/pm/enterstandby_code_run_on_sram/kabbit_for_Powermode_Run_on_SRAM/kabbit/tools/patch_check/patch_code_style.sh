#!/bin/bash

exec_dir=../../../
exec_rm_sh=remove_git_trace_file_end_space.sh
git_trace_kabbit=kabbit
git_trace_ulibs=ulibs
current_dir=$(pwd)

cp $exec_rm_sh $exec_dir
cd $exec_dir
./$exec_rm_sh $git_trace_kabbit
./$exec_rm_sh $git_trace_ulibs
rm -rf $exec_rm_sh
cd $current_dir

