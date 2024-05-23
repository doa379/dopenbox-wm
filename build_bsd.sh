#! /bin/sh

if [ "$1" = "debug" ]
then
  bmake -f make_bsd DEBUG=1
  exit 0
fi

bmake -f make_bsd $@
