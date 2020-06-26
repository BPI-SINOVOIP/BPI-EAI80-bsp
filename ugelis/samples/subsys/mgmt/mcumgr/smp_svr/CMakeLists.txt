# Top-level CMakeLists.txt for the skeleton application.
#
# Copyright (c) 2017 Open Source Foundries Limited
#
# SPDX-License-Identifier: Apache-2.0
#
# This provides a basic application structure suitable for communication using
# mcumgr.  It can be used as a starting point for new applications.

# Standard uGelis application boilerplate.
include($ENV{UGELIS_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(NONE)

target_sources(app PRIVATE
    src/main.c
)

ugelis_link_libraries_ifdef(CONFIG_FILE_SYSTEM_NFFS NFFS)
