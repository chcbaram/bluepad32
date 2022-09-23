set(srcs 
"main.c"
"esp32_main.cpp"
)

set(requires "bluepad32")

list(APPEND requires "bluepad32_esp32")


idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "."
                    REQUIRES "${requires}")
