操作步骤：
	1.在mkimgconfig.ini文件中配置好需要的参数
	2.执行mkfsimg.exe在data文件夹中将res文件夹打包成文件系统并生成fat16.img文件
	3.执行mkimg.exe生成烧录的mkimg.bin文件

说明：
    文件系统：
        1.通过mkimgconfig.ini文件配置当前目录下data/res文件夹生成文件系统的格式，size为生成img文件的大小（MB），
        由于末尾没有补零，生成的img文件实际大小会偏小（size的作用是设置文件系统存储参数）。
        2.type值为4，表示当前文件系统的格式为FAT16。（目前仅支持FAT16,FAT32会在以后按需开放）
    烧写打包：
        1.通过mkimgconfig.ini文件配置烧写打包文件的相关参数。
        2.ecpt=boot1 system data privatedata为加密功能配置，右边的参数表示需要AES加密的区域。如需要改变加密区域，只需更改右边的
        相关参数。（填入需要加密的部分，以空格隔开）
    AES加密：
        1.当前目录下的mkimgconfig.ini文件中的key section内容为AES加密的秘钥，可以修改，但必须为16字节长度。
        2.AES加密采用的是CBC模式，其保密性更好，VI项也是从key section中vi值进行加载，可以通过修改vi值自定义vi，但必须为16字节
        长度。
        3.解密：因为加密是对各子文件进行加密后填充到mkimg.img文件中，解密也需按照文件存储空间进行解密。解密需要秘钥和AES的vi，如boot1文件起
		始地址为0x1000，整个boot1占用空间为0x10000，真实大小为0x2E7B，解密时需读取的长度必须为16的倍数，本例取0x2E80，对此长度为0x2E80的bytes
		进行解码，解码后需去除str'\0'对应的bytes 0x00，最终得到的boot1才会和写入前的boot1完全一致。