#!/bin/bash

echo Building ctest...

g++ -o ctest.out -g channeltest.cpp stdiodevice.cpp channel.cpp subchannel.cpp memory.cpp

exit