string(TOUPPER ${PROJECT_NAME} PROJECT_MACRO)
string(TOLOWER ${PROJECT_NAME} PROJECT_MICRO)

# Finding doxygen
find_program(DOXYGEN_EXECUTABLE doxygen)

# Finding qhelpgenerator
file(REAL_PATH "${Qt6_DIR}/../../../" QtRootDir)

configure_file(Doxyfile.in Doxyfile @ONLY)

find_program(QHG_EXECUTABLE qhelpgenerator PATHS ${QtRootDir}/bin ${QtRootDir}/libexec)

if (DOXYGEN_EXECUTABLE)
    add_custom_target(docs ALL
        ${DOXYGEN_EXECUTABLE} "Doxyfile"
        SOURCES Doxyfile.in
    )
endif()
