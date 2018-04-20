#NAME: Andrew Ding, Caleb Chau
#EMAIL: andrewxding@ucla.edu, caleb.h.chau@gmail.com
#ID: 504748356, 204805602

.SILENT:

default:
	cp lab3b.py lab3b
	chmod +x lab3b lab3b.py
clean:
	rm -f lab3b-504748356.tar.gz lab3b

dist: clean default
	tar -zcvf lab3b-504748356.tar.gz lab3b.py Makefile README
