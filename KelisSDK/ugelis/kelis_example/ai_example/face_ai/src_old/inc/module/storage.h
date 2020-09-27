#ifndef __STORAGE_H__
#define __STORAGE_H__

typedef void (*StorageCallback)(char *stage);

int ai_load_ops_data(unsigned int ops_offset, unsigned int ops_len, unsigned int img_offset, unsigned int img_len, unsigned char *ops_data, unsigned char *image);

void ai_init_storage();

#endif
