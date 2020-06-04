#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include "zlib.h"




//#define COMPRESS_DEBUG 1
//#define LOG_DEBUG
#define LOG_ERR


#ifndef LOG_DEBUG
    #define log_debug(...)      do {} while (0)
#else
    #define log_debug           printf
#endif

#ifndef LOG_ERR
    #define log_err(...)        do {} while (0)
#else
    #define log_err             printf
#endif

struct compress_head
{
    unsigned int size; /*head image size*/
    unsigned int compress_num;
    unsigned int section_desc_offset;
    struct compress_section_desc *section_desc; /*not write to file*/
} __attribute__((packed));


struct compress_section_desc
{
    unsigned int index;
    unsigned int offset; /*compress image offset*/
    unsigned int length; /*real compress length*/
};

struct compress_config
{
    char *src;
    char *tar;
    int fd_src;
    int fd_tar;
    struct compress_head *head;
    unsigned int per_size; /*size for compress for one piece source*/
    char compress_level;
    unsigned int tar_length;  /*output*/
};





static int do_compress(struct compress_config *config);
static int make_head_image(struct compress_head *head, int head_fd);
static int prepare_file_open(const char *src_name, const char *target_name, const char *head_name,
                             int *pfd_src, int *pfd_tar, int *pfd_head);
static int prepare_alloc_compress_head(int fd_src, unsigned int compress_per_size, struct compress_head *head);





static int prepare_file_open(const char *src_name, const  char *target_name, const char *head_name,
                             int *pfd_src, int *pfd_tar, int *pfd_head)
{
    int fd_src;
    int fd_tar;
    int fd_head;

    fd_src = open(src_name, O_RDWR, 0755);
    if (fd_src < 0)
    {
        log_err("fail to open src image\n");
        return -1;
    }
    *pfd_src = fd_src;


    fd_tar = open(target_name, O_RDWR | O_TRUNC | O_CREAT, 0755);
    if (fd_tar < 0)
    {
        log_err("fail to open target image\n");
        return -1;
    }
    *pfd_tar = fd_tar;


    fd_head = open(head_name, O_RDWR | O_TRUNC | O_CREAT, 0755);
    if (fd_head < 0)
    {
        log_err("fail to open fd_head image\n");
        return -1;
    }
    *pfd_head  = fd_head;

    return 0;
}




static int prepare_alloc_compress_head(int fd_src, unsigned int compress_per_size, struct compress_head *head)
{
    unsigned int length;
    unsigned int compress_num;
    struct compress_section_desc *p =  NULL;

    length = lseek(fd_src, 0, SEEK_END);
    lseek(fd_src, 0, SEEK_SET);
    compress_num =  length / compress_per_size;
    if (length % compress_per_size)
    {
        compress_num++;
    }

    head->compress_num = compress_num;
    head->size = sizeof(struct compress_head) + sizeof(struct compress_section_desc) * (head->compress_num) - sizeof(p);
    head->section_desc_offset = sizeof(struct compress_head) - sizeof(p);


    /*for do_compress to head, we will malloc more 1 that we need  */
    head->section_desc = malloc((sizeof(struct compress_section_desc)) * (compress_num + 1));
    if (!head->section_desc)
    {
        log_err("fail to malloc section_desc\n");
        return -1;
    }



    return 0;

}

static int do_compress(struct compress_config *config)
{
    char *src = config->src;
    char *tar = config->tar;


    int fd_src = config->fd_src;
    int fd_tar = config->fd_tar;
    unsigned int per_size = config->per_size;
    struct compress_head *head = config->head;
    unsigned int n;
    unsigned long k;
    int ret = 0;
    unsigned int i = 0;
    unsigned int compress_total = 0;
    unsigned int uncompress_total = 0;
    unsigned long uncomprLen  = config->per_size;
    unsigned long tar_length = config->per_size;
    unsigned long alin4_length;
    int adjust_alin4_flag = 0;

#ifdef COMPRESS_DEBUG
    char *temp_src_bak = NULL;
    temp_src_bak = malloc(config->per_size);
    if (!temp_src_bak)
    {
        log_err("Err:malloc temp_src_bak buf \n");
        return -1;
    }
#endif


    head->section_desc[0].offset = head->size;
    do
    {

        n = read(fd_src, src, per_size);
        if (n < 0)
        {
            log_err("read src file err n=%d\n", n);
            return -1;
        }
        else if (n == 0)
        {
#ifdef COMPRESS_DEBUG
            log_debug("####test pass###\n");
#endif
            break;
        }

#ifdef COMPRESS_DEBUG
        memcpy(temp_src_bak, src, n);
#endif


        tar_length = config->per_size;
        memset(tar, 0, tar_length);
        ret = compress2(tar, &tar_length, src, n, config->compress_level);
        if (ret)
        {
            log_err("compress2 err:%d\n", ret);
            return -1;
        }

        if (tar_length >= n)
        {
            log_err("compress length is more than src length\n");
            return -1;
        }

#ifdef COMPRESS_DEBUG
        compress_total += tar_length;
        uncompress_total += n;
        log_debug("\n####compress[%d]: uncompress:%d compress:%d  <_current_rate:%f_><__total_rate:%f_>\n",
                  (i + 1), n, tar_length, ((float)tar_length / (float)n), ((float)compress_total / (float)uncompress_total));

        uncomprLen = config->per_size;
        memset(src, 0, uncomprLen);
        ret = uncompress(src, &uncomprLen, tar, tar_length);
        if (ret)
        {
            log_err("uncompress err:%d\n", ret);
            return -1;
        }

        if (uncomprLen != n)
        {
            log_err("uncompress length is less than compress length\n");
            return -1;
        }
        log_debug("***uncompress: compress:%d uncompress:%d\n", tar_length, uncomprLen);
        if (memcmp(src, temp_src_bak, uncomprLen))
        {
            log_err("!!!!compress data err\n");
            return -1;
        }
#endif
        /*check wether to alin4 byte*/
        if (tar_length % 4)
        {
            log_debug("\n#####Not aline4 tar_length:%lu #######\n", tar_length);
            alin4_length = tar_length + 4;
            alin4_length = alin4_length & (~(4 - 1));
            adjust_alin4_flag = 1;
            log_debug("#####adjust aline4 tar_length:%lu #######\n", alin4_length);
        }
        else
        {
            alin4_length = tar_length;
            adjust_alin4_flag = 0;
        }

        if (write(fd_tar, tar, alin4_length) < 0)
        {
            log_err("write target file err\n");
            return -1;
        }

        /*fill head*/
        head->section_desc[i].index = i;
        head->section_desc[i].length = tar_length;
        head->section_desc[i + 1].offset = head->section_desc[i].offset + alin4_length;

        /*update for next loop*/
        i++;
    } while (1);


    return 0;

}




static int make_head_image(struct compress_head *head, int head_fd)
{
    unsigned int i;
    unsigned int length_section_desc = head->compress_num * (sizeof(struct compress_section_desc));

    lseek(head_fd, 0, SEEK_SET);
    if (write(head_fd, head, (head->section_desc_offset)) < 0)
    {
        log_err("write head file err\n");
        return -1;
    }

    if (write(head_fd, head->section_desc, length_section_desc) < 0)
    {
        log_err("write head file err\n");
        return -1;
    }


#if 0
    lseek(head_fd, 0, SEEK_SET);

    read(head_fd, &debug_head, head->section_desc_offset);
    printf("size:%d compress_num:%d section_desc_offset:%d\n", debug_head.size, debug_head.compress_num, debug_head.section_desc_offset);

    read(head_fd, head_section_buf, length_section_desc);

    for (i = 0; i < head->compress_num; i++)
    {
        printf("index:%d offset:%d length:%d\n", head->section_desc[i].index, head->section_desc[i].offset, head->section_desc[i].length);
    }
#endif


    return 0;
}




/**
 * argv[1] src_file_name
 * argv[2] target_file_name
 * argv[3] head_file_name
 * argv[4] src_file per piece : defaut 64k
 * argv[5] compress level
 */
int main(int argc, char *argv[])
{
    int ret = 0;

    int fd_src;
    int fd_tar;
    int fd_head;
    char *src = NULL;
    char *tar = NULL;
    struct compress_head head;
    struct compress_config config;

    char *src_file_name            = argv[1];
    char *target_file_name         = argv[2];
    char *head_file_name           = argv[3];
    unsigned int src_per_piece     = (atoi(argv[4]));
    int compress_level             = atoi(argv[5]);

    src = malloc(src_per_piece);
    if (!src)
    {
        log_err("Err:malloc src buf \n");
        return ret;
    }
    tar = malloc(src_per_piece);
    if (!tar)
    {
        log_err("Err:malloc tar buf \n");
        return ret;
    }

    ret = prepare_file_open(src_file_name, target_file_name, head_file_name,
                            &fd_src, &fd_tar, &fd_head);
    if (ret)
    {
        log_err("Err:prepare_file_open\n");
        return ret;
    }

    ret = prepare_alloc_compress_head(fd_src, src_per_piece, &head);
    if (ret)
    {
        log_err("Err:prepare_alloc_compress_head\n");
        return ret;
    }

    config.src             = src;
    config.tar             = tar;
    config.fd_src          = fd_src;
    config.fd_tar          = fd_tar;
    config.head            = &head;
    config.per_size        = src_per_piece;
    config.compress_level  = compress_level;
    ret = do_compress(&config);
    if (ret)
    {
        log_err("Err:do_compress\n");
        return ret;
    }


    ret = make_head_image(&head, fd_head);
    if (ret)
    {
        log_err("Err:make_head_image\n");
        return ret;
    }

    log_debug("success build compress %s \n", target_file_name);

    return ret;

}





















































