#!/bin/bash
exec_dir=../../../
exec_rm_sh=remove_git_trace_file_end_space.sh
git_trace_ugelis=ugelis
git_trace_ulibs=ulibs
current_dir=$(pwd)

#echo -e "start rm $git_trace_ugelis end space"
cp $exec_rm_sh $exec_dir
cd $exec_dir
./$exec_rm_sh $git_trace_ugelis
./$exec_rm_sh $git_trace_ulibs
rm -rf $exec_rm_sh
cd $current_dir
#echo -e "finish rm $git_trace_ugelis end space"
