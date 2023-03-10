if [ ! -d ./rootfs ]
then
	wget alpine.tar.gz http://dl-cdn.alpinelinux.org/alpine/v3.10/releases/x86_64/alpine-minirootfs-3.10.0-x86_64.tar.gz
        mkdir rootfs
        mv alpine-minirootfs-3.10.0-x86_64.tar.gz ./rootfs
        cd rootfs
        tar -xvf alpine-minirootfs-3.10.0-x86_64.tar.gz
        cd ..
        rm rootfs/alpine-minirootfs-3.10.0-x86_64.tar.gz
fi
