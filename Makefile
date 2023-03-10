all: rootfs
	g++ *.cpp -o mini_container

rootfs:
	./getRootFs.sh
clean:
	rm mini_container
