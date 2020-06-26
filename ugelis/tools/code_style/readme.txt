------------------------------------------------                                        
            自动重新格式化代码说明
------------------------------------------------
#安装
    ./install.sh

#作用
    每次git commit前自动执行代码格式化工作

#具体说明
    ----------------------
    提交前自动执行脚本
    ----------------------
    安装后的.git/hooks/pre-commit将在每次git commit前自动执行安装后的
    ./git/hooks/auto_reformat_code.py
    

    ---------------
    格式化代码
    ---------------
    自动执行python脚本将调用astyle工具格式化当前git仓库下被修改，或新添加的.c/.cpp/.h文件
    主动执行命令为：
    python ./tools/code_style/auto_reformat_code.py

#其它
    astyle参数为-A1 -c -s4 -xV -S -N -xt4 -xW -w -Y -p -U -k3 -W3 -j  -xf -xh -H --suffix=none
    具体意义请查阅astyle官网


    Thanks!


    Angrad.Yang
    2019/05/10



