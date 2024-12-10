#!/usr/bin/env bash

SOCKET_PATH=${1:="/tmp/zero_copy_socket"}

trap 'rm -f $SOCKET_PATH' EXIT

nc -lU "$SOCKET_PATH" >/dev/null
