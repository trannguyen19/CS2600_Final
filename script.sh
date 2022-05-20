#!/bin/bash
IP=192.168.64.184
RESPOND=1
TEMP=2
HUM=3
PUB=4
PRE=0
NOW=0
TIME=1
CUR=0;
REVERSE=0



./mosquitto_sub.exe -h "$IP" -t "esp32/temp" | while read -r RESPOND
do  
    

    TEMP=${RESPOND:13:5}
    HUM=${RESPOND:30:5}
    echo "Temperature:$TEMP  Humidity: $HUM"

    if [[ ${TEMP::2} -gt 80 ]]
    then 
        PUB="HOT"
   
    elif [[ ${TEMP::2} -gt 60 ]] && [[ ${TEMP::2} -lt 80 ]]
    then 
        PUB="NORMAL"

    elif [[ ${TEMP::2} -lt 60 ]]
    then 
        PUB="COLD"

    fi
    NOW=$(date +"%T")
    CUR=${NOW:6:2}
    TIME=10#$(( CUR - PRE ))
    REVERSE=10#$(( PRE - CUR ))

    if [[ ${TIME} -gt 10 ]] || [[ ${REVERSE} -gt 10 ]]
    then
        PRE=$(( CUR - 0 ))
        ./mosquitto_pub.exe -h "$IP" -t "esp32/main" -m "$PUB"

    fi
done
