#!/bin/bash

rm -rf gen-php
/usr/bin/thrift --gen php -r service.thrift
/usr/bin/thrift --gen php -r struct.thrift 

sed -i "s/namespace ;/require_once '.\/Thrift\/Type\/TType.php';\nrequire_once '.\/Thrift\/Type\/TMessageType.php';\nrequire_once '.\/Thrift\/Exception\/TApplicationException.php';/g" gen-php/*

