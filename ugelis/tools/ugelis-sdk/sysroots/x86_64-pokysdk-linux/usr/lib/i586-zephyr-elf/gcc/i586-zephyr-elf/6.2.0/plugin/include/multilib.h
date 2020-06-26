static const char *const multilib_raw[] = {
". !msoft-float;",
"soft-float msoft-float;",
NULL
};

static const char *const multilib_reuse_raw[] = {
NULL
};

static const char *const multilib_matches_raw[] = {
"mno-80387 msoft-float;",
"msoft-float msoft-float;",
NULL
};

static const char *multilib_extra = "";

static const char *const multilib_exclusions_raw[] = {
NULL
};

static const char *multilib_options = "msoft-float";
