/* targert*/

/*用户选择平台*/
/*
//#define GM6721_FPGA 1
//#define GM6721_EVB_144PIN 1
//#define GM6721_EVB_176PIN 1
//#define GM6721_EVB_144PIN 1
*/
//#define TO_BE_REPLACE = 1
    
  
#ifdef GM6721_FPGA
    #include <config/sys/gm6721_soc.h>              //soc相关 中断和基地址  :用户不会改
    #include <config/feature/gm6721_fpga_feature.h>  //kocnfig :kernle/driver/middleware ：用户根据这个板子创建自己的功能
    #include <config/board/gm6721_fpga_board.h>      //用户自定义板文件和dts文件 ：用户根据这个板子创建自己的板子
#endif


#ifdef GM6721_EVB_144PIN
    #include <config/sys/gm6721_soc.h>              //soc相关 中断和基地址  :用户不会改
    #include <config/feature/gm6721_evb_144pin_feature.h>  //kocnfig :kernle/driver/middleware ：用户根据这个板子创建自己的功能
    #include <config/board/gm6721_evb_144pin_board.h>      //用户自定义板文件和dts文件 ：用户根据这个板子创建自己的板子
#endif

#ifdef GM6721_EVB_176PIN
    #include <config/sys/gm6721_soc.h>              //soc相关 中断和基地址  :用户不会改
    #include <config/feature/gm6721_evb_176pin_feature.h>  //kocnfig :kernle/driver/middleware ：用户根据这个板子创建自己的功能
    #include <config/board/gm6721_evb_176pin_board.h>      //用户自定义板文件和dts文件 ：用户根据这个板子创建自己的板子
#endif

#ifdef STM32_EVB  
    #include <config/stm32_soc.h>                //soc相关 中断和基地址
    #include <config/feature/stm32_evb_feature.h>    //kocnfig :kernle/driver/middleware：用户根据这个板子创建自己的功能
    #include <config/board/gm6721_evb_feature.h>   //用户自定义板文件和dts文件 ：用户根据这个板子创建自己的板子
#endif











