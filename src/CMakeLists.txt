
add_library(connected_components
    disjoint_set.hpp
    fill_segments.cpp
    fill_segments.hpp
)
target_include_directories(connected_components PRIVATE deps)
