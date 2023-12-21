#pragma once

#include <Arduino.h>
#include <Notecard.h>
#include <random>
#include <math.h>
#include <assert.h>
#include "NotecardComms.h"
#include "small_img.h"


extern Notecard notecard;

#define TINY_SIZE (2*1024)
#define TINY_SIZE_NAME 2k
#define SMALL_SIZE (20*1024)
#define SMALL_SIZE_NAME 20k
#define MEDIUM_SIZE (60*1024)
#define MEDIUM_SIZE_NAME 60k
#define LARGE_SIZE (91*1023)    // not a power of 2
#define LARGE_SIZE_NAME 90k9
#define XLARGE_SIZE (120*1023)  // ditto
#define XLARGE_SIZE_NAME 119k8


struct BinaryImage {
    const char* name;
    size_t length;
    const uint8_t* data;
};

/**
 * @brief Interface describing a generator that produces a stream of bytes.
 *
 */
struct BinaryGenerator {

    BinaryGenerator() {}
    virtual ~BinaryGenerator();

    virtual size_t length() const=0;
    virtual size_t offset() const=0;
    virtual size_t remaining() const
    {
        return length()-offset();
    }
    virtual bool isEOF() const
    {
        return remaining()==0;
    }

    virtual void reset()=0;
    virtual int read()=0;
    virtual size_t read(uint8_t* buf, size_t length)=0;
    virtual size_t skip(size_t count)=0;
};

/**
 * @brief Manages the offset and length, and provides a simple implementation of
 * read(buffer, length).
 */
class AbstractBinaryGenerator : public BinaryGenerator
{

protected:
    size_t _offset;
    const size_t _length;

    void resetOffset()
    {
        _offset = 0;
    }
    void incOffset(size_t amount)
    {
        _offset += amount;
    }

    virtual int readNext()=0;

public:
    AbstractBinaryGenerator(size_t length) : _offset(0), _length(length) {}

    bool isEOF() const
    {
        return _offset >= _length;
    }

    virtual size_t offset() const override
    {
        return _offset;
    }
    virtual size_t length() const override
    {
        return _length;
    }

    int read() override
    {
        if (isEOF()) {
            return -1;
        } else {
            int ch = readNext();
            _offset++;
            return ch;
        }
    }

    size_t skip(size_t length) override
    {
        length = std::min(remaining(), length);
        size_t count = 0;
        while (length --> 0) {
            int ch = readNext();
            (void)ch;
            assert(ch >= 0);
            count++;
        }
        incOffset(count);
        return count;
    }

    size_t read(uint8_t* buf, size_t length) override
    {
        length = std::min(remaining(), length);
        size_t count = 0;
        while (length --> 0) {
            int ch = readNext();
            assert(ch >= 0);
            buf[count++] = uint8_t(ch);
        }
        incOffset(count);
        return count;
    }
};

/**
 * @brief Maintains a view of another generator.
 *
 */
class BinaryGeneratorView : public AbstractBinaryGenerator
{
    BinaryGenerator& _image;
    size_t _viewOffset;

    int readNext() override
    {
        return _image.read();
    }

public:

    BinaryGeneratorView(BinaryGenerator& image, size_t offset, size_t length)
        : AbstractBinaryGenerator(length), _image(image), _viewOffset(offset)
    {
    }

    void reset() override
    {
        _image.reset();
        _image.skip(_viewOffset);
        resetOffset();
    }

    size_t read(uint8_t* buf, size_t length) override
    {
        // cap the length to what's remaining in this view
        length = std::min(remaining(), length);
        size_t count = _image.read(buf, length);
        incOffset(count);
        return count;
    }

};

/**
 * @brief Generates binary data from a buffer. The length of the generated data
 * is assumed to be less than or equal to the length of the buffer.
 */
class BufferBinaryGenerator : public AbstractBinaryGenerator
{
    const uint8_t* _buffer;

    int readNext() override
    {
        return _buffer[_offset];
    }

public:
    BufferBinaryGenerator(const uint8_t* buffer, size_t length)
        : AbstractBinaryGenerator(length), _buffer(buffer)
    {}

    BufferBinaryGenerator(const BinaryImage& image):
        AbstractBinaryGenerator(image.length), _buffer(image.data) {}

    void reset() override
    {
        resetOffset();
    }

    size_t read(uint8_t* buffer, size_t length) override
    {
        size_t toRead = std::min(remaining(), length);
        memcpy(buffer, _buffer+offset(), toRead);
        incOffset(toRead);
        return toRead;
    }
};


class RandomBinaryGenerator: public AbstractBinaryGenerator
{
    std::uniform_int_distribution<uint8_t> _distribution;
    std::ranlux24_base _generator;
    int _seed;

protected:
    int readNext()
    {
        return _distribution(_generator);
    }

public:
    RandomBinaryGenerator(size_t length, uint32_t seed)
        : AbstractBinaryGenerator(length), _seed(seed)
    {
        _generator.seed(_seed);
    }

    void reset()
    {
        resetOffset();
        _generator.seed(_seed);
    }
};


class RepeatedBinaryGenerator: public AbstractBinaryGenerator
{
    BinaryGenerator& _source;

    int readNext() override
    {
        if (_source.isEOF()) {
            _source.reset();
        }
        return _source.read();
    }

public:

    RepeatedBinaryGenerator(BinaryGenerator& source, size_t length)
        : AbstractBinaryGenerator(length), _source(source) {}

    void reset() override
    {
        resetOffset();
        _source.reset();
    }

    size_t read(uint8_t* buffer, size_t length)
    {
        size_t toRead = std::min(length, remaining());
        size_t count = 0;
        while (toRead > 0) {
            size_t bytesRead = _source.read(buffer+count, toRead);
            toRead -= bytesRead;
            count += bytesRead;
            if (!bytesRead) {
                _source.reset();
            }
        }
        incOffset(count);
        return count;
    }
};

class NotecardBinary
{

public:

    static const size_t MAX_CHUNK_SIZE = 4096;
    static const size_t MAX_I2C_CHUNK_SIZE = MAX_CHUNK_SIZE;
    static const size_t MAX_SERIAL_CHUNK_SIZE = MAX_CHUNK_SIZE;
    static const size_t MAX_AUX_SERIAL_CHUNK_SIZE = MAX_CHUNK_SIZE;

    size_t maxChunkSize(NotecardInterface iface)
    {
        size_t chunkSize = 0;
        switch (iface) {
        case NOTECARD_IF_I2C:
            chunkSize = MAX_I2C_CHUNK_SIZE;
            break;
        case NOTECARD_IF_SERIAL:
            chunkSize = MAX_SERIAL_CHUNK_SIZE;
            break;
        case NOTECARD_IF_AUX_SERIAL:
            chunkSize = MAX_AUX_SERIAL_CHUNK_SIZE;
            break;
        }
        return chunkSize;
    }


    void notecardDebugStreamInitialize()
    {
#if defined(NOTECARD_DEBUG_STLINK)
        static HardwareSerial stlinkSerial(PIN_VCP_RX, PIN_VCP_TX);
        stlinkSerial.begin(115200);
        notecard.setDebugOutputStream(stlinkSerial);
#else
        notecard.setDebugOutputStream(Serial);
#endif
    }

    bool notecardConnectionMode()
    {
        J *req = notecard.newRequest("hub.set");

#if !defined(PRODUCT_UID) && defined(DEFAULT_PRODUCT_UID)
#define PRODUCT_UID DEFAULT_PRODUCT_UID
#endif

// This is the unique Product Identifier for your device
#ifndef PRODUCT_UID
#define PRODUCT_UID "" // "com.my-company.my-name:my-project"
#pragma message "PRODUCT_UID is not defined in this example. Please ensure your Notecard has a product identifier set before running this example or define it in code here. More details at https://dev.blues.io/tools-and-sdks/samples/product-uid"
#endif

        if (PRODUCT_UID[0]) {
            JAddStringToObject(req, "product", PRODUCT_UID);
        }

        JAddStringToObject(req, "mode", "continuous");

        return notecard.sendRequestWithRetry(req, 5); // 5 seconds
    }

    /**
     * @brief This struct is passed to the transfer callback after each iteration of the Notecard's
     * binary buffer being filled to the requested capacity.
     */
    struct TransferDetails {
        const char* name;                   // a name for this transfer
        size_t transferred;                 // bytes transferred so far (not including this transfer)
        size_t total;                       // the total number of bytes to be transferred
        size_t currentTransferIndex;        // The index (i.e. chunk) of this transfer, 0-based.
        size_t currentTransferSize;         // the current transfer size (the combined size of all card.binary.pot requests)
        const uint8_t* currentTransferMD5;  // MD5 of the current transfer data
        BinaryGenerator& currentTransferImage;      // the current image for this transfer
        bool isComplete;                    // true when this is the last transfer
        BinaryGenerator& totalImage;        // the complete image
        const uint8_t* totalMD5;            // MD5 hash for the complete transfer. valid only when isComplete is true.
        size_t totalChunks;                 // the total number of chunks sent. Valid only when isComplete is true.
    };

    struct TransferHandlerContext {
        int tries;
    };

    using transfer_cb_t = std::function<bool(const TransferDetails&, TransferHandlerContext&)>;

    /**
     * @brief A no-op callback that returns success. Use this when no action is needed after filling the Notecard binary buffer beyond
     * the validation that always happens.
     */
    static bool accept_transfer_callback(const TransferDetails& details, const TransferHandlerContext& ctx)
    {
        return true;
    }


    struct BinaryTransfer {
        /**
         * @brief The name of the image being transferred.
         */
        const char* imageName;

        /**
         * @brief The complete image to sent, chunked, and with a maximum card.binary buffer size.
         */
        BinaryGenerator& image;
        /**
         * @brief The size of each chunk of data sent to the notecard.
         */
        size_t chunkSize;
        /**
         * @brief A work buffer used to hold each encoded chunk.
         */
        uint8_t* buffer;
        /**
         * @brief The size of the work buffer
         */
        size_t bufferLength;

        /**
         * @brief The maximum amount of data to put in the Notecard's binary buffer.
         */
        size_t maxBinarySize;

        /**
         * @brief When true, validate the data using `card.binary.get`.
         */
        bool validate;

        /**
         * @brief The chunk sizes to use for retrieving the binary via card.binary.get.
         * Default is to use maxCardBinary.
         */
        const int32_t* validateChunkSizes;
        size_t validateChunkSizesCount;


        /**
         * @brief A transfer callback. Once the data has been (optionally) validated, invoke the callback to
         * process the binary on the notecard further. (E.g. via note.add or web.get)
         */
        transfer_cb_t transfer_cb;
    };

    bool transferBinary(const BinaryTransfer& tx)
    {
        return _transferBinary(tx.imageName, tx.image, tx.chunkSize, tx.buffer, tx.bufferLength, tx.maxBinarySize, tx.validate, tx.validateChunkSizes, tx.validateChunkSizesCount, tx.transfer_cb);
    }

    /**
     * @brief Transfers an image to the notecard in chunks of `chunkSize`, with a maximum
     * transferred to the Notecard being `maxBinarySize` until all the data has been transferred.
     * A callback is provided to allow the caller to handle each complete buffer transfer to the notecard,
     * before the buffer is reset and the next one begins.
     * @param imageName The name of the image.
     * @param image The binary image to send.
     * @param chunkSize The size of each chunk sent with `card.binary.put`
     * @param buffer A transfer buffer large enough to hold each encoded chunk.
     * @param bufferLength The length of the transfer buffer.
     * @param maxBinarySize The maximum amount of data to transfer via successive calls to `card.binary.put` before
     *  clearing the buffer.
     * @param validate When true `card.binary.get` is used to validate the buffer contents. Note: This has the
     *  side-effect of clearing the buffer.
     * @param validateChunkSizes The chunk sizes to use when validating the binary buffer using `card.binary.get`.
     * @param validateChunkSizesCount The number of chunk sizes in the validateChunkSizes array. When 0, the entire buffer is validated
     * with one call to `card.binary.get`.
     * @param transfer_cb Callback function that is invoked after every successful complete transfer to the Notecard.
     * @return true
     * @return false
     */
    bool _transferBinary(const char* imageName, BinaryGenerator& image, size_t chunkSize, uint8_t* buffer, size_t bufferLength, size_t maxBinarySize, bool validate, const int32_t* validateChunkSizes, size_t validateChunkSizesCount, transfer_cb_t transfer_cb)
    {
        const size_t totalSize = image.length();
        if (totalSize==0) {
            notecard.logDebugf("Image length is 0!");
            return false;
        }

        if (!checkBufferSize(chunkSize, bufferLength)) {
            return false;
        }

        notecard.logDebugf("Sending image of size %u, in transport chunks of size %u, with a maximum Notecard binary size of %u\n", totalSize, chunkSize, maxBinarySize);

        image.reset();
        size_t totalTransferred = 0;
        size_t totalTransferCount = 0;
        uint8_t totalMD5[NOTE_MD5_HASH_SIZE];
        NoteMD5Context totalMD5Context;
        NoteMD5Init(&totalMD5Context);

        // transfer up to maxTransferSize bytes via chunked NoteBinaryTransmit
        while (totalTransferred!=totalSize) {
            uint8_t currentMD5[NOTE_MD5_HASH_SIZE];
            NoteMD5Context currentMD5Context;
            NoteMD5Init(&currentMD5Context);

            size_t chunkIndex = 0;
            const size_t binaryTransferSize = std::min(maxBinarySize, image.remaining());
            BinaryGeneratorView transferImage(image, totalTransferred, binaryTransferSize);
            size_t bytesToTransfer = binaryTransferSize;
            // do the transfer in chunks
            while (bytesToTransfer) {
                const size_t thisChunk = bytesToTransfer >= chunkSize ? chunkSize : bytesToTransfer;
                const size_t offset = chunkSize * chunkIndex;
                notecard.logDebugf("Sending transport chunk %d, offset: %d, length: %d...\n", chunkIndex, offset, thisChunk);

                size_t bytesRead = transferImage.read(buffer, thisChunk);
                if (bytesRead != thisChunk) {
                    notecard.logDebugf("Insufficient data from image: expected %u, got %u\n", thisChunk, bytesRead);
                    return false;
                }

                // update the MD5 contexts before note-c modifies the buffer
                NoteMD5Update(&currentMD5Context, buffer, thisChunk);
                NoteMD5Update(&totalMD5Context, buffer, thisChunk);

                auto bufferFull = []() -> bool { return false; };

                if (transferBinaryChunk(buffer, thisChunk, bufferLength, offset, bufferFull)) {
                    bytesToTransfer -= thisChunk;
                    chunkIndex++;
                } else {
                    // exit the loop so we can print out messages and optionally handle
                    // validation (even on error)
                    goto outer; // sometimes necessary since we dont have labelled breaks in C++
                }

            }
outer:
            const size_t actualTransferSize = binaryTransferSize-bytesToTransfer;
            if (bytesToTransfer) {
                notecard.logDebugf("TRANSFER FAILED: %s - transferred %d(0x%x) bytes of %d(0x%x)", imageName, actualTransferSize, actualTransferSize, binaryTransferSize, binaryTransferSize);
                return false;
            }

            NoteMD5Final(currentMD5, &currentMD5Context);

            notecard.logDebugf("%s: Successfully transferred %d(0x%x) bytes.\n", imageName, binaryTransferSize, binaryTransferSize);
            notecard.logDebug("Validating received binary data...\n");

            TransferDetails tx = {
                .name = imageName,
                .transferred = totalTransferred,
                .total = totalSize,
                .currentTransferIndex = totalTransferCount,
                .currentTransferSize = binaryTransferSize,
                .currentTransferMD5 = currentMD5,
                .currentTransferImage = transferImage,
                .isComplete =  (totalTransferred + binaryTransferSize)==totalSize,
                .totalImage = image,
                .totalMD5 = totalMD5,
            };

            if (tx.isComplete) {
                NoteMD5Final(totalMD5, &totalMD5Context);
                tx.totalChunks = tx.currentTransferIndex+1;
            }

            if (validate) {
                int32_t defaultSizes[1] = { 0 };
                if (!validateChunkSizesCount || !validateChunkSizes) {
                    validateChunkSizesCount = 1;
                    validateChunkSizes = defaultSizes;
                }
                for (size_t idx=0; idx<validateChunkSizesCount; idx++) {
                    int32_t validateChunkSize = validateChunkSizes[idx];
                    int32_t actualChunkSize = validateChunkSize;
                    if (actualChunkSize<=0) {
                        actualChunkSize += tx.currentTransferImage.length();
                    }
                    notecard.logDebugf("Validating buffer contents with length given=%d(actual=%d)\n", validateChunkSize, actualChunkSize);
                    if (!validateBinaryReceived(tx, buffer, bufferLength, actualChunkSize)) {
                        notecard.logDebugf("FAIL: validation failed with chunk size given=%d(actual=%d)\n", validateChunkSize, actualChunkSize);
                        return false;
                    }
                }
            }

            int tries = 0;
            TransferHandlerContext ctx = { .tries = 0 };
            while (++ctx.tries <= 5) {
                if (!transfer_cb(tx, ctx)) {
                    notecard.logDebugf("FAIL: (try %d) %s - Validation cancelled by transfer handler\n", ctx.tries, imageName);
                    if (ctx.tries<0) {
                        notecard.logDebugf("Handler is not idempotent, giving up.\n");
                    }
                } else {
                    if (tries>1) {
                        notecard.logDebugf("SUCCESS: (try %d) %s - transfer handler successful\n", ctx.tries, imageName);
                    }
                    ctx.tries = 0;  // say we're good
                    break;
                }
            }
            if (ctx.tries) {
                return false;
            }

            totalTransferred += actualTransferSize;
            totalTransferCount++;
            if (totalTransferred!=totalSize) {
                // reset the buffer for the next load
                NoteBinaryStoreReset();
            }
        }
        return totalTransferred==totalSize;
    }

    bool checkBufferSize(size_t chunkSize, size_t bufferSize)
    {
        size_t requiredSize = NoteBinaryCodecMaxEncodedLength(chunkSize);

        NoteDebugf("Requested size: %u, available size: %u\n", requiredSize, bufferSize);

        if (requiredSize > bufferSize) {
            NoteDebugf("The required buffer size is larger than the allocated buffer: %d > %d\n", requiredSize, bufferSize);
            return false;
        }
        return true;
    }

    /**
     * @brief Compares the buffer sent to the notecard with the data read back.
     * @param tx    Current transfer details.
     * @param buffer Scratch buffer for running the validation.
     * @param bufferLength Length of the scratch buffer.
     * @param validateChunkSize
     * @return true
     * @return false
     */
    static bool validateBinaryReceived(const TransferDetails& tx, uint8_t* buffer, size_t bufferLength, size_t validateChunkSize)
    {
        bool success = false;

        tx.currentTransferImage.reset();    // back to the beginning to the image can be re-read.
        size_t rxBufferSize = NoteBinaryCodecMaxEncodedLength(validateChunkSize);
        uint8_t* rxBuffer = nullptr;
        const char* err;

        notecard.logDebugf("Validating card binary, %u/%u size %u\n", tx.transferred, tx.total, tx.currentTransferSize);

        if (nullptr==(rxBuffer=(uint8_t*)malloc(rxBufferSize))) {
            notecard.logDebugf("Unable to allocate %d bytes for receive buffer.\n", rxBufferSize);
        } else {
            size_t offset = 0;
            size_t totalBytesRead = 0;
            bool ok = true;
            while (ok && tx.currentTransferImage.remaining()) {
                ok = false;
                const size_t chunkLength = std::min(tx.currentTransferImage.remaining(), validateChunkSize);
                if ((err = NoteBinaryStoreReceive(rxBuffer, rxBufferSize, offset, chunkLength))!=nullptr) {
                    notecard.logDebugf("Error receiving binary: %s\n", err);
                } else {
                    notecard.logDebugf("card.binary.get Receive buffer size requested: %d\n", rxBufferSize);
                    notecard.logDebugf("Validating received data, length %d (0x%x)...\n", chunkLength, chunkLength);
                    size_t chunkBytesRead = 0;
                    // read and validate the image against the buffer in chunks.
                    BinaryGenerator& image = tx.currentTransferImage;   // alias
                    while (chunkBytesRead < chunkLength) {
                        size_t thisChunk = std::min(bufferLength, chunkLength);
                        size_t chunkRead = image.read(buffer, thisChunk);
                        assert(chunkRead == thisChunk);

                        for (size_t i=0; i<chunkRead; i++) {
                            const uint8_t* src = rxBuffer+chunkBytesRead;
                            if (src[i]!=buffer[i]) {
                                size_t offset = chunkBytesRead + i;
                                notecard.logDebugf("Received buffer differs from image at offset %d(0x%x): %d != %d", offset, offset, buffer[i], src[i]);
                                goto cancel;
                            }
                        }
                        chunkBytesRead += chunkRead;
                    }
                    totalBytesRead += chunkBytesRead;
                    ok = true;
                }
            }
cancel:
            if (totalBytesRead==tx.currentTransferImage.length()) {
                notecard.logDebug("Validated received data.\n");

#if defined(NOTE_BINARY_CHECK_EXTRA_DATA_TIMEOUT)
                // This is not necessary to catch the error, since other requests afterwards will fail.
                // note-c also includes a test for extra data, but it doesn't always succeed at lower baudrates
                // see how much data is left on the serial bus
                size_t duration = 0;
                size_t size = readDataUntilTimeout(Serial1, NOTE_BINARY_CHECK_EXTRA_DATA_TIMEOUT, rxBuffer, rxBufferSize, receivedLength, duration);
                if (size) {
                    notecard.logDebugf("additional data after binary: %d (0x%x) bytes read in %dms\n", size, size, duration);
                } else
#endif
                    success = true;
            }
        }
        free(rxBuffer);

        return success;
    }

    bool transferBinaryChunk(uint8_t* chunk, size_t chunkLength, size_t bufferLength, size_t offset, std::function<bool()> binaryBufferFull)
    {
        const char *err = NoteBinaryStoreTransmit(chunk, chunkLength, bufferLength, offset);

        if (err) {
            NoteDebugf("Error transferring binary chunk: %s\n", err);
            if (!strstr(err, "exceeds available memory")) {
                return binaryBufferFull();
            }
            return false;
        }
        return true;
    }

    static bool waitForNotecardConnected(uint32_t timeoutMs)
    {
        uint32_t start = millis();
        bool connected = false;
        while (!connected && millis()-start<timeoutMs) {
            if (J *rsp = NoteRequestResponse(NoteNewRequest("hub.status"))) {
                if (!NoteResponseError(rsp)) {
                    connected = JGetBool(rsp, "connected");
                    if (!connected) {
                        delay(10000);
                    }
                } else {
                    delay(2000);
                }
                JDelete(rsp);
            }
        }
        return connected;
    }
};


#ifndef NOT_CONNECTED_TIMEOUT
#define NOT_CONNECTED_TIMEOUT 5*60*1000
#endif


#define MIME_APPLICATION_OCTETSTREAM "application/octet-stream"

/**
 * @brief Helpers needed by file handlers.
 *
 */
class FileHandler
{
protected:
    /**
     * @brief The route alias to the MD5 server. Used to retrieve the chunks sent, the complete transfer MD5/length
     * and to delete the directory containing the chunk(s).
     */
    const char* alias;
    /**
     * @brief When non-null, sets the `content` property of the `web.get` request.
     * This is a workaround for a bug in Notehub that tests for `binary/octet-stream` rather than
     * the IANA-registered `application/octet-stream`.
     */
    const char* chunkContent;
public:

    FileHandler(const char* alias, const char* chunkContent) :
        alias(alias), chunkContent(chunkContent) {}

    bool validateReceivedContent(const NotecardBinary::TransferDetails& tx, const char* name, bool chunked)
    {
        // to verify chunks and the total content, the md5 server must be started with the `--save` flag
        bool success = true;

        if (chunked) {
            // this is also used with `note.add` which may return before Notehub has routed
            // the event. Try for a while.
            bool shouldRetry = true;
            int retries = 10;   // it can take a minute for the note to be sent and routed
            while (shouldRetry && retries-->0) {
                success = validateReceivedChunk(tx, name, &shouldRetry);
                if (!success) {
                    delay(10000);
                }
            }
        }
        if (success && tx.isComplete) {
            success = validateTotalContent(tx, name, chunked);
        }
        if (!success || tx.isComplete) {
            success = success && deleteContent(tx, name);
        }
        return success;
    }

    J* parseWebResponse(size_t length)
    {
        // decode the json response as a troubleshooting aid
        size_t rxSize = NoteBinaryCodecMaxEncodedLength(length);
        uint8_t rxBuf[rxSize];
        uint32_t dataLen = length;
        J* response = nullptr;
        const char* err = NoteBinaryStoreReceive(rxBuf, rxSize, 0, dataLen);
        if (err) {
            notecard.logDebugf("error retrieving payload: %s\n", err);
        } else {
            notecard.logDebug("response content from card.binary: ");
            notecard.logDebug((const char*)rxBuf);
            notecard.logDebug("\n");
            response = JParse((char*)rxBuf);
        }
        return response;
    }

private:
    /**
     * @brief Performs a web.get request and compares the content with the chunk just processed.
     * This doesn't depend on the state of the binary buffer, and is idempotent.
     * @param tx        The transfer details about the current part of the image to verify.
     * @param name      The name of the image as given to the endpoint.
     * @param shouldRetry  [out] whether the error represents a retriable error.
     */
    bool validateReceivedChunk(const NotecardBinary::TransferDetails& tx, const char* name, bool* shouldRetry)
    {
        bool success = false;
        bool retry = false;
        J *req = notecard.newRequest("web.get");
        JAddStringToObject(req, "route", alias);
        if (name) {
            char buf[256];
            buf[0] = 0;
            snprintf(buf, sizeof(buf), "%s?chunk=%d", name, tx.currentTransferIndex);
            JAddStringToObject(req, "name", buf);
        }
        // todo - remove this
        JAddStringToObject(req, "content", "application/octet-stream");
        JAddBoolToObject(req, "binary", true);
        J* rsp = notecard.requestAndResponse(req);
        if (!rsp) {
            notecard.logDebug("NULL response to `web.get`\n");
        } else {
            int result = JGetNumber(rsp, "result");
            J* body = JGetObject(rsp, "body");
            int cobs = JGetNumber(rsp, "cobs");
            int length = JGetNumber(rsp, "length");

            const char* err = JGetString(rsp, "err");
            retry = result==404 || result==503 || result==504;
            if (err && err[0]) {
                notecard.logDebugf("web.get failed with error: %s\n", err);
            } else if (result<200 || result>=300) {
                notecard.logDebugf("web.get result was %d\n", result);
            } else if (!body) {
                notecard.logDebugf("web.get body is not present\n");
            } else if (!cobs) {
                notecard.logDebugf("web.get expected response property 'cobs' is not present\n");
            } else if (!length) {
                notecard.logDebugf("web.get expected response property 'length' is not present\n");
            } else if (length!=int(tx.currentTransferSize)) {
                notecard.logDebugf("web.get length!=chunkTransferSize (%d!=%d)\n", length, tx.currentTransferSize);
            } else {
                // validate the binary buffer against the last chunk sent
                size_t bufferLength = 4097; // just to be nasty ;-)
                uint8_t* buffer = (uint8_t*)malloc(bufferLength);
                if (buffer) {
                    // TODO: NOTE_C_BINARY_RX_ALL doesn't work at present. Using the real size.
                    success = NotecardBinary::validateBinaryReceived(tx, buffer, bufferLength, length);
                    free(buffer);
                } else {
                    notecard.logDebugf("Unable to allocate 4k buffer to verify content\n");
                }
            }
            if (!success && length && length<1024) {
                J* response_content = parseWebResponse(length);
                JDelete(response_content);
            }

            JDelete(rsp);
        }
        *shouldRetry = retry && !success;
        return success;
    }

    /**
     * @brief Validates all chunks sent to the endpoint by retrieving the md5 and length
     * of all chunks sent.
     *
     * @param tx
     * @param name
     * @param chunked Whether there were multiple chunks sent to the endpoint or not.
     */
    bool validateTotalContent(const NotecardBinary::TransferDetails& tx, const char* name, bool chunked)
    {
        // get the total md5 for all chunks
        bool success = false;
        J *req = notecard.newRequest("web.get");
        JAddStringToObject(req, "route", alias);
        if (name) {
            JAddStringToObject(req, "name", name);
        }
        J* rsp = notecard.requestAndResponse(req);
        if (!rsp) {
            notecard.logDebug("NULL response to `web.get`\n");
        } else {
            int result = JGetNumber(rsp, "result");
            J* body = JGetObject(rsp, "body");
            int cobs = JGetNumber(rsp, "cobs");
            const char* err = JGetString(rsp, "err");
            if (err && err[0]) {
                notecard.logDebugf("web.get failed with error: %s\n", err);
            } else if (result<200 || result>=300) {
                notecard.logDebugf("web.get result was %d\n", result);
            } else if (!body) {
                notecard.logDebugf("web.get body is not present\n");
            } else if (cobs) {
                notecard.logDebugf("web.get unexpected response property 'cobs' is present\n");
            } else {
                size_t expectedChunks = chunked ? tx.totalChunks : 1;
                size_t actualLength = JGetNumber(body, "length");
                size_t actualChunks = JGetNumber(body, "chunks");
                const char* actualMD5String = JGetString(body, "md5");
                char expectedMD5String[NOTE_MD5_HASH_STRING_SIZE];
                NoteMD5HashToString((uint8_t*)tx.totalMD5, expectedMD5String, sizeof(expectedMD5String));
                if (!actualMD5String) {
                    actualMD5String = "not a string";
                }
                if (strcmp(actualMD5String, expectedMD5String)) {
                    notecard.logDebugf("web.get MD5 actual!=expected: %s!=%s\n", actualMD5String, expectedMD5String);
                } else if (actualLength!=tx.total) {
                    notecard.logDebugf("web.get total length: actual!=expected: %d!=%d\n", actualLength, tx.total);
                } else if (actualChunks!=expectedChunks) {
                    notecard.logDebugf("web.get total chunks: actual!=expected: %d!=%d\n", actualChunks, expectedChunks);
                } else {
                    notecard.logDebugf("web.get response validated: md5=%s, chunks=%d, length=%d.\n", actualMD5String, actualChunks, actualLength);
                    success = true;
                }
            }
            JDelete(rsp);
        }
        return success;
    }

    /**
     * @brief Perform a `web.delete` request to remove the directory containing the
     * saved files.
     */
    bool deleteContent(const NotecardBinary::TransferDetails& tx, const char* name)
    {
        bool success = false;
        if (J* req = notecard.newRequest("web.delete")) {
            JAddStringToObject(req, "route", alias);
            if (name) {
                JAddStringToObject(req, "name", name);
            }
            J* rsp = notecard.requestAndResponse(req);
            if (rsp) {
                int result = JGetNumber(rsp, "result");
                if (result != 200) {
                    notecard.logDebugf("HTTP status %d trying to delete %s\n", result, name);
                } else {
                    success = true;
                }
                JDelete(rsp);
            } else {
                notecard.logDebugf("NULL response received to `web.delete`.\n");
            }
        } else {
            notecard.logDebugf("Could not create request.\n");
        }
        return success;
    }
};

/**
 * @brief Posts data to the md5 server endpoint and validates the expected length and md5 against
 * the response received. Can operated in chunked mode, or unchunked mode (from the perspective
 * of the endpoint.)
 *
 * In Chunked mode, the endpoint receives each filled card.binary buffer as it arrives.
 * In Unchunked mode, the endpoint receives just one payload containing the entire image.
 *
 * After the data has been sent, a `web.get` request is used to retrieve the total length and md5
 * for the entire image, and the number of chunks received.
 */
class WebPostHandler : public FileHandler
{
    const char* name;
    const char* content;
    bool chunked;   // When true send each chunk directly to the endpoint. When false, have notehub combine the chunks and send all at once.
    bool webPut;

public:
    /**
     * @brief Construct a new Web Post Handler object
     *
     * @param alias
     * @param name
     * @param content
     * @param chunked When true, each completed binary transfer is sent to the server. When false
     *  the transfers are combined and sent to the server in one hit.
     * @param verify  In chunked mode, verify the md5 of all chunks received by the server.
     */
    WebPostHandler(const char* alias, const char* name, const char* content, bool chunked, bool webPut)
        : FileHandler(alias, MIME_APPLICATION_OCTETSTREAM), name(name), content(content), chunked(chunked), webPut(webPut) {}

    /**
     * @brief Sends a web.post request with the binary buffer as content. In chunked mode,
     * each request is sent to the endpoint immediately. In non-chunked mode, the requests
     * are assembled by notehub and sent as a single request to the endpoint.
     *
     * @param tx
     * @return true
     * @return false
     */
    bool handleTransfer(const NotecardBinary::TransferDetails& tx, const NotecardBinary::TransferHandlerContext& ctx)
    {
        bool success = false;

        if (!NotecardBinary::waitForNotecardConnected(NOT_CONNECTED_TIMEOUT)) {
            notecard.logDebug("Cannot perform web.* request, Notecard not connected.");
            return false;
        }

        if (tx.currentTransferSize==tx.total) {
            chunked = false;    // only one chunk equal to the total
        }

        // in "chunked" mode, each chunk is sent to the server (no offset/total in web.post).
        // The expected MD5 is the current transfer MD5
        // in "non-chunked" mode, only the last request (when tx.isComplete is true) sends data to the route
        // No response is expected.
        const uint8_t* expectedMD5 = chunked ? tx.currentTransferMD5 : tx.totalMD5;
        const size_t expectedLength = chunked ? tx.currentTransferSize : tx.total;

        if (J *req = notecard.newRequest(webPut ? "web.put" : "web.post")) {
            JAddStringToObject(req, "route", alias);
            if (name) {
                char buf[256];
                buf[0] = 0;
                strncat(buf, name, sizeof(buf)-1);
                if (chunked) {
                    snprintf(buf, sizeof(buf), "%s?chunk=%d", name, tx.currentTransferIndex);
                }
                JAddStringToObject(req, "name", buf);
            }
            if (content) {
                JAddStringToObject(req, "content", content);
            }
            JAddBoolToObject(req, "binary", true);
            JAddBoolToObject(req, "verify", true);
            if (!chunked) { // have notehub concatenate all the chunks
                JAddNumberToObject(req, "offset", tx.transferred);
                JAddNumberToObject(req, "total", tx.total);
            }
            J* rsp = notecard.requestAndResponse(req);
            if (rsp) {
                int result = JGetNumber(rsp, "result");
                J* body = JGetObject(rsp, "body");
                int cobs = JGetNumber(rsp, "cobs");
                int length = JGetNumber(rsp, "length");
                J* response_content = nullptr;
                const char* err = nullptr;
                if (cobs && length) {
                    response_content = parseWebResponse(length);
                }

                err = JGetString(rsp, "err");
                if (err && err[0]) {
                    notecard.logDebugf("web.post failed with error: %s\n", err);
                }
                // Notehub returns 100: Continue for all web.post with offset apart from the last
                else if (!chunked && !tx.isComplete && result!=100) {
                    notecard.logDebugf("expected web.post result of 100 for all fragments apart from the last, but was %d\n", result);
                } else if ((chunked || tx.isComplete) && (result!=200)) {
                    notecard.logDebugf("web.post result of 200 expected, but was %d\n", result);
                }
                // only get a "real" response from the endpoint when chunking, or when the final fragment is sent to notehub
                else if (!chunked && !tx.isComplete) {
                    success = true;
                } else {
                    if (!body) {
                        notecard.logDebugf("web.post body is not present\n");
                    } else if (!cobs) {
                        notecard.logDebugf("web.post response 'cobs' not present\n");
                    } else if (!length) {
                        notecard.logDebugf("web.post response 'length' not present\n");
                    } else {
                        char expectedMD5String[NOTE_MD5_HASH_STRING_SIZE];
                        // need to tighten up the note-c APIs with respect to const
                        NoteMD5HashToString((uint8_t*)expectedMD5, expectedMD5String, sizeof(expectedMD5String));
                        const char* receivedMD5string = JGetString(response_content, "md5");
                        const char* contentType = JGetString(response_content, "Content-Type");
                        size_t contentLength = JGetNumber(response_content, "length");

                        if (!receivedMD5string) {
                            receivedMD5string = "not a string";
                        }
                        if (strcmp(receivedMD5string, expectedMD5String)) {
                            notecard.logDebugf("web.post MD5 actual!=expected: %s!=%s\n", receivedMD5string, expectedMD5String);
                        } else if (content && (!contentType || strcmp(contentType, content))) {
                            notecard.logDebugf("web.post Content-Type actual!=expected: %s!=%s\n", contentType, content);
                        } else if (!contentLength || (contentLength!=expectedLength)) {
                            notecard.logDebugf("web.post Content-Length actual!=expected: %s!=%s\n", contentLength, expectedLength);
                        } else {
                            notecard.logDebug("web.post response validated.\n");
                            success = true;
                        }
                    }
                }
                JDelete(response_content);
                JDelete(rsp);
            }

            if (success) {
                success = validateReceivedContent(tx, name, chunked);
            }
        }
        return success;
    }

    // Turns a reference to a member function into a regular std::function by binding the 'this' argument.
    NotecardBinary::transfer_cb_t transfer_callback()
    {
        using namespace std::placeholders;
        return std::bind(&WebPostHandler::handleTransfer, this, _1, _2);
    }
};

class NoteAddHandler : public FileHandler
{
    const char* name;
public:
    /**
     * @brief Construct a new NoteAdd Handler object
     * @param alias Alias of the route that directs to the MD5 server.
     * @param name  The name of the file being transferred.
     */
    NoteAddHandler(const char* alias, const char* name)
        : FileHandler(alias, MIME_APPLICATION_OCTETSTREAM), name(name) {}

    /**
     * @brief Sends a note.add request with the binary buffer as the payload.
     *
     * @param tx
     * @return true
     * @return false
     */
    bool handleTransfer(const NotecardBinary::TransferDetails& tx, NotecardBinary::TransferHandlerContext& ctx)
    {
        bool success = false;
        bool chunked = (tx.currentTransferSize!=tx.total);   // only one chunk equal to the total

        if (!NotecardBinary::waitForNotecardConnected(NOT_CONNECTED_TIMEOUT)) {
            notecard.logDebug("Cannot perform note.add request, Notecard not connected.");
            return false;
        }

        if (J* req = notecard.newRequest("note.add")) {
            JAddStringToObject(req, "file", "cardbinary.qo");
            JAddBoolToObject(req, "binary", true);
            JAddBoolToObject(req, "live", true);
            JAddBoolToObject(req, "sync", true);
            J* body = JAddObjectToObject(req, "body");
            JAddStringToObject(body, "name", name);

            char md5String[NOTE_MD5_HASH_STRING_SIZE];
            NoteMD5HashToString((uint8_t*)tx.currentTransferMD5, md5String, sizeof(md5String));

            JAddStringToObject(body, "md5", md5String);
            JAddNumberToObject(body, "length", tx.currentTransferImage.length());
            JAddNumberToObject(body, "offset", tx.transferred);
            JAddNumberToObject(body, "total", tx.total);
            if (chunked) {
                JAddNumberToObject(body, "chunk", tx.currentTransferIndex);
            }
            J* rsp = notecard.requestAndResponse(req);
            if (rsp) {
                const char* err = JGetString(rsp, "err");
                if (err && err[0]) {
                    notecard.logDebugf("note.add error: %s\n", err);
                } else {
                    success = true;
                }
                JDelete(rsp);
            } else {
                notecard.logDebugf("no response from Notecard\n");
            }
        } else {
            notecard.logDebugf("Could not allocate request\n");
        }

        if (success) {
            success = validateReceivedContent(tx, name, chunked);
            // todo - check the card.binary size. If it has changed, we cannot retry
            // since the buffer is cleared. Retry will have to happen at a higher level,
            // resending the buffer.
            ctx.tries = -1; // indicate this can't be retried.
        }
        return success;
    }

    NotecardBinary::transfer_cb_t transfer_callback()
    {
        using namespace std::placeholders;
        return std::bind(&NoteAddHandler::handleTransfer, this, _1, _2);
    }
};

extern BinaryImage small_binary;
extern BufferBinaryGenerator small_image;