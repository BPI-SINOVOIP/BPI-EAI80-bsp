.. _hello_world:

Hello World
###########

Overview
********


1: flash_bringup :
   address space --> code run from flash(0x10000000)
                     data run from sram (0x20000000)

2: sdram_bringup:
   address space --> code run from sdram(0xC0000000+ReserveSize(64K))
                     data run from sdram (code address + codeImageSize)

3: sram_bringup:(This Mode is only for debug )
   address space --> code run from sram(0x20000000)
                     data run from sram(code address + codeImageSize)


###########
how to compiler
  flash_bringup prj --> cd flash_bringup
                        mkdir build
                        cmake ../
                        make


 sdram_bringup prj --> cd sdram_bringup
                       mkdir build
                       cmake ../
                       make

 sram_bringup prj -->  cd sram_bringup
                       mkdir build
                       cmake ../
                       make






