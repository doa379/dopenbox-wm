#! /bin/sh

if [ "$1" = "dbg" ]
then
  bmake -f make_bsd DEBUG=1
fi

bmake -f make_bsd $@
