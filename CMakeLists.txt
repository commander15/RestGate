cmake_minimum_required(VERSION 3.30)

project(RestGate
    DESCRIPTION "Bridge between DataGate and RestLink"
    VERSION     "1.1.0"
    LANGUAGES   CXX C
)

# Project settings

option(RESTGATE_BUILD_TESTS        "Build RestGate tests" OFF)
option(RESTGATE_BUILD_DOC          "Build RestGate documentation" OFF)
option(RESTGATE_BUILD_TRANSLATIONS "Build RestGate translations" OFF)
option(RESTGATE_BUILD_EXAMPLES     "Build RestGate Examples" OFF)

set(RESTGATE_LOCALES en fr)

# CMake options
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# CXX Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# C Standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)

# Registering project module path
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Including utility scripts
include(cmake/Utils.cmake)

# Qt setup

set(CMAKE_AUTOMOC ON)

set(QT_MODULES Core Network)

if (RESTGATE_BUILD_TESTS)
    list(APPEND QT_MODULES Test)
endif()

if (RESTGATE_BUILD_TRANSLATIONS)
    list(APPEND QT_MODULES LinguistTools)
endif()

find_package(Qt6 REQUIRED COMPONENTS ${QT_MODULES})

qt_standard_project_setup(
    REQUIRES 6.8.0
    I18N_SOURCE_LANGUAGE      en
    I18N_TRANSLATED_LANGUAGES ${RESTGATE_LOCALES}
)

qt_policy(SET QTP0002 NEW)
qt_policy(SET QTP0003 NEW)

# Other dependencies
include(cmake/Dependencies.cmake)

# Project directories

add_subdirectory(src)

if (RESTGATE_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if (RESTGATE_BUILD_DOC)
    add_subdirectory(doc)
endif()

if (RESTGATE_BUILD_TRANSLATIONS)
    qt_collect_translation_source_targets(SOURCE_TARGETS)

    qt_add_translations(RestGate SOURCE_TARGETS ${SOURCE_TARGETS}
        TS_FILE_BASE restgate
        TS_FILE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/translations
        QM_FILES_OUTPUT_VARIABLE QM_FILES
    )
    
    # Ensure the build directory has a translations folder
    set(TRANSLATIONS_BUILD_DIR "${CMAKE_BINARY_DIR}/translations")
    file(MAKE_DIRECTORY ${TRANSLATIONS_BUILD_DIR})

    # Copy QM files to build directory after they are generated
    add_custom_command(
        TARGET RestGate_lrelease POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QM_FILES} ${TRANSLATIONS_BUILD_DIR}
        COMMENT "Copying generated QM files to ${TRANSLATIONS_BUILD_DIR}"
    )

    install(FILES ${QM_FILES} DESTINATION translations)
endif()

if (RESTGATE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
