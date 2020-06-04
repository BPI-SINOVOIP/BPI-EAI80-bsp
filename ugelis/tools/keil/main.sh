#!/bin/sh
replace_path() {
    echo $1
    sed -ri 's/((\.\.\\){2,})(driver)/\1..\\ulibs\\hal\\gree\\driver/g' $1
    sed -ri 's/((\.\.\\){2,})(include\\cmsis)/\1..\\ulibs\\hal\\cmsis\\Include/g' $1
    sed -ri 's/((\.\.\\){2,})(include)/\1..\\ulibs\\hal\\gree\\include/g' $1
    sed -ri 's/((\.\.\\){2,})(lib)/\1..\\ulibs\\hal\\gree\\lib/g' $1
    sed -ri 's/((\.\.\\){2,})(arch)/\1..\\ulibs\\hal\\gree\\arch/g' $1
    sed -ri 's/((\.\.\\){2,})(ulibs\\hal\\gree\\)(arch\\arm\\soc\\gm6721x\\startup_gm6721x\.s)/\.\.\\\.\.\\\.\.\\\4/g' $1
    sed -ri 's/((\.\.\\){2,})(ulibs\\hal\\gree\\)(include\\config\;)/\1\3\4\.\.\\\.\.\\\.\.\\\4/g' $1
    sed -ri 's/((\.\.\\){2,})(ulibs\\hal\\gree\\)(include\\arch\;)/\1\3\4\.\.\\\.\.\\\.\.\\\4/g' $1
}

replace_path2()
{
    echo $1
    sed -ri 's/((\.\.\\){1,})(boards)/\1boards\\arm/g' $1
    sed -ri 's/((\.\.\\){1,})(arch\\arm\\core\\)/\1arch\\arm\\core-armcc\\/g' $1
    sed -ri 's/(<GroupName>)(core)(<\/GroupName>)/\1core-armcc\3/g' $1
}

path="`find $UGELIS_BASE/example -name "*.uvprojx"`"
for x in $path; do
   replace_path2 $x
done
