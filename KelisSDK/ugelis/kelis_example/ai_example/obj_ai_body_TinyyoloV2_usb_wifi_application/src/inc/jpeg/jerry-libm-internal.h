/* Copyright 2016 Samsung Electronics Co., Ltd.
 * Copyright 2016 University of Szeged
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This file is based on work under the following copyright and permission
 * notice:
 *
 *     Copyright (C) 2004 by Sun Microsystems, Inc. All rights reserved.
 *
 *     Permission to use, copy, modify, and distribute this
 *     software is freely granted, provided that this notice
 *     is preserved.
 *
 *     @(#)fdlibm.h 1.5 04/04/22
 */

#ifndef JERRY_LIBM_INTERNAL_H
#define JERRY_LIBM_INTERNAL_H

/* Sometimes it's necessary to define __LITTLE_ENDIAN explicitly
   but these catch some common cases. */


#define __LITTLE_ENDIAN
#ifdef __LITTLE_ENDIAN
    #define __HI(x) *(1 + (int *) &x)
    #define __LO(x) *(int *) &x
#else /* !__LITTLE_ENDIAN */
    #define __HI(x) *(int *) &x
    #define __LO(x) *(1 + (int *) &x)
#endif /* __LITTLE_ENDIAN */

/*
 * ANSI/POSIX
 */
extern double acos(double);
extern double asin(double);
extern double atan(double);
extern double atan2(double, double);
extern double cos(double x);
extern double sin(double);
extern double tan(double);

extern double exp(double x);
extern double log(double x);

extern double pow(double x, double y);
extern double sqrt(double x);

extern double ceil(double x);
extern double fabs(double);
extern double floor(double);
extern double fmod(double, double);

#endif /* !JERRY_LIBM_INTERNAL_H */
