cmake_minimum_required(VERSION 3.20)

project(stud_sys VERSION 1.0.0 LANGUAGES C)

include(CTest)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(STUDSYS_LANG "PL" CACHE STRING "TUI language: PL or EN")
set_property(CACHE STUDSYS_LANG PROPERTY STRINGS PL EN)
option(STUDSYS_MQTT "Enable MQTT support through Eclipse Paho MQTT C" OFF)
option(STUDSYS_WERROR "Treat compiler warnings as errors" OFF)

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
    $<$<C_COMPILER_ID:GNU,Clang,AppleClang>:-Wextra>
)
if(STUDSYS_WERROR)
    target_compile_options(studsys_build_options INTERFACE
        $<$<C_COMPILER_ID:GNU,Clang,AppleClang>:-Werror>
    )
endif()
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
    scheduler.c
    zlecenia.c
    konfig.c
    blank/term_ncurses.c
    blank/wewy.c
    blank/wewy_pola.c
    blank/wewy_raporty.c
    blank/wewy_dane.c
    blank/wewy_blankiet.c
    blank/wewy_liczby.c
    budz.c
    ui_wpis.c
    admin.c
    sys_rap.c
    sprawdz.c
    rap_bl.c
    randf.c
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
    target_include_directories(test_scheduler PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    target_link_libraries(test_scheduler PRIVATE studsys_build_options)
    add_test(NAME scheduler COMMAND test_scheduler)

    add_executable(test_terminal
        tests/test_terminal.c
        $<TARGET_OBJECTS:studsys_common>
        $<TARGET_OBJECTS:studsys_test_core>
    )
    target_link_libraries(test_terminal PRIVATE studsys_build_options)
    add_test(NAME terminal COMMAND test_terminal)

    # Test modulu, ktory wlacza testowany plik .c (dostep do funkcji static):
    # linkowany ze wszystkimi modulami oprocz tego pliku.
    function(studsys_module_test name test_src module_src)
        set(sources ${STUDSYS_COMMON_SOURCES})
        list(REMOVE_ITEM sources ${module_src})
        add_executable(${name} ${test_src} ${sources} $<TARGET_OBJECTS:studsys_test_core>)
        target_include_directories(${name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tests")
        target_link_libraries(${name} PRIVATE studsys_build_options)
        add_test(NAME ${name} COMMAND ${name})
    endfunction()
    studsys_module_test(test_budz tests/test_budz.c budz.c)
    studsys_module_test(test_mqtt tests/test_mqtt.c mqtt_pub.c)

    add_executable(test_wewy
        tests/test_wewy.c
        $<TARGET_OBJECTS:studsys_common>
        $<TARGET_OBJECTS:studsys_test_core>
    )
    target_include_directories(test_wewy PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    target_link_libraries(test_wewy PRIVATE studsys_build_options)
    add_test(NAME test_wewy COMMAND test_wewy)

    # Smoke test UI: program w pseudoterminalu, ekran przez pyte (Python).
    # STUDSYS_UI_TESTS=AUTO - gdy jest Python z pyte; ON - wymagany (CI).
    set(STUDSYS_UI_TESTS "AUTO" CACHE STRING "UI smoke test (pty + pyte): AUTO, ON or OFF")
    set_property(CACHE STUDSYS_UI_TESTS PROPERTY STRINGS AUTO ON OFF)
    if(NOT STUDSYS_UI_TESTS STREQUAL "OFF")
        find_package(Python3 COMPONENTS Interpreter)
        set(studsys_pyte_ok FALSE)
        if(Python3_Interpreter_FOUND)
            execute_process(COMMAND ${Python3_EXECUTABLE} -c "import pyte"
                            RESULT_VARIABLE studsys_pyte_rc OUTPUT_QUIET ERROR_QUIET)
            if(studsys_pyte_rc EQUAL 0)
                set(studsys_pyte_ok TRUE)
            endif()
        endif()
        if(studsys_pyte_ok)
            add_test(NAME ui_smoke
                     COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/tests/ui_smoke.py"
                             $<TARGET_FILE:stud_sys> ${STUDSYS_LANG_NORMALIZED})
            set_tests_properties(ui_smoke PROPERTIES TIMEOUT 120)

            # pytanie <t/n> silnika formularzy (potwierdz_tn) w pty
            add_executable(tn_probe
                tests/tn_probe.c
                $<TARGET_OBJECTS:studsys_common>
                $<TARGET_OBJECTS:studsys_test_core>
            )
            target_link_libraries(tn_probe PRIVATE studsys_build_options)
            add_test(NAME ui_tn
                     COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/tests/ui_tn.py"
                             $<TARGET_FILE:tn_probe>)
            set_tests_properties(ui_tn PROPERTIES TIMEOUT 60)
        elseif(STUDSYS_UI_TESTS STREQUAL "ON")
            message(FATAL_ERROR "STUDSYS_UI_TESTS=ON requires Python 3 with pyte (pip install pyte)")
        else()
            message(STATUS "UI smoke test skipped: Python 3 with pyte not found")
        endif()
    endif()
endif()
