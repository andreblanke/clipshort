find_package(CURL         REQUIRED)
find_package(LibClipboard REQUIRED)
find_package(X11          REQUIRED)

find_package(XCB          REQUIRED)
find_package(Threads      REQUIRED)

if (NOT X11_Xfixes_FOUND)
    message(FATAL_ERROR "Unable to find required X11 extension XFixes")
endif ()

link_libraries(${CURL_LIBRARIES})
link_libraries(${LIBCLIPBOARD_LIBRARIES})
link_libraries(${X11_LIBRARIES})
link_libraries(${X11_Xfixes_LIB})

link_libraries(${XCB_LIBRARIES})
link_libraries(${CMAKE_THREAD_LIBS_INIT})

add_executable(
    ${PROJECT_NAME}

    clipshort.c
    net.c
    urlshort.c
)
