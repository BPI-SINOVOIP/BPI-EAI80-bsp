
#ifndef _UST_EDGE_H_
#define _UST_EDGE_H_

#ifdef __cplusplus
extern "C" {
#endif

 
/*element name*/
#define ELEMENT_EDGE               	"EDGE"
/*pad name*/
#define ELEMENT_EDGE_PAD_SRC       	"EDGEPADSRC"
#define ELEMENT_EDGE_PAD_SINK       "EDGEPADSINK"
#ifdef CONFIG_UST_PLUGIN_EDGE_QUEUE
#define ELEMENT_EDGE_QUEUE          "EDGEQUEUE"
#define ELEMENT_EDGE_QUEUE_PAD_SINK "EDGEQUEUEPADSINK"
#define ELEMENT_EDGE_QUEUE_PAD_SRC  "EDGEQUEUEPADSRC"
#endif

#ifdef __cplusplus
}
#endif

#endif /* _UST_EDGE_H_ */





