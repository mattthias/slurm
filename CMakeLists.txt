cmake_minimum_required(VERSION 3.0)

# Project name 
project(slurm)

# We need ncurses
set(CURSES_NEED_NCURSES true)
find_package(Curses REQUIRED)

# we do different things with curses and ncurses so we need to add this definition
# i doubt that there still operating systems out there without ncurses but you never know
if (CURSES_HAVE_NCURSES_H)
  add_definitions(-D_HAVE_NCURSES)
endif()

add_definitions(-D__Debian__)

set(SLURM_SOURCES slurm.c)

add_executable(slurm ${SLURM_SOURCES})

target_link_libraries(slurm ncurses)
