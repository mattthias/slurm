cmake_minimum_required(VERSION 2.8)

# Project name
project(slurm)

# We need ncurses
set(CURSES_NEED_NCURSES true)
find_package(Curses REQUIRED)

if (CURSES_HAVE_NCURSES_H)
  add_definitions(-D_HAVE_NCURSES)
endif()

add_definitions(-D__Debian__)

set(SLURM_SOURCES slurm.c)

add_executable(slurm ${SLURM_SOURCES})

target_link_libraries(slurm ncurses)
