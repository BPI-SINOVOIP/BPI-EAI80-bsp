#!/bin/bash
cd ..
rm -rf *image
make
./imageCompressTool faceai_full_weights_ops.bin  compress_faceai_full_weights_ops.image head_faceai_full_weights_ops.image 65536 8
touch merge_head_compress_faceai_full_weights_ops.image
chmod -R 777 merge_head_compress_faceai_full_weights_ops.image
cat head_faceai_full_weights_ops.image >> merge_head_compress_faceai_full_weights_ops.image
cat compress_faceai_full_weights_ops.image >>  merge_head_compress_faceai_full_weights_ops.image
