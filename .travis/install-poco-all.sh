#!/bin/sh
set -ex
wget http://pocoproject.org/releases/poco-1.6.0/poco-1.6.0-all.tar.gz
tar -xvf poco-1.6.0-all.tar.gz
(
	## poco will always build with gcc on linux, therefore pass in the gcc cflags
	cd poco-1.6.0-all
	./configure --cflags="-Wno-pragmas" \
		--prefix=/usr \
		--no-tests \
		--no-samples \
		--omit=Data/MySQL,Data/ODBC
	make -s
	sudo make install
)