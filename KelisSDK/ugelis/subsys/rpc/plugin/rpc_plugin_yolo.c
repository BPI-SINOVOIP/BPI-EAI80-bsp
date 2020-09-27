#include <ugelis.h>
#include <kernel_structs.h>
#include <toolchain.h>
#include <linker/sections.h>
#include <misc/dlist.h>
#include <init.h>
#include <gm_hal_rpc.h>
#include <rpc_core.h>
#include <rpc_plugin_yolo.h>

//#define SLAVE_YOLO_DEBUG 1

#ifdef SLAVE_YOLO_DEBUG
    #define SlaveYoloDebug       printk
#else
    #define SlaveYoloDebug(...)  do{}while(0);
#endif

/*--------------------buble Sort ---------------------*/ 
void bubleSort(Box_t data[], int n) 
{ 
     int i,j;
	 Box_t temp;
     for(j=0;j<n-1;j++) 
	 { 
	     for(i=0;i<n-j-1;i++) 
		 { 
	    	if(data[i].score<data[i+1].score) //descent: < ascent: >
			{
                temp = data[i];
                data[i] = data[i+1];
                data[i+1] = temp;
            }
	     }
	}  
}

/**
*
* This function run average pooling
*
* @param a array
*
* @return
*
*
* @note     None.
*
******************************************************************************/

int nms_comparator(const void *pa, const void *pb){
    Box_t a = *(Box_t *)pa;
    Box_t b = *(Box_t *)pb;
    double diff = a.score - b.score;

    if(diff < 0) return 1;
    else if(diff > 0) return -1;
    return 0;
}


double overlap(double l1, double r1, double l2, double r2){
    double left = l1 > l2 ? l1 : l2;
    double right = r1 < r2 ? r1 : r2;
    return right - left;

}

double box_intersection(Box_t a, Box_t b){
    double w = overlap(a.box[1], a.box[3], b.box[1], b.box[3]);
    double h = overlap(a.box[0], a.box[2], b.box[0], b.box[2]);
    if(w < 0 || h < 0) return 0;
    double area = w*h;
    return area;
}

double box_union(Box_t a, Box_t b)
{
    double i = box_intersection(a, b);
    double u = (a.box[2] - a.box[0])*(a.box[3] - a.box[1]) + (b.box[2] - b.box[0])*(b.box[3] - b.box[1]) - i;
    return u;
}

double box_iou(Box_t a, Box_t b){
    double c;
    if (box_intersection(a, b)/box_intersection(a,a)>box_intersection(a, b)/box_intersection(b,b))
        c=box_intersection(a, b)/box_intersection(a,a);
    else
        c=box_intersection(a, b)/box_intersection(b,b);
    if (c<box_intersection(a, b)/box_union(a, b))
    c=box_intersection(a, b)/box_union(a, b);
    return c;

}

static double exp(double x)
{
    x = 1.0 + x / 256.0;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    x *= x;
    return x;
}


static int PostYolo(ObjBodyData *p_body_data, short int *p_OutputImage, double *d_box, double *box_class_probs, Box_t *s_box)
{
    int i = 0, j = 0, r = 0, c = 0, channel = 0, maxIndex = 0;
    double box_x = 0, box_y = 0, box_w = 0, box_h = 0, box_confidence = 0, sum = 0, maxData = 0 ;
    double thresh = 0;
	int out_width , out_height , box_channel , confidence_channel , box_num , class_channel ;
	double nms_thresh;
	int NumClasses, gap;
    int cnt = 0;
	double anchers[5][2];
    Box_t * head = NULL;
    Box_t * previous = NULL;
	double *box = d_box;
	Box_t * p = NULL;
    int flag = 0;	
	int tmp_pkt = 0;

	//for each class, select the corresponding box
    Box_t *s = s_box;

	if(d_box == NULL || box_class_probs == NULL || s_box == NULL)
	{
		return RET_ERROR;
	}

	if (p_body_data->det_type == DET_BODY)
	{
		thresh = 0.5;  //0.6
	}
	else if (p_body_data->det_type == DET_GEST)
	{
		 thresh = 0.3;//0.20; //0.8 for old weigth bin
	}

	if (p_body_data->det_type == DET_BODY)
	{
		out_width = 7;
		out_height = 7;
		box_channel = 4; 
		confidence_channel = 1;
		box_num = 5;
		class_channel = 1;
	}
    else if (p_body_data->det_type == DET_GEST)
	{
	     out_width = 7;
		 out_height = 7;
		 box_channel = 4;
		 confidence_channel = 1;
		 box_num = 5;	
		 class_channel = 2; 
	}
    
	if (p_body_data->det_type == DET_BODY)
	{
		nms_thresh = 0.5;
		NumClasses = 1;
		gap = box_channel + confidence_channel + class_channel;
	}

	else if (p_body_data->det_type == DET_GEST)
	{
		nms_thresh = 0.3;
		NumClasses = class_channel;
		gap = box_channel + confidence_channel + class_channel;
	}

    memset(p_body_data->box, 0x00, sizeof(Box_t) * 100);
	p_body_data->boxcnt = 0;

	if (p_body_data->det_type == DET_BODY)
	{
		anchers[0][0] = 0.57;
		anchers[0][1] = 1.42;

		anchers[1][0] = 1.44;
		anchers[1][1] = 3.71;

		anchers[2][0] = 2.62;
		anchers[2][1] = 6.4;

    	anchers[3][0] = 4.64;
		anchers[3][1] = 7.91;
		
		anchers[4][0] = 8.23;
		anchers[4][1] = 9.51;
	}
	
	else if (p_body_data->det_type == DET_GEST)
	{
		anchers[0][0] = 0.28;
		anchers[0][1] = 0.47;

		anchers[1][0] = 0.60;
		anchers[1][1] = 0.69;

		anchers[2][0] = 0.91;
		anchers[2][1] = 1.01;

    	anchers[3][0] = 1.76;
		anchers[3][1] = 2.47;
		
		anchers[4][0] = 4.01;
		anchers[4][1] = 5.40;		
	}	

    for(channel = 0; channel < 5; ++channel){
        for(r = 0; r < 7; ++r){
            for(c = 0; c < 7; ++c){
                //Extract data from CNN result
                box_x = (1/(1+exp(-(double)(((short int)p_OutputImage[channel*gap*7*7+r*7+c]) / 256.0))) + c)/7;
                box_y = (1/(1+exp(-(double)(((short int)p_OutputImage[(channel*gap+1)*7*7+r*7+c]) / 256.0))) + r)/7;
                box_w = exp((double)(((short int)p_OutputImage[(channel*gap+2)*7*7+r*7+c]) / 256.0)) * anchers[channel][0] / 7;
                box_h = exp((double)(((short int)p_OutputImage[(channel*gap+3)*7*7+r*7+c]) / 256.0)) * anchers[channel][1] / 7;
                box_confidence = 1 / (1+exp(-(double)(((short int)p_OutputImage[(channel*gap+4)*7*7+r*7+c]) / 256.0)));

                //box
                box[channel*7*7+r*7+c] = (box_y - (box_h / 2)) * 224;
                box[7*7*5 + channel*7*7+r*7+c] = (box_x - (box_w / 2)) * 224;
                box[7*7*5*2 + channel*7*7+r*7+c] = (box_y + (box_h / 2)) * 224;
                box[7*7*5*3 + channel*7*7+r*7+c] = (box_x + (box_w / 2)) * 224;

                //get sum
                sum = 0;
                maxData = 0;
                maxIndex = 0;
                flag = 0;
                for(i = 0; i < NumClasses; ++i){
                    box_class_probs[i*7*7*5 + channel*7*7+r*7+c] = exp((double)(((short int)p_OutputImage[(channel*gap+5+i)*7*7+r*7+c]) / 256.0));                    		    		    
                    sum += box_class_probs[i*7*7*5 + channel*7*7+r*7+c];
                }

                //find max data
                for(i = 0; i < NumClasses; ++i){
                    box_class_probs[i*7*7*5 + channel*7*7+r*7+c] = box_class_probs[i*7*7*5 + channel*7*7+r*7+c] / sum * box_confidence;
                    if(box_class_probs[i*7*7*5 + channel*7*7+r*7+c] >= thresh && box_class_probs[i*7*7*5 + channel*7*7+r*7+c] > maxData){
                        maxData = box_class_probs[i*7*7*5 + channel*7*7+r*7+c];
                        maxIndex = i;
                        flag = 1;
                    }
                }

                if(flag ==1){
                    Box_t * currentBox = k_malloc(sizeof(Box_t));  //malloc(0, sizeof(Box_t))
                    if(currentBox == NULL)
						return RET_ERROR;
                    currentBox->class = maxIndex;
                    currentBox->score = maxData;
                    currentBox->next = NULL;
                    for(i = 0; i < 4; ++i){
                        currentBox->box[i] = box[i*7*7*5 + channel*7*7+r*7+c];
                    }
                    if(head == NULL){
                        head = currentBox;
                        previous = currentBox;
                    }else{
                        previous->next = currentBox;
                        previous = currentBox;
                    }
                }
            }
        }
    }

    Box_t * currentBox = head;

    for(i = 0; i < NumClasses; ++i){
        currentBox = head;
        int Numbox = 0;
        while(currentBox != NULL){
            if(currentBox -> class == i){
                s[Numbox].score = currentBox->score;
                for(j = 0; j < 4; ++j){
                    s[Numbox].box[j] = currentBox->box[j];
                }
                ++Numbox;
            }
			p = currentBox;
            currentBox = currentBox->next;
			if(p != NULL)
				k_free(p);
		    p = NULL;

			if(Numbox > 16)
				break;
        }
        if(Numbox == 1){
			
			p_body_data->box[p_body_data->boxcnt] = s[0];
			p_body_data->box[p_body_data->boxcnt].class = i;
			p_body_data->boxcnt++;			
#if 0			
            printf("class: %d\n\r",i);
            printf("score: %lf\n\r",s[0].score);
            printf("box_y_min: %lf\n\r",s[0].box[0] < 0 ? 0 : s[0].box[0]);
            printf("box_x_min: %lf\n\r",s[0].box[1] < 0 ? 0 : s[0].box[1]);
            printf("box_y_max: %lf\n\r",s[0].box[2] < 0 ? 0 : s[0].box[2]);
            printf("box_x_max: %lf\n\r",s[0].box[3] < 0 ? 0 : s[0].box[3]);
#endif			
        }else if(Numbox >= 2){
            //qsort(s,Numbox,sizeof(Box_t),nms_comparator);/*wizephen.wang*/
            //quicksort(s,Numbox,sizeof(Box_t),nms_comparator); /*qsort means quick sort ???*/
            //quicksort(s,Numbox,sizeof(Box_t),1); /*qsort means quick sort ???*/
            bubleSort(s, Numbox);
            for(r = 0; r < Numbox; ++r){
                for(c = r+1; c < Numbox; ++c){
                    if(box_iou(s[r],s[c]) > nms_thresh){
                        s[c].score = 0;
                    }
                }
            }
            for(r = 0; r < Numbox; ++r){
                if(s[r].score > 0){
					p_body_data->box[p_body_data->boxcnt] = s[r];
					p_body_data->box[p_body_data->boxcnt].class = i;
					p_body_data->boxcnt ++;		
                }
            }
        }
    }


	return RET_OK;
}

typedef struct RPC_inputdata_private
{
	struct easynet_ops_param *param;
	unsigned char *input;;
	unsigned short int *network_input;	
} RPC_inputdata_private_T;

typedef struct rpc_yolo_private
{
	double *d_box;
	double *box_class_probs;
	Box_t *s_box;
	unsigned short *pOutputImage;
	ObjBodyData *pobj_body_data;
	
} rpc_yolo_private;

struct easynet_ops_param
{
    uint32_t sync;
    uint32_t op;
    uint32_t len;
};
/* 
 * Operator parameter of image input
 */
struct op_image_input_param
{
    uint32_t ch;       /*< input channel */
    uint32_t row;      /*< input row */
    uint32_t col;      /*< input column */
};


/*****************************************************************************/
/**
*
* This function will do pre-processing of data (data / 256 - 0.5)
* Then reformat the data into convolution format (8 line in a group)
*
* @param    none
*
* @return   none
*
* @note     None.
*
******************************************************************************/
static int pre_processing(struct easynet_ops_param *param, unsigned char *image_input_addr, unsigned short int *inputImage)
{
    struct op_image_input_param *op_param = (struct op_image_input_param *)(((void *)param) + sizeof(struct easynet_ops_param));
    int r;
    int c;
    int channel;
    int data;
    unsigned short temp_data;
    uint32_t ch_num, row_num, col_num;
    int index = 0;
    index = 0;
	RET_CODE ret = RET_OK;

    ch_num = op_param->ch;
    row_num = op_param->row;
    col_num = op_param->col;

    /*
    * Move data from UART buffer to input Buffer and do pre-processing, also convert data from 8 bit to 16 bit
    */
    for (channel = 0; channel < ch_num; ++channel)
    {
        for (r = 0; r < row_num; ++r)
        {
            for (c = 0; c < col_num; ++c)
            {
                if (channel != ch_num - 1)
                {
                    data = image_input_addr[channel * row_num * col_num + r * col_num + c];
                    temp_data = data;
                    inputImage[index++] = temp_data;
                }
                else
                {
                    inputImage[index++] = 0;
                }
            }
        }
    }

	return ret;
}


static RET_CODE RPC_PrePocess(RPC_Package_T *pack)
{
	 RPC_inputdata_private_T *prpc_input_pri =  (RPC_inputdata_private_T *)pack->privData;
	 pre_processing(prpc_input_pri->param, prpc_input_pri->input, prpc_input_pri->network_input);
}


static RET_CODE RPC_YoloPocess(RPC_Package_T *pack)
{
    RET_CODE ret = RET_OK;

	rpc_yolo_private *prpc_yolo_pri =  (rpc_yolo_private *)pack->privData;

	ret = PostYolo(prpc_yolo_pri->pobj_body_data, prpc_yolo_pri->pOutputImage, prpc_yolo_pri->d_box, prpc_yolo_pri->box_class_probs,prpc_yolo_pri->s_box);

    return ret;
}

static int RPC_YoloInit (struct device *dev)
{
    RET_CODE ret = RET_OK;
	ret = RPC_SlaveRegisterCmd(RPC_CMD_YOLO, RPC_YoloPocess);
	ret = RPC_SlaveRegisterCmd(RPC_PRE_PROCESSING, RPC_PrePocess);
	return ret;
}


DEVICE_DEFINE(rpc_yolo, DEV_RPC_YOLO_NAME, RPC_YoloInit, NULL, \
              NULL, NULL, POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEVICE,  NULL) ;



