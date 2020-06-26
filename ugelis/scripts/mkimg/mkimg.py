import sys
import os
import getopt
import argparse
from Crypto.Cipher import AES
import configparser

class dir():
	boot1Dir        = ''
	sysDir          = ''
	dataDir         = ''
	prvDataDir      = ''
	outDir          = ''
	current_path    = ''
	base_path       = ''
	boot1RelaPath   = ''

	part_num = 3
	info_addr = 0x10000000
	info_size = 40
	info_load_addr = 0x10000000
	boot1_addr = 0x10001000
	boot1_size = 0
	boot1_load_addr = 0x10001000
	system_addr = 0x10010000
	system_size = 0
	system_load_addr = 0x10010000
	data_addr = 0x10800000
	data_size = 0
	data_load_addr = 0x10800000
	data_select = 0
	priv_data_addr = 0x10FFF800
	priv_data_size = 0
	priv_data_load_addr = 0x10FFF800
	priv_data_select = 0
	pointer_addr = 0x00000000
	padding_lenth = 0
	key = ''
	iv = ''
	ecpt = []
	tmp = ''

def usage():#用法提示
	print("Usage:\n"
		"  mount [-b boot_dir] [-s syself_dir ] [-r res_dir] [-f out_dir]\n ")
	exit(-1)

def read_ini():
	workPath = os.path.split(os.path.realpath(__file__))[0]
	ini_name = 'mkimgconfig.ini'
	ini_path = os.path.join(workPath, ini_name)
	if os.path.exists(ini_path):
		config = configparser.ConfigParser()
		config.read(ini_path)
		dir.info_addr = int(config.get('mkimg', 'info_addr'), 16)
		dir.info_load_addr = int(config.get('mkimg', 'info_load_addr'), 16)
		dir.boot1_addr = int(config.get('mkimg', 'boot1_addr'), 16)
		dir.boot1_load_addr = int(config.get('mkimg', 'boot1_load_addr'), 16)
		dir.system_addr = int(config.get('mkimg', 'system_addr'), 16)
		dir.system_load_addr = int(config.get('mkimg', 'system_load_addr'), 16)
		dir.data_addr = int(config.get('mkimg', 'data_addr'), 16)
		dir.data_load_addr = int(config.get('mkimg', 'data_load_addr'), 16)
		dir.data_select = int(config.get('mkimg', 'data_select'))
		dir.priv_data_addr = int(config.get('mkimg', 'priv_data_addr'), 16)
		dir.priv_data_load_addr = int(config.get('mkimg', 'priv_data_load_addr'), 16)
		dir.priv_data_select = int(config.get('mkimg', 'priv_data_select'))
		dir.key = config.get('key', 'key')
		dir.iv = config.get('key', 'IV')
		dir.ecpt = config.get('encryption', 'ecpt').split(' ')
		print("encryption module:%s" % dir.ecpt)
	else:
		print('init file not exist')


def encryption(file):
	with open (file, 'rb+') as f:
		text = f.read()
		cryptor = AES.new(dir.key ,AES.MODE_CBC, dir.iv)
		add = 16 - (len(text) % 16)
		if len(text) % 16:
			text = text + ('\0'.encode(encoding='utf-8') * add)
		tmp = cryptor.encrypt(text)
		return tmp

def decryption(text):
	cryptor = AES.new(dir.key, AES.MODE_CBC, iv)
	result = cryptor.decrypt(text)
	result = result.rstrip('\0'.encode(encoding='utf-8'))
	return result

def writeFile(src, dst, offset, encrypt_flag):
	with open(dst, 'rb+') as f:
		f.seek(offset)
		if encrypt_flag == 1:
			f.write(dir.tmp)
		else:
			f.write(open(src, 'rb').read())

def writeHex(src, dst, offset):
#	x = int(src)
#	src = x.to_bytes(4, byteorder='little', signed=True)
	x = hex(int(src)).split('0x')[1]
	x = x.zfill(8)
	seq = (x[6], x[7], x[4], x[5], x[2], x[3], x[0], x[1])
	y = ''.join(seq)
	src = bytes.fromhex(y)
	with open(dst, 'rb+') as f:
		f.seek(offset)
		f.write(src)

def write2Img(src, dst, offset, encrypt_flag):
	if os.path.isfile(src):
		writeFile(src, dst, offset, encrypt_flag)
	else:
		print('Dir error!')
	# if os.path.isdir(src):
	#     writeDir()

def padding(src, dst, size, value):
	dir.padding_lenth = dst - src - size
	with open(dir.outDir, 'ab') as f:
		f.write(value*dir.padding_lenth)

def align():
	size = os.path.getsize(dir.outDir)
	count = 1024 - size%1024
	with open(dir.outDir, 'ab') as f:
		f.write(b'\xFF'*count)
	
def mkInfo():
	with open(dir.outDir, 'wb') as f:
		f.truncate()
	dir.boot1_size = os.path.getsize(dir.boot1Dir)
	dir.system_size = os.path.getsize(dir.sysDir)
	dir.data_size = os.path.getsize(dir.dataDir)
	dir.priv_data_size = os.path.getsize(dir.prvDataDir)
	dir.part_num += dir.data_select
	dir.part_num += dir.priv_data_select
	writeHex(dir.part_num, dir.outDir, 0)
	writeHex(dir.info_addr, dir.outDir, 4)
	writeHex(0x1000, dir.outDir, 8)
	writeHex(dir.info_load_addr, dir.outDir, 12)
	writeHex(dir.boot1_addr, dir.outDir, 16)
	writeHex(dir.boot1_size, dir.outDir, 20)
	writeHex(dir.boot1_load_addr, dir.outDir, 24)
	writeHex(dir.system_addr, dir.outDir, 28)
	writeHex(dir.system_size, dir.outDir, 32)
	writeHex(dir.system_load_addr, dir.outDir, 36)
	if dir.data_select:
		writeHex(dir.data_addr, dir.outDir, 40)
		writeHex(dir.data_size, dir.outDir, 44)
		writeHex(dir.data_load_addr, dir.outDir, 48)
		dir.info_size += 12
	if dir.priv_data_select:
		writeHex(dir.priv_data_addr, dir.outDir, 52)
		writeHex(dir.priv_data_size, dir.outDir, 56)
		writeHex(dir.priv_data_load_addr, dir.outDir, 60)
		dir.info_size += 12

def mkBoot1():
	dir.boot1Dir = os.path.join(dir.base_path, dir.boot1Dir)
	dir.boot1Dir = os.path.abspath(dir.boot1Dir)
	dir.pointer_addr = dir.boot1_addr - dir.info_addr
	print('boot1_path:%s' %dir.boot1Dir)
	if 'boot1' in dir.ecpt:
		dir.tmp = encryption(dir.boot1Dir)
		write2Img(dir.boot1Dir, dir.outDir, dir.pointer_addr, 1)
	else:
		write2Img(dir.boot1Dir, dir.outDir, dir.pointer_addr, 0)

def mkSys():
	dir.sysDir = os.path.join(dir.base_path, dir.sysDir)
	dir.sysDir = os.path.abspath(dir.sysDir)
	dir.pointer_addr = dir.system_addr - dir.info_addr
	print('sysDir: %s' %dir.sysDir)
	if 'system' in dir.ecpt:
		dir.tmp = encryption(dir.sysDir)
		write2Img(dir.sysDir, dir.outDir, dir.pointer_addr, 1)
	else:
		write2Img(dir.sysDir, dir.outDir, dir.pointer_addr, 0)

def mkData():
	dir.dataDir = os.path.join(dir.base_path, dir.dataDir)
	dir.dataDir = os.path.abspath(dir.dataDir)
	dir.pointer_addr = dir.data_addr - dir.info_addr
	print('resDir: %s' %dir.dataDir)
	if 'data' in dir.ecpt:
		dir.tmp = encryption(dir.sysDir)
		write2Img(dir.dataDir, dir.outDir, dir.pointer_addr, 1)
	else:
		write2Img(dir.dataDir, dir.outDir, dir.pointer_addr, 0)
        
def mkPrvData():
	dir.prvDataDir = os.path.join(dir.base_path, dir.prvDataDir)
	dir.prvDataDir = os.path.abspath(dir.prvDataDir)
	dir.pointer_addr = dir.priv_data_addr - dir.info_addr
	print('prvDataDir: %s' %dir.prvDataDir)
	if 'privatedata' in dir.ecpt:
		dir.tmp = encryption(dir.prvDataDir)
		write2Img(dir.prvDataDir, dir.outDir, dir.pointer_addr, 1)
	else:
		write2Img(dir.prvDataDir, dir.outDir, dir.pointer_addr, 0)

def main():
	parser = argparse.ArgumentParser(description='mkimg')
	parser.add_argument("-b", "--boot1", nargs=1, required=True, help="boot1 file name")
	parser.add_argument("-s", "--system", nargs=1, required=True, help="system file name")
	parser.add_argument("-d", "--data", nargs=1, required=True, help="data file name")
	parser.add_argument("-p", "--privatedata", nargs=1, required=True, help="privatedata file name")
	parser.add_argument("-o", "--output", nargs=1, required=True, help="img file name")
	parser.add_argument("-k", "--key", nargs=1, required=True, help="key file name")
	args = parser.parse_args()
	dir.boot1Dir = args.boot1[0]
	dir.sysDir = args.system[0]
	dir.dataDir = args.data[0]
	dir.prvDataDir = args.privatedata[0]
	dir.outDir = args.output[0]
	dir.keyDir = args.key[0]
	if not os.path.exists(dir.outDir):
		print('outDir:%s' % dir.outDir)
	read_ini()
	mkInfo()
	padding(dir.info_addr, dir.boot1_addr, dir.info_size, b'\xFF')
	mkBoot1()
	padding(dir.boot1_addr, dir.system_addr, dir.boot1_size, b'\xFF')
	mkSys()
	if dir.data_select:
		padding(dir.system_addr, dir.data_addr, dir.system_size, b'\xFF')
		mkData()
	if dir.priv_data_select:
		padding(dir.data_addr, dir.priv_data_addr, dir.data_size, b'\xFF')
		mkPrvData()
	align()
	print('all is done')

if __name__ == "__main__":
	main()


