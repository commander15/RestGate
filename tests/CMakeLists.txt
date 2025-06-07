enable_testing()

find_package(GTest REQUIRED)

add_executable(RestGateTest
    main.cpp
)

target_compile_definitions(RestGateTest PRIVATE TEST_DATA_DIR="${PROJECT_SOURCE_DIR}/testdata")

target_link_libraries(RestGateTest PRIVATE GTest::gtest Qt::Test RestGate)
