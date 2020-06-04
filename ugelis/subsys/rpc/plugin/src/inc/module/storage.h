#ifndef __STORAGE_H__
#define __STORAGE_H__

typedef void (*StorageCallback) (char *stage);

int ai_data_read(unsigned char *wei_addr, unsigned char *cmd_addr, unsigned char *base_addr);
void ai_init_storage(StorageCallback sc);

#endif
