
#include "NotecardBinary.h"
#include "NotecardComms.h"
#include "test_binary_generators.h"
#include "test_unity_util.h"
#include "utils.h"  // for XSTR
#ifndef TEST_CARD_BINARY_EXHAUSTIVELY
#define TEST_CARD_BINARY_EXHAUSTIVELY 0
#endif


NotecardBinary cardBinary;

#if !defined(NOTEHUB_PROXY_ROUTE_ALIAS) && defined(DEFAULT_NOTEHUB_PROXY_ROUTE_ALIAS)
#define NOTEHUB_PROXY_ROUTE_ALIAS DEFAULT_NOTEHUB_PROXY_ROUTE_ALIAS
#endif

#ifndef NOTEHUB_PROXY_ROUTE_ALIAS
#error "NOTEHUB_PROXY_ROUTE_ALIAS not defined"
#endif

// The filter is useful for running a subset of tests, especially when they are defined
// as regular functions rather than macros.
// #define TEST_CARD_BINARY_FILTER strstr(name, "serial")

#ifndef TEST_CARD_BINARY_FILTER
#define TEST_CARD_BINARY_FILTER true
#endif

bool testsuite_card_binary_filter(const char* name)
{
    return TEST_CARD_BINARY_FILTER;
}

void testsuite_card_binary_setUp(void)
{
    cardBinary.notecardDebugStreamInitialize();
}

void testsuite_card_binary_tearDown(void)
{
}

void assert_initialize_notecard(NotecardInterface iface)
{
    TEST_ASSERT_TRUE_MESSAGE(initialize_notecard_interface(iface), "Unable to initialize Notecard I/O.");
}

// todo - don't guess at the buffer size use the Note
uint8_t transfer_buffer[NotecardBinary::MAX_CHUNK_SIZE + 128];
size_t max_binary_length;

void AssertNoteBinaryReset()
{
    const char* err = NoteBinaryStoreReset();
    if (err) {
        notecard.logDebugf("Error calling NoteBinaryReset %s\n", err);
        TEST_FAIL_MESSAGE("NoteBinaryReset failed.");
    }
}

struct BinaryTestArgs {
    /**
     * @brief The upper limit on how much binary data to send to the notecard. Default value is
     *   maximum buffer size returned from `card.binary`.
     */
    size_t _maxCardBinary;

    /**
     * @brief The chunk sizes to use for retrieving the binary via card.binary.get.
     * Default is to use maxCardBinary.
     */
    const int* _validateChunkSizes;
    size_t _validateChunkSizesCount;

    enum Handler {
        NONE,
        WEB_POST_CHUNKED,   // multiple calls to web.post, each resulting a request to the web.post endpoint
        WEB_POST_UNCHUNKED, // multiple calls to web.post, combined by Notehub resulting in one call to the web.post endpoint.
        NOTE_ADD
    } _handler;


    BinaryTestArgs()
    {
        _maxCardBinary = 0;
        _validateChunkSizes = nullptr;
        _validateChunkSizesCount = 0;
        _handler = NONE;
    }

    BinaryTestArgs& handler(Handler handler)
    {
        _handler = handler;
        return *this;
    }

    BinaryTestArgs& maxCardBinary(size_t max)
    {
        _maxCardBinary = max;
        return *this;
    }

    template<const size_t N> BinaryTestArgs& validateChunkSizes(const int (&sizes)[N])
    {
        _validateChunkSizesCount = N;
        _validateChunkSizes = sizes;
        return *this;
    }

};

/**
 * @brief Primary function that runs a single binary transfer over a given Notecard interface.
 *
 * @param generator The binary data to transfer
 * @param iface The Notecard interface to use for the communication with Notecard
 * @param chunkSize The amount of binary data transferred in each chunk (card.binary.put request)
 */
void binaryTransferTest(const char* name, BinaryGenerator& generator, NotecardInterface iface, size_t chunkSize, const BinaryTestArgs& testArgs)
{
    assert_initialize_notecard(iface);
    // ensure notecard is in the correct mode for binary file transfer.
    // Todo this is only required when using web.* to retrieve/upload the uploaded binary.
    TEST_ASSERT_TRUE_MESSAGE(cardBinary.notecardConnectionMode(), "Unable to set Notecard connection mode");

    AssertNoteBinaryReset();

    size_t maxCardBinary = testArgs._maxCardBinary;
    if (!maxCardBinary) {
        if (!max_binary_length) {
            TEST_FAIL_MESSAGE("max_binary_length not set. Be sure the get_max_length test has been run first.");
            return;
        } else {
            maxCardBinary = max_binary_length;
        }
    }

    if (!chunkSize) {
        chunkSize = cardBinary.maxChunkSize(iface);
    }

    NotecardBinary::BinaryTransfer tx = {
        .imageName = name,
        .image = generator,
        .chunkSize = chunkSize,
        .buffer = transfer_buffer,
        .bufferLength = sizeof(transfer_buffer),
        .maxBinarySize = maxCardBinary,
        .validate = true,
        .transfer_cb = NotecardBinary::accept_transfer_callback
    };

    // allocate all the handlers on the stack, easier memory management. They're not large.

    WebPostHandler webPostHandler(NOTEHUB_PROXY_ROUTE_ALIAS, tx.imageName, MIME_APPLICATION_OCTETSTREAM, false, false);
    WebPostHandler webPostChunkedHandler(NOTEHUB_PROXY_ROUTE_ALIAS, tx.imageName, MIME_APPLICATION_OCTETSTREAM, true, false);
    WebPostHandler webPutHandler(NOTEHUB_PROXY_ROUTE_ALIAS, tx.imageName, MIME_APPLICATION_OCTETSTREAM, false, true);
    WebPostHandler webPutChunkedHandler(NOTEHUB_PROXY_ROUTE_ALIAS, tx.imageName, MIME_APPLICATION_OCTETSTREAM, true, true);
    NoteAddHandler noteAddHandler(NOTEHUB_PROXY_ROUTE_ALIAS, tx.imageName);

    // NoteAddHandler noteAddHandler;
    switch (testArgs._handler) {
    case BinaryTestArgs::NONE:
        tx.transfer_cb = NotecardBinary::accept_transfer_callback;
        break;
    case BinaryTestArgs::WEB_POST_CHUNKED:
        tx.transfer_cb = webPostChunkedHandler.transfer_callback();
        break;
    case BinaryTestArgs::WEB_POST_UNCHUNKED:
        tx.transfer_cb = webPostHandler.transfer_callback();
        break;
    case BinaryTestArgs::NOTE_ADD:
        tx.transfer_cb = noteAddHandler.transfer_callback();
        break;
    default:
        TEST_FAIL_MESSAGE("Unknown transfer handler");
        return;
    }

    TEST_ASSERT_TRUE_MESSAGE(cardBinary.transferBinary(tx), "binary transfer failed");

    AssertNoteBinaryReset();
}



const BinaryTestArgs webpost = BinaryTestArgs().handler(BinaryTestArgs::WEB_POST_UNCHUNKED);
const BinaryTestArgs webpost_chunked = BinaryTestArgs().handler(BinaryTestArgs::WEB_POST_CHUNKED);
const BinaryTestArgs note_add = BinaryTestArgs().handler(BinaryTestArgs::NOTE_ADD);
const BinaryTestArgs validate;

// negative sizes validate based on the size of the card.binary minus the absolute value. So 0 means
// validate using a buffer exactly large enough to hold the entire transferred binary.
const int validate_chunk_sizes[9] = { 255, 256, 1234, 4095, 4096, -3, -2, -1 /* binary size -1 */, 0 /* binary size */};
const BinaryTestArgs validate_in_chunks = BinaryTestArgs().validateChunkSizes(validate_chunk_sizes);



// This Variadic macro allows parameters to be passed to the constructor of `image`.
// The macro declares a lambda function with the desired test name, and no capture,
// so it's equivalent to a function pointer.
// imagename: the name of the image, used in the test name. Note this should be a token, not a string.
// image: An image builder that is constructed with the size, and the optional variadic arguments.
// It's expected to provide `BinaryGenerator& operator()` to retrieve the built image.
// iface: a NotecardInterface enum value, such as NOTECARD_IF_SErIAL.
// ifacename: A friendly name for the notecard interface, appears in the test name.
// size: the size of the image to create/transfer.
// sizename: a name for the size, appears in the test name.
// testArgs: additional test arguments

// This global avoids any capture on the lambda so it remains a regular C functions.
const BinaryTestArgs* currentTestArgs;

#define RUN_SIZE(imagename, image, iface, ifacename, size, sizename, testArgs, ...) { \
  currentTestArgs = &testArgs; \
  UnityTestFunction test_ ## imagename ## _ ## ifacename ## _ ## sizename ## testArgs = []() { \
    image builder = image(size __VA_OPT__(,) __VA_ARGS__); \
    binaryTransferTest(XSTR(imagename), builder(), iface, 0, *currentTestArgs); \
  }; \
  RUN_FILTER(test_ ## imagename ## _ ## ifacename ## _ ## sizename ## testArgs ); \
  currentTestArgs = nullptr; \
}

// Tests binary transfer over a range of sizes.
#define RUN_ALL_SIZES(imagename, image, iface, ifacename, testArgs, ...) \
  RUN_SIZE(imagename, image, iface, ifacename, TINY_SIZE, TINY_SIZE_NAME, testArgs, __VA_ARGS__); \
  RUN_SIZE(imagename, image, iface, ifacename, SMALL_SIZE, SMALL_SIZE_NAME, testArgs, __VA_ARGS__); \
  RUN_SIZE(imagename, image, iface, ifacename, MEDIUM_SIZE, MEDIUM_SIZE_NAME, testArgs, __VA_ARGS__); \
  RUN_SIZE(imagename, image, iface, ifacename, LARGE_SIZE, LARGE_SIZE_NAME, testArgs, __VA_ARGS__); \
  RUN_SIZE(imagename, image, iface, ifacename, XLARGE_SIZE, XLARGE_SIZE_NAME, testArgs, __VA_ARGS__); \
// todo - add sizes greater than one buffer size

// Tests binary transfer over a range of sizes (using RUN_ALL_SIZES) and over all Notecard Interfaces.
#define RUN_ALL_SIZES_ALL_IFACES(imagename, image, testArgs, ...) \
  RUN_ALL_SIZES(imagename, image, NOTECARD_IF_AUX_SERIAL, auxserial, testArgs, __VA_ARGS__); \
  RUN_ALL_SIZES(imagename, image, NOTECARD_IF_I2C, i2c, testArgs, __VA_ARGS__); \
  RUN_ALL_SIZES(imagename, image, NOTECARD_IF_SERIAL, serial, testArgs, __VA_ARGS__);

// Tests binary transfer over AUX serial, using the given baud rate and chunk size.
#define RUN_AUX_SERIAL_BAUDRATE_CHUNK(imagename, image, size, sizename, baudrate, chunkSize, testArgs, ...) \
  UnityTestFunction test_ ## imagename ## _ ## aux_serial ## _ ## baudrate ## _ ## sizename ## chunk ## chunkSize ## _ ## testArgs = []() { \
    image builder = image(size __VA_OPT__(,) __VA_ARGS__); \
    set_aux_serial_baudrate(baudrate); \
    binaryTransferTest(builder(), NOTECARD_IF_AUX_SERIAL, chunkSize, testArgs); \
  }; \
  RUN_FILTER(test_ ## imagename ## _ ## aux_serial ## _ ## baudrate ## _ ## sizename ## chunk ## chunkSize ## _ ## testArgs);

#define RUN_AUX_SERIAL_BAUDRATE(imagename, image, size, sizename, baudrate, testArgs, ...) \
  UnityTestFunction test_ ## imagename ## _ ## aux_serial ## _ ## baudrate ## _ ## sizename = []() { \
    image builder = image(size __VA_OPT__(,) __VA_ARGS__); \
    set_aux_serial_baudrate(baudrate); \
    binaryTransferTest(builder(), NOTECARD_IF_AUX_SERIAL); \
  }; \
  RUN_FILTER(test_ ## imagename ## _ ## aux_serial ## _ ## baudrate ## _ ## sizename);

#define RUN_AUX_SERIAL_ALL_CHUNK_SIZES(imagename, image, size, sizename, baudrate, testArgs, ...) \
    RUN_AUX_SERIAL_BAUDRATE_CHUNK(imagename, image, size, sizename, baudrate, testArgs, 256); \
    RUN_AUX_SERIAL_BAUDRATE_CHUNK(imagename, image, size, sizename, baudrate, testArgs, 512); \
    RUN_AUX_SERIAL_BAUDRATE_CHUNK(imagename, image, size, sizename, baudrate, testArgs, 1024); \
    RUN_AUX_SERIAL_BAUDRATE_CHUNK(imagename, image, size, sizename, baudrate, testArgs, 2048); \
    RUN_AUX_SERIAL_BAUDRATE_CHUNK(imagename, image, size, sizename, baudrate, testArgs, 4096);

#define RUN_AUX_SERIAL_SELECT_CHUNK_SIZES(imagename, image, size, sizename, baudrate, ...) \
    RUN_AUX_SERIAL_BAUDRATE_CHUNK(imagename, image, size, sizename, baudrate, testArgs, 256); \
    RUN_AUX_SERIAL_BAUDRATE_CHUNK(imagename, image, size, sizename, baudrate, testArgs, 4096);


// Tests binary transfer over AUX serial using a variety of baudrates
#define RUN_AUX_SERIAL_ALL_BAUDRATES(imagename, image, size, sizename, testArgs, ...) \
  RUN_AUX_SERIAL_BAUDRATE(imagename, image, size, sizename, 9600, testArgs, __VA_ARGS__); \
  RUN_AUX_SERIAL_BAUDRATE(imagename, image, size, sizename, 14400, testArgs, __VA_ARGS__); \
  RUN_AUX_SERIAL_BAUDRATE(imagename, image, size, sizename, 19200, testArgs, __VA_ARGS__); \
  RUN_AUX_SERIAL_BAUDRATE(imagename, image, size, sizename, 38400, testArgs, __VA_ARGS__); \
  RUN_AUX_SERIAL_BAUDRATE(imagename, image, size, sizename, 57600, testArgs, __VA_ARGS__); \
  RUN_AUX_SERIAL_BAUDRATE(imagename, image, size, sizename, 115200, testArgs, __VA_ARGS__);

#define RUN_AUX_SERIAL_SELECT_BAUDRATES(imagename, image, size, sizename, testArgs, ...) \
  RUN_AUX_SERIAL_BAUDRATE(imagename, image, size, sizename, 9600, testArgs, __VA_ARGS__); \
  RUN_AUX_SERIAL_BAUDRATE(imagename, image, size, sizename, 115200, testArgs, __VA_ARGS__);


#ifndef TEST_AUX_SERIAL_EXHAUSTIVELY
#define TEST_AUX_SERIAL_EXHAUSTIVELY TEST_CARD_BINARY_EXHAUSTIVELY
#endif

#if TEST_AUX_SERIAL_EXHAUSTIVELY
#define RUN_AUX_SERIAL_BAUDRATES RUN_AUX_SERIAL_SELECT_BAUDRATES
#else
#define RUN_AUX_SERIAL_BAUDRATES RUN_AUX_SERIAL_ALL_BAUDRATES
#endif


/**
 * @brief Macro that expands to a matrix of tests.
 */
#define RUN_AUX_SERIAL_ALL_SIZES_ALL_BAUDRATES(imagename, image, testArgs, ...) \
  RUN_AUX_SERIAL_ALL_BAUDRATES(imagename, image, TINY_SIZE, TINY_SIZE_NAME, testArgs, __VA_ARGS__); \
  RUN_AUX_SERIAL_ALL_BAUDRATES(imagename, image, SMALL_SIZE, SMALL_SIZE_NAME, testArgs, __VA_ARGS__); \
  RUN_AUX_SERIAL_ALL_BAUDRATES(imagename, image, MEDIUM_SIZE, MEDIUM_SIZE_NAME, testArgs, __VA_ARGS__); \
  RUN_AUX_SERIAL_ALL_BAUDRATES(imagename, image, LARGE_SIZE, LARGE_SIZE_NAME, testArgs, __VA_ARGS__); \
  RUN_AUX_SERIAL_ALL_BAUDRATES(imagename, image, MAX_SIZE, MAX_SIZE_NAME, testArgs, __VA_ARGS__);



const size_t EXPECTED_MAX_BINARY_LENGTH = 130554;

/**
 * @brief Retrieves the maximum card.binary length.
 */
void test_get_max_binary_length()
{
    assert_initialize_notecard(NOTECARD_IF_I2C);

    AssertNoteBinaryReset();

    J* rsp = NoteRequestResponseWithRetry(NoteNewRequest("card.binary"), 10);
    J* max_item = nullptr;
    if (rsp==nullptr || !JIsNullString(rsp, "err") || !JIsNumber(max_item=JGetObjectItem(rsp, "max"))) {
        TEST_FAIL_MESSAGE("Couldn't retrieve the maximum binary length.");
    } else {
        int length = JNumberValue(max_item);
        if (length<1024) {
            TEST_FAIL_MESSAGE("card.binary max is too small.");
        }
        max_binary_length = length;
        TEST_ASSERT_EQUAL(EXPECTED_MAX_BINARY_LENGTH, max_binary_length);
    }
    JDelete(rsp);
}

/**
 * @brief Tests the maximum binary transfer size over a given Notecard interface.
 * Requires `test_get_max_binary_length()` to have been called first.
 * @param nif The notecard interface to use for the test.
 */
void base_test_max_length(NotecardInterface nif, const BinaryTestArgs& testArgs)
{
    if (max_binary_length==0) {
        TEST_FAIL_MESSAGE("could not get max binary length");
    }
    uint8_t buf[257];  // 256+1 so that the pattern "steps upwards"
    for (uint16_t i=0; i<sizeof(buf); i++) {
        buf[i] = uint8_t(i);
    }
    buf[64] = 0;            // throw in some "special" values.
    buf[128] = '\n';
    buf[132] = '\r';
    buf[256] = 0x7;
    BufferBinaryGenerator pattern(buf, sizeof(buf));
    RepeatedBinaryGenerator image(pattern, max_binary_length);
    binaryTransferTest("maxlength", image, nif, 0, testArgs);
}

TEST(test_max_length_serial)
{
    base_test_max_length(NOTECARD_IF_SERIAL, validate);
}

TEST(test_max_length_i2c)
{
    base_test_max_length(NOTECARD_IF_I2C, validate);
}

TEST(test_max_length_aux_serial)
{
    set_aux_serial_baudrate();
    base_test_max_length(NOTECARD_IF_AUX_SERIAL, validate);
}

#define RUN_AUX_SERIAL_ALL_SIZES(imagename, image, baudrate, ...) \
    RUN_AUX_SERIAL_BAUDRATE(imagename, image, 1*1024, 1k, baudrate, __VA_ARGS__); \
    RUN_AUX_SERIAL_BAUDRATE(imagename, image, 2*1024, 2k, baudrate, __VA_ARGS__); \
    RUN_AUX_SERIAL_BAUDRATE(imagename, image, 4*1024, 4k, baudrate, __VA_ARGS__); \
    RUN_AUX_SERIAL_BAUDRATE(imagename, image, 8*1024, 8k, baudrate, __VA_ARGS__); \
    RUN_AUX_SERIAL_BAUDRATE(imagename, image, 16*1024, 16k, baudrate, __VA_ARGS__); \
    RUN_AUX_SERIAL_BAUDRATE(imagename, image, 40*1024, 40k, baudrate, __VA_ARGS__); \
    RUN_AUX_SERIAL_BAUDRATE(imagename, image, 80*1024, 80k, baudrate, __VA_ARGS__); \
    RUN_AUX_SERIAL_BAUDRATE(imagename, image, 120*1024, 120k, baudrate, __VA_ARGS__); \
    RUN_AUX_SERIAL_BAUDRATE(imagename, image, 160*1024, 160k, baudrate, __VA_ARGS__);

// These tests are intended to be fast-executing, and test the essential functionality.
#define RUN_SMOKE_TESTS(interface, interfacename) \
    { /* validate the received data using a variety of chunk sizes. */ \
        RUN_SIZE(Random_2345, BuildRandom, interface, interfacename, 50*1026, 50k, validate_in_chunks, 2345); \
    } \
    {  /* maximum card.binary size is 1k, transferring 5k total, with chunked web.post on each binary buffer */ \
        BinaryTestArgs max10k_webpost = BinaryTestArgs().maxCardBinary(1*1023).handler(BinaryTestArgs::WEB_POST_UNCHUNKED); \
        RUN_SIZE(Random_1234, BuildRandom, interface, interfacename, 5*1026, 5k, max10k_webpost, 1234); \
    } \
    { /* maximum card.binary size is 1k, transferring 5k total, with chunked web.post on each binary buffer */ \
        BinaryTestArgs max10k_webpost_chunked = BinaryTestArgs().maxCardBinary(1*1023).handler(BinaryTestArgs::WEB_POST_CHUNKED); \
        RUN_SIZE(Random_1234, BuildRandom, interface, interfacename, 5*1026, 5k, max10k_webpost_chunked, 1234); \
    }

void waitForNotecardConnected()
{
    // TODO: waitForNotecardConnected takes timeout in milliseconds, so 5*60 seems wrong?
    TEST_ASSERT_TRUE_MESSAGE(NotecardBinary::waitForNotecardConnected(5*60), "Notecard not connected");
}

/**
 * @brief Runs the card.binary test suite.
 *
 */
void testsuite_card_binary()
{
    TEST_SUITE(card_binary);

    // initialize max_binary_length for the max_length tests
    RUN_FILTER(test_get_max_binary_length);

    RUN_FILTER(waitForNotecardConnected);

    // TODO: This test doesn't work with a note.add because it'll try to do a web.get to validate and find nothing there.
    // {
    //     BinaryTestArgs max1k_note_add = BinaryTestArgs().maxCardBinary(1*1023).handler(BinaryTestArgs::NOTE_ADD);
    //     RUN_SIZE(Random_1234, BuildRandom, NOTECARD_IF_I2C, i2c, 5*1026, 5k, max1k_note_add, 1234);
    // }

    RUN_SMOKE_TESTS(NOTECARD_IF_I2C, i2c);
    set_aux_serial_baudrate();
    RUN_SMOKE_TESTS(NOTECARD_IF_AUX_SERIAL, auxserial);
    RUN_SMOKE_TESTS(NOTECARD_IF_SERIAL, serial);

//    RUN_AUX_SERIAL_ALL_BAUDRATES(all_sevens, AllSevens, TINY_SIZE, TINY_SIZE_NAME);
//    RUN_AUX_SERIAL_ALL_BAUDRATES(all_sevens, AllSevens, 4*1024, 4k);

    //RUN_SIZE(all_ones, BuildAllSameValue, NOTECARD_IF_SERIAL, serial, 6*1024, 6k, 1);

#if 0
    RUN_ALL_SIZES_ALL_IFACES(all_sevens, AllSevens);

    RUN_ALL_SIZES_ALL_IFACES(random_1234, BuildRandom, 1234);
    RUN_ALL_SIZES_ALL_IFACES(all_zeros, AllZeros);
    RUN_ALL_SIZES_ALL_IFACES(72k_binary, SmallBinaryImage);
#endif

    RUN_FILTER(test_max_length_i2c);
    RUN_FILTER(test_max_length_aux_serial);
    RUN_FILTER(test_max_length_serial);
}
