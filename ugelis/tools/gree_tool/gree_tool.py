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
#import Ymodem
import unittest
from enum import Enum
import logging

#from Updatetool_SerialUSB import g_nTotalWriteSize, g_nTotalDownloadSize
#from gree_flash import g_nTotalWriteSize, g_nTotalDownloadSize
import numpy as np
#import logging

# g_nTotalDownloadSize = 0
# g_nTotalWriteSize = 0


# import unittest
import ctypes
import numpy as np
class Ymodem:
    def __init__(self):
        logging.info("do Ymodem init ....")
        pass

    def Ymodem_Transmit(self, buf, sendFileName, sizeFile, Pser):
        # global g_nTotalWriteSize
        logging.info("do Ymodem_Transmit ....")
        CRC16_F = 0
        receiverReady = 0
        errors = 0
        self.FileName = []
        self.receivedC = []
        self.receivedC.append(0)
        self.data = []
        self.YsizeFile = sizeFile
        self.ser = Pser
        self.packet_data = [0 for i in range(1029)]
        for i in range(len(sendFileName)):
            if(i  >= 64):
                break
            self.FileName.append(sendFileName[i])
        CRC16_F = 1

        #Prepare first block
        self.Ymodem_PrepareIntialPacket()
        tempCRC = self.Cal_CRC16(128, self.data)
        self.data.append(tempCRC >> 8)
        self.data.append(tempCRC & 0xFF)



        #Wait for receiver ready
        while True:
            if (self.Receive_Byte(3000) == 0) and self.receivedC[0] == 0x43: #request to send data
            # if (self.Receive_Byte(1000) == 0x43):
                receiverReady = 1
                break

            if (receiverReady == 0):
                errors = errors + 1

        if receiverReady == 0:
            return  1

        #Send file name
        ackReceived = 0
        self.receivedC[0] = 0x00
        errors = 0


        while True:
            # self.ser.write('\x01\x02\x03'.encode('utf-8'))
            #self.Ymodem_SendPacket(133,self.data)
            # self.ser.write(bytes([a_data[0]]))
            # self.ser.write(bytes([a_data[1]]))
            # self.ser.write(bytes([a_data[2]]))
            self.ser.write(self.data)
            # self.ser.write(a_data)
            #Wait for Ack and 'C'
            if (self.Receive_Byte(100000) == 0) and (self.receivedC[0] == 0x06): # ACK  ((uint8_t)0x06)  /** acknowledge */
            # if (self.Receive_Byte(100000) == 0x06):
                if(self.Receive_Byte(100000) == 0) and (self.receivedC[0] == 0x43): #request to send data
                # if (self.Receive_Byte(100000) == 0x43):
                    ackReceived = 1
                    #print("i have read")
            if ackReceived == 0:
                errors = errors + 1

            if  (ackReceived == 1) or (errors > 0x32): #MAX_RETRY_COUNT         0x32
                break

        if errors >= 0x32:
            return  errors

        self.buf_ptr = buf[:]
        self.size = sizeFile
        self.blkNumber = 0x01
        #Here 1024 bytes package is used to send the packets

        #Resend packet if NAK  for a count of 10 else end of commuincation
        ackReceived = 0
        self.receivedC[0] = 0x00
        errors = 0
        while self.size:
            #Prepare next packet
            self.Ymodem_PreparePacket()

            pktsize = 1024 if self.size >= 1024 else 128
            tempCRC = self.Cal_CRC16(pktsize, self.packet_data[:1027])
            self.packet_data[pktsize+3] = tempCRC >> 8
            self.packet_data[pktsize+3+1] = tempCRC & 0xFF
            # self.Send_Byte(tempCRC >> 8)
            # self.Send_Byte(tempCRC & 0xff)

            ackReceived = 0
            self.receivedC[0] = 0x00
            errors = 0

            for i in range(1029):
                if self.packet_data[i] > 255 or self.packet_data[i] < 0:
                    print("error")
            # K = 1
            while True:
                #Send next packet
                # pktsize = 1024 if self.size >=1024 else 128
                # self.Ymodem_SendPacket(pktsize+5,self.packet_data)
                if pktsize == 1024:
                    self.ser.write(self.packet_data)
                elif pktsize == 128:
                    self.ser.write(self.packet_data[:133])

                # Wait for Ack
                if (self.Receive_Byte(100000) == 0) and (self.receivedC[0] == 0x06):
                # if (self.Receive_Byte(100000) == 0x06):
                    ackReceived = 1
                    if self.size > pktsize:
                        # g_nTotalWriteSize = g_nTotalWriteSize + pktsize
                        self.buf_ptr = self.buf_ptr[pktsize:]
                        self.size = self.size - pktsize

                        if self.blkNumber == (10*1024*1024/1024): #MAX_SEND_SIZE  (10*1024*1024)
                            return 0xFF
                        else:
                            self.blkNumber = self.blkNumber + 1
                    else:
                        # g_nTotalWriteSize = g_nTotalWriteSize + self.size
                        self.buf_ptr = self.buf_ptr[pktsize:]
                        self.size = 0
                else:
                    errors = errors + 1
                    if self.receivedC[0] == 0x18: #two of these in succession aborts transfer
                        if (self.Receive_Byte(100000) == 0) and (self.receivedC[0] == 0x18):
                            return errors

                if (ackReceived == 1) or (errors > 0x32):#MAX_RETRY_COUNT
                    break

            if errors >= 0x32:
                return errors

        ackReceived = 0
        self.receivedC[0] = 0x00
        errors = 0
        while True:
            self.Send_Byte(0x04) #end of transmission
            if (self.Receive_Byte(100000) == 0) and (self.receivedC[0] == 0x15):#negative acknowledge
            # if (self.Receive_Byte(10000) == 0x15):
                self.Send_Byte(0x04)
                if (self.Receive_Byte(100000) == 0) and (self.receivedC[0] == 0x06):#acknowledge
                    if (self.Receive_Byte(100000) == 0) and (self.receivedC[0] == 0x43):  # acknowledge
                        ackReceived = 1
            else:
                if self.receivedC[0] == 0x18: #two of these in succession aborts transfer
                    if (self.Receive_Byte(100000) == 0) and (self.receivedC[0] == 0x18):
                        errors = errors + 1
                        return  errors

            if ackReceived == 0:
                errors = errors + 1

            if (ackReceived == 1) or (errors > 0x32):#MAX_RETRY_COUNT
                break

        if errors >= 0x32:
            return  errors

        #Last packet preparation
        ackReceived = 0
        self.receivedC[0] = 0x00
        errors = 0

        self.data[0] = 0x01 #indicate 128 byte data packet
        self.data[1] = 0
        self.data[2] = 0xFF

        for i in range(3, 131):
            self.data[i] = 0x00

        tempCRC = self.Cal_CRC16(128, self.data)
        self.data[131] = tempCRC >> 8
        self.data[132] = tempCRC >> 0xFF


        while True:
            #Send Packet
            # self.Ymodem_SendPacket(131+2,self.data)
            self.ser.write(self.data)


            #Wait for Ack
            # if (self.Receive_Byte(100000) == 0) and (self.receivedC[0] == 0x45): #acknowledge
            if (self.Receive_Byte(100000) == 0):  # acknowledge
                #Packet transfered correctly
                ackReceived = 1
            else:
                errors = errors + 1
                if self.receivedC[0] == 0x18:
                    if (self.Receive_Byte(100000) == 0) and self.receivedC[0] == 0x18:
                        return errors
            if (ackReceived == 1) or (errors > 0x32):#MAX_RETRY_COUNT
                break

        #Resend packet if NAK  for a count of 10  else end of commuincation
        if errors >= 0x32:
            return  errors


        # self.ser.rts = False
        # unittest.TestCase.assertTrue(not self.ser.cts, "CTS -> 1")
        # unittest.TestCase.assertTrue(not self.ser.cts, "CTS -> 0")
        #file trasmitted successfully
        return 0

    def Ymodem_PrepareIntialPacket(self):

        #Make first three packet
        self.data.append(0x01 & 0xFF) #/** indicate 128 byte data packet */
        self.data.append(0x00 & 0xFF)
        self.data.append(0xFF & 0xFF)

        #Filename Packet has valid data
        i = 0
        while (i< len(self.FileName)) and (i < 64): #FILE_NAME_LENGTH        64
            self.data.append(ord(self.FileName[i]) & 0xFF)
            i = i + 1

        self.data.append(0x00)
        # # file_ptr = str(self.YsizeFile)
        fsize = self.YsizeFile
        self.data.append((fsize >> 24) & (0xFF))
        self.data.append((fsize >> 16) & (0xFF))
        self.data.append((fsize >> 8) & (0xFF))
        self.data.append((fsize >> 0) & (0xFF))

        j = 0
        i = i + 4 + 1 + 3

        j = i
        while j < 128 + 3: #PACKET_SIZE 128
            self.data.append(0x00)
            j = j + 1


    def Receive_Byte(self,time_out):
        try:
            self.ser.timeout = time_out
            feedback = self.ser.read(1)
            # self.receivedC.append(int.from_bytes(feedback, byteorder='little', signed=False))
            self.receivedC[0] = int.from_bytes(feedback, byteorder='little', signed=False)
            # return int.from_bytes(feedback, byteorder='little', signed=False)
            return 0
        except:
            return -1


    def Send_Byte(self, c):
        if ((isinstance(c, int) == True and ((c >= 0) and (c < 0x80)))) :
            self.ser.write(bytes([c]))
        elif isinstance(c, int) == True and (c < 0):
            # self.ser.write(chr(c & 0xFF).encode())
            self.ser.write(bytes([(c & 0xFF)]))
        elif (isinstance(c,int) == True and (c >= 0x80)):
            self.ser.write(bytes([c]))
        else:
            self.ser.write(chr(ord(c)).encode())
        return 0

    def Ymodem_SendPacket(self,length,data):
        for i in range(length):
            self.Send_Byte(data[i])



    def Cal_CRC16(self,size,data):
        crc = 0
        i = 0
        while i < size:
            crc  = self.UpdateCRC16(crc,data[i+3])
            i = i + 1

        crc = self.UpdateCRC16(crc, 0)
        crc = self.UpdateCRC16(crc, 0)
        return (crc & 0xFFFF)


    def UpdateCRC16(self,crcIn, byte):
        crc = crcIn
        if isinstance(byte,str):
            ine = (ord(byte) | 0x100)
        else:
            ine = (byte | 0x100)

        while True:
            crc = crc << 1
            ine = ine << 1
            if (ine & 0x100):
                crc = crc + 1
            if (crc & 0x10000):
                crc = crc ^ 0x1021
            if ((ine) & (0x10000)):
                break
        return  crc & 0xFFFF

    def CalChecksum(self,size,data):
        sum = 0
        i = 0
        while i < size:
            if isinstance(data[i], int):
                sum = sum + data[i]
            else:
                sum = sum + ord(data[i])
            i = i + 1
        return sum & 0xFF

    def Ymodem_PreparePacket(self):
        packetsize = self.size
        tmpsize = self.size
        packetsize = 1024 if packetsize >=1024  else 128
        tmpsize = self.size if self.size < packetsize else packetsize

        if  packetsize == 1024:
            self.packet_data[0] = 0x02 #indicate 1024 byte data packet
        else:
            self.packet_data[0] = 0x01 #indicate 128 byte data packet

        self.packet_data[1] = self.blkNumber
        self.packet_data[2] = ~self.blkNumber + 0xFF + 1
        file_ptr = self.buf_ptr[:]

        j = 0
        for i in range(3, tmpsize+3):
            self.packet_data[i] = file_ptr[j]
            j = j + 1

        if tmpsize <= packetsize:
            for i in range(tmpsize + 3, packetsize+3):
                self.packet_data[i] = 0x1A

g_nTotalDownloadSize = 0
g_nTotalWriteSize = 0
g_ser = None


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
            # self.ser.write(bytes([a_data[0]]))
            # self.ser.write('\x01\x02\x03'.encode('utf-8'))
            #self.ser.write(bytes([c]))
            
            while 1:
              a = self.ser.read(1)  
              logging.info(a)
              if a == b':' : 
                break;
            logging.info("send '<' to EAI enter control mode ....")
            while 1:
              self.ser.write(b'<')
              a = self.ser.read(1)  
              logging.info(a)
              if a == b'>' : 
                break;

            while 1:
              self.ser.write(b'B')
              a = self.ser.read(1)  
              logging.info(a)
              if a == b'B' : 
                break;

            while 1:
              self.ser.write(b'A')
              a = self.ser.read(1)  
              logging.info(a)
              if a == b'A' : 
                break;

            while 1:
              self.ser.write(b'#')
              a = self.ser.read(1)  
              logging.info(a)
              if a == b'#' : 
                break;

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
            sys.exit(1)
        #config.read('./Config.ini')
        self.m_partInfo = file_config()
        try:
            logging.info("config.get SETTING STORAGE")
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
                logging.info("todo Ymodem()")
                self.a = Ymodem()
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

