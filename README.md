# slurm - yet another network load monitor

slurm started as a FreeBSD port of the Linux ppp link monitor called pppstatus
by Gabriel Montenegro. Hendrik Scholz ripped of the ppp dependent parts and the
email checks to turn in into a generic network load monitor for *BSD, Linux,
HP-UX and Solaris.

## features:
 - "realtime" traffic statistics
 - three graph modes: combined RX and TX and two split views
 - can monitor any network device
 - curses ascii graphics
 - ascii theme support

## slurm was tested on:
 - Debian GNU/Linux 10/11 (buster/bullseye)
 - FreeBSD 12
 - CentOS 7/8

## Compiling:
 - Install meson and libncurses6-dev e.g.:
on Debian (if you don't want to use the Debian package of slurm)

         sudo apt install build-essential meson libncurses6-dev

or on CentOS / RedHat

        # Add EPEL on CentOS 7
        sudo yum install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm

        # Enable PowerTools repo in CentOS 8
        sudo dnf config-manager --set-enabled powertools

        sudo yum groupinstall 'Development Tools'
        sudo yum install meson ncurses-devel

or on FreeBSD 12

        pkg install meson pkgconf ncurses

or on Alpine Linux

       apk add gcc musl-dev pkgconf meson ncurses-dev linux-headers

or on OpenBSD 6.9

       pkg_add meson
       # some patches to the code are necessary...

 - Setup a build directory

        meson setup _build

 - Run meson to build

        cd _build/
        meson compile

 - Install the program

        sudo meson install

 - Try it!

        slurm -i eth0 # or whatever your interface is

Please report bugs and feature requests in the github bugtracker:
 website: https://github.com/mattthias/slurm

License:
 As pppstatus is licensed under the GPL, slurm is too.
