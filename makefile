scheduler:
	g++ ./scheduler.cpp -lncurses -std=c++11 -o scheduler
	g++ ./writer.cpp -o writer
clean:
	rm scheduler
	rm writer
archive:
	tar cvf ../CS3790_4374851_Assign02.tar .
