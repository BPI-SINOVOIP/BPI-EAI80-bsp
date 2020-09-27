#ifndef __FACE_RECOGNIZE_H__
#define __FACE_RECOGNIZE_H__

#include <worker/worker.h>

void face_recognize_thread(void *data, void *param, void *not_used);
int face_recognize_init(easynet_proc_param_t *param);
int face_recognize_exit(easynet_proc_param_t *param);
int face_recognize_frame_in(easynet_proc_param_t *param);

#endif //__FACE_RECOGNIZE_H__

