
add_library(PTHREAD INTERFACE)

target_include_directories(PTHREAD INTERFACE ${UGELIS_BASE}/include/posix)

ugelis_library()
ugelis_library_sources(pthread_common.c)
ugelis_library_sources(pthread_cond.c)
ugelis_library_sources(pthread_mutex.c)
ugelis_library_sources(pthread_barrier.c)
ugelis_library_sources(pthread.c)
ugelis_library_sources(pthread_sched.c)
ugelis_library_sources(clock.c)
ugelis_library_sources(sleep.c)
ugelis_library_sources(timer.c)
ugelis_library_sources(pthread_rwlock.c)
ugelis_library_sources(semaphore.c)
ugelis_library_sources(pthread_key.c)
ugelis_library_sources_ifdef(CONFIG_POSIX_MQUEUE mqueue.c)
ugelis_library_sources_ifdef(CONFIG_POSIX_FS fs.c)

ugelis_library_link_libraries(PTHREAD)
target_link_libraries(PTHREAD INTERFACE ugelis_interface)
