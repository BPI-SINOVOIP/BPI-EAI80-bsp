static const char *const multilib_raw[] = {
". !msoft-float !EL !EB;",
"soft-float msoft-float !EL !EB;",
"el !msoft-float EL !EB;",
"eb !msoft-float !EL EB;",
"soft-float/el msoft-float EL !EB;",
"soft-float/eb msoft-float !EL EB;",
NULL
};

static const char *const multilib_reuse_raw[] = {
NULL
};

static const char *const multilib_matches_raw[] = {
"mel EL;",
"meb EB;",
"m4650 msingle-float;",
"msoft-float msoft-float;",
"EL EL;",
"EB EB;",
NULL
};

static const char *multilib_extra = "";

static const char *const multilib_exclusions_raw[] = {
NULL
};

static const char *multilib_options = "msoft-float EL/EB";
