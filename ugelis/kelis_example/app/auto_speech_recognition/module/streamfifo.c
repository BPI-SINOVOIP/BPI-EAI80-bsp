#include "streamfifo.h"

int dma_send(unsigned int pkt_length, int base_address)
{
    streamfifo_init((uint32_t *)base_address, pkt_length, AHB_WRAP4, UNLEN16, NORMAL_MODE);
    return 0;
}

void streamfifo_init(uint32_t *src, uint32_t len, uint8_t burst_type, uint8_t burst_unlen, uint8_t continue_mode)
{

    uint32_t level = 0;
    STRFIFO->CTRL = 0;

    if (burst_type == AHB_WRAP4)
    {
        level = 4 - 1;
    }
    else if (burst_type == AHB_WRAP8)
    {
        level = 8 - 1;
    }
    else if (burst_type == AHB_WRAP16)
    {
        level = 16 - 1;
    }
    else if (burst_type == AHB_SINGLE)
    {
        level = 1 - 1;
    }
    else if (burst_type == AHB_INCR)
    {
        switch (burst_unlen)
        {
            case UNLEN4    :
                level = 4   - 1;
                break;
            case UNLEN8    :
                level = 8   - 1;
                break;
            case UNLEN16   :
                level = 16  - 1;
                break;
            case UNLEN32   :
                level = 32  - 1;
                break;
            case UNLEN64   :
                level = 64  - 1;
                break;
            case UNLEN128  :
                level = 128 - 1;
                break;
            case UNLEN256  :
                level = 256 - 1;
                break;
            case UNLEN512  :
                level = 512 - 1;
                break;
            default :
                break;
        }
    }

    STRFIFO->SRC  = (uint32_t) src;
    if (continue_mode == CONTINUE_MODE)
    {
        STRFIFO->DATACNT = 0x00ffffff;
    }
    else
    {
        STRFIFO->DATACNT = len;
    }
    STRFIFO->CTRL = (1 << 31)             | //[31]   done_ie
                    (1 << 30)             | //[30]   over_ie
                    (1 << 29)             | //[29]   under_ie
                    (0 << 28)             | //[28]   full_ie
                    ((1024 - level) << 8) | //[17:8] dma_level
                    (burst_unlen << 5)    | //[7:5]  burst unlen : 2->len 16
                    (burst_type << 2)     | //[4:2]  burst type : 1->INCR
                    (1 << 1)              | //[1]    internal fifo enable
                    (1 << 0)              ; //[0]    STRFIFO enable


}

