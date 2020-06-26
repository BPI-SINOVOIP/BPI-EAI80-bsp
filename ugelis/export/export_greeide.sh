#!/bin/bash

sample_name=
EXPORT_NAME=ugelis_eclipse
BOARD_NAME=gm6721_evb_176pin
CURR_DIR=$(cd `dirname $0`; cd ..; pwd)
PROJECT_DIR=$CURR_DIR/samples/gm6721_prj/example
sample_list=$(ls -l $PROJECT_DIR  | awk '/^d/ {print $NF}')
all_dep=

function list_project()
{
    echo "============Valid project is followings============="
    for pro in $sample_list
    do
        echo $pro
    done
    echo
}

function check_project()
{
    echo
    echo -n "project name ["${sample_name}"]: "
    for pro in $sample_list
    do
        if [ $sample_name = $pro ]; then
            return 1 
        fi
    done
    return 0 
}

function compile_project()
{
    source ./ugelis-env.sh
    mkdir -p $PROJECT_DIR/$sample_name/build
    cd $PROJECT_DIR/$sample_name/build
    cmake ../
    make
}

function copy_all_header()
{
    echo -e "\n===> copy_all_header"
    if [ ! -d $CURR_DIR/$EXPORT_NAME/ugelis/arch/arm/core/cortex_m ]; then
        mkdir -p $CURR_DIR/$EXPORT_NAME/ugelis/arch/arm/core/cortex_m
    fi
    cp $CURR_DIR/arch/arm/core/cortex_m/vector_table.h $CURR_DIR/$EXPORT_NAME/ugelis/arch/arm/core/cortex_m/

    cp $CURR_DIR/arch/arm/include -ar $CURR_DIR/$EXPORT_NAME/ugelis/arch/arm/

    if [ ! -d $CURR_DIR/$EXPORT_NAME/ugelis/arch/arm/soc/gree_gm/gm6721x ]; then
        mkdir -p $CURR_DIR/$EXPORT_NAME/ugelis/arch/arm/soc/gree_gm/gm6721x
    fi
    cp $CURR_DIR/arch/arm/soc/gree_gm/gm6721x/soc.h $CURR_DIR/$EXPORT_NAME/ugelis/arch/arm/soc/gree_gm/gm6721x/

    if [ ! -d $CURR_DIR/$EXPORT_NAME/ugelis/boards/arm/$BOARD_NAME ]; then
        mkdir -p $CURR_DIR/$EXPORT_NAME/ugelis/boards/arm/$BOARD_NAME
    fi
    cp $CURR_DIR/boards/arm/$BOARD_NAME/board.h $CURR_DIR/$EXPORT_NAME/ugelis/boards/arm/$BOARD_NAME/
    cp $CURR_DIR/boards/arm/$BOARD_NAME/mem_cfg.h $CURR_DIR/$EXPORT_NAME/ugelis/boards/arm/$BOARD_NAME/

    if [ ! -d $CURR_DIR/$EXPORT_NAME/ugelis/drivers/serial ]; then
        mkdir -p $CURR_DIR/$EXPORT_NAME/ugelis/drivers/serial
    fi
    cp $CURR_DIR/drivers/serial/uart_gm.h $CURR_DIR/$EXPORT_NAME/ugelis/drivers/serial/

    if [ ! -d $CURR_DIR/$EXPORT_NAME/ugelis/drivers/gpio ]; then
        mkdir -p $CURR_DIR/$EXPORT_NAME/ugelis/drivers/gpio
    fi
    cp $CURR_DIR/drivers/gpio/gpio_gm.h $CURR_DIR/$EXPORT_NAME/ugelis/drivers/gpio/
    cp $CURR_DIR/drivers/gpio/gpio_utils.h $CURR_DIR/$EXPORT_NAME/ugelis/drivers/gpio/

    if [ ! -d $CURR_DIR/$EXPORT_NAME/ugelis/ext/fs ]; then
        mkdir -p $CURR_DIR/$EXPORT_NAME/ugelis/ext/fs
    fi
    cp $CURR_DIR/ext/fs/fat -ar $CURR_DIR/$EXPORT_NAME/ugelis/ext/fs/

    if [ ! -d $CURR_DIR/$EXPORT_NAME/ugelis/ext/hal ]; then
        mkdir -p $CURR_DIR/$EXPORT_NAME/ugelis/ext/hal
    fi
    cp $CURR_DIR/ext/hal/cmsis -ar $CURR_DIR/$EXPORT_NAME/ugelis/ext/hal/
    
    cd $CURR_DIR/ext/hal/gree
    gree_headers=`find ./ -name "*.h"`
    for header in $gree_headers
    do
    	dir=${header%/*}     #delete the last /xxx.h
        if [ ! -d $CURR_DIR/$EXPORT_NAME/ugelis/ext/hal/gree/$dir ]; then
            mkdir -p $CURR_DIR/$EXPORT_NAME/ugelis/ext/hal/gree/$dir
        fi
    	cp $header $CURR_DIR/$EXPORT_NAME/ugelis/ext/hal/gree/$header
        echo "cp from $header to $CURR_DIR/$EXPORT_NAME/ugelis/ext/hal/gree/$header"
    done
    cd -

    cp $CURR_DIR/include -ar $CURR_DIR/$EXPORT_NAME/ugelis/

    if [ ! -d $CURR_DIR/$EXPORT_NAME/ugelis/kernel ]; then
        mkdir -p $CURR_DIR/$EXPORT_NAME/ugelis/kernel
    fi
    cp $CURR_DIR/kernel/include -ar $CURR_DIR/$EXPORT_NAME/ugelis/kernel/

    if [ ! -d $CURR_DIR/$EXPORT_NAME/ugelis/lib/libc/minimal ]; then
        mkdir -p $CURR_DIR/$EXPORT_NAME/ugelis/lib/libc/minimal
    fi
    cp $CURR_DIR/lib/libc/minimal/include -ar $CURR_DIR/$EXPORT_NAME/ugelis/lib/libc/minimal/
    cp $CURR_DIR/lib/libc/minimal/include -ar $CURR_DIR/$EXPORT_NAME/ugelis/lib/libc/minimal/

    cp $PROJECT_DIR/$sample_name/build/ugelis/include/generated -ar $CURR_DIR/$EXPORT_NAME/ugelis/include/
}

function copy_all_dep()
{
    echo -e "\n===> copy_all_depends"

    #cp all dependences
    cd $PROJECT_DIR/$sample_name/build/ugelis
    all_dep=`find ./ -name "*.obj"`
    for dep in $all_dep
    do
	    dep=`echo $dep | sed -e 's|/CMakeFiles/.*dir/|/|g'`
	    dep=`echo $dep | sed -e 's|/boards/boards/|/boards/|g'`
        file=${dep%.*}    #delete .obj
        dir=${file%/*}    #delete the last /xxx.c

        if [ ! -d $CURR_DIR/$EXPORT_NAME/ugelis/$dir ]; then
            mkdir -p $CURR_DIR/$EXPORT_NAME/ugelis/$dir
        fi

        # c file
        if [ -f $CURR_DIR/$file ]; then
            echo "cp from $CURR_DIR/$file to $CURR_DIR/$EXPORT_NAME/ugelis/$file"
            cp $CURR_DIR/$file $CURR_DIR/$EXPORT_NAME/ugelis/$file
        fi

        # header file
        header_file=${file/.c/.h}
        if [ -f $CURR_DIR/$header_file ]; then
            echo "cp from $CURR_DIR/$header_file to $CURR_DIR/$EXPORT_NAME/ugelis/$header_file"
            cp $CURR_DIR/$header_file $CURR_DIR/$EXPORT_NAME/ugelis/$header_file
        fi
    done

    echo

    #cp sample files
    if [ ! -d $CURR_DIR/$EXPORT_NAME/ugelis/sample ]; then
        mkdir -p $CURR_DIR/$EXPORT_NAME/ugelis/sample
    fi
    cp $PROJECT_DIR/$sample_name/src/* -ar $CURR_DIR/$EXPORT_NAME/ugelis/sample/

    #cp link file
    if [ ! -d $CURR_DIR/$EXPORT_NAME/ugelis/include/arch/arm/cortex_m/scripts ]; then
        mkdir -p $CURR_DIR/$EXPORT_NAME/ugelis/include/arch/arm/cortex_m/scripts
    fi
    cp -f $PROJECT_DIR/$sample_name/build/ugelis/linker.cmd $CURR_DIR/$EXPORT_NAME/ugelis/include/arch/arm/cortex_m/scripts/linker.ld

    #need to delete these in linker.cmd
        #.intList :
        #{
        #KEEP(*(.irq_info))
        # KEEP(*(.intList))
        # } > IDT_LIST
    ld_file=$CURR_DIR/$EXPORT_NAME/ugelis/include/arch/arm/cortex_m/scripts/linker.ld
    line_num=$(sed -n '$=' ${ld_file})
    sed -i $(($line_num-5)),$(($line_num-1))d ${ld_file}
}

function delete_item()
{
    rm -rf $CURR_DIR/$EXPORT_NAME/ugelis/ext/fs/fat/option/cc932.c #compile error
    rm -rf $CURR_DIR/$EXPORT_NAME/ugelis/ext/fs/fat/option/cc936.c #compile error
    rm -rf $CURR_DIR/$EXPORT_NAME/ugelis/ext/fs/fat/option/cc949.c #compile error
    rm -rf $CURR_DIR/$EXPORT_NAME/ugelis/ext/fs/fat/option/cc950.c #compile error
    rm -rf $CURR_DIR/$EXPORT_NAME/ugelis/ext/fs/fat/option/ccsbcs.c #compile error
    rm -rf $CURR_DIR/$EXPORT_NAME/ugelis/ext/fs/fat/option/syscall.c #compile error
    rm -rf $CURR_DIR/$EXPORT_NAME/ugelis/ext/fs/fat/option/unicode.c #compile error
    
    # following gree driver is not ready, so rm them this moment
    #rm -rf $CURR_DIR/$EXPORT_NAME/ugelis/ext/hal/gree/driver/sai
    #rm -rf $CURR_DIR/$EXPORT_NAME/ugelis/ext/hal/gree/driver/vout

    rm -rf $CURR_DIR/$EXPORT_NAME/ugelis/ext/hal/gree/include/driver/hal_i2c.h #compile error
    
    rm -rf $CURR_DIR/$EXPORT_NAME/ugelis/include/arch/x86
    rm -rf $CURR_DIR/$EXPORT_NAME/ugelis/include/drivers/clock_control/stm32*
    rm -rf $CURR_DIR/$EXPORT_NAME/ugelis/ext/fs/fat/diskio.c

    rm -rf $CURR_DIR/$EXPORT_NAME/ugelis/include/generated/syscall_dispatch.c

    rm -rf `find $CURR_DIR/$EXPORT_NAME/ugelis -name "Kbuild*" -o -name "Kconfig*"`
    rm -rf `find $CURR_DIR/$EXPORT_NAME/ugelis -name "*.pdf" -o -name "*.rtf"`
    rm -rf `find $CURR_DIR/$EXPORT_NAME/ugelis -name "Makefile"`
}

function create_dot_project()
{
    cp $CURR_DIR/export/.project.tmpl $CURR_DIR/$EXPORT_NAME/.project
    sed -i "s|{{exportProjectName}}|$EXPORT_NAME|g" $CURR_DIR/$EXPORT_NAME/.project
}

function create_dot_cproject()
{
    cp $CURR_DIR/export/.cproject.tmpl $CURR_DIR/$EXPORT_NAME/.cproject
    sed -i "s|{{exportProjectName}}|$EXPORT_NAME|g" $CURR_DIR/$EXPORT_NAME/.cproject
}

function create_makefile_targets()
{
    cp $CURR_DIR/export/makefile.targets.tmpl $CURR_DIR/$EXPORT_NAME/makefile.targets
    sed -i "s|{{exportProjectName}}|$EXPORT_NAME|g" $CURR_DIR/$EXPORT_NAME/makefile.targets

    linker="..\/ugelis\/include\/arch\/arm\/cortex_m\/scripts\/linker.ld"
    sed -i "s|{{linkerName}}|$linker|g" $CURR_DIR/$EXPORT_NAME/makefile.targets
}

function export_project()
{
    echo -e "Exporting project [" $PROJECT_DIR/$sample_name "]\n"

    #1, compile sample firstly
    compile_project

    #2, copy dependences
    copy_all_header
    copy_all_dep

    #3, delete usless file
    delete_item

    #4, creat GreeIDE compiling files
    create_dot_project
    create_dot_cproject
    create_makefile_targets
}



#=======================main========================#

if [ $# = 0 ]; then
    echo "No project name input!"
    list_project

elif [ "$1" = "list" ]; then
    list_project

elif [ "$1" = "help" ]; then
    echo "Usage: ./export_for_GreeIDE.sh <sample_name> [export_name]"
    echo "    1, [export_name] default is ugelis_eclipse"
    echo -e "    2, execute [./export_for_GreeIDE.sh list] can list all sample project name\n"

else
    sample_name=$1
    if [ $# -gt 1 ]; then #notice: $# is the count of parameters send to this script, exclude the script itself
        EXPORT_NAME=$2
    fi

    check_project
    if [ $? = 1 ]; then
        echo -e "Valid\n"
        if [ -d $CURR_DIR/$EXPORT_NAME/ ]; then
            rm -rf $CURR_DIR/$EXPORT_NAME/
        fi
        export_project

        #genrate config folder
        $CURR_DIR/export/config.py -from $PROJECT_DIR/$sample_name/build/ugelis -to $CURR_DIR/$EXPORT_NAME/ugelis -template $CURR_DIR/export

        echo -e "\nCongratulations! Export project successfully!!\n"
    else 
        echo -e "Invalid\n"
        list_project
    fi

fi

