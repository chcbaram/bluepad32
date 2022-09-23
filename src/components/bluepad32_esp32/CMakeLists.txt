# See: https://gitlab.com/ricardoquesada/bluepad32/-/issues/9

set(srcs "Esp32Bluepad32.cpp"
        "Esp32Console.cpp"
        "Esp32Gamepad.cpp")

set(requires "bluepad32")

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "include"
                    REQUIRES ${requires})
