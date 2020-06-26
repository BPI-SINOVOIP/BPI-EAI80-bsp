static const char *const multilib_raw[] = {
". !mthumb !marm !march=armv6s-m !march=armv7-m !march=armv7e-m !mfloat-abi=softfp !mfloat-abi=hard !mfpu=fpv5-sp-d16 !mfpu=fpv5-d16 !mfpu=fpv4-sp-d16;",
"thumb mthumb !marm !march=armv6s-m !march=armv7-m !march=armv7e-m !mfloat-abi=softfp !mfloat-abi=hard !mfpu=fpv5-sp-d16 !mfpu=fpv5-d16 !mfpu=fpv4-sp-d16;",
"arm !mthumb marm !march=armv6s-m !march=armv7-m !march=armv7e-m !mfloat-abi=softfp !mfloat-abi=hard !mfpu=fpv5-sp-d16 !mfpu=fpv5-d16 !mfpu=fpv4-sp-d16;",
"fpu !mthumb !marm !march=armv6s-m !march=armv7-m !march=armv7e-m !mfloat-abi=softfp mfloat-abi=hard !mfpu=fpv5-sp-d16 !mfpu=fpv5-d16 !mfpu=fpv4-sp-d16;",
"armv6-m mthumb !marm march=armv6s-m !march=armv7-m !march=armv7e-m !mfloat-abi=softfp !mfloat-abi=hard !mfpu=fpv5-sp-d16 !mfpu=fpv5-d16 !mfpu=fpv4-sp-d16;",
"armv7-m mthumb !marm !march=armv6s-m march=armv7-m !march=armv7e-m !mfloat-abi=softfp !mfloat-abi=hard !mfpu=fpv5-sp-d16 !mfpu=fpv5-d16 !mfpu=fpv4-sp-d16;",
"armv7e-m mthumb !marm !march=armv6s-m !march=armv7-m march=armv7e-m !mfloat-abi=softfp !mfloat-abi=hard !mfpu=fpv5-sp-d16 !mfpu=fpv5-d16 !mfpu=fpv4-sp-d16;",
"armv7e-m/softfp/fpv5-sp-d16 mthumb !marm !march=armv6s-m !march=armv7-m march=armv7e-m mfloat-abi=softfp !mfloat-abi=hard mfpu=fpv5-sp-d16 !mfpu=fpv5-d16 !mfpu=fpv4-sp-d16;",
"armv7e-m/softfp/fpv5-d16 mthumb !marm !march=armv6s-m !march=armv7-m march=armv7e-m mfloat-abi=softfp !mfloat-abi=hard !mfpu=fpv5-sp-d16 mfpu=fpv5-d16 !mfpu=fpv4-sp-d16;",
"armv7e-m/softfp mthumb !marm !march=armv6s-m !march=armv7-m march=armv7e-m mfloat-abi=softfp !mfloat-abi=hard !mfpu=fpv5-sp-d16 !mfpu=fpv5-d16 mfpu=fpv4-sp-d16;",
"armv7e-m/fpu/fpv5-sp-d16 mthumb !marm !march=armv6s-m !march=armv7-m march=armv7e-m !mfloat-abi=softfp mfloat-abi=hard mfpu=fpv5-sp-d16 !mfpu=fpv5-d16 !mfpu=fpv4-sp-d16;",
"armv7e-m/fpu/fpv5-d16 mthumb !marm !march=armv6s-m !march=armv7-m march=armv7e-m !mfloat-abi=softfp mfloat-abi=hard !mfpu=fpv5-sp-d16 mfpu=fpv5-d16 !mfpu=fpv4-sp-d16;",
"armv7e-m/fpu mthumb !marm !march=armv6s-m !march=armv7-m march=armv7e-m !mfloat-abi=softfp mfloat-abi=hard !mfpu=fpv5-sp-d16 !mfpu=fpv5-d16 mfpu=fpv4-sp-d16;",
NULL
};

static const char *const multilib_reuse_raw[] = {
NULL
};

static const char *const multilib_matches_raw[] = {
"mcpu=cortex-m0 march=armv6s-m;",
"mcpu=cortex-m0.small-multiply march=armv6s-m;",
"mcpu=cortex-m0plus march=armv6s-m;",
"mcpu=cortex-m0plus.small-multiply march=armv6s-m;",
"mcpu=cortex-m1 march=armv6s-m;",
"mcpu=cortex-m1.small-multiply march=armv6s-m;",
"march=armv6-m march=armv6s-m;",
"mcpu=cortex-m3 march=armv7-m;",
"mcpu=cortex-m4 march=armv7e-m;",
"mcpu=cortex-m7 march=armv7e-m;",
"mthumb mthumb;",
"marm marm;",
"march=armv6s-m march=armv6s-m;",
"march=armv7-m march=armv7-m;",
"march=armv7e-m march=armv7e-m;",
"mfloat-abi=softfp mfloat-abi=softfp;",
"mfloat-abi=hard mfloat-abi=hard;",
"mfpu=fpv5-sp-d16 mfpu=fpv5-sp-d16;",
"mfpu=fpv5-d16 mfpu=fpv5-d16;",
"mfpu=fpv4-sp-d16 mfpu=fpv4-sp-d16;",
NULL
};

static const char *multilib_extra = "";

static const char *const multilib_exclusions_raw[] = {
NULL
};

static const char *multilib_options = "mthumb/marm march=armv6s-m/march=armv7-m/march=armv7e-m mfloat-abi=softfp/mfloat-abi=hard mfpu=fpv5-sp-d16/mfpu=fpv5-d16/mfpu=fpv4-sp-d16";
