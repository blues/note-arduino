# Copyright 2023 Blues Inc.  All rights reserved.
# Use of this source code is governed by licenses granted by the
# copyright holder including that found in the LICENSE file.

# Notestation Development Environment
# Extends the CI image with pre-installed Arduino board platforms for faster development

# Define global arguments
ARG USER="blues"

# Use the CI image as base
FROM ghcr.io/blues/note_arduino_ci:latest
# FROM note_arduino_ci:latest

# Import global arguments
ARG USER="blues"

# Install all Arduino board platforms for Notestation development
# This provides a complete development environment without needing to install platforms on-the-fly
USER ${USER}
RUN ["dash", "-c", "\
    arduino-cli core install adafruit:nrf52 \
 && arduino-cli core install adafruit:samd \
 && arduino-cli core install arduino:avr \
 && arduino-cli core install arduino:mbed_nano \
 && arduino-cli core install arduino:samd \
 && arduino-cli core install esp32:esp32 \
 && arduino-cli core install rp2040:rp2040 \
 && arduino-cli core install SparkFun:apollo3 \
 && arduino-cli core install STMicroelectronics:stm32 \
"]
