# Distributed under the MIT software license, see the accompanying
# file LICENSE or http://www.opensource.org/licenses/mit-license.php.

cmake_minimum_required(VERSION 3.5)
project(qryptonight)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_ERROR_DEPRECATED FALSE)
set(CMAKE_WARN_DEPRECATED FALSE)

set(BUILD_TESTS ON CACHE BOOL "Enables tests")
set(BUILD_PYTHON ON CACHE BOOL "Enables python wrapper")
set(BUILD_WEBASSEMBLY OFF CACHE BOOL "Enables emscripten build")

set(SANITIZE_ADDRESS OFF  CACHE BOOL "Enables address sanitizer")
set(SANITIZE_THREAD  OFF CACHE BOOL "Enables thread sanitizer")
set(SANITIZE_UNDEFINED  OFF CACHE BOOL "Enables undefined sanitizer")

message(STATUS "BUILD_TESTS    " ${BUILD_TESTS})
message(STATUS "PYTHON WRAPPER " ${BUILD_PYTHON})
message(STATUS "WEBASSEMBLY    " ${BUILD_WEBASSEMBLY})

if(WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -DWIN32_LEAN_AND_MEAN)
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3")
endif()

if (BUILD_PYTHON)
    find_package(SWIG REQUIRED)
    INCLUDE(${SWIG_USE_FILE})
    unset(SWIG_LANG_TYPE)
endif ()

if (SANITIZE_ADDRESS)
    message(STATUS Sanitizer: Address)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
    set (CMAKE_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
endif()

if (SANITIZE_THREAD)
    message(STATUS Sanitizer: Thread)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
    set (CMAKE_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fsanitize=thread")
endif()

if (SANITIZE_UNDEFINED)
    message(STATUS Sanitizer: Undefined)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
    set (CMAKE_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fsanitize=undefined")
endif()

FIND_PACKAGE(Boost 1.58 REQUIRED)

################################################################################
# Find hwloc
################################################################################
option(HWLOC_ENABLE "Enable or disable the requirement of hwloc" ON)
if(HWLOC_ENABLE)
    find_path(HWLOC_INCLUDE_DIR
            NAMES
            hwloc.h
            PATHS
            /opt/local
            /usr/local
            /usr
            ENV "PROGRAMFILES(X86)"
            ENV "HWLOC_ROOT"
            PATH_SUFFIXES
            include)

    find_library(HWLOC
            NAMES
            libhwloc.lib
            hwloc
            PATHS
            ENV "HWLOC_ROOT"
            PATH_SUFFIXES
            lib)

    if(HWLOC STREQUAL "HWLOC-NOTFOUND" OR ${HWLOC_INCLUDE_DIR} STREQUAL "HWLOC_INCLUDE_DIR-NOTFOUND")
        message(FATAL_ERROR "hwloc NOT found: use `-DHWLOC_ENABLE=OFF` to build without hwloc support")
    else()
        set(REF_CRYPTONIGHT_LIBS ${REF_CRYPTONIGHT_LIBS} ${HWLOC})
        include_directories(AFTER ${HWLOC_INCLUDE_DIR})
    endif()
else()
    add_definitions("-DCONF_NO_HWLOC")
endif()

if(WIN32 AND HWLOC_ENABLE)
    execute_process(COMMAND dumpbin.exe /SYMBOLS /NOLOGO
        ${HWLOC}
        OUTPUT_VARIABLE HWLOC_SYMS
        RESULT_VARIABLE DUMPBIN_RESULT)

        if(DUMPBIN_RESULT EQUAL 0)
            string(REGEX MATCH "__IMPORT_DESCRIPTOR_[^\n]+" HWLOC_DLLNAME ${HWLOC_SYMS})
            string(REGEX REPLACE "__IMPORT_DESCRIPTOR_(.*)" "\\1.dll" HWLOC_DLLNAME ${HWLOC_DLLNAME})
            get_filename_component(HWLOC_ROOT ${HWLOC} DIRECTORY)
            get_filename_component(HWLOC_DLL "${HWLOC_ROOT}/../bin/${HWLOC_DLLNAME}" ABSOLUTE)
        endif()
endif()

if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "-march=native -mtune=native ${CMAKE_CXX_FLAGS} -msse2 -maes")
    set(CMAKE_C_FLAGS "-march=native -mtune=native -fPIC ${CMAKE_C_FLAGS} -msse2 -maes")
endif()

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src/api
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/xmr-stak
)

file(GLOB LIB_QRYPTONIGHT_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/qryptonight
        )

file(GLOB_RECURSE LIB_QRYPTONIGHT_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/misc/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/pow/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/qryptonight/*.cpp"
        )

file(GLOB_RECURSE TEST_QRYPTONIGHT_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/cpp/*.cpp")

file(GLOB REF_CRYPTONIGHT_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/xmr-stak/xmrstak/backend/cpu/crypto/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/xmr-stak/xmrstak/backend/cpu/hwlocMemory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/xmr-stak/xmrstak/jconf.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/xmr-stak/xmrstak/misc/console.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/xmr-stak/xmrstak/misc/utility.cpp
        )

file(GLOB REF_CRYPTONIGHT_C_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/deps/xmr-stak/xmrstak/backend/cpu/crypto/*.c )

SET_SOURCE_FILES_PROPERTIES(${LIB_QRYPTONIGHT_SRC} PROPERTIES LANGUAGE CXX)
SET_SOURCE_FILES_PROPERTIES(${TEST_QRYPTONIGHT_SRC} PROPERTIES LANGUAGE CXX)

# Compile C files as a static lib
add_library(cryptonight-c-lib STATIC
        ${REF_CRYPTONIGHT_C_SRC}
        )
set_property(TARGET cryptonight-c-lib PROPERTY C_STANDARD 99)

if (BUILD_TESTS)
    message(STATUS "GTests enabled")

    ##############################
    # Google Test
    # Based on instructions in https://github.com/google/googletest/tree/master/googletest#incorporating-into-an-existing-cmake-project
    # Download and unpack googletest at configure time
    configure_file(CMakeLists.txt.gtest.in googletest-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)
    if (result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif ()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)
    if (result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif ()

    # Prevent overriding the parent project's compiler/linker settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Add googletest directly to our build. This defines
    # the gtest and gtest_main targets.
    add_subdirectory(
            ${CMAKE_BINARY_DIR}/googletest-src
            ${CMAKE_BINARY_DIR}/googletest-build
    )

    # The gtest/gtest_main targets carry header search path
    # dependencies automatically when using CMake 2.8.11 or
    # later. Otherwise we have to add them here ourselves.
    if (CMAKE_VERSION VERSION_LESS 2.8.11)
        include_directories("${gtest_SOURCE_DIR}/include")
    endif ()

    ###########################
    include(CTest)

    add_executable(qryptonight_test
            ${TEST_QRYPTONIGHT_SRC}
            ${LIB_QRYPTONIGHT_SRC}
            ${REF_CRYPTONIGHT_SRC}
            )

    target_include_directories( qryptonight_test PRIVATE
        ${LIB_QRYPTONIGHT_INCLUDES} ${Boost_INCLUDE_DIRS})

    target_link_libraries(qryptonight_test
            gtest_main
            ${REF_CRYPTONIGHT_LIBS}
            cryptonight-c-lib
            )

    if(WIN32)
        target_link_libraries(qryptonight_test wsock32 ws2_32)
    endif()

    add_test(gtest ${PROJECT_BINARY_DIR}/qryptonight_test)

endif ()

## SWIG + API - Python related stuff
if (BUILD_PYTHON)
    message(STATUS "Python wrapper enabled")

    if (NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    endif ()

    set(language python)
    find_package(PythonLibs 3.4 REQUIRED)
    include_directories(
          ${PYTHON_INCLUDE_PATH}
    )
    set(CMAKE_SWIG_OUTDIR ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/pyqryptonight)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/pyqryptonight)

    set(SWIG_INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/api/qryptonight.i)
    set(SWIG_LANG_LIBRARIES ${PYTHON_LIBRARIES})
    set_source_files_properties(${SWIG_INTERFACE} PROPERTIES CPLUSPLUS ON)
    set_property(SOURCE ${SWIG_INTERFACE} PROPERTY SWIG_FLAGS "-includeall" "-ignoremissing")
    message(STATUS "CMAKE_SWIG_OUTDIR: " ${CMAKE_SWIG_OUTDIR})
    message(STATUS "CMAKE_LIBRARY_OUTPUT_DIRECTORY: " ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

    # Enable threading in the wrapper (useful for GIL-safe callbacks)
    set( CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS} "-threads" )

    # Intentionally use a deprecated version to provide support for the raspberry pi
    SWIG_ADD_MODULE(pyqryptonight
            ${language}
            ${SWIG_INTERFACE}
            ${LIB_QRYPTONIGHT_SRC}
            ${REF_CRYPTONIGHT_SRC}
            )

    SWIG_LINK_LIBRARIES(pyqryptonight
            ${SWIG_LANG_LIBRARIES}
            ${REF_CRYPTONIGHT_LIBS}
            cryptonight-c-lib)

    if(WIN32)
        swig_link_libraries(pyqryptonight wsock32 ws2_32)
    endif()

    include_directories(
            ${PYTHON_INCLUDE_PATH}
            ${LIB_QRYPTONIGHT_INCLUDES}
            ${Boost_INCLUDE_DIRS}
    )

    set_target_properties(${SWIG_MODULE_pyqryptonight_REAL_NAME} PROPERTIES DEBUG_POSTFIX "_d")

    if(WIN32 AND HWLOC_ENABLE)
        add_custom_command(TARGET ${SWIG_MODULE_pyqryptonight_REAL_NAME}
            POST_BUILD
            COMMENT "Copying HWLOC DLL to package folder"
            COMMAND ${CMAKE_COMMAND} -E
              copy_if_different "${HWLOC_DLL}" "${CMAKE_CURRENT_SOURCE_DIR}/pyqryptonight/")
    endif()

    add_custom_command(TARGET ${SWIG_MODULE_pyqryptonight_REAL_NAME}
            POST_BUILD
            COMMENT "Moving SWIG files to output dir"
            COMMAND ${CMAKE_COMMAND} -E
            copy_if_different $<TARGET_FILE:${SWIG_MODULE_pyqryptonight_REAL_NAME}>
            ${CMAKE_CURRENT_SOURCE_DIR}/pyqryptonight/$<TARGET_LINKER_FILE_NAME:${SWIG_MODULE_pyqryptonight_REAL_NAME}>
            )

    add_custom_command(TARGET ${SWIG_MODULE_pyqryptonight_REAL_NAME}
            POST_BUILD
            COMMENT ${swig_extra_generated_files}
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${swig_extra_generated_files} ${CMAKE_CURRENT_SOURCE_DIR}/pyqryptonight/pyqryptonight.py
            )

endif ()

if (BUILD_WEBASSEMBLY)
    set(JS_QRL_INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/jswrapper/jsqrlwrapper.cpp)
    set(JS_DILITHIUM_INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/jswrapper/jsdilwrapper.cpp)
    set(JS_KYBINTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/jswrapper/jskybwrapper.cpp)

    message(STATUS "webassembly enabled")
    add_library(jsqryptonight SHARED
            ${JS_QRL_INTERFACE}
            ${LIB_QRYPTONIGHT_SRC}
            ${REF_CRYPTONIGHT_SRC}
            )
    target_include_directories( jsqryptonight PRIVATE
            ${LIB_QRL_INCLUDES} )

endif ()
