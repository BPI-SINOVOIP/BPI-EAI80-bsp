#include "ff_wrapper.h"
#include "jpeg.h"
#include <string.h>

#define BUF_DEBUG   1

extern imagedatabuf_t *g_buf;

void file_close(FIL *fp)
{
#ifdef  BUF_DEBUG
    g_buf->seek_flag = 0;
#else
    fclose(fp);
    fp =  NULL;

#endif
}

void file_seek(FIL *fp, unsigned int offset)
{
#ifdef  BUF_DEBUG
    g_buf->seek_flag = 1 ;
    g_buf->seek_set_buf = &g_buf->beginbuf[offset];
#else
    fseek(fp, offset, SEEK_SET); // ????   SEEK_SET： 文件开头  ,SEEK_CUR： 当前位置 ,SEEK_END： 文件结尾
#endif
}

void write_word(FIL *fp, unsigned short value)
{
#ifdef  BUF_DEBUG
    if (g_buf->seek_flag == 0)
    {
        g_buf->buf[0] = value & 0xFF;
        g_buf->buf[1] = (value & 0xFF00) >> 8 ;
        g_buf->buf += 2;
    }
    else
    {
        g_buf->seek_set_buf[0] = value & 0xFF;
        g_buf->seek_set_buf[1] = (value & 0xFF00) >> 8 ;
        g_buf->seek_set_buf += 2;
    }
#else
    UINT bytes;
    int len = fwrite(&value, sizeof(unsigned short), 1, fp);
#endif
}

void write_long(FIL *fp, unsigned int value)
{

#ifdef  BUF_DEBUG
    if (g_buf->seek_flag == 0)
    {
        g_buf->buf[0] = value & 0xFF;
        g_buf->buf[1] = (value & 0xFF00) >> 8 ;
        g_buf->buf[2] = (value & 0xFF0000) >> 16 ;
        g_buf->buf[3] = (value & 0xFF000000) >> 24 ;
        g_buf->buf += 4;
    }
    else
    {
        g_buf->seek_set_buf[0] = value & 0xFF;
        g_buf->seek_set_buf[1] = (value & 0xFF00) >> 8 ;
        g_buf->seek_set_buf[2] = (value & 0xFF0000) >> 16 ;
        g_buf->seek_set_buf[3] = (value & 0xFF000000) >> 24 ;
        g_buf->seek_set_buf += 4;
    }
#else
    UINT bytes;
    int len = fwrite(&value, sizeof(unsigned int), 1, fp);
#endif
}

void write_data(FIL *fp, void *data, unsigned int size)
{
#ifdef  BUF_DEBUG

    printf("func[%s],line=%d \n", __FUNCTION__, __LINE__);
    if (g_buf->seek_flag == 0)
    {
        memcpy((unsigned char *)g_buf->buf, data, size);
        g_buf->buf += size;
    }
    else if (g_buf->seek_flag == 1)
    {
        memcpy((unsigned char *)g_buf->seek_set_buf, data, size);
        g_buf->seek_set_buf += size;
    }
    printf("func[%s],line=%d \n", __FUNCTION__, __LINE__);

#else
    UINT bytes;
    int len = fwrite(data, sizeof(char), size, fp);
    if (len != size)
    {
        printf(" debug  error\n");
    }
#endif
}
