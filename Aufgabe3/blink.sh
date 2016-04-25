echo timer > /sys/class/leds/beaglebone\:green\:usr1/trigger;
echo 500 > /sys/class/leds/beaglebone\:green\:usr1/delay_off;
echo 1000 > /sys/class/leds/beaglebone\:green\:usr1/delay_on;

echo timer > /sys/class/leds/beaglebone\:green\:usr3/trigger;
echo 1000 > /sys/class/leds/beaglebone\:green\:usr3/delay_off;
echo 500 > /sys/class/leds/beaglebone\:green\:usr3/delay_on;