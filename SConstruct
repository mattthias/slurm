import os

env = Environment()

if ARGUMENTS.get('debian'):
	env.Append(CPPDEFINES=['__Debian__'])

if os.uname()[0] == 'Linux':
	env.Append(CPPDEFINES=['_HAVE_NCURSES','_HAVE_NCURSES_COLOR'])
elif os.uname()[0] == 'GNU/kFreeBSD':
	env.Append(CPPDEFINES=['_HAVE_NCURSES','_HAVE_NCURSES_COLOR'])
elif os.uname()[0] == 'Darwin':
	env.Append(CPPDEFINES=['_HAVE_NCURSES','_HAVE_NCURSES_COLOR'])
	
env.Append( LIBS = ['ncurses'] )
env.Program(target='slurm', source=['slurm.c'])
