cmake_minimum_required(VERSION 3.15)
project(DisableLGhubAutoUpdate
    VERSION 0.1.0
    DESCRIPTION "Disable LG Hub auto-update"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME}
    Main.cpp
    src/AddLGFWRule.cpp
    src/ChangeLGServices.cpp
    src/GlobalErrorCode.cpp
    src/LGServices.cpp
    src/LGhubJson.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:_DEBUG;_CONSOLE>
    $<$<CONFIG:Release>:NDEBUG;_CONSOLE>
    UNICODE
    _UNICODE
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /utf-8)

    target_link_options(${PROJECT_NAME} PRIVATE
        /MANIFESTUAC:level='requireAdministrator'
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    comsuppw
    ole32
    oleaut32
    advapi32
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/Release/$<CONFIG>"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_SOURCE_DIR}/Debug"
)