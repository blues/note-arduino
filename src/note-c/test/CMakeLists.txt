cmake_minimum_required(VERSION 3.20)

project(UnitTests)

include(FetchContent)

# Catch2 v3 requires C++14.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(STATUS "Downloading Catch2...")
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.3.0
)
FetchContent_MakeAvailable(Catch2)

# Add specific build flags for Catch2
if(TARGET Catch2)
    target_compile_options(Catch2
        PUBLIC
            -m32
    )
endif()

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

include(Catch)

set(TEST_TARGETS "")

macro(add_test TEST_NAME)
    add_executable(
        ${TEST_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_NAME}.cpp
    )
    target_include_directories(${TEST_NAME}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
        PRIVATE ${NOTE_C_SRC_DIR}
    )
    target_link_libraries(${TEST_NAME}
        PRIVATE note_c
        PRIVATE Catch2::Catch2WithMain
    )
    target_compile_options(${TEST_NAME}
        PRIVATE -Og
        PRIVATE -ggdb
    )

    list(APPEND TEST_TARGETS ${TEST_NAME})

    catch_discover_tests(${TEST_NAME})
endmacro(add_test)

add_test(_crcAdd_test)
add_test(_crcError_test)
add_test(_errDoc_test)
add_test(_i2cChunkedReceive_test)
add_test(_i2cChunkedTransmit_test)
add_test(_i2cNoteChunkedReceive_test)
add_test(_i2cNoteChunkedTransmit_test)
add_test(_i2cNoteQueryLength_test)
add_test(_i2cNoteReset_test)
add_test(_i2cNoteTransaction_test)
add_test(_j_tolower_test)
add_test(_noteChunkedReceive_test)
add_test(_noteChunkedTransmit_test)
add_test(_noteHardReset_test)
add_test(_noteHeartbeat_test)
add_test(_noteI2CReceive_test)
add_test(_noteI2CReset_test)
add_test(_noteI2CTransmit_test)
add_test(_noteJSONTransaction_test)
add_test(_noteSerialAvailable_test)
add_test(_noteSerialReceive_test)
add_test(_noteSerialReset_test)
add_test(_noteSerialTransmit_test)
add_test(_noteSetActiveInterface_test)
add_test(_noteTransaction_calculateTimeoutMs_test)
add_test(_serialChunkedReceive_test)
add_test(_serialChunkedTransmit_test)
add_test(_serialNoteReset_test)
add_test(_serialNoteTransaction_test)
add_test(JAddBinaryToObject_test)
add_test(JAllocString_test)
add_test(JAtoI_test)
add_test(JAtoN_test)
add_test(JBaseItemType_test)
add_test(JBoolValue_test)
add_test(JContainsString_test)
add_test(JGetArray_test)
add_test(JGetBinaryFromObject_test)
add_test(JGetBool_test)
add_test(JGetInt_test)
add_test(JGetItemName_test)
add_test(JGetNumber_test)
add_test(JGetObject_test)
add_test(JGetString_test)
add_test(JGetType_test)
add_test(JIntValue_test)
add_test(JIsExactString_test)
add_test(JIsNullString_test)
add_test(JIsPresent_test)
add_test(JItoA_test)
add_test(JNtoA_test)
add_test(JNumberValue_test)
add_test(JPrintUnformatted_test)
add_test(JSON_number_handling_test)
add_test(JStringValue_test)
add_test(JType_test)
add_test(NoteAdd_test)
add_test(NoteAuxSerialFlowControl_test)
add_test(NoteBinaryCodecDecode_test)
add_test(NoteBinaryCodecEncode_test)
add_test(NoteBinaryCodecMaxDecodedLength_test)
add_test(NoteBinaryCodecMaxEncodedLength_test)
add_test(NoteBinaryStoreDecodedLength_test)
add_test(NoteBinaryStoreEncodedLength_test)
add_test(NoteBinaryStoreReceive_test)
add_test(NoteBinaryStoreReset_test)
add_test(NoteBinaryStoreTransmit_test)
add_test(NoteClearLocation_test)
add_test(NoteDebug_test)
add_test(NoteDebugf_test)
add_test(NoteDebugSyncStatus_test)
add_test(NoteDelayMs_test)
add_test(NoteErrorClean_test)
add_test(NoteFactoryReset_test)
add_test(NoteFree_test)
add_test(NoteGetActiveInterface_test)
add_test(NoteGetContact_test)
add_test(NoteGetEnv_test)
add_test(NoteGetEnvNumber_test)
add_test(NoteGetFnDebugOutput_test)
add_test(NoteGetFnHeartbeat_test)
add_test(NoteGetFn_test)
add_test(NoteGetFnI2C_test)
add_test(NoteGetI2CAddress_test)
add_test(NoteGetI2CMtu_test)
add_test(NoteGetFnI2CMutex_test)
add_test(NoteGetFnMutex_test)
add_test(NoteGetFnNoteMutex_test)
add_test(NoteGetFnSerial_test)
add_test(NoteGetFnTransaction_test)
add_test(NoteGetLocation_test)
add_test(NoteGetLocationMode_test)
add_test(NoteGetMs_test)
add_test(NoteGetNetStatus_test)
add_test(NoteGetServiceConfig_test)
add_test(NoteGetStatus_test)
add_test(NoteGetTemperature_test)
add_test(NoteGetVersion_test)
add_test(NoteGetVoltage_test)
add_test(NoteI2CAddress_test)
add_test(NoteI2CMax_test)
add_test(NoteIsConnected_test)
add_test(NoteLocalTimeST_test)
add_test(NoteLocationValid_test)
add_test(NoteMalloc_test)
add_test(NoteNewCommand_test)
add_test(NoteNewRequest_test)
add_test(NotePayload_test)
add_test(NotePayloadRetrieveAfterSleep_test)
add_test(NotePayloadSaveAndSleep_test)
add_test(NotePrint_test)
add_test(NotePrintf_test)
add_test(NotePrintln_test)
add_test(NoteRegion_test)
add_test(NoteRequest_test)
add_test(NoteRequestResponse_test)
add_test(NoteRequestResponseJSON_test)
add_test(NoteRequestResponseWithRetry_test)
add_test(NoteRequestWithRetry_test)
add_test(NoteReset_test)
add_test(NoteResponseError_test)
add_test(NoteResumeTransactionDebug_test)
add_test(NoteSendToRoute_test)
add_test(NoteSerialHooks_test)
add_test(NoteSetActiveInterface_test)
add_test(NoteSetContact_test)
add_test(NoteSetEnvDefault_test)
add_test(NoteSetEnvDefaultNumber_test)
add_test(NoteSetFn_test)
add_test(NoteSetFnDebugOutput_test)
add_test(NoteSetFnHeartbeat_test)
add_test(NoteSetFnDefault_test)
add_test(NoteSetFnDisabled_test)
add_test(NoteSetFnI2C_test)
add_test(NoteSetFnI2CMutex_test)
add_test(NoteSetFnMutex_test)
add_test(NoteSetFnNoteMutex_test)
add_test(NoteSetFnSerial_test)
add_test(NoteSetFnSerialDefault_test)
add_test(NoteSetFnI2CDefault_test)
add_test(NoteSetFnTransaction_test)
add_test(NoteSetI2CAddress_test)
add_test(NoteSetI2CMtu_test)
add_test(NoteSetLocation_test)
add_test(NoteSetLocationMode_test)
add_test(NoteSetLogLevel_test)
add_test(NoteSetProductID_test)
add_test(NoteSetRequestTimeout_test)
add_test(NoteSetSerialNumber_test)
add_test(NoteSetSyncMode_test)
add_test(NoteSetUploadMode_test)
add_test(NoteSleep_test)
add_test(NoteSuspendTransactionDebug_test)
add_test(NoteTemplate_test)
add_test(NoteTime_test)
add_test(NoteTimeSet_test)
add_test(NoteTransaction_test)
add_test(NoteTransactionHooks_test)
add_test(NoteUserAgent_test)
add_test(NoteWake_test)

if(NOTE_C_COVERAGE)
    find_program(LCOV lcov REQUIRED)
    message(STATUS "Found lcov: ${LCOV}")

    target_compile_options(note_c PUBLIC --coverage -g -O0)
    target_link_libraries(note_c gcov)

    # Create the coverage directory and run ctest.
    add_custom_target(
        coverage
        COMMAND mkdir -p coverage
        # Zero out the coverage counters from any previous runs.
        COMMAND lcov --zerocounters --directory ${NOTE_C_SRC_DIR}
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    # These files are third party code that we aren't interested in testing
    # ourselves, so we don't care about coverage for them.
    set(
        EXCLUDE_FROM_COVERAGE
        "n_atof.c;n_b64.c;n_cjson.c;n_ftoa.c;n_md5.c;n_str.c"
    )
    foreach(EXCLUDE_FILE ${EXCLUDE_FROM_COVERAGE})
        string(APPEND LCOV_EXCLUDE "--exclude '*/${EXCLUDE_FILE}' ")
    endforeach()

    # We don't care about coverage for the tests or testing infrastructure.
    string(APPEND LCOV_EXCLUDE "--exclude '*/test/*' ")
    string(APPEND LCOV_EXCLUDE "--exclude '*/build/_deps/*' ")

    separate_arguments(LCOV_EXCLUDE_LIST NATIVE_COMMAND "${LCOV_EXCLUDE}")
    # Run lcov to produce a coverage report in the coverage directory.
    add_custom_command(
        TARGET coverage POST_BUILD
        COMMAND lcov --capture --no-external --directory ${NOTE_C_SRC_DIR} --rc lcov_branch_coverage=1 ${LCOV_EXCLUDE_LIST} --output-file lcov.info
        COMMAND lcov --summary --rc lcov_branch_coverage=1 lcov.info
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/coverage
    )
    # The tests have to be built before we can generate the coverage report.
    add_dependencies(coverage ${TEST_TARGETS})
    # Delete the coverage directory when make clean is run.
    set_property(
        DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
        ${CMAKE_CURRENT_BINARY_DIR}/coverage
    )
endif(NOTE_C_COVERAGE)
