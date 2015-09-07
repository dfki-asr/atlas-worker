#!/bin/sh
set -ex
wget -q http://www.eu.apache.org/dist/activemq/activemq-cpp/3.8.4/activemq-cpp-library-3.8.4-src.tar.gz
tar -zxf activemq-cpp-library-3.8.4-src.tar.gz
(
	cd activemq-cpp-library-3.8.4
	./configure --prefix=/usr
	make V=0
	sudo make install
)
