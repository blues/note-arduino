#include "NotecardBinary.h"

Notecard notecard;


BinaryImage small_binary = { .name = "small", .length=small_img_len, .data=small_img_map };


BufferBinaryGenerator small_image(small_binary);


// ensure the virtual destructor is defined.
BinaryGenerator::~BinaryGenerator() {}


