include($ENV{UGELIS_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(NONE)

target_include_directories(app PRIVATE $ENV{UGELIS_BASE}/drivers)
target_sources(app PRIVATE src/hello.c)
