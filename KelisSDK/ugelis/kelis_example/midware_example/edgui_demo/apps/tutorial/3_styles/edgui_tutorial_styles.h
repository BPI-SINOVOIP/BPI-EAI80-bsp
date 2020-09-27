/**
 * @file edgui_tutorial_styles.h
 *
 */

#ifndef EDGUI_TUTORIAL_STYLES_H
#define EDGUI_TUTORIAL_STYLES_H

#ifdef __cplusplus
edgui_tutorialtern "C" {
#endif

    /*********************
     *      INCLUDES
     *********************/
#ifdef EDGUI_CONF_INCLUDE_SIMPLE
#include "edgui.h"
#include "edgui_conf.h"
#else
#include <edgui/edgui.h>
#include <edgui/edgui_conf.h>
#endif
	
#include "edgui_prj_conf.h"


#if EDGUI_USE_TUTORIALS

    /*********************
     *      DEFINES
     *********************/

    /**********************
     *      TYPEDEFS
     **********************/

    /**********************
     * GLOBAL PROTOTYPES
     **********************/
    void edgui_tutorial_styles(void);

    /**********************
     *      MACROS
     **********************/

#endif /*EDGUI_USE_TUTORIALS*/

#ifdef __cplusplus
} /* edgui_tutorialtern "C" */
#endif

#endif /*EDGUI_TUTORIAL_STYLES_H*/
