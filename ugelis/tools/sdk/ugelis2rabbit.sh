#/bin/bash

usage() {
    echo -e ""
    echo -e "                      ------------"
    echo -e "                      Script Usage"
    echo -e "                      ------------"
    echo -e ""
    echo -e "NAME"
    echo -e "        ugelis2rabbit.sh - Copy drivers from ugelis os to rabbit keil project."
    echo -e ""
    echo -e "SYNOPSIS"
    echo -e "        ugelis2rabbit.sh [MODULE_NAME] [EXTRA_MODULE]"
    echo -e ""
    echo -e "DESCRIPTION"
    echo -e "        MODULE_NAME"
    echo -e "            module name such as rtc,lvd,adc, etc."
    echo -e "            which is same as the driver name. eg: gm_hal_[MODULE_NAME].c"
    echo -e ""
    echo -e "        EXTRA_MODULE"
    echo -e "            extral module name, for example, rtc needs utc module."
    echo -e ""
    echo -e "EXAMPLE"
    echo -e "        ugelis2rabbit.sh lvd"
    echo -e "        ugelis2rabbit.sh rtc utc"
    echo -e ""
    exit -1
}

function copy_file(){

    #copy driver directory
    echo "copy driver directory:"
    src_dir=$UGELIS_BASE/ext/hal/gree/driver/$1
    dst_dir=$UGELIS_BASE/keil_project/rabbit/driver/$1/

    if [ -d $src_dir ]; then
        cp -rfv $src_dir/* $dst_dir
    else
        echo "$src_dir does not exist."
    fi
    
    #copy header files
    echo "copy header files:"
    for var in $@
        do
            src_file=$UGELIS_BASE/ext/hal/gree/include/driver/gm_hal_$var.h
            dst_file=$UGELIS_BASE/keil_project/rabbit/include/driver/gm_hal_$var.h
            if [ -f $src_file ]; then
                cp -v $src_file $dst_file
            else
                echo "$src_file does not exist."
            fi
        done
}

if [ $# -gt 0 ]; then
    copy_file $@
else
    usage
fi
