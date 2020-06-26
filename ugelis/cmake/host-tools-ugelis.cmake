# Lots of duplications here.
# FIXME: maintain this only in one place.

# We need to separate actual toolchain from the host-tools required by uGelis
# and currently provided by the uGelis SDK. Those tools will need to be
# provided for different OSes and sepearately from the toolchain.

set_ifndef(UGELIS_SDK_INSTALL_DIR "$ENV{UGELIS_SDK_INSTALL_DIR}")
set(UGELIS_SDK_INSTALL_DIR ${UGELIS_SDK_INSTALL_DIR} CACHE PATH "uGelis SDK install directory")

if(NOT UGELIS_SDK_INSTALL_DIR)
  # Until https://github.com/ugelisproject-rtos/ugelis/issues/4912 is
  # resolved we use UGELIS_SDK_INSTALL_DIR to determine whether the user
  # wants to use the uGelis SDK or not.
  return()
endif()

set(REQUIRED_SDK_VER 0.9.3)
set(TOOLCHAIN_VENDOR zephyr)
set(TOOLCHAIN_ARCH x86_64)

set(sdk_version_path ${UGELIS_SDK_INSTALL_DIR}/sdk_version)
if(NOT (EXISTS ${sdk_version_path}))
  message(FATAL_ERROR
    "The file '${UGELIS_SDK_INSTALL_DIR}/sdk_version' was not found. \
Is UGELIS_SDK_INSTALL_DIR=${UGELIS_SDK_INSTALL_DIR} misconfigured?")
endif()

file(READ ${sdk_version_path} SDK_VERSION)
if(${REQUIRED_SDK_VER} VERSION_GREATER ${SDK_VERSION})
  message(FATAL_ERROR "The SDK version you are using is old, please update your SDK.
You need at least SDK version ${REQUIRED_SDK_VER}.
The new version of the SDK can be download from:
https://github.com/ugelisproject-rtos/meta-ugelis-sdk/releases/download/${REQUIRED_SDK_VER}/ugelis-sdk-${REQUIRED_SDK_VER}-setup.run
")
endif()

if(MINGW)
  set(TOOLCHAIN_HOME ${UGELIS_SDK_INSTALL_DIR}/sysroots/i686-pokysdk-mingw32)
else()
  set(TOOLCHAIN_HOME ${UGELIS_SDK_INSTALL_DIR}/sysroots/${TOOLCHAIN_ARCH}-pokysdk-linux)

endif()

# Path used for searching by the find_*() functions, with appropriate
# suffixes added. Ensures that the SDK's host tools will be found when
# we call, e.g. find_program(QEMU qemu-system-x86)
list(APPEND CMAKE_PREFIX_PATH ${TOOLCHAIN_HOME}/usr)

# TODO: Use find_* somehow for these as well?
set_ifndef(QEMU_BIOS            ${TOOLCHAIN_HOME}/usr/share/qemu)
set_ifndef(OPENOCD_DEFAULT_PATH ${TOOLCHAIN_HOME}/usr/share/openocd/scripts)
