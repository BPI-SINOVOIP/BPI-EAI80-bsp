#ifndef _UST_SRC_ELEMENT_H_
#define _UST_SRC_ELEMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ELEMENT_SRC                  "SRC"
#define ELEMENT_SRC_PAD_SRC          "SRCPADSRC"


#ifdef CONFIG_SRC_ELEMENT_QUEUE
	#define ELEMENT_SRC_QUEUE            "SRCQUEUE"
	#define ELEMENT_SRC_QUEUE_PAD_SINK   "SRCQUEUESINKPAD"
	#define ELEMENT_SRC_QUEUE_PAD_SRC    "SRCQUEUESRCPAD"
#endif

#ifdef __cplusplus
}
#endif

#endif /* _UST_SRC_ELEMENT_H_ */


