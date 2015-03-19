#!/bin/sh
set -ex
wget -q http://downloads.sourceforge.net/project/assimp/assimp-3.1/assimp-3.1.1_no_test_models.zip
unzip assimp-3.1.1_no_test_models.zip
(
	## poco will always build with gcc on linux, therefore pass in the gcc cflags
	cd assimp-3.1.1
	mkdir build
	cd build
	cmake -DCMAKE_INSTALL_PREFIX=/usr ../
	make
	sudo make install
)
