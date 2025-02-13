#LINUX DRIVER DEVELOPMENT
    This repo is created to keep track of all the device drivers I've created while learning about linux drivers mostly by using the "Linux Device Drivers" book by Jonathan Corbet. This repo is not meant for production there may be alot of errors and vulnerabilities here.

## drivers:
- Hello world driver:
    It prints hello world, which can be viewed using dmesg.
- Scull driver:
    This is a fairly more complex driver, It is a type of char driver which can handle both read and write operations, we can send data to it using $ echo "some text" > /dev/scull and read ffrom it using $ cat /dev/scull. Note the /dev/scull refers to the driver file which has to be defined by the user using $ mknod /dev/scull c (MAJOR NUMBER) (MINOR NUMBER). The MAJOR NUMBER will be visible in the dmesg when the module is loaded. The MINOR NUMBER is 0 unless changed by the user.

## Loading and unloading:
- To load the driver we use "insmod driverName.ko" to compile the driver just simple run make in the driver dir.
- To unload the driver we use "rmmod driverName.
