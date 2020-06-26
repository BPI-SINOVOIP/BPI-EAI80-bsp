#!/bin/bash

SDK_NAME=
CURR_DIR=$(cd `dirname $0`; cd ..; pwd)
SDK_DIR=$CURR_DIR/../../uGelisSDK
SDK_UGELIS_DIR=$SDK_DIR/ugelis
SDK_ULIBS_DIR=$SDK_DIR/ulibs
SDK_KELIS_EXPLAME_DIR=$SDK_UGELIS_DIR/kelis_example
SDK_UGELIS_EXPLAME_DIR=$SDK_UGELIS_DIR/ugelis_example
KELIS_DIR=$RELEASE_KEILS_DIR/
all_dep=

function remove_gitfile()
{
    echo "============remove git folder============="
    rm $SDK_DIR/.repo  -rf  
    rm $SDK_UGELIS_DIR/.git  -rf     
    rm $SDK_UGELIS_DIR/.git*		
    rm $SDK_ULIBS_DIR/.git  -rf 
    rm $SDK_ULIBS_DIR/.git*      
    rm $SDK_KELIS_EXPLAME_DIR/.git  -rf  
    rm $SDK_KELIS_EXPLAME_DIR/.git*      
}

function remove_keilotherdir()
{
	echo "============remove other dir=============\n"
    rm $SDK_UGELIS_DIR/ztest  -rf			
    rm $SDK_UGELIS_DIR/ugelis_example  -rf    
    rm $SDK_UGELIS_DIR/tests  -rf				
    rm $SDK_UGELIS_DIR/scripts  -rf	
    rm $SDK_UGELIS_DIR/export  -rf			
    rm $SDK_UGELIS_DIR/samples  -rf			
    rm $SDK_UGELIS_DIR/dts  -rf				
    rm $SDK_UGELIS_DIR/cmake  -rf				
}

function remove_keilfile()
{
	echo "============remove file=============\n"
	find $SDK_UGELIS_DIR -maxdepth 1 -type f -exec rm -fr {} \;
    find $SDK_UGELIS_DIR -iname "cmake*" -exec rm -fr {} \;
    find $SDK_UGELIS_DIR -iname "*.cmake" -exec rm -fr {} \;
    find $SDK_UGELIS_DIR -name "Kconfig" -type f -exec rm -fr {} \;
    find $SDK_UGELIS_DIR -name "Kconfig.*" -type f -exec rm -fr {} \;
    find $SDK_UGELIS_DIR -iname "readme" -type f -exec rm -fr {} \;
    find $SDK_UGELIS_DIR -name "*.txt" -type f -exec rm -fr {} \;
    find $SDK_UGELIS_DIR -name "*.rst" -type f -exec rm -fr {} \;
    find $SDK_UGELIS_DIR -name "*.py" -type f -exec rm -fr {} \;
    find $SDK_UGELIS_DIR -name "*.yaml" -type f -exec rm -fr {} \;
    find $SDK_UGELIS_DIR -iname "*defconfig" -type f -exec rm -fr {} \;
    find $SDK_UGELIS_DIR -iname "*.conf" -type f -exec rm -fr {} \;
    
    find $SDK_ULIBS_DIR -iname "cmake*" -exec rm -fr {} \;
    find $SDK_ULIBS_DIR -iname "*.cmake" -exec rm -fr {} \;
    find $SDK_ULIBS_DIR -name "Kconfig" -type f -exec rm -fr {} \;
    find $SDK_ULIBS_DIR -name "Kconfig.*" -type f -exec rm -fr {} \;
    find $SDK_ULIBS_DIR -iname "readme" -type f -exec rm -fr {} \;
    find $SDK_ULIBS_DIR -name "*.txt" -type f -exec rm -fr {} \;
    find $SDK_ULIBS_DIR -name "*.rst" -type f -exec rm -fr {} \;
    find $SDK_ULIBS_DIR -name "*.py" -type f -exec rm -fr {} \;
    find $SDK_ULIBS_DIR -name "*.yaml" -type f -exec rm -fr {} \;
    find $SDK_ULIBS_DIR -iname "*defconfig" -type f -exec rm -fr {} \;
    find $SDK_ULIBS_DIR -iname "*.conf" -type f -exec rm -fr {} \;
}

function remove_keilplatform()
{
	echo "============remove platform=============\n"
	rm $SDK_UGELIS_DIR/arch/arm/core-gcc  -rf
}

function release_keilproject()
{
    echo "===> release_keilproject\n"

    if [ ! -d $SDK_DIR ]; then
        mkdir -p $SDK_DIR
    fi
    cp $CURR_DIR/../* $SDK_DIR -rf

    #1, compile sample firstly
    remove_gitfile

    #2, copy dependences
    remove_keilotherdir
    remove_keilfile

    #3, delete usless file
    remove_keilplatform
}

function remove_ugelisotherdir()
{
	echo "============remove other dir=============\n"
    rm $SDK_UGELIS_DIR/ztest  -rf			
    rm $SDK_UGELIS_DIR/kelis_example  -rf    
    rm $SDK_UGELIS_DIR/tests  -rf								
}

function remove_ugelisplatform()
{
	echo "============remove platform=============\n"
	rm $SDK_UGELIS_DIR/arch/arm/core-armcc  -rf
}

function release_linuxproject()
{
    echo "===> release_linuxproject\n"

    if [ ! -d $SDK_DIR ]; then
        mkdir -p $SDK_DIR
    fi
    cp $CURR_DIR/../* $SDK_DIR -rf

    #1, compile sample firstly
    remove_gitfile

    #2, copy dependences
    remove_ugelisotherdir

    #3, delete usless file
    remove_ugelisplatform
}
#=======================main========================#

if [ "$1" = "help" ]; then
    echo "Usage: ./release_sdk.sh <sdk_name> [export_name]"
    echo "    1, [sdk_name] default is kelis"
    echo -e "    2, execute [./release_sdk.sh list] can list all sample project name\n"
else
    if [ "$1" = "ugelis" ]; then
    	SDK_NAME=uGelisSDK
    else
    	SDK_NAME=KelisSDK
    fi
    
	SDK_DIR=$CURR_DIR/../../$SDK_NAME
	SDK_UGELIS_DIR=$SDK_DIR/ugelis
	SDK_ULIBS_DIR=$SDK_DIR/ulibs
	SDK_KELIS_EXPLAME_DIR=$SDK_UGELIS_DIR/kelis_example
	SDK_UGELIS_EXPLAME_DIR=$SDK_UGELIS_DIR/ugelis_example
	KELIS_DIR=$RELEASE_KEILS_DIR/
    
    if [ "$1" = "ugelis" ]; then
    	release_linuxproject
    else
    	release_keilproject
    fi
fi