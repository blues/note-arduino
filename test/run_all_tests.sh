#!/bin/sh

all_tests_result=0

if [ 0 -eq $all_tests_result ]; then
  echo && echo 'Compiling and running Notecard Test Suite...'
  g++ -fprofile-arcs -ftest-coverage -Wall -Wextra -Werror -Wpedantic -std=c++11 -O0 -g \
    src/Notecard.cpp \
    test/Notecard.test.cpp \
    test/mock/mock-arduino.cpp \
    test/mock/mock-note-c-note.c \
    test/mock/NoteI2c_Mock.cpp \
    test/mock/NoteLog_Mock.cpp \
    test/mock/NoteSerial_Mock.cpp \
    -Isrc \
    -Itest \
    -DNOTE_MOCK
  if [ 0 -eq $? ]; then
    valgrind --leak-check=full --error-exitcode=66 ./a.out
    tests_result=$?
    if [ 0 -eq ${tests_result} ]; then
      echo 'Notecard tests passed!'
    else
      echo "Notecard tests failed!"
    fi
    all_tests_result=$((all_tests_result+tests_result))
  else
    all_tests_result=999
  fi
fi

if [ 0 -eq $all_tests_result ]; then
  echo && echo 'Compiling and running NoteI2c_Arduino Test Suite (no flags)...'
  g++ -fprofile-arcs -ftest-coverage -Wall -Wextra -Werror -Wpedantic -std=c++11 -O0 -g \
    src/NoteI2c_Arduino.cpp \
    test/NoteI2c_Arduino.test.cpp \
    test/mock/mock-arduino.cpp \
    test/mock/mock-note-c-note.c \
    -Isrc \
    -Itest \
    -DNOTE_MOCK
  if [ 0 -eq $? ]; then
    valgrind --leak-check=full --error-exitcode=66 ./a.out
    tests_result=$?
    if [ 0 -eq ${tests_result} ]; then
      echo 'NoteI2c_Arduino tests passed!'
    else
      echo "NoteI2c_Arduino tests failed!"
    fi
    all_tests_result=$((all_tests_result+tests_result))
  else
    all_tests_result=999
  fi
fi

if [ 0 -eq $all_tests_result ]; then
  echo && echo 'Compiling and running NoteI2c_Arduino Test Suite (-DWIRE_HAS_END)...'
  g++ -fprofile-arcs -ftest-coverage -Wall -Wextra -Werror -Wpedantic -std=c++11 -O0 -g \
    src/NoteI2c_Arduino.cpp \
    test/NoteI2c_Arduino.test.cpp \
    test/mock/mock-arduino.cpp \
    test/mock/mock-note-c-note.c \
    -Isrc \
    -Itest \
    -DNOTE_MOCK \
    -DWIRE_HAS_END
  if [ 0 -eq $? ]; then
    valgrind --leak-check=full --error-exitcode=66 ./a.out
    tests_result=$?
    if [ 0 -eq ${tests_result} ]; then
      echo 'NoteI2c_Arduino tests passed! (-DWIRE_HAS_END)'
    else
      echo "NoteI2c_Arduino tests failed!"
    fi
    all_tests_result=$((all_tests_result+tests_result))
  else
    all_tests_result=999
  fi
fi

if [ 0 -eq $all_tests_result ]; then
  echo && echo 'Compiling and running NoteLog_Arduino Test Suite...'
  g++ -fprofile-arcs -ftest-coverage -Wall -Wextra -Werror -Wpedantic -std=c++11 -O0 -g \
    src/NoteLog_Arduino.cpp \
    test/NoteLog_Arduino.test.cpp \
    test/mock/mock-arduino.cpp \
    -Isrc \
    -Itest \
    -DNOTE_MOCK
  if [ 0 -eq $? ]; then
    valgrind --leak-check=full --error-exitcode=66 ./a.out
    tests_result=$?
    if [ 0 -eq ${tests_result} ]; then
      echo 'NoteLog_Arduino tests passed!'
    else
      echo "NoteLog_Arduino tests failed!"
    fi
    all_tests_result=$((all_tests_result+tests_result))
  else
    all_tests_result=999
  fi
fi

if [ 0 -eq $all_tests_result ]; then
  echo && echo 'Compiling and running NoteSerial_Arduino Test Suite...'
  g++ -fprofile-arcs -ftest-coverage -Wall -Wextra -Werror -Wpedantic -std=c++11 -O0 -g \
    src/NoteSerial_Arduino.cpp \
    test/NoteSerial_Arduino.test.cpp \
    test/mock/mock-arduino.cpp \
    -Isrc \
    -Itest \
    -DNOTE_MOCK
  if [ 0 -eq $? ]; then
    valgrind --leak-check=full --error-exitcode=66 ./a.out
    tests_result=$?
    if [ 0 -eq ${tests_result} ]; then
      echo 'NoteSerial_Arduino tests passed!'
    else
      echo "NoteSerial_Arduino tests failed!"
    fi
    all_tests_result=$((all_tests_result+tests_result))
  else
    all_tests_result=999
  fi
fi

# Print summary statement
if [ 0 -eq ${all_tests_result} ]; then
  echo && echo 'All tests have passed!' && echo

  # Run coverage if available
  if [ $(which lcov) ]; then
    rm mock-*.gc?? *_Mock.gc?? *test.gc?? \
    && gcov --version \
    && lcov --version \
    && mkdir -p ./coverage \
    && lcov --capture \
      --directory . \
      --exclude '/note-arduino/test/*' \
      --output-file ./coverage/lcov.info \
      --rc lcov_branch_coverage=1
    if [ ! -f "./coverage/lcov.info" ]; then
      echo "COVERAGE REPORT NOT PRODUCED!!!";
      all_tests_result=998
    else
      lcov --summary ./coverage/lcov.info
    fi
  fi
else
  echo && echo 'TESTS FAILED!!!'
fi

# Clean testing artifacts
rm -f *.gcda *.gcno ./a.out

exit $all_tests_result
