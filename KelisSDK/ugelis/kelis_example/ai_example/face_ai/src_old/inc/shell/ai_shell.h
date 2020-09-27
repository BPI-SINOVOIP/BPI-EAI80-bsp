#ifndef __AI_SHELL_H__
#define __AI_SHELL_H__

#include "ai_common.h"

void ai_init_shell(FaceAIData *data);
void show_capture(FaceAIData *fad, frame_data_t *data, int ignore[], int start, int end);
void ui_clear_textarea(FaceAIData *fad);
void ui_clear_cam();
void ai_show_text(char *text);
int import_record(struct easynet_dev *dev, FaceAIData *fad, int backup_base);
int delete_record(FaceAIData *fad, int label);

#endif //__AI_SHELL_H__

