streams: streams.c
	gcc -o streams streams.c

clean:
	rm streams

test: streams
	bash test.sh