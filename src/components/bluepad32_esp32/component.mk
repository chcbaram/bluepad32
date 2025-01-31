#
# Main component makefile.
#
# This Makefile can be left empty. By default, it will take the sources in the
# src/ directory, compile them and link them into lib(subdirectory_name).a
# in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#

$(call compile_only_if,$(CONFIG_BLUEPAD32_PLATFORM_ESP32), Esp32Bluepad32.o Esp32Console.o Esp32Gamepad.o)
