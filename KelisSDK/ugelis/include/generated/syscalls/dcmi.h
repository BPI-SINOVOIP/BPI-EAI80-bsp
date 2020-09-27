
/* auto-generated by gen_syscalls.py, don't edit */

#ifndef _ASMLANGUAGE

#include <syscall_list.h>
#include <syscall_macros.h>

#ifdef __cplusplus
extern "C" {
#endif

K_SYSCALL_DECLARE2(K_SYSCALL_DCMI_START_STREAMING, dcmi_start_streaming, int, struct device *, dev, uint32_t, mode);

K_SYSCALL_DECLARE1(K_SYSCALL_DCMI_STOP_STREAMING, dcmi_stop_streaming, int, struct device *, dev);

K_SYSCALL_DECLARE2(K_SYSCALL_DCMI_SET_FMT_VID_CAP, dcmi_set_fmt_vid_cap, int, struct device *, dev, const struct dcmi_format *, f);

K_SYSCALL_DECLARE2(K_SYSCALL_DCMI_GET_FMT_VID_CAP, dcmi_get_fmt_vid_cap, int, struct device *, dev, struct dcmi_format *, f);

K_SYSCALL_DECLARE3(K_SYSCALL_DCMI_IOCTL, dcmi_ioctl, int, struct device *, dev, uint32_t, cmd, void *, arg);

#ifdef __cplusplus
}
#endif

#endif