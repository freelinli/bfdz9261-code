#!/bin/sh

BUILD_OUT_PATH=./
FW_NAND2_PATH=./ota

make
rm -rf  ${FW_NAND2_PATH}/ota.zip
rm -rf  ${FW_NAND2_PATH}/system_org/*
cp ${BUILD_OUT_PATH}a.out ${FW_NAND2_PATH}/system_org/main
cp  ${BUILD_OUT_PATH}configinfo.json ${FW_NAND2_PATH}/system_org

rm -rf ${BUILD_OUT_PATH}/ota.zip
cd ${FW_NAND2_PATH}
tar -zcvf ota.tar.gz ./system_org/*
sudo chmod 777 ota.tar.gz
#zip -r ${BUILD_OUT_PATH}/ota.zip ./system_org/*
cd ../
