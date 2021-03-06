#   Copyright 2017 Comcast Cable Communications Management, LLCD
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W -g -fprofile-arcs -ftest-coverage -O0")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -W  -g -fprofile-arcs -ftest-coverage -O0")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -O0")
set (AKER_SRC ../src/main.c)
set (AKER_COMMON_LIBS gcov -lcunit -lcimplog -lwrp-c -lpthread -lmsgpackc -lnanomsg -Wl,--no-as-needed -ltrower-base64)

if(NOT DISABLE_VALGRIND)
set (MEMORY_CHECK valgrind --leak-check=full --show-reachable=yes -v)
endif ()

link_directories ( ${LIBRARY_DIR} )

#-------------------------------------------------------------------------------
#   test_schedule
#-------------------------------------------------------------------------------
add_test(NAME test_schedule COMMAND ${MEMORY_CHECK} ./test_schedule)
add_executable(test_schedule test_schedule.c ../src/schedule.c)
target_link_libraries (test_schedule ${AKER_COMMON_LIBS})
