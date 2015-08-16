cmake_minimum_required(VERSION 2.8)

# Project name
project(slurm)

# We need ncurses
set(CURSES_NEED_NCURSES true)
find_package(Curses REQUIRED)

if (CURSES_HAVE_NCURSES_H)
  add_definitions(-D_HAVE_NCURSES)
endif()

set(SLURM_SOURCES slurm.c)

# CentOS 6 / CentOS 7 only have very old cmake version where the FindCurses
# module does not work well.  It stops to search for ncurses if it found
# curses. Thats bad for us so we set asume that we have ncurses when the os is
# some RedHat like
execute_process(COMMAND /bin/ls /etc/redhat-release OUTPUT_VARIABLE IS_REDHAT ERROR_QUIET)
if (IS_REDHAT )
  MESSAGE( STATUS "RedHat's cmake ist to old (FindCurse is somewhat broken) so we set _HAVE_NCURSES by hand.")
  add_definitions(-D_HAVE_NCURSES)
endif()

add_executable(slurm ${SLURM_SOURCES})

target_link_libraries(slurm ncurses)

# install
install(TARGETS slurm DESTINATION bin)
install(DIRECTORY themes/ DESTINATION share/slurm PATTERN "*.theme")
