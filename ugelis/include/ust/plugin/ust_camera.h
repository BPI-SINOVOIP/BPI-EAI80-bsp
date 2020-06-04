#ifndef _UST_CAMERA_H_
#define _UST_CAMERA_H_

#ifdef __cplusplus
extern "C" {
#endif


/*element name*/
#define ELEMENT_CAMERA                  "CAMERA"
/*pad name*/
#define ELEMENT_CAMERA_PAD_SRC          "CAMERAPADSRC"

#ifdef CONFIG_UST_PLUGIN_CAMERA_QUEUE
#define ELEMENT_CAMERA_QUEUE            "CAMERAQUEUE"
#define ELEMENT_CAMERA_QUEUE_PAD_SINK   "CAMERAQUEUEPADSINK"
#define ELEMENT_CAMERA_QUEUE_PAD_SRC    "CAMERAQUEUEPADSRC"
#endif


#ifdef __cplusplus
}
#endif

#endif /* _UST_CAMERA_H_ */


