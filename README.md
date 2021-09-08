# slurm - yet another network load monitor

slurm started as a FreeBSD port of the Linux ppp link monitor called pppstatus
by Gabriel Montenegro. Hendrik Scholz ripped off the ppp dependent parts and the
email checks to turn it into a generic network load monitor for *BSD, Linux,
HP-UX and Solaris.

## Features:

 - "realtime" traffic statistics
 - three graph modes: combined RX and TX and two split views
 - can monitor any network device
 - curses ascii graphics
 - ascii theme support

## Tested on:

 - Debian GNU/Linux 8.0
 - FreeBSD 10
 - CentOS 6/7

## Compiling:

 - Install cmake and libncurses5-dev e.g.:

   - Debian (if you don't want to use the Debian package of slurm)

         apt-get install build-essential
         apt-get install cmake libncurses5-dev

   - CentOS / RedHat
         
         yum groupinstall 'Development Tools'
         yum install cmake ncurses-devel

 - Create a build directory

        mkdir _build

 - Run cmake to create the Makefiles

        cd _build/
        cmake ..

 - Run make to build the binary

        make
 - Install the program

        sudo make install

 - Try it!

        slurm -i eth0 # or whatever your interface is

Please report bugs and feature requests in the github bugtracker:
 website: https://github.com/mattthias/slurm

License:
 As pppstatus is licensed under the GPL, slurm is too.
