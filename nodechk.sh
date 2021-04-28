#!/bin/bash

checkproc=$(echo $(ps -ef | grep -E "[s]erver.js"))
if [[ -z $checkproc ]]; then
  cd /opt/node/
  nohup /usr/bin/node /opt/node/server.js >> /opt/node/nodechk.log &
fi
