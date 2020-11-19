#!/bin/bash

# this script cleans and re-deploys the lkm_sort module


MODULE_NAME="lkm_sort"
DEV_NAME="/dev/lkm_sort"


# clean any leftovers before deploy
sudo rm $DEV_NAME 2>/dev/null
sudo rmmod $MODULE_NAME

# deploy
sudo dmesg -C
sudo insmod lkm_sort.ko
curr_major=`dmesg |grep  -m 1 "lkm_sort module loaded" | awk '{print $(NF)}'`
sudo mknod /dev/lkm_sort c $curr_major 0
sudo chmod o+w /dev/lkm_sort