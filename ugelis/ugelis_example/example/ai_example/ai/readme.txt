cmake -DBOARD=banpiboard_qfn80pin ..     to choose banana pi board

this project support body and gesture recognition

body weight addr is 0x10210000 in flash (load address is defined as OBJ_OPS_FLASH_OFFSET in config.h)
ges weight addr is 0x107c0000 in flash (load address is defined as GES_OPS_FLASH_OFFSET in config.h)
use Config.ini replace ugelisflash tool Config.ini