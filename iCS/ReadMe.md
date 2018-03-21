# iTETRIS

Quick guide on how to install the iTETRIS platform.  
The current version of iCS will work with ns3.20 and the applications provided in the svn  
The platform should work with any version of Sumo. Tested with sumo0.22

### Version
2015-04-08

### Prerequisites

To successfully build the code under ubuntu the following libraries are needed:  
>libxerces-c3.1  
>autoconf  
>automake  
>libtool  
>libxml++2.6-dev  
>libfox-1.6-dev  
>libgl1-mesa-dev  
>libglu1-mesa-dev  
>libgdal1-dev  
>libproj-dev  
>GeographicLib-1.36

### Installation

iTETRIS is composed of four main components that have to be compiled and installed to successfully run the platform.  
The following commands can be used to install the platform on ubuntu. The installation order is not important.
* Sumo installation
```sh
$ cd 'sumo-folder'
$ make -f Makefile.cvs
$ ./configure --with-xerces-libraries=/usr/local/lib --with-xerces-includes=/usr/local/include/xercesc
$ make
$ sudo make install
```
* ns3 installation
```sh
$ cd 'ns3-folder'
$ ./waf configure
$ ./waf
$ sudo ./waf install
```
* iCS installation
```sh
$ cd 'iCS-folder'
$ make -f Makefile.cvs
$ ./configure --enable-sumo --enable-ns3 --enable-applications --with-geographic-libraries=/usr/local/lib --with-geographic-includes=/usr/local/include/GeographicLib --with-xerces-libraries=/usr/local/lib --with-xerces-includes=/usr/local/include/xercesc --enable-log
$ make
$ sudo make install
```
* Application installation
```sh
$ cd 'application-folder'
$ libtoolize --force
$ aclocal
$ autoconf
$ automake -a
$ ./configure
$ make
$ sudo make install
```
