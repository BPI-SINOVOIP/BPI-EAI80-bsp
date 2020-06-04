/* targert*/

#ifndef __TARGET_CONFIG_H__
#define __TARGET_CONFIG_H__

#include "config.h"

#include <config/sys/gm6721x_soc.h>                        //soc系列相关 中断和基地址  :用户不会改
#include <config/feature/gm6721x_feature.h>                //kernle/driver/middleware

#ifdef GM6721_EVB_144PIN
    #include <config/sys/gm6721_soc.h>                  //具体soc的相关配置，用户不用关心
    #include <config/board/gm6721_evb_144pin_board.h>      //用户自定义板文件和dts文件 ：用户根据这个板子创建自己的板子
#endif

#ifdef GM6721_EVB_QFN80PIN
    #include <config/sys/gm6721_soc.h>                  //具体soc的相关配置，用户不用关心
    #include <config/board/gm6721_evb_qfn80pin_board.h>      //用户自定义板文件和dts文件 ：用户根据这个板子创建自己的板子
#endif

#ifdef GM6721_EVB_QFN40PIN
    #include <config/sys/gm6721_soc.h>                  //具体soc的相关配置，用户不用关心
    #include <config/board/gm6721_evb_qfn40pin_board.h>      //用户自定义板文件和dts文件 ：用户根据这个板子创建自己的板子
#endif

#ifdef GM6721_EVB_176PIN
    #include <config/sys/gm6721_soc.h>                  //具体soc的相关配置，用户不用关心
    #include <config/board/gm6721_evb_176pin_board.h>      //用户自定义板文件和dts文件 ：用户根据这个板子创建自己的板子
#endif

#ifdef F6721B_EVB_144PIN
    #include <config/sys/f6721b_soc.h>                  //具体soc的相关配置，用户不用关心
    #include <config/board/f6721b_evb_144pin_board.h>      //用户自定义板文件和dts文件 ：用户根据这个板子创建自己的板子
#endif

#ifdef F6721B_EVB_176PIN
    #include <config/sys/f6721b_soc.h>                   //具体soc的相关配置，用户不用关心
    #include <config/board/f6721b_evb_176pin_board.h>      //用户自定义板文件和dts文件 ：用户根据这个板子创建自己的板子
#endif

#ifdef F6721B_EVB_QFN40PIN
    #include <config/sys/f6721b_soc.h>                   //具体soc的相关配置，用户不用关心
    #include <config/board/f6721b_evb_qfn40pin_board.h>      //用户自定义板文件和dts文件 ：用户根据这个板子创建自己的板子
#endif

#ifdef F6721B_EVB_QFN80PIN
    #include <config/sys/f6721b_soc.h>                  //具体soc的相关配置，用户不用关心
    #include <config/board/f6721b_evb_qfn80pin_board.h>      //用户自定义板文件和dts文件 ：用户根据这个板子创建自己的板子
#endif

#ifdef F6721B_RINGSCANER
    #include <config/sys/f6721b_soc.h>
    #include <config/board/f6721b_ringscaner_board.h>
#endif

#endif








