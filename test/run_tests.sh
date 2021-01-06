g++ mock/mock-arduino.cpp mock/mock-note.c ../src/Notecard.cpp notecard-test.cpp -std=c++17 -I. -I../src -DMOCK
if [ 0 -eq $? ]; then
  ./a.out
  tests_result=$?
  echo
  if [ 0 -eq ${tests_result} ]; then
    echo 'All tests passed!'
  else
    echo "Test <${tests_result}> failed!"
  fi
fi
