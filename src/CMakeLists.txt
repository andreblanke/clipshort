find_package(CURL REQUIRED)
find_package(X11  REQUIRED)

if (NOT X11_Xfixes_FOUND)
    message(FATAL_ERROR)
endif ()

link_libraries(${CURL_LIBRARIES})
link_libraries(${X11_LIBRARIES})
link_libraries(${X11_Xfixes_LIB})

add_executable(
    ${PROJECT_NAME}

    clipboard.c
    clipshort.c
)
