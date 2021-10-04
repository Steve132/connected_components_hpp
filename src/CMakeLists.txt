
add_library(connected_components
    disjoint_set.hpp
    fill_segments.cpp
    fill_segments.hpp
)
target_include_directories(connected_components PRIVATE deps)
target_compile_features(connected_components PRIVATE cxx_std_17)
find_package(OpenMP)
if(OpenMP_FOUND)
    target_link_libraries(connected_components PRIVATE OpenMP::OpenMP_CXX)
endif()

if(NOT MSVC)
target_compile_options(connected_components
  PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:$<$<CONFIG:RelWithDebInfo>:-march=native>>
    $<$<COMPILE_LANGUAGE:C>:$<$<CONFIG:RelWithDebInfo>:-march=native>>
    $<$<COMPILE_LANGUAGE:CXX>:$<$<CONFIG:Release>:-march=native -fopt-info-missed=missed.all>>
    $<$<COMPILE_LANGUAGE:C>:$<$<CONFIG:Release>:-march=native>>
    $<$<COMPILE_LANGUAGE:CXX>:$<$<CONFIG:MinSizeRel>:-march=native>>
    $<$<COMPILE_LANGUAGE:C>:$<$<CONFIG:MinSizeRel>:-march=native>>
)
endif()
