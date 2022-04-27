#!/bin/bash
#FIXME
overlay_dir="/sys/kernel/config/device-tree/overlays/socfpga"
overlay_dtbo="rfs-overlay.dtbo"
overlay_rbf="Module5_Sample_HW.rbf"

if [ -d $overlay_dir ];then
  echo "Deleting $overlay_dir"
  rmdir $overlay_dir
fi
echo "Copy DTBO and RBF"
cp $overlay_dtbo /lib/firmware/
cp $overlay_rbf /lib/firmware/

echo "creating $overlay_dir"
mkdir $overlay_dir

echo "Doing Device Tree Overlay"
echo $overlay_dtbo > $overlay_dir/path

echo "Successfully Device Tree Overlay Done."