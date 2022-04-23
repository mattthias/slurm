project('slurm', 'c')


# We want ncurses but meson wraps ncurses and curses into the curses dependency
# https://mesonbuild.com/Dependencies.html#curses
# dependency "ncurses" doesn't work on OpenBSD so we use "curses" on all OS
curses_dep = dependency('curses', version: '>=5')

add_global_arguments('-D_HAVE_NCURSES', language: 'c')

# build and install slurm binary
executable('slurm', 'slurm.c', dependencies: curses_dep, install:true )

# install manpage and theme files
install_man('slurm.1')
install_subdir('themes',
               install_dir: get_option('datadir') / 'slurm',
               strip_directory: true)
