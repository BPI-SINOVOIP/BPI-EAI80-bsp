// Copyright 2016 ETH Zurich and University of Bologna.
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the “License”); you may not use this file except in
// compliance with the License. You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an “AS IS” BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include <ugelis.h>
#include "board.h"
#include <misc/printk.h>
#include <gm_soc.h>




extern void test_setup();
extern void test_clear();
extern void test_run();
extern int test_check();

int main()
{
    int check;

    test_setup();

    test_clear();


    test_run();


    check = test_check();
    if (check)
    {
        printf("!!!##FAIL:AES_CBC\n");
    }
    else
    {
        printf("!!!##PASS:AES_CBC\n");
    }

    return 0;
}
