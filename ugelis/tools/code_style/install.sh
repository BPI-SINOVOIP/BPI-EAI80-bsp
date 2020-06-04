#!/bin/sh

script_dir=`dirname $0`
git_dir=`git rev-parse --git-dir`
cp -v $script_dir/pre-commit $git_dir/hooks/pre-commit
cp -v $script_dir/auto_reformat_code.py $git_dir/hooks/auto_reformat_code.py
