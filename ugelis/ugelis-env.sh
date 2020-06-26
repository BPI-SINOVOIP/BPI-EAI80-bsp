#!/bin/bash
#
# Copyright (c) 2015 Wind River Systems, Inc.
#
# SPDX-License-Identifier: Apache-2.0
#

# In zsh the value of $0 depends on the FUNCTION_ARGZERO option which is
# set by default. FUNCTION_ARGZERO, when it is set, sets $0 temporarily
# to the name of the function/script when executing a shell function or
# sourcing a script. POSIX_ARGZERO option, when it is set, exposes the
# original value of $0 in spite of the current FUNCTION_ARGZERO setting.
#
# Note: The version of zsh need to be 5.0.6 or above. Any versions below
# 5.0.6 maybe encoutner errors when sourcing this script.
if [ -n "$ZSH_VERSION" ]; then
	DIR="${(%):-%N}"
	if [ $options[posixargzero] != "on" ]; then
		setopt posixargzero
		NAME=$(basename -- "$0")
		setopt posixargzero
	else
		NAME=$(basename -- "$0")
	fi
else
	DIR="${BASH_SOURCE[0]}"
	NAME=$(basename -- "$0")
fi

if [ "X$NAME" "==" "Xugelis-env.sh" ]; then
    echo "Source this file (do NOT execute it!) to set the uGelis Kernel environment."
    exit
fi

# You can further customize your environment by creating a bash script called
# .ugelisrc in your home directory. It will be automatically
# run (if it exists) by this script.

if uname | grep -q "MINGW"; then
    win_build=1
    PWD_OPT="-W"
else
    win_build=0
    PWD_OPT=""
fi

# identify OS source tree root directory
export UGELIS_BASE=$( builtin cd "$( dirname "$DIR" )" && pwd ${PWD_OPT})
unset PWD_OPT

scripts_path=${UGELIS_BASE}/scripts
if [ "$win_build" -eq 1 ]; then
    scripts_path=$(echo "/$scripts_path" | sed 's/\\/\//g' | sed 's/://')
fi
unset win_build
if ! echo "${PATH}" | grep -q "${scripts_path}"; then
    export PATH=${scripts_path}:${PATH}
fi
unset scripts_path

# enable custom environment settings
ugelis_answer_file=~/ugelis-env_install.bash
[ -f ${ugelis_answer_file} ] && {
	echo "Warning: Please rename ~/ugelis-env_install.bash to ~/.ugelisrc";
	. ${ugelis_answer_file};
}
unset ugelis_answer_filei
ugelis_answer_file=~/.ugelisrc
[ -f ${ugelis_answer_file} ] &&  . ${ugelis_answer_file};
unset ugelis_answer_file
	
export UGELIS_TOOLCHAIN_VARIANT=ugelis
#export UGELIS_SDK_INSTALL_DIR=/opt/ugelis-sdk
export UGELIS_SDK_INSTALL_DIR=${UGELIS_BASE}/tools/ugelis-sdk

GCC_CORE_SOURCE_DIR=${UGELIS_BASE}/arch/arm/core-gcc
GCC_CORE_LINK_DIR=${UGELIS_BASE}/arch/arm/core

#script for kelis
KELIS_DIR=${UGELIS_BASE}/kelis_example
UGELIS_EXAMPLE=${UGELIS_BASE}/ugelis_example
ARMCC_CORE_SOURCE_DIR=${UGELIS_BASE}/arch/arm/core-armcc

#for abstract core dir for gcc-core#
#if [ -d "${GCC_CORE_LINK_DIR}" ]; then
#	rm -rf ${GCC_CORE_LINK_DIR};
#fi
#if [ -d "${KELIS_DIR}" ]; then #kelis
if [ -d "${UGELIS_EXAMPLE}" ]; then #gcc
    if [ ! -h ${GCC_CORE_LINK_DIR} ] ; then
      ln -s ${GCC_CORE_SOURCE_DIR} ${GCC_CORE_LINK_DIR}
    fi
else #kelis
    ln -s ${ARMCC_CORE_SOURCE_DIR} ${GCC_CORE_LINK_DIR}
fi

#link ulibs to ./ext
ULIBS_DIR=${UGELIS_BASE}/../ulibs
EXT_ULIBS_DIR=${UGELIS_BASE}/ext
EXT_ULIBS_GREE_DIR=${UGELIS_BASE}/ext/hal/gree
if [ ! -d "${EXT_ULIBS_GREE_DIR}" ]; then
    #copy ulibs/* to ext/*
    ulibs_files=`ls ${ULIBS_DIR}`
    for x in $ulibs_files; do
        path=${EXT_ULIBS_DIR}/$x
        src_path=${ULIBS_DIR}/$x
        if [ ! -d $path ];then
            if [ ! -f $path ];then
                ln -s $src_path $path
            fi
        #since ext/hal exists, to link file more depthly
        else
            depth_1_files=`ls $src_path`
            for xx in $depth_1_files; do
                ln -s $src_path/$xx $path/$xx
            done
        fi
    done
fi

function croot()
{
	cd $UGELIS_BASE
}
###config env finifsh###
