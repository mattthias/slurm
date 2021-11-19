project('slurm', 'c')

add_global_arguments('-D_HAVE_NCURSES', language: 'c')

ncurses_dep = dependency('ncurses', version: '>=5') 
executable('slurm', 'slurm.c', dependencies: ncurses_dep )
