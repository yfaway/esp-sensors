#!/bin/bash

# Perform the followings:
#  1. Replace all text starting with "$" in a file with the provided values.
#  2. Compile the code.
#  3. Upload the code.
#
# Usage: ./compile-and-deploy -s|--wifiSsid "ssid" -p|--wifiPassword "password"
#              -t|topic1 "mqtt topic1" -u|topic2 "mqtt topic1"

ARDUINO=/home/yf/Downloads/arduino-1.8.13/arduino
VERIFY_ONLY=false

# Copy the original file; we will replace place holders in this file with
# passed-in parameters.
FILE_NAME=/tmp/sketch.ino
cp "${@: -1}" $FILE_NAME 

# @see https://stackoverflow.com/questions/402377/using-getopts-to-process-long-and-short-command-line-options
# NOTE: This requires GNU getopt.  On Mac OS X and FreeBSD, you have to install this
# separately; see below.
TEMP=`getopt -o s:p:t:u:v --long wifiSsid:,wifiPassword:,topic1:,topic2:verifyOnly \
             -- "$@"`
if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"
while true; do
  case "$1" in
    -s | --wifiSsid ) 
		sed -i "s/\$wifiSsid/$2/g" $FILE_NAME;
		shift 2;;
    -p | --wifiPassword )
		sed -i "s/\$wifiPassword/$2/g" $FILE_NAME;
		shift 2;;
    -t | --topic1 )
		sed -i "s,\$topic1,$2,g" $FILE_NAME;
		shift 2;;
    -u | --topic2 )
		sed -i "s,\$topic2,$2,g" $FILE_NAME;
		shift 2;;
    -v | --verifyOnly )
        VERIFY_ONLY=true;
        shift;;
    -- )
        shift;
        break ;;
    * ) 
        break ;;
  esac
done

if [ "$VERIFY_ONLY" == true ] ; then
    $ARDUINO --verify $FILE_NAME
else
    $ARDUINO --upload $FILE_NAME
fi
