set(EXECUTABLE_OUTPUT_PATH ${UNITTEST_OUTPUT_PATH}/AST/)

file(GLOB src *.cpp)

add_executable(PlanUnitTest ${src})

target_link_libraries(PlanUnitTest liboss xml2 gtest pthread)
add_dependencies(PlanUnitTest liboss xml2 gtest pthread)

include_directories(../../)
include_directories(../../../OSS)
include_directories(../../../common)
include_directories(../../../FsCommon)
include_directories(../../../Template)
include_directories(../../../Communication)
