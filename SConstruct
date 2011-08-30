import os

env = Environment()
#if os.uname()[0] == 'Linux':
#	env.Append(CPPDEFINES=['__linux__'])
env.Append( LIBS = ['ncurses'] )

env.Program(target='slurm', source=['slurm.c'])


