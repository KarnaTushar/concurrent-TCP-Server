#!/bin/bash

for i in {1..20} do
  echo $i;
  serverResponse=$(telnet 127.0.0.1 8080)
  echo serverResponse
done
