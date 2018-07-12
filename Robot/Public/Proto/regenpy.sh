#!/bin/bash

rm -rf gen-py
/usr/bin/thrift --gen py -r service.thrift

# triorobot
rm -rf ../../../Release/Project/TrioRobot/Client/gen-py
cp -R gen-py ../../../Release/Project/TrioRobot/Client/

# triox
rm -rf ../../../Release/Project/TrioX/Client/gen-py
cp -R gen-py ../../../Release/Project/TrioX/Client/

# trior
rm -rf ../../../Release/Project/TrioR/Client/gen-py
cp -R gen-py ../../../Release/Project/TrioR/Client/

# triowordfilter
rm -rf ../../../Release/Project/TrioWordFilter/Client/gen-py
cp -R gen-py ../../../Release/Project/TrioWordFilter/Client/

rm -rf gen-py
