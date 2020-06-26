"""
ugelis SDK
Copyright (c) 2018-2020 Gree Appliances Inc.

This python makes a clean-up repository for Ugelis OS source code !
A reference usage is as follows:
python cleanup_ugelis_repo.py -s ./0711/ugelis/Source/OS/ -d ./0726/ugelis_keil -i keil -v
"""

import sys
from os.path import join, abspath, dirname, exists
from os.path import basename, relpath, normpath, splitext, isfile, getsize, isdir, split
from os import makedirs, walk, mkdir, listdir, remove
import copy
import errno
import time
import shutil
import argparse

src_file_parent_directory = ""
dest_file_parent_directory = ""
verbose = False

# Logging and output
def log(msg):
    sys.stdout.write(msg)

def message(msg):
    return "[Ugelis] :%s\n" % msg

def info(msg, level=1):
    if level <= 0 or verbose:
        for line in msg.splitlines():
            log(message(line))

def action(msg):
    for line in msg.splitlines():
        log(message(line))

def error(msg, code=-1):
    for line in msg.splitlines():
        sys.stderr.write("[Ugelis] ERROR: %s\n" % line)
    sys.stderr.write("---\n")
    sys.exit(code)


"""
Files with an extension name as follows are
required by KEIL IDE
"""
keil_extname_list = [
                # c code for compiling
                '.c', '.h',
                # assembly code for compiling
                '.s',
                # c++ code for compiling
                '.cpp', '.hpp',
                # Object files for compiling
                '.o',
                # library files for compiling
                '.a'
                 ]

"""
Files with an extension name as follows are
required by GreeIDE IDE
"""
greeide_extname_list = [
                # c code for compiling
                '.c', '.h',
                # assembly code for compiling
                '.s',
                # c++ code for compiling
                '.cpp', '.hpp',
                # Object files for compiling
                '.o',
                # library files for compiling
                '.a', '.so'
                ]

extname_dict = {'keil': keil_extname_list, 'greeide': greeide_extname_list}

rm_dirs_dict = {'arch':   ['arch/x86', 'arch/arm/soc/nxp_kinetis', 'arch/arm/soc/st_stm32', 'arch/arm/soc/ti_simplelink'],
                'boards': ['boards/x86', 'boards/arm/cc3200_launchxl', 'boards/arm/frdm_k64f', 'boards/arm/stm32f4_apollo', 'boards/arm/stm32f4_disco'],
                'doc':    ['doc'],
                'dts':    ['dts'],
                'kernel': ['kernel/configs'],
                'msic':   ['msic/generated'],
                'mytools':['mytools'],
                'scripts':['scripts'],
                'subsys': ['subsys/usb', 'subsys/usb_stm', 'subsys/usb_stm_v1.6.3'],
                'samples':['samples/basic', 'samples/bluetooth', 'samples/cc3200', 'samples/drivers', 'samples/environmental_sensing','samples/grove', 'samples/hello_world',
                           'samples/ipm', 'samples/legacy', 'samples/logger-hook', 'samples/net', 'samples/nfc', 'samples/philosophers', 'samples/power', 'samples/sensor',
                           'samples/shell', 'samples/static_lib', 'samples/stm32f4_apollo_demo', 'samples/synchronization', 'samples/testing', 'samples/usb', 'samples/wifi_station'],
                'tests':  ['tests']
                }

"""
Directories to be removed, only applied in ugelis OS
"""
dirs_to_be_removed = [
                    # arm folder
                    'arch/x86',
                    'arch/arm/soc/nxp_kinetis',
                    'arch/arm/soc/st_stm32',
                    'arch/arm/soc/ti_simplelink',

                    # boards folder
                    'boards/x86',
                    'boards/arm/cc3200_launchxl',
                    'boards/arm/frdm_k64f',
                    'boards/arm/stm32f4_apollo',
                    'boards/arm/stm32f4_disco',

                    # doc folder
                    'doc',

                    # dts folder
                    'dts',

                    # kernel folder
                    'kernel/configs',

                    # misc folder
                    'misc/generated',

                    # mytools folder
                    'mytools',

                    # script folder
                    'scripts',

                    # subsys folder
                    'subsys/usb',
                    'subsys/usb_stm',
                    'subsys/usb_stm_v1.6.3',

                    # samples folder
                    'samples/basic',
                    'samples/bluetooth',
                    'samples/cc3200',
                    'samples/drivers',
                    'samples/environmental_sensing',
                    'samples/grove',
                    'samples/hello_world',
                    'samples/ipm',
                    'samples/legacy',
                    'samples/logger-hook',
                    'samples/net',
                    'samples/nfc',
                    'samples/philosophers',
                    'samples/power',
                    'samples/sensor',
                    'samples/shell',
                    'samples/static_lib',
                    'samples/stm32f4_apollo_demo',
                    'samples/synchronization',
                    'samples/testing',
                    'samples/usb',
                    'samples/wifi_station',

                    # tests folder
                    'tests'
                    ]



file_cnts = 0

"""
Check whether both source and destination directories  exist
or not
"""
def check_parent_directory(src_dir, dest_dir):
    try:
        if not exists(src_dir):
            print('The source directory does not exist')
            sys.exit(1)

        if not exists(dest_dir):
            makedirs(dest_dir)
    except IOError:
        error("Unable to create destination directory")

"""
Copy sub-folders and all files into destination directory
"""
def copy_files2_destdir(src_dir, dest_dir):
    global file_cnts
    info(u"Source directory: %s " %src_dir)
    info(u"Distination directory: %s" %dest_dir)
    info(u"%s current folders: %s already coped %s file(s)" %(time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time())), src_dir, file_cnts))

    for f in listdir(src_dir):
        src_file = join(src_dir, f)
        dest_file = join(dest_dir, f)

        if isfile(src_file):
            if not exists(dest_dir):
                makedirs(dest_dir)
            file_cnts += 1

            if not exists(dest_file) or (exists(dest_file) and getsize(dest_file) != getsize(src_file)):
                open(dest_file, "wb").write(open(src_file,"rb").read())
                info(u"%s %s copy completed " %(time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time())), dest_file))
            else:
                info(u"%s %s, already exists, skipped" %(time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time())), dest_file))

        if isdir(src_file):
            copy_files2_destdir(src_file, dest_file)

"""
Reserve files with the extension names of ext_names
and remove unrelated files in specified directory
"""
def remove_unrelated_files(dest_dir, ext_names):
    global file_cnts

    info(u"%s current folders: %s " % (time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time())), dest_dir))

    for f in listdir(dest_dir):
        src_file = join(dest_dir, f)

        if isfile(src_file) and exists(src_file):
            files = split(src_file)
            files_compiled = files[-1]
            _, ext_name = splitext(files_compiled)
            ext_name = ext_name.lower()
            if ext_name not in ext_names:
                remove(src_file)
                info(u"%s %s removed " % (
                time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time())), src_file))

        if isdir(src_file):
            remove_unrelated_files(src_file, ext_names)

"""remove unrequired directories"""
"""
def remove_unrequired_dirs(dest_dirs):
    global dest_file_parent_directory
    for d in dest_dirs:
        rm_dir = normpath(join(dest_file_parent_directory, d))
        # remove all trees
        if isdir(rm_dir):
            shutil.rmtree(rm_dir)
            print(u"%s current folders: %s removed" % (time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time())), rm_dir))
"""

def remove_unrequired_dirs(dest_dirs):
    global dest_file_parent_directory

    for k, dest_dirs in dest_dirs.items():
        for d in dest_dirs:
            rm_dir = normpath(join(dest_file_parent_directory, d))
            # remove all trees
            if isdir(rm_dir):
                shutil.rmtree(rm_dir)
                info(u"%s current folders: %s removed" % (time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time())), rm_dir))

def main():

    global src_file_parent_directory, dest_file_parent_directory, extname_dict, dirs_to_be_removed, verbose

    parser = argparse.ArgumentParser()

    parser.add_argument("-v", "--verbose", help="increase output verbostiy",
                        action="store_true")

    parser.add_argument("-s", "--source", dest="src_dir", default=None,
                        help="the source directory to be exported")

    parser.add_argument("-d", "--destination", dest="dst_dir", default=None,
                        help="the destination directory where stores clean-up source repository")

    parser.add_argument("-i", "--ide", dest="target_ide", default=None,
                        help="the target IDE for source, Only supports KEIL and GreeIDE")

    args = parser.parse_args()

    if not args.src_dir:
        print("No source specified")
        sys.exit(1)

    if not args.dst_dir:
        print("No destination specified")
        sys.exit(1)

    if not args.target_ide:
        print("No target ide specified")
        sys.exit(1)

    verbose = args.verbose

    src_file_parent_directory = args.src_dir
    if not exists(src_file_parent_directory):
        print(u"Source directory: %s not exist" %src_file_parent_directory)
        sys.exit(1)

    dest_file_parent_directory = join(args.dst_dir, 'ugelis_os')
    if not exists(dest_file_parent_directory):
        try:
            makedirs(dest_file_parent_directory)
        except:
            error("Create destination directory %s failed" %split(dest_file_parent_directory)[0])

    if args.target_ide.lower() not in extname_dict.keys():
        print("Only Keil & GreeIDE are supported")
        sys.exit(1)

    action("=============================================================")
    """
    action("Check source and destination directory")
    check_parent_directory(src_file_parent_directory, dest_file_parent_directory)
    """

    action("Copy directory ...")
    copy_files2_destdir(src_file_parent_directory, dest_file_parent_directory)
    action("Copy directory Done !")

    action("Remove unrelated files !")
    remove_unrelated_files(dest_file_parent_directory, extname_dict['keil'])

    action("Remove unrelated folders !")
    remove_unrequired_dirs(rm_dirs_dict)

    action("A clean-up source repository created !")

    sys.exit(0)

if __name__ == "__main__":
    main()