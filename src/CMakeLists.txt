# Core library
add_subdirectory(lib)

# CMake Packaging

include(CMakePackageConfigHelpers)

set(PACKAGE_NAME    RestGate)
set(PACKAGE_VERSION ${PROJECT_VERSION})
set(PACKAGE_ROOT    .)

configure_package_config_file(${PACKAGE_NAME}Config.cmake.in ${PACKAGE_NAME}Config.cmake
    PATH_VARS PACKAGE_ROOT
    INSTALL_DESTINATION lib/cmake/${PACKAGE_NAME}
)

write_basic_package_version_file(${PACKAGE_NAME}ConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
    ARCH_INDEPENDENT
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake
    DESTINATION lib/cmake/${PACKAGE_NAME}
)

install(EXPORT RestGateTargets
    NAMESPACE ${PACKAGE_NAME}::
    FILE ${PACKAGE_NAME}Targets.cmake
    DESTINATION lib/cmake/${PACKAGE_NAME}
)
