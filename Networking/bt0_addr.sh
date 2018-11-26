#! /bin/sh
# Assign stuff when bt0 comes up

set -e
sleep 1
ip -6 addr add fd00:a::1/64 dev bt0
service radvd restart >/dev/null 2>&1

