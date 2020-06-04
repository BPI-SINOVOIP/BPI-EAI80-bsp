#!/bin/bash

#default 64k

cd ..
rm -rf *image
make
./imageCompressTool body_full_weights_ops.bin  compress_body_full_weights_ops.image head_body_full_weights_ops.image 65536 8
touch merger_head_compress_body_full_weights_ops.image
chmod -R 777 merger_head_compress_body_full_weights_ops.image
cat head_body_full_weights_ops.image >> merger_head_compress_body_full_weights_ops.image
cat compress_body_full_weights_ops.image >> merger_head_compress_body_full_weights_ops.image

