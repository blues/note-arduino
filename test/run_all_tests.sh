#!/bin/sh

all_tests_result=0

g++ -fprofile-arcs -ftest-coverage -Wall -Wextra -Wpedantic -std=c++11 -O0 -g \
  src/Notecard.cpp \
  src/NoteI2c_Arduino.cpp \
  src/NoteLog_Arduino.cpp \
  src/NoteSerial_Arduino.cpp \
  test/Notecard.test.cpp \
  test/mock/mock-arduino.cpp \
  test/mock/mock-note-c-note.c \
  -Isrc \
  -Itest \
  -DNOTE_MOCK
if [ 0 -eq $? ] && [ 0 -eq $all_tests_result ]; then
  echo
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

g++ -fprofile-arcs -ftest-coverage -Wall -Wextra -Wpedantic -std=c++11 -O0 -g \
  src/NoteI2c_Arduino.cpp \
  test/NoteI2c_Arduino.test.cpp \
  test/mock/mock-arduino.cpp \
  test/mock/mock-note-c-note.c \
  -Isrc \
  -Itest \
  -DNOTE_MOCK
if [ 0 -eq $? ] && [ 0 -eq $all_tests_result ]; then
  echo
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

g++ -fprofile-arcs -ftest-coverage -Wall -Wextra -Wpedantic -std=c++11 -O0 -g \
  src/NoteI2c_Arduino.cpp \
  test/NoteI2c_Arduino.test.cpp \
  test/mock/mock-arduino.cpp \
  test/mock/mock-note-c-note.c \
  -Isrc \
  -Itest \
  -DNOTE_MOCK \
  -DWIRE_HAS_END
if [ 0 -eq $? ] && [ 0 -eq $all_tests_result ]; then
  echo
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

g++ -fprofile-arcs -ftest-coverage -Wall -Wextra -Wpedantic -std=c++11 -O0 -g \
  src/NoteLog_Arduino.cpp \
  test/NoteLog_Arduino.test.cpp \
  test/mock/mock-arduino.cpp \
  -Isrc \
  -Itest \
  -DNOTE_MOCK
if [ 0 -eq $? ] && [ 0 -eq $all_tests_result ]; then
  echo
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

g++ -fprofile-arcs -ftest-coverage -Wall -Wextra -Wpedantic -std=c++11 -O0 -g \
  src/NoteSerial_Arduino.cpp \
  test/NoteSerial_Arduino.test.cpp \
  test/mock/mock-arduino.cpp \
  -Isrc \
  -Itest \
  -DNOTE_MOCK
if [ 0 -eq $? ] && [ 0 -eq $all_tests_result ]; then
  echo
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

echo
if [ 0 -eq ${all_tests_result} ]; then
  echo 'All tests have passed!'
  gcovr --print-summary --sort-percentage --exclude-throw-branches --delete \
    --object-directory . \
    --root src \
    --exclude .*_error.* \
  && rm ./a.out *.gcno
else
  echo 'TESTS FAILED!!!'
fi
