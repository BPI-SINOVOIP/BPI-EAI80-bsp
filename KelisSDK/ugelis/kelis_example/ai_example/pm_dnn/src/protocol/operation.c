#include <ugelis.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "operation.h"

void soft_powermanager_init(power_mgr_t *pm_info)
{
    pm_info->alpha = 0.1;
    pm_info->beta = 0.0;
    pm_info->delta = 0.0;
    pm_info->status = AIR_IDLE;
    pm_info->factor_status = FACTOR_IDLE;
    pm_info->threshold = 0;
    pm_info->monitor_flag = false;
    pm_info->factor_adjust_time = SPEED_ADJUST_TIME;
    pm_info->duration = 0;
    pm_info->update_speed_duration = 0;
    pm_info->start_temp = 0;
    pm_info->end_temp = 0;
    pm_info->output_indoorfan_speed = 0.0;
}

static float indoorfan_speed_setting(dnn_context_t *cxt)
{
    float output_indoorfan_speed = cxt->indoorfan_speed;

    if (cxt->temp_delta > 60)
    {
        output_indoorfan_speed = cxt->target_indoorfan_speed + 40;
    }
    else if (cxt->temp_delta > 45)
    {
        output_indoorfan_speed = cxt->target_indoorfan_speed + 30;
    }
    else if (cxt->temp_delta > 30)
    {
        output_indoorfan_speed = cxt->target_indoorfan_speed + 20;
    }
    else if (cxt->temp_delta > 15)
    {
        output_indoorfan_speed = cxt->target_indoorfan_speed + 10;
    }
    else
    {
        output_indoorfan_speed = cxt->target_indoorfan_speed;
    }

    return output_indoorfan_speed;
}

int soft_powermanager_operation(power_mgr_t *pm_info, dnn_context_t *cxt)
{
    switch (pm_info->status)
    {
        case AIR_IDLE:
            if (cxt->reset == 0x1)
            {
                pm_info->alpha = 0.1;
                pm_info->beta = 0.0;
                pm_info->delta = 0.0;
                pm_info->status = AIR_IDLE;
                pm_info->factor_status = FACTOR_IDLE;
                pm_info->threshold = 0;
                pm_info->monitor_flag = false;
                pm_info->factor_adjust_time = SPEED_ADJUST_TIME;
                pm_info->duration = 0;
                pm_info->update_speed_duration = 0;
                pm_info->start_temp = 0;
                pm_info->end_temp = 0;
                pm_info->output_indoorfan_speed = 0.0;
            }
            else
            {
                if (cxt->temp_delta > 10)
                {
                    pm_info->status = AIR_CHANGE;
                    pm_info->delta = 0.0;

                    /* enter chang status for the first time, alpha value is 10% */
                    pm_info->capacity_status = AIR_DOWN;
                    pm_info->alpha = 0.1;

                    pm_info->start_temp = cxt->temperature;

                    pm_info->update_speed_duration = 60;
                    pm_info->update_speed_duration += cxt->duration;
                    /* don't modify speed immediately, waiting for 1min */
                    pm_info->output_indoorfan_speed = cxt->indoorfan_speed;
                    pm_info->output_coolingcapacity = (1 - pm_info->alpha) * cxt->coolingcapacity;
                    pm_info->delta += (cxt->coolingcapacity - pm_info->output_coolingcapacity);
                }
                else
                {
                    pm_info->status = AIR_STABLE;
                    pm_info->factor_status = FACTOR_IDLE;
                    pm_info->threshold = 0;
                    pm_info->monitor_flag = false;
                    pm_info->factor_adjust_time = SPEED_ADJUST_TIME;

                    if (cxt->temp_delta <= 5)
                    {
                        pm_info->beta = 0.025;
                    }
                    else
                    {
                        pm_info->beta = 0;
                    }

                    /* enter stabel status for the first time, delta value is zero */
                    pm_info->delta = 0.0;

                    pm_info->output_indoorfan_speed = cxt->indoorfan_speed;
                    pm_info->output_coolingcapacity = (1 - pm_info->beta) * cxt->coolingcapacity;
                }

                pm_info->duration = 0;
                pm_info->duration += cxt->duration;
            }
            break;
        case AIR_STABLE:
            if (cxt->reset == 0x1)
            {
                pm_info->alpha = 0.1;
                pm_info->beta = 0.0;
                pm_info->delta = 0.0;
                pm_info->status = AIR_IDLE;
                pm_info->factor_status = FACTOR_IDLE;
                pm_info->threshold = 0;
                pm_info->monitor_flag = false;
                pm_info->factor_adjust_time = SPEED_ADJUST_TIME;
                pm_info->duration = 0;
                pm_info->update_speed_duration = 0;
                pm_info->start_temp = 0;
                pm_info->end_temp = 0;
                pm_info->output_indoorfan_speed = 0.0;
            }
            else if (cxt->temp_delta > 10)
            {
                pm_info->status = AIR_CHANGE;
                pm_info->delta = 0.0;
                pm_info->capacity_status = AIR_DOWN;
                pm_info->alpha = 0.1;
                pm_info->duration = 0;
                pm_info->update_speed_duration = 0;
                pm_info->duration += cxt->duration;
                pm_info->update_speed_duration += cxt->duration;
                pm_info->start_temp = cxt->temperature;

                /* don't modify speed immediately, waiting for 2min */
                pm_info->output_indoorfan_speed = cxt->indoorfan_speed;
                pm_info->output_coolingcapacity = (1 - pm_info->alpha) * cxt->coolingcapacity;
                pm_info->delta += (cxt->coolingcapacity - pm_info->output_coolingcapacity);
            }
            else
            {
                if (pm_info->duration >= pm_info->factor_adjust_time)
                {
                    pm_info->duration = 0;
                    if (cxt->temp_delta <= 1)
                    {
                        if ((false == pm_info->monitor_flag) && (SPEED_ADJUST_TIME == pm_info->factor_adjust_time))
                        {
                            pm_info->monitor_flag = true;
                        }

                        pm_info->factor_status = FACTOR_UP;

                        pm_info->beta += 0.025;
                        if (pm_info->beta > 0.2)
                        {
                            pm_info->beta = 0.2;
                        }
                    }
                    else if ((cxt->temp_delta >= 5) && (pm_info->beta != 0.0))
                    {
                        if ((true == pm_info->monitor_flag) && (FACTOR_UP == pm_info->factor_status))
                        {
                            pm_info->threshold++;
                            if (pm_info->threshold >= 2)
                            {
                                pm_info->factor_adjust_time = FACTOR_ADJUST_TIME;
                                pm_info->monitor_flag = false;
                                pm_info->threshold = 0;
                            }
                        }

                        pm_info->factor_status = FACTOR_DOWN;
                        pm_info->beta -= 0.05;
                        if (pm_info->beta < 0.0)
                        {
                            pm_info->beta = 0.0;
                        }
                    }
                }

                pm_info->duration += cxt->duration;
                pm_info->output_indoorfan_speed = cxt->indoorfan_speed;
                pm_info->output_coolingcapacity = (1 - pm_info->beta) * cxt->coolingcapacity;
            }
            break;
        case AIR_CHANGE:
            if (cxt->reset == 0x1)
            {
                pm_info->alpha = 0.1;
                pm_info->beta = 0.0;
                pm_info->delta = 0.0;
                pm_info->status = AIR_IDLE;
                pm_info->factor_status = FACTOR_IDLE;
                pm_info->threshold = 0;
                pm_info->monitor_flag = false;
                pm_info->factor_adjust_time = SPEED_ADJUST_TIME;
                pm_info->duration = 0;
                pm_info->update_speed_duration = 0;
                pm_info->start_temp = 0;
                pm_info->end_temp = 0;
                pm_info->output_indoorfan_speed = 0.0;
            }
            else if ((cxt->temp_delta <= 10 && pm_info->delta <= 0.0)
                     || (cxt->temp_delta <= 5))
            {
                pm_info->status = AIR_STABLE;
                pm_info->factor_status = FACTOR_IDLE;
                pm_info->threshold = 0;
                pm_info->monitor_flag = false;
                pm_info->factor_adjust_time = SPEED_ADJUST_TIME;
                pm_info->delta = 0.0;

                if (cxt->temp_delta <= 5)
                {
                    pm_info->beta = 0.025;
                }
                else
                {
                    pm_info->beta = 0.0;
                }

                pm_info->duration = 0;
                pm_info->update_speed_duration = 0;
                pm_info->duration += cxt->duration;

                pm_info->output_indoorfan_speed = cxt->target_indoorfan_speed;
                pm_info->output_coolingcapacity = (1 - pm_info->beta) * cxt->coolingcapacity;
            }
            else
            {
                /* finish a period of time during which reduce temperature, and restart new period */
                if (cxt->temp_delta > 10 && pm_info->delta <= 0.0)
                {
                    pm_info->status = AIR_CHANGE;
                    pm_info->delta = 0.0;
                    pm_info->capacity_status = AIR_DOWN;
                    pm_info->alpha = 0.1;
                    pm_info->duration = 0;
                    pm_info->duration += cxt->duration;
                    pm_info->start_temp = cxt->temperature;

                    pm_info->output_indoorfan_speed = cxt->indoorfan_speed;
                    pm_info->output_coolingcapacity = (1 - pm_info->alpha) * cxt->coolingcapacity;
                    pm_info->delta += (cxt->coolingcapacity - pm_info->output_coolingcapacity);
                }
                else
                {
                    if (pm_info->capacity_status == AIR_UP && cxt->freq > FREQ_LIMIT)
                    {
                        pm_info->capacity_status = AIR_DOWN;
                    }
                    else if (pm_info->capacity_status == AIR_DOWN && cxt->freq <= FREQ_LIMIT && pm_info->delta > 0.0)
                    {
                        pm_info->capacity_status = AIR_UP;
                    }

                    if (pm_info->update_speed_duration >= SPEED_ADJUST_TIME)
                    {
                        pm_info->update_speed_duration = 0;
                        pm_info->output_indoorfan_speed = indoorfan_speed_setting(cxt);
                    }
                    else
                    {
                        pm_info->output_indoorfan_speed = cxt->indoorfan_speed;
                    }

                    pm_info->update_speed_duration += cxt->duration;

                    if (pm_info->capacity_status == AIR_DOWN)
                    {
                        if (pm_info->duration >= FACTOR_ADJUST_TIME)
                        {
                            pm_info->duration = 0;
                            pm_info->end_temp = cxt->temperature;

                            if (cxt->temp_delta > 30)
                            {
                                if ((pm_info->start_temp - pm_info->end_temp) >= 10)
                                {
                                    pm_info->alpha += 0.025;
                                }
                                else
                                {
                                    pm_info->alpha -= 0.025;
                                }
                            }
                            else
                            {
                                if ((pm_info->start_temp - pm_info->end_temp) >= 5)
                                {
                                    pm_info->alpha += 0.025;
                                }
                                else
                                {
                                    pm_info->alpha -= 0.025;
                                }
                            }

                            if (pm_info->alpha > 0.1)
                            {
                                pm_info->alpha = 0.1;
                            }
                            else if (pm_info->alpha < 0.0)
                            {
                                pm_info->alpha = 0.0;
                            }
                            pm_info->start_temp = cxt->temperature;
                        }

                        pm_info->duration += cxt->duration;
                        pm_info->output_coolingcapacity = (1 - pm_info->alpha) * cxt->coolingcapacity;
                        pm_info->delta += (cxt->coolingcapacity - pm_info->output_coolingcapacity);
                    }
                    else if (pm_info->capacity_status == AIR_UP)
                    {
                        if (pm_info->duration >= FACTOR_ADJUST_TIME)
                        {
                            pm_info->duration = 0;
                            pm_info->end_temp = cxt->temperature;

                            if (cxt->temp_delta > 30)
                            {
                                if ((pm_info->start_temp - pm_info->end_temp) >= 10)
                                {
                                    pm_info->alpha += 0.025;
                                }
                                else
                                {
                                    pm_info->alpha -= 0.025;
                                }
                            }
                            else
                            {
                                if ((pm_info->start_temp - pm_info->end_temp) >= 5)
                                {
                                    pm_info->alpha += 0.025;
                                }
                                else
                                {
                                    pm_info->alpha -= 0.025;
                                }
                            }

                            if (pm_info->alpha > 0.05)
                            {
                                pm_info->alpha = 0.05;
                            }
                            else if (pm_info->alpha < 0.0)
                            {
                                pm_info->alpha = 0.0;
                            }

                            pm_info->start_temp = cxt->temperature;
                        }

                        pm_info->duration += cxt->duration;
                        pm_info->output_coolingcapacity = (1 + pm_info->alpha) * cxt->coolingcapacity;
                        pm_info->delta += (cxt->coolingcapacity - pm_info->output_coolingcapacity);
                    }
                }
            }
            break;
        default:
            break;
    }

    return 0;
}

