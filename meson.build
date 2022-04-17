project('slurm', 'c')

add_global_arguments('-D_HAVE_NCURSES', language: 'c')

ncurses_dep = dependency('ncurses', version: '>=5') 

# build and install slurm binary
executable('slurm', 'slurm.c', dependencies: ncurses_dep, install:true )

# install manpage and theme files
install_man('slurm.1')
install_subdir('themes',
               install_dir: get_option('datadir') / 'slurm',
               strip_directory: true)
