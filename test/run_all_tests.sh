#!/bin/sh

all_tests_result=0

g++ -Wall -Wextra -Wpedantic src/Notecard.cpp src/NoteI2c_Arduino.cpp src/NoteLog_Arduino.cpp src/NoteSerial_Arduino.cpp test/Notecard.test.cpp test/mock/mock-arduino.cpp test/mock/mock-note-c-note.c -std=c++11 -Isrc -Itest -DNOTE_MOCK
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

g++ -Wall -Wextra -Wpedantic src/NoteI2c_Arduino.cpp test/NoteI2c_Arduino.test.cpp test/mock/mock-arduino.cpp test/mock/mock-note-c-note.c -std=c++11 -Isrc -Itest -DNOTE_MOCK
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

g++ -Wall -Wextra -Wpedantic src/NoteI2c_Arduino.cpp test/NoteI2c_Arduino.test.cpp test/mock/mock-arduino.cpp test/mock/mock-note-c-note.c -std=c++11 -Isrc -Itest -DNOTE_MOCK -DWIRE_HAS_END
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

g++ -Wall -Wextra -Wpedantic src/NoteLog_Arduino.cpp test/NoteLog_Arduino.test.cpp test/mock/mock-arduino.cpp -std=c++11 -Isrc -Itest -DNOTE_MOCK
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

g++ -Wall -Wextra -Wpedantic src/NoteSerial_Arduino.cpp test/NoteSerial_Arduino.test.cpp test/mock/mock-arduino.cpp -std=c++11 -Isrc -Itest -DNOTE_MOCK
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
else
  echo 'TESTS FAILED!!!'
fi
