# nttraffd Network Bandwidth Logger

nttraffd is a small, lightweight daemon designed to log how much traffic passes through each interface on a server and send this information back to a central server.

## Features

  * Data Transfer Bandwith Logging
  * Central Server to Store Data
  * Pretty graphs

## Installation

To build this package on Ubuntu 10.04, you will need:

apt-get install build-essential libcurl-dev

And the build command to be run inside the src directory is:

make && make install
 
The config file then needs to be created and edited here:

/etc/nttraffd.conf
 
And finally, run it with:

/usr/local/bin/nttraffd
 
## Starting on boot

To start this on boot, please use the demo init.d script in the etc/init.d folder and copy it to your /etc/init.d folder.

cp etc/init.d/nttraffd /etc/init.d/

Then activate it for on boot with:

update-rc.d nttraffd defaults
 
And you should be sorted.

## Documentation & Usage

Please see the docs folder for more details. 

## Open Source Projects Used

This was origionally based on the code from the DD-WRT project along with some other libs.

 * [DD-WRT Source Code](http://svn.dd-wrt.com:8000/)
 * [LibCurl](http://curl.haxx.se/libcurl/)

## License

The GPL version 3, read it at [http://www.gnu.org/licenses/gpl.txt](http://www.gnu.org/licenses/gpl.txt)

##Contributing

Any help is always welcome, please contact sam [at] infitialis.com and we can discuss any help you would like to give.
