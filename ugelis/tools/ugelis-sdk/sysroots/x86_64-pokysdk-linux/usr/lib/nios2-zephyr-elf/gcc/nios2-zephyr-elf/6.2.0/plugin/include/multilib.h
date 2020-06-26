static const char *const multilib_raw[] = {
". !mno-hw-mul !mhw-mulx !mcustom-fpu-cfg=60-1 !mcustom-fpu-cfg=60-2;",
"nomul mno-hw-mul !mhw-mulx !mcustom-fpu-cfg=60-1 !mcustom-fpu-cfg=60-2;",
"mulx !mno-hw-mul mhw-mulx !mcustom-fpu-cfg=60-1 !mcustom-fpu-cfg=60-2;",
"fpu-60-1 !mno-hw-mul !mhw-mulx mcustom-fpu-cfg=60-1 !mcustom-fpu-cfg=60-2;",
"fpu-60-2 !mno-hw-mul !mhw-mulx !mcustom-fpu-cfg=60-1 mcustom-fpu-cfg=60-2;",
"nomul/fpu-60-1 mno-hw-mul !mhw-mulx mcustom-fpu-cfg=60-1 !mcustom-fpu-cfg=60-2;",
"nomul/fpu-60-2 mno-hw-mul !mhw-mulx !mcustom-fpu-cfg=60-1 mcustom-fpu-cfg=60-2;",
"mulx/fpu-60-1 !mno-hw-mul mhw-mulx mcustom-fpu-cfg=60-1 !mcustom-fpu-cfg=60-2;",
"mulx/fpu-60-2 !mno-hw-mul mhw-mulx !mcustom-fpu-cfg=60-1 mcustom-fpu-cfg=60-2;",
NULL
};

static const char *const multilib_reuse_raw[] = {
NULL
};

static const char *const multilib_matches_raw[] = {
"mno-hw-mul mno-hw-mul;",
"mhw-mulx mhw-mulx;",
"mcustom-fpu-cfg=60-1 mcustom-fpu-cfg=60-1;",
"mcustom-fpu-cfg=60-2 mcustom-fpu-cfg=60-2;",
NULL
};

static const char *multilib_extra = "";

static const char *const multilib_exclusions_raw[] = {
NULL
};

static const char *multilib_options = "mno-hw-mul/mhw-mulx mcustom-fpu-cfg=60-1/mcustom-fpu-cfg=60-2";
