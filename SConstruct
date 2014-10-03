import os

env = Environment()

if ARGUMENTS.get('debian'):
    env.Append(CPPDEFINES=['__Debian__'])
flags = ["CPPFLAGS", "CFLAGS", "CXXFLAGS"]
for flag in flags:
    try:
        env[flag] = os.environ[flag]
        print "Imported \"%s\" from environment with value \"%s\"." % (flag, env[flag])
    except:
        print "No \"%s\" in environment found" % flag

# if LDFLAGS exists put them into scons LINKFLAGS
try:
    env['LINKFLAGS'] = os.environ['LDFLAGS']
    print "Imported \"LDFLAGS\" from environment as \"LINKFLAGS\" with value \"%s\"." % env['LINKFLAGS']
except:
    print "No \"LDFLAGS\" in environment found."

if os.uname()[0] == 'Linux':
    env.Append(CPPDEFINES=['_HAVE_NCURSES','_HAVE_NCURSES_COLOR'])

elif os.uname()[0] == 'GNU/kFreeBSD':
    env.Append(CPPDEFINES=['_HAVE_NCURSES','_HAVE_NCURSES_COLOR'])

elif os.uname()[0] == 'Darwin':
    env.Append(CPPDEFINES=['_HAVE_NCURSES','_HAVE_NCURSES_COLOR'])

env.Append( LIBS = ['ncurses'] )
env.Program(target='slurm', source=['slurm.c'])
