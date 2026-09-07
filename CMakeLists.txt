cmake_minimum_required(VERSION 3.20)

project(stud_sys VERSION 1.0.0 LANGUAGES C)

include(CTest)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(STUDSYS_LANG "PL" CACHE STRING "TUI language: PL or EN")
set_property(CACHE STUDSYS_LANG PROPERTY STRINGS PL EN)
option(STUDSYS_MQTT "Enable MQTT support through Eclipse Paho MQTT C" OFF)

string(TOUPPER "${STUDSYS_LANG}" STUDSYS_LANG_NORMALIZED)
if(NOT STUDSYS_LANG_NORMALIZED MATCHES "^(PL|EN)$")
    message(FATAL_ERROR "STUDSYS_LANG must be PL or EN (got: ${STUDSYS_LANG})")
endif()

set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)

add_library(studsys_build_options INTERFACE)
target_include_directories(studsys_build_options INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/blank"
    "${CMAKE_CURRENT_SOURCE_DIR}/opcjesys"
    ${CURSES_INCLUDE_DIRS}
)
target_compile_definitions(studsys_build_options INTERFACE
    _NCURSES_
    "LANG_${STUDSYS_LANG_NORMALIZED}"
)
target_compile_options(studsys_build_options INTERFACE
    $<$<C_COMPILER_ID:GNU,Clang,AppleClang>:-Wall>
    $<$<C_COMPILER_ID:GNU,Clang,AppleClang>:-Wno-format>
    $<$<C_COMPILER_ID:GNU,Clang,AppleClang>:-Wno-pointer-sign>
    $<$<C_COMPILER_ID:GNU,Clang,AppleClang>:-Wno-comment>
)
target_link_libraries(studsys_build_options INTERFACE ${CURSES_LIBRARIES})
if(UNIX)
    target_link_libraries(studsys_build_options INTERFACE m)
endif()

if(STUDSYS_MQTT)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(PAHO_MQTT QUIET IMPORTED_TARGET paho-mqtt3c)
    endif()

    if(TARGET PkgConfig::PAHO_MQTT)
        target_link_libraries(studsys_build_options INTERFACE PkgConfig::PAHO_MQTT)
    else()
        find_path(PAHO_MQTT_INCLUDE_DIR MQTTClient.h)
        find_library(PAHO_MQTT_LIBRARY NAMES paho-mqtt3c)
        if(NOT PAHO_MQTT_INCLUDE_DIR OR NOT PAHO_MQTT_LIBRARY)
            message(FATAL_ERROR
                "STUDSYS_MQTT=ON requires Eclipse Paho MQTT C (paho-mqtt3c)")
        endif()
        target_include_directories(studsys_build_options INTERFACE
            "${PAHO_MQTT_INCLUDE_DIR}"
        )
        target_link_libraries(studsys_build_options INTERFACE
            "${PAHO_MQTT_LIBRARY}"
        )
    endif()
    target_compile_definitions(studsys_build_options INTERFACE _MQTT_)
endif()

set(STUDSYS_COMMON_SOURCES
    konfig.c
    blank/term_ncurses.c
    blank/konsola.c
    blank/wewy.c
    budz.c
    admin.c
    sys_rap.c
    sprawdz.c
    mqtt_pub.c
    sensor_sim.c
)

add_library(studsys_common OBJECT ${STUDSYS_COMMON_SOURCES})
target_link_libraries(studsys_common PRIVATE studsys_build_options)

add_executable(stud_sys core.c $<TARGET_OBJECTS:studsys_common>)
target_link_libraries(stud_sys PRIVATE studsys_build_options)

if(BUILD_TESTING)
    add_library(studsys_test_core OBJECT core.c)
    target_compile_definitions(studsys_test_core PRIVATE main=stud_sys_app_main)
    target_link_libraries(studsys_test_core PRIVATE studsys_build_options)

    add_executable(test_scheduler
        tests/test_scheduler.c
        $<TARGET_OBJECTS:studsys_common>
        $<TARGET_OBJECTS:studsys_test_core>
    )
    target_link_libraries(test_scheduler PRIVATE studsys_build_options)
    add_test(NAME scheduler COMMAND test_scheduler)
endif()
