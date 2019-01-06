include(FindPkgConfig REQUIRED)

set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig")

pkg_check_modules(PC_libclipboard REQUIRED QUIET libclipboard)

find_path(libclipboard_INCLUDE_DIR
    NAMES libclipboard.h
    PATHS ${PC_libclipboard_INCLUDE_DIRS}
)
find_library(libclipboard_LIB
    NAMES libclipboard.a
    PATHS ${PC_libclipboard_LIBRARY_DIRS}
)

set(libclipboard_VERSION ${PC_libclipboard_VERSION})

mark_as_advanced(
    libclipboard_FOUND
    libclipboard_INCLUDE_DIR
    libclipboard_VERSION
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(libclipboard
    REQUIRED_VARS libclipboard_INCLUDE_DIR
    VERSION_VAR   libclipboard_VERSION
)

if (libclipboard_FOUND)
    set(libclipboard_INCLUDE_DIRS ${libclipboard_INCLIDE_DIR})
endif ()
