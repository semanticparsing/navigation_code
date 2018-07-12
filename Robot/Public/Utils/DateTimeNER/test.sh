#!/bin/sh
SCRIPT_PATH=$(dirname $(readlink -f $0))
cd $SCRIPT_PATH

path=$SCRIPT_PATH
REPO_ROOT=
while [[ $path != "/" ]];do
  [ -f $path/BLADE_ROOT ] && { REPO_ROOT=$path/; break; }
  path=$(dirname $path)
done
[ "${REPO_ROOT}"x = ""x ] && { exit -1; }

TEST_SRC_PATH=$REPO_ROOT/Robot/Public/Utils/DateTimeNER
cd $TEST_SRC_PATH
blade build

TEST_BIN_PATH=$REPO_ROOT/build64_release/Robot/Public/Utils/DateTimeNER
cp $TEST_BIN_PATH/ner_test $SCRIPT_PATH/

cd $SCRIPT_PATH
ulimit -c unlimited
$SCRIPT_PATH/ner_test \
--v=5 \
--net_file=DateTimeNER.net \
--dir_path=$SCRIPT_PATH/Data
