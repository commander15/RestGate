qt_add_library(RestGate)

set_target_properties(RestGate PROPERTIES
    OUTPUT_NAME RestGate
    VERSION     ${PROJECT_VERSION}
)

configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h @ONLY)

target_headers(RestGate
    PUBLIC
        restgate.h
        global.h config.h
        laravelloginmanager.h laraveldatamanager.h
    PRIVATE
        debug_p.h
)

target_sources(RestGate
    PRIVATE
        debug.cpp
        laravelloginmanager.cpp laraveldatamanager.cpp
)

target_include_directories(RestGate 
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<INSTALL_INTERFACE:include/RestGate>
)

target_compile_definitions(RestGate
    PUBLIC
        $<IF:$<STREQUAL:$<TARGET_PROPERTY:TYPE>,STATIC_LIBRARY>,RESTGATE_STATIC,RESTGATE_SHARED>
    INTERFACE
        RESTGATE_LIB
    PRIVATE
        RESTGATE_BUILD
)

target_link_libraries(RestGate PUBLIC Qt::Core Qt::Network DataGate::DataGate RestLink::RestLink)

install(TARGETS RestGate
    EXPORT RestGateTargets
    ARCHIVE        DESTINATION lib
    LIBRARY        DESTINATION lib
    RUNTIME        DESTINATION bin
    PUBLIC_HEADER  DESTINATION include/RestGate
    #PRIVATE_HEADER DESTINATION include/RestGate/private
)

generate_target_headers(RestGate FOLDER RestGate)
