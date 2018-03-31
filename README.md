root@raspdev:/home/pi# echo "4" > /sys/class/gpio/export
root@raspdev:/home/pi# echo "17" > /sys/class/gpio/export
root@raspdev:/home/pi# echo "out" > /sys/class/gpio/gpio4/direction
root@raspdev:/home/pi# echo "in" > /sys/class/gpio/gpio17/direction
root@raspdev:/home/pi# echo "1" > /sys/class/gpio/gpio4/value
root@raspdev:/home/pi# echo "0" > /sys/class/gpio/gpio4/value
root@raspdev:/home/pi# cat /sys/class/gpio/gpio17/value
1
root@raspdev:/home/pi# cat /sys/class/gpio/gpio17/value
0
root@raspdev:/home/pi# echo "4" > /sys/class/gpio/unexport
root@raspdev:/home/pi# echo "17" > /sys/class/gpio/unexport