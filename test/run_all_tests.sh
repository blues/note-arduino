#!/bin/bash

GREEN='\x1B[0;32m'
RED='\x1B[0;31m'
YELLOW='\x1B[0;33m'
DEFAULT='\x1B[0;0m'

all_tests_result=0

# This fixes a problem when running valgrind in a Docker container when the
# host machine is running Fedora. See https://stackoverflow.com/a/75293014.
ulimit -n 1024

# Note that we use -Wno-deprecated-declarations in the compilation commands
# below because we have deprecated some note-arduino functions (e.g. logDebug),
# but we still have unit tests for them.

if [ 0 -eq $all_tests_result ]; then
  echo && echo -e "${YELLOW}Compiling and running Notecard Test Suite...${DEFAULT}"
  g++ -fprofile-arcs -ftest-coverage -Wall -Wextra -Werror -Wpedantic -Wno-deprecated-declarations -std=c++11 -O0 -g \
    src/Notecard.cpp \
    test/Notecard.test.cpp \
    test/mock/mock-arduino.cpp \
    test/mock/mock-note-c-note.c \
    test/mock/NoteI2c_Mock.cpp \
    test/mock/NoteLog_Mock.cpp \
    test/mock/NoteSerial_Mock.cpp \
    test/mock/NoteTime_Mock.cpp \
    test/mock/NoteTxn_Mock.cpp \
    -Isrc \
    -Itest \
    -DNOTE_MOCK \
    -o failed_test_run
  if [ 0 -eq $? ]; then
    valgrind --leak-check=full --error-exitcode=66 ./failed_test_run
    tests_result=$?
    if [ 0 -eq ${tests_result} ]; then
      echo -e "${GREEN}Notecard tests passed!${DEFAULT}"
    else
      echo -e "${RED}Notecard tests failed!${DEFAULT}"
    fi
    all_tests_result=$((all_tests_result+tests_result))
  else
    all_tests_result=999
  fi
fi

if [ 0 -eq $all_tests_result ]; then
  echo && echo -e "${YELLOW}Compiling and running NoteI2c_Arduino Test Suite (no flags)...${DEFAULT}"
  g++ -fprofile-arcs -ftest-coverage -Wall -Wextra -Werror -Wpedantic -Wno-deprecated-declarations -std=c++11 -O0 -g \
    src/NoteI2c_Arduino.cpp \
    test/NoteI2c_Arduino.test.cpp \
    test/mock/mock-arduino.cpp \
    test/mock/mock-note-c-note.c \
    -Isrc \
    -Itest \
    -DNOTE_MOCK \
    -o failed_test_run
  if [ 0 -eq $? ]; then
    valgrind --leak-check=full --error-exitcode=66 ./failed_test_run
    tests_result=$?
    if [ 0 -eq ${tests_result} ]; then
      echo -e "${GREEN}NoteI2c_Arduino tests passed!${DEFAULT}"
    else
      echo -e "${RED}NoteI2c_Arduino tests failed!${DEFAULT}"
    fi
    all_tests_result=$((all_tests_result+tests_result))
  else
    all_tests_result=999
  fi
fi

if [ 0 -eq $all_tests_result ]; then
  echo && echo -e "${YELLOW}Compiling and running NoteI2c_Arduino Test Suite (-DWIRE_HAS_END)...${DEFAULT}"
  g++ -fprofile-arcs -ftest-coverage -Wall -Wextra -Werror -Wpedantic -Wno-deprecated-declarations -std=c++11 -O0 -g \
    src/NoteI2c_Arduino.cpp \
    test/NoteI2c_Arduino.test.cpp \
    test/mock/mock-arduino.cpp \
    test/mock/mock-note-c-note.c \
    -Isrc \
    -Itest \
    -DNOTE_MOCK \
    -DWIRE_HAS_END \
    -o failed_test_run
  if [ 0 -eq $? ]; then
    valgrind --leak-check=full --error-exitcode=66 ./failed_test_run
    tests_result=$?
    if [ 0 -eq ${tests_result} ]; then
      echo -e "${GREEN}NoteI2c_Arduino tests passed! (-DWIRE_HAS_END)${DEFAULT}"
    else
      echo -e "${RED}NoteI2c_Arduino tests failed!${DEFAULT}"
    fi
    all_tests_result=$((all_tests_result+tests_result))
  else
    all_tests_result=999
  fi
fi

if [ 0 -eq $all_tests_result ]; then
  echo && echo -e "${YELLOW}Compiling and running NoteLog_Arduino Test Suite...${DEFAULT}"
  g++ -fprofile-arcs -ftest-coverage -Wall -Wextra -Werror -Wpedantic -Wno-deprecated-declarations -std=c++11 -O0 -g \
    src/NoteLog_Arduino.cpp \
    test/NoteLog_Arduino.test.cpp \
    test/mock/mock-arduino.cpp \
    -Isrc \
    -Itest \
    -DNOTE_MOCK \
    -o failed_test_run
  if [ 0 -eq $? ]; then
    valgrind --leak-check=full --error-exitcode=66 ./failed_test_run
    tests_result=$?
    if [ 0 -eq ${tests_result} ]; then
      echo -e "${GREEN}NoteLog_Arduino tests passed!${DEFAULT}"
    else
      echo -e "${RED}NoteLog_Arduino tests failed!${DEFAULT}"
    fi
    all_tests_result=$((all_tests_result+tests_result))
  else
    all_tests_result=999
  fi
fi

if [ 0 -eq $all_tests_result ]; then
  echo && echo -e "${YELLOW}Compiling and running NoteSerial_Arduino Test Suite...${DEFAULT}"
  g++ -fprofile-arcs -ftest-coverage -Wall -Wextra -Werror -Wpedantic -Wno-deprecated-declarations -std=c++11 -O0 -g \
    src/NoteSerial_Arduino.cpp \
    test/NoteSerial_Arduino.test.cpp \
    test/mock/mock-arduino.cpp \
    test/mock/mock-note-c-note.c \
    -Isrc \
    -Itest \
    -DNOTE_MOCK \
    -o failed_test_run
  if [ 0 -eq $? ]; then
    valgrind --leak-check=full --error-exitcode=66 ./failed_test_run
    tests_result=$?
    if [ 0 -eq ${tests_result} ]; then
      echo -e "${GREEN}NoteSerial_Arduino tests passed!${DEFAULT}"
    else
      echo -e "${RED}NoteSerial_Arduino tests failed!${DEFAULT}"
    fi
    all_tests_result=$((all_tests_result+tests_result))
  else
    all_tests_result=999
  fi
fi

if [ 0 -eq $all_tests_result ]; then
  echo && echo -e "${YELLOW}Compiling and running NoteTxn_Arduino Test Suite...${DEFAULT}"
  g++ -fprofile-arcs -ftest-coverage -Wall -Wextra -Werror -Wpedantic -Wno-deprecated-declarations -std=c++11 -O0 -g \
    src/NoteTxn_Arduino.cpp \
    test/NoteTxn_Arduino.test.cpp \
    test/mock/mock-arduino.cpp \
    test/mock/mock-note-c-note.c \
    -Isrc \
    -Itest \
    -DNOTE_MOCK \
    -o failed_test_run
  if [ 0 -eq $? ]; then
    valgrind --leak-check=full --error-exitcode=66 ./failed_test_run
    tests_result=$?
    if [ 0 -eq ${tests_result} ]; then
      echo -e "${GREEN}NoteTxn_Arduino tests passed!${DEFAULT}"
    else
      echo -e "${RED}NoteTxn_Arduino tests failed!${DEFAULT}"
    fi
    all_tests_result=$((all_tests_result+tests_result))
  else
    all_tests_result=999
  fi
fi

# Print summary statement
if [ 0 -eq ${all_tests_result} ]; then
  echo && echo -e "${GREEN}All tests have passed!${DEFAULT}" && echo

  # Run coverage if available
  if [ $(which lcov) ]; then
    rm -f mock-*.gc?? *_Mock.gc?? *test.gc??
    gcov --version \
    && lcov --version \
    && mkdir -p ./coverage \
    && lcov --capture \
      --directory . \
      --no-external \
      --exclude 'test/*' \
      --output-file ./coverage/lcov.info \
      --rc branch_coverage=1
    if [ ! -f "./coverage/lcov.info" ]; then
      echo -e "${YELLOW}COVERAGE REPORT NOT PRODUCED!!!${DEFAULT}";
      all_tests_result=998
    else
      lcov --summary --rc branch_coverage=1 ./coverage/lcov.info
    fi
  fi
  rm -f failed_test_run
else
  echo && echo -e "${RED}TESTS FAILED!!!${DEFAULT}"
fi

# Clean testing artifacts
rm -f *.gcda *.gcno

exit $all_tests_result
