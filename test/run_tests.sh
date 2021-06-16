#!/bin/bash

all_tests_result=0

g++ -Wall -Wextra -Wpedantic mock/mock-arduino.cpp mock/mock-note-c-note.c ../src/Notecard.cpp ../src/NoteI2c_Arduino.cpp ../src/NoteSerial_Arduino.cpp Notecard.test.cpp -std=c++11 -I. -I../src -DMOCK
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

g++ -Wall -Wextra -Wpedantic mock/mock-arduino.cpp mock/mock-note-c-note.c ../src/NoteI2c_Arduino.cpp NoteI2c_Arduino.test.cpp -std=c++11 -I. -I../src -DMOCK
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

g++ -Wall -Wextra -Wpedantic mock/mock-arduino.cpp mock/mock-note-c-note.c ../src/NoteI2c_Arduino.cpp NoteI2c_Arduino.test.cpp -std=c++11 -I. -I../src -DMOCK -DWIRE_HAS_END
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

g++ -Wall -Wextra -Wpedantic mock/mock-arduino.cpp ../src/NoteSerial_Arduino.cpp NoteSerial_Arduino.test.cpp -std=c++11 -I. -I../src -DMOCK
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
