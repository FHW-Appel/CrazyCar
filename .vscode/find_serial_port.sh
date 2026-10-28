#!/usr/bin/env bash

set -euo pipefail


# Known USB Vendor IDs (VID) for Arduino and similar devices
# 2341: Arduino SA
# 2a03: Arduino SA
# 1b4f: SparkFun Electronics
# 1a86: CH340 clones
# 0403: Future Technology Devices International, Ltd
# 10c4: Silicon Labs
VIDS="2341|2a03|1b4f|1a86|0403|10c4"

# Iterate over all available serial devices and check their vendor ID.
# If the vendor ID matches one of the known IDs, print the device path, vendor ID, product ID, and model name.
SERIAL_PORT=""
for dev in /dev/ttyACM* /dev/ttyUSB*; do
  [ -e "$dev" ] || continue
  props=$(udevadm info -q property -n "$dev")
  vid=$(echo "$props" | sed -n 's/^ID_VENDOR_ID=//p')
  pid=$(echo "$props" | sed -n 's/^ID_MODEL_ID=//p')
  if echo "$vid" | grep -Eq "^($VIDS)$"; then
    # echo "$dev  (${vid}:${pid})  $(echo "$props" | sed -n 's/^ID_MODEL=//p')"
    SERIAL_PORT="$dev"
    break;
  fi
done

echo "${SERIAL_PORT}"