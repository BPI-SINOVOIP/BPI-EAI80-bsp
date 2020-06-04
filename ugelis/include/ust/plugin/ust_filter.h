
#ifndef _UST_FILTER_H_
#define _UST_FILTER_H_

#ifdef __cplusplus
extern "C" {
#endif

 
/*element name*/
#define ELEMENT_FILTER               	"FILTER"
/*pad name*/
#define ELEMENT_FILTER_PAD_SRC       	"FILTERPADSRC"
#define ELEMENT_FILTER_PAD_SINK       	"FILTERPADSINK"
#ifdef CONFIG_UST_PLUGIN_FILTER_QUEUE
#define ELEMENT_FILTER_QUEUE            "FILTERQUEUE"
#define ELEMENT_FILTER_QUEUE_PAD_SINK   "FILTERQUEUEPADSINK"
#define ELEMENT_FILTER_QUEUE_PAD_SRC    "FILTERQUEUEPADSRC"
#endif

#ifdef __cplusplus
}
#endif

#endif /* _UST_FILTER_H_ */





