#!/bin/sh
	if [ ! -d "bin" ]; then 
		mkdir bin
	fi
	cd src/
	make
	mv -f mf ../bin
	cd ../
	
