

add_library(smjnitests SHARED EXCLUDE_FROM_ALL)

target_link_libraries(smjnitests PRIVATE
    smjni
)

set_property(TARGET smjnitests PROPERTY CXX_STANDARD 17)
set_property(TARGET smjnitests PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET smjnitests PROPERTY CXX_VISIBILITY_PRESET hidden)
set_property(TARGET smjnitests PROPERTY VISIBILITY_INLINES_HIDDEN ON)

target_sources(smjnitests PRIVATE
    smjnitests.cpp
)

install(TARGETS smjnitests DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../build/native)
