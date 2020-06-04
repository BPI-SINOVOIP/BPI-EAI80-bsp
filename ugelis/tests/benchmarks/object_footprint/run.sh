#!/bin/sh
rm results.txt
rm -rf results
for conf in `ls -v *.conf`; do
    echo $conf
    app=$(basename $conf .conf)
    build_dir=results/$(basename $conf .conf)

    cmake -DBOARD=arduino_101 -DCONF_FILE=$conf -H. -B${build_dir}
    cmake --build ${build_dir}

    ${UGELIS_BASE}/scripts/sanitycheck -z ${build_dir}/ugelis/${app}.elf >> results.txt
done

