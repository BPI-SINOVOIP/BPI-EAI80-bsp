#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'test.ui'
#
# Created by: PyQt5 UI code generator 5.11.3
#
# WARNING! All changes made in this file will be lost!

__author__ = 'Chris.xie'
__verison__ = 'V1.0'

import sys
import os
import termios
import tty
#from six.moves import configparser
import configparser
import time
import serial
import Ymodem
import unittest
from enum import Enum
import logging

g_nTotalDownloadSize = 0
g_nTotalWriteSize = 0

class LIST_ITEMS(Enum):
    ITEM_NO = 0,
    ITEM_NAME = 1
    ITEM_OFFSET = 2
    ITEM_PART_LEN = 3
    ITEM_FILE = 4
    ITEM_FILE_LEN = 5
    ITEM_PATH = 6

class RET_CODE(Enum):
    RET_OK = 0
    RET_ERROR = 1
    RET_BUSY = 2
    RET_TIMEOUT =3
    RET_INVPARAM = 4

class Download(object):

    def __init__(self):
        logging.basicConfig(level=logging.DEBUG)
        self.m_partInfo = None
        self.count = 0
        self.Read_inifile_config()
        self.DownloadSerial()
        self.ser = None

    def DownloadSerial(self):
        try:
            logging.info("open serial....")
            self.ser = serial.Serial(port = self.m_partInfo.com, baudrate = self.baud, parity = self.parity,
                                   stopbits = self.stopbit)
            if self.ser.is_open == False:
                self.ser = serial.Serial(port=self.m_partInfo.com, baudrate=self.baud, parity=self.parity, stopbits=self.stopbit)
                self.ser.open()
            logging.info("open serial done....")
            self.DtsRstOp()
            logging.info("reset done....")
            logging.info("start download....")
            ret = self.SerialProgramFlash()
            if ret != RET_CODE.RET_OK.value:
                self.ser.close()

            self.ser.close()
        except Exception as e:
            logging.error("uncaught exception: %s" % e)

    def DtsRstOp(self):
        # self.ser.rts = True
        # self.ser.dtr = True
        # time.sleep(0.05)
        # unittest.TestCase.assertTrue(not self.ser.cts, "CTS -> 1")
        # unittest.TestCase.assertTrue(not self.ser.dtr, "DSR -> 1")
        self.ser.rts = False
        self.ser.dtr = False
        time.sleep(0.05)
        unittest.TestCase.assertTrue(not self.ser.cts, "CTS -> 0")
        unittest.TestCase.assertTrue(not self.ser.dtr, "DSR -> 0")
        time.sleep(0.05)
        self.ser.rts = True
        unittest.TestCase.assertTrue(not self.ser.cts, "CTS -> 1")

        time.sleep(0.5)
        self.ser.rts = False
        unittest.TestCase.assertTrue(not self.ser.cts, "CTS -> 0")

    #读写配置ini文件
    def Read_inifile_config(self):
        global g_nTotalDownloadSize
        config = configparser.ConfigParser()
        try:
	    #config.read(r'./Config.ini', encoding = 'gbk')
            config.read(r'./config.ini')
        except Exception as e:
            logging.error(str(e))
        #config.read('./Config.ini')
        self.m_partInfo = file_config()
        try:
            self.storage = config.get('SETTING', 'STORAGE')
        except Exception as e:
            logging.error(str(e))
#        self.storage = config.get('SETTING', 'STORAGE')
        self.baud = config.getint('SETTING', 'BAUDRATE')
        self.parity = config.getint('SETTING', 'PARITY')
        if self.parity == 0:
            self.parity = serial.PARITY_NONE
        elif self.parity == 1:
            self.parity = serial.PARITY_ODD
        elif self.parity == 2:
            self.parity = serial.PARITY_EVEN
        self.stopbit = config.getint('SETTING', 'STOPBIT')

        self.m_partInfo.file = config.get('SETTING', 'FILE')
        self.m_partInfo.com = config.get('SETTING', 'COM')
        CurrentDirpath = os.getcwd()
        File_Path = CurrentDirpath + '/' + self.m_partInfo.file
        logging.info("FILE=" + File_Path)
        logging.info("COM=" + self.m_partInfo.com)
        logging.info("BAUDRATE="  + str(self.baud))
        logging.info("PARITY="  + str(self.parity))
        logging.info("STOPBIT="  + str(self.stopbit))
        try:
            file_len = os.path.getsize(File_Path)
            self.m_partInfo.dirfilepath = File_Path
            self.m_partInfo.filesize = file_len
            g_nTotalDownloadSize = self.m_partInfo.filesize
            logging.info("FILESIZE=" + str(g_nTotalDownloadSize))
        except Exception as e:
            logging.error("File is not exist")
            self.m_partInfo.filesize = -1

    def SerialProgramFlash(self):
        logging.info("do SerialProgramFlash ....")

        if self.SerialCmdUpgFirmware(self.m_partInfo.partStartAddr,self.m_partInfo.dirfilepath,
                                     self.m_partInfo.filesize) != RET_CODE.RET_OK.value:
            logging.error("download fail....")
            return RET_CODE.RET_ERROR.value
        logging.info("download done....")
        return RET_CODE.RET_OK.value

    def SerialCmdUpgFirmware(self, flashAddr, partFileName, partFilelen):
        logging.info("FILE=" + partFileName)
        logging.info("SIZE="  + str(partFilelen))
        logging.info("flashAddr="  + str(flashAddr))
        try:
            with open(partFileName, 'rb') as f:
                f.seek(0, 0)
                ymodemBuff = f.read(partFilelen)
                logging.info("todo Ymodem.Ymodem")
                self.a = Ymodem.Ymodem()
                # ymodemBuff = str(binascii.b2a_hex(ymodemBuff))
                if self.a.Ymodem_Transmit(ymodemBuff, partFileName, partFilelen,self.ser) != 0:
                    return RET_CODE.RET_ERROR.value
                # QApplication.processEvents()
        except Exception as e:
            logging.error("uncaught exception: %s" % e)

        return RET_CODE.RET_OK.value


class file_config():
    def __init__(self):
        self.file = ''#文件名字
        self.filesize = 0 #文件大小
        self.partStartAddr = ''
        self.dirfilePath = ''
        self.com=''


if __name__ == '__main__':
    ui = Download()

