#!/bin/bash

targetdir=$1
git_result_val=0



function modifyFile(){
    changefile=$1
   # echo " [Remove spave : $changefile] "
    sed -i 's/\s*$//' $changefile
}

function choseRemvoeBackSpaceFile(){
    #chose todo file#
    git_result_val=$(git status|grep "：")
    #remove str before :#
    git_result_val=$(echo "$git_result_val" |sed -r 's/.*(：)/\1/')
    #remove :#
    git_result_val=$(echo "$git_result_val" |sed -r 's/.*(：)//')
}


function gitPreProcess(){
    cd $1
    git add . > /dev/null
    clear
}

function gitPostProcess(){
    git add .  > /dev/null
}

function gitCommit(){
    git add .  > /dev/null
   # git commit -m "remove space"  > /dev/null
}


function doFileRemveBack(){
    for file in $git_result_val
    do
        if [ -f $file ]
        then
            modifyFile $file
        fi
    done
}

##############script entry###################

if [ $# -ne 1 ]
then
	echo "please input serchpath"
    exit -1
fi

#step1: git add#
gitPreProcess $targetdir

#step2:chose process file#
choseRemvoeBackSpaceFile

#step3:git add file#
gitPostProcess

#step4:reomve space#
doFileRemveBack






