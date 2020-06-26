#!/bin/bash



OS_DIR=$(cd `dirname $0`; cd ..; pwd)


#=======================main========================#
while read sample
do
	echo ==================================
	echo "exporting $sample ..."
	$OS_DIR/export/export_greeide.sh ${sample} export/${sample}
	echo "export $sample done "
done < $OS_DIR/export/export_greeide_list.txt




