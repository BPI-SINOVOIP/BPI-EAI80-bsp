#!/usr/bin/env python
"""
 mbed CMSIS-DAP debugger
 Copyright (c) 2006-2013 ARM Limited

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
"""

import argparse
import os
import shutil
import re




argparser = argparse.ArgumentParser(
    description='config script')

argparser.add_argument(
    '-to',
    '--to_dir',
    help='path to export ugelis base dir')

argparser.add_argument(
    '-from',
    '--from_dir',
    help='path to ugelis base dir which need to export from')

argparser.add_argument(
    '-template',
    '--template_dir',
    help='path to template dir')



def main(args):
    print("Enter config.py")

    from_dir = args.from_dir
    to_dir = args.to_dir
    template_dir = args.template_dir
    print("from_dir:" + from_dir)
    print("to_dir:" + to_dir)
    print("template_dir:" + template_dir)

    # create config folder
    config_dir = to_dir + r'/include/config'
    exists = os.path.exists(config_dir)
    if exists:
        shutil.rmtree(config_dir)
    os.makedirs(config_dir)
    board_dir = config_dir + r'/board'
    os.makedirs(board_dir)
    feature_dir = config_dir + r'/feature'
    os.makedirs(feature_dir)
    sys_dir = config_dir + r'/sys'
    os.makedirs(sys_dir)

    config_from_file = (from_dir + r'/.config')
    board_name = ""
    soc_name = ""
    for line in open(config_from_file):
        if line[0:13] == "CONFIG_BOARD=":
            board_name = line[14:-2]
        if line[0:11] == "CONFIG_SOC=":
            soc_name = line[12:-2]
        if line[0:13] == "CONFIG_BOARD_":
            target_name = line[13:-3]

    # generate board header file
    shutil.copyfile(to_dir+r'/include/generated/generated_dts_board.h', board_dir + r"/" + board_name + '_board.h')

    # generate soc and feature header files
    soc_file = sys_dir + r"/" + soc_name + "_soc.h"
    soc_fd = open(soc_file, 'a+')
    feature_file = feature_dir + r"/" + board_name + "_feature.h"
    feature_fd = open(feature_file, 'a+')
    soc_end = False
    new_line = ""
    for line in open(config_from_file):
        new_line = ""
        if line[0] == "#":
            if line[2:9] == "CONFIG_":
                pass
            else:
                if line[1] == "\n":
                    new_line = "/********************************************/\n"
                else:
                    new_line = "/*"
                    new_line += (line[1:-1] + " */")
                    new_line += "\n"
        elif line[0:7] == "CONFIG_":
            new_line = "#define "
            index = line.find("=")
            new_line += line[:index]
            if line[index+1] == "y":
                new_line += " 1"
                new_line += "\n"
            else:
                new_line += " " + line[index+1:]
        else:
            new_line = line

        if line[2:15] == "Board Options":
            soc_end = True
        if soc_end == False:
            soc_fd.write(new_line)
        else:
            feature_fd.write(new_line)
    soc_fd.close()
    feature_fd.close()

    # generate target_config hearder
    target_config_file = config_dir + r"/target_config.h"
    shutil.copyfile(template_dir + r'/template_target_config.h', target_config_file)
    target_txt = "#define " + target_name + " 1"
    f = open(target_config_file, 'r+')
    all_lines = f.readlines()
    f.seek(0, 0)
    # f.truncate()
    for line in all_lines:
        line_new = line.replace("//#define TO_BE_REPLACE = 1", target_txt)
        f.write(line_new)
    f.close()

    # autoconf.h and generated_dts_board.h
    file_temp = to_dir + r'/include/generated/autoconf.h'
    if os.path.exists(file_temp):
        os.remove(file_temp)
    file_temp = to_dir + r'/include/generated/generated_dts_board.conf'
    if os.path.exists(file_temp):
        os.remove(file_temp)
    file_temp = to_dir + r'/include/generated/generated_dts_board.h'
    os.remove(file_temp)
    shutil.copyfile(template_dir + r'/template_generated_dts_board.h', to_dir + r"/include/generated/generated_dts_board.h")
    shutil.copyfile(template_dir + r'/template_autoconf.h', to_dir + r"/include/generated/autoconf.h")

    print("Leave config.py")

if __name__ == '__main__':
    args = argparser.parse_args()
    main(args)
