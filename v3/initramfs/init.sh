#!/bin/busybox sh 

/bin/busybox --install -s /bin/

clear

# Say Hello
echo '######################'
echo 'Starting system'
echo '######################'

#mount
echo '---> Mounting...'
mount -t tmpfs none /tmp
echo '---> done'

# populate dev
mdev -s

echo 0 > /proc/sys/kernel/printk

#start init program and display system infos
echo '---> start binary'
./bin/start

# unpack scripts for server
echo '---> unpack needed files'
tar xfv /www/web.tar | > /dev/null
tar xfv /etc/udhcpc.tar | > /dev/null
mv /scripts/udhcpc /etc/udhcpc

# remove archives
rm -rf /scripts
rm -f etc/udhcpc.tar
rm -f www/web.tar
echo '---> done'

# start server
echo '----> Starting Server'

udhcpc -s /etc/udhcpc/simple.script
ifconfig lo 127.0.0.1
route add -host 127.0.0.1 lo
httpd -h /www/ &

#end
echo '--> Starting shell'
setsid cttyhack sh
