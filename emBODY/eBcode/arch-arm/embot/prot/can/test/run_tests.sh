#!/usr/bin/env bash
# run_tests.sh
# It runs on a host PC (Linux or WSL, g++ >= 9) the verification of embot::prot::can:
# 1. compat:    compat.cpp is built against robotology/icub-firmware devel and against the code of this folder (..),
#               the outputs are compared and the only differences must be those in expected_compat.diff.
# 2. roundtrip: roundtrip.cpp is built against the code of this folder only (it uses the Sender / Receiver API) and run.
#
# usage: ./run_tests.sh <icub-firmware devel> <icub-firmware-shared devel> <icub-firmware-shared to use w/ this code> [c++17|c++20]
# 
# Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
# Author:  Marco Accame
# email:   marco.accame@iit.it
#

set -u
HERE=$(cd "$(dirname "$0")" && pwd)
DEVFW=${1:?icub-firmware devel}
DEVSH=${2:?icub-firmware-shared devel}
SH=${3:?icub-firmware-shared to use with this code}
STD=${4:-c++17}

CANREL=emBODY/eBcode/arch-arm/embot/prot/can
COREREL=embot/core
SRC="embot_prot_can.cpp embot_prot_can_motor_polling.cpp embot_prot_can_motor_periodic.cpp embot_prot_can_analog_polling.cpp embot_prot_can_analog_periodic.cpp embot_prot_can_inertial_periodic.cpp embot_prot_can_skin_periodic.cpp embot_prot_can_bootloader.cpp"
FLAGS="-std=$STD -O1 -g -fsanitize=address,undefined"
OUT=$HERE/out
mkdir -p $OUT

build() # <test.cpp> <prot/can dir> <embot/core dir> <exe> [extra flags]
{
    local T=$1 CAN=$2 CORE=$3 EXE=$4; shift 4
    g++ $FLAGS -I$CAN -I$CORE -I$HERE/stub "$@" $HERE/$T $(for f in $SRC; do echo $CAN/$f; done) $CORE/embot_core_binary.cpp -o $EXE
}

echo "== building compat against robotology devel"
# -fpermissive is needed only by g++ on the PC for the _Float16 initialisation in embot_prot_can_motor.h of devel
build compat.cpp $DEVFW/$CANREL $DEVSH/$COREREL $OUT/compat_devel -fpermissive 2>/dev/null || { echo "build failed"; exit 1; }
echo "== building compat against this code"
build compat.cpp $HERE/.. $SH/$COREREL $OUT/compat_this || { echo "build failed"; exit 1; }
echo "== building roundtrip against this code"
build roundtrip.cpp $HERE/.. $SH/$COREREL $OUT/roundtrip_this || { echo "build failed"; exit 1; }

$OUT/compat_devel > $OUT/compat_devel.txt || { echo "compat_devel crashed"; exit 1; }
$OUT/compat_this  > $OUT/compat_this.txt  || { echo "compat_this crashed"; exit 1; }
diff $OUT/compat_devel.txt $OUT/compat_this.txt > $OUT/compat.diff

echo
if cmp -s $OUT/compat.diff $HERE/expected_compat.diff; then
    echo "COMPAT: OK. devel and this code differ only by the intentional changes:"
else
    echo "COMPAT: UNEXPECTED DIFFERENCES between devel and this code:"
fi
cat $OUT/compat.diff

echo
$OUT/roundtrip_this > $OUT/roundtrip.txt
RC=$?
tail -1 $OUT/roundtrip.txt
[ $RC -eq 0 ] && echo "ROUNDTRIP: OK" || { echo "ROUNDTRIP: FAILED. see out/roundtrip.txt"; grep FAILED $OUT/roundtrip.txt; }
