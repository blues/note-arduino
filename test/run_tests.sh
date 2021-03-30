#!/bin/bash

all_tests_result=0

g++ -Wall -Wextra -Wpedantic mock/mock-arduino.cpp mock/mock-note-c-note.c ../src/Notecard.cpp ../src/NoteSerial_Arduino.cpp Notecard.test.cpp -std=c++14 -I. -I../src -DMOCK
if [ 0 -eq $? ]; then
  echo
  ./a.out
  tests_result=$?
  if [ 0 -eq ${tests_result} ]; then
    echo 'Notecard tests passed!'
  else
    echo "Test <${tests_result}> failed!"
  fi
  all_tests_result=$((all_tests_result+tests_result))
fi

g++ -Wall -Wextra -Wpedantic mock/mock-arduino.cpp ../src/NoteSerial_Arduino.cpp NoteSerial_Arduino.test.cpp -std=c++14 -I. -I../src -DMOCK
if [ 0 -eq $? ]; then
  echo
  ./a.out
  tests_result=$?
  if [ 0 -eq ${tests_result} ]; then
    echo 'NoteSerial_Arduino tests passed!'
  else
    echo "Test <${tests_result}> failed!"
  fi
  all_tests_result=$((all_tests_result+tests_result))
fi

echo
if [ 0 -eq ${all_tests_result} ]; then
  echo 'All tests passed!'
else
  echo 'TESTS FAILED!!!'
fi
