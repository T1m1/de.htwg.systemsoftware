#!/bin/busybox sh 

/bin/busybox --install -s /bin/

clear

# Say Hello
echo '######################'
echo 'Starting system'
echo '######################'

#mount
echo '--> Mounting...'
mount -t proc proc /proc
mount -t sysfs none /sys
mount -t tmpfs none /tmp
echo '--> done'

# populate dev
mdev -s


echo 0 > /proc/sys/kernel/printk

#start init program and display system infos
echo '--> start binary'
./bin/start

# unpack scripts for server
echo 'unpack needed files'
tar xfv www/web.tar
tar xfv etc/udhcpc.tar

rm -f etc/udhcpc.tar
rm -f www/web.tar
echo 'done'


#end
echo '--> Starting shell'
setsid cttyhack sh
