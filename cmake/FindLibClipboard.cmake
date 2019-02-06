find_package(PkgConfig QUIET)

if (PKG_CONFIG_FOUND)
    set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:/usr/local/lib/pkgconfig")

    pkg_check_modules(PC_LIBCLIPBOARD QUIET LIBCLIPBOARD)
endif ()

find_path(LIBCLIPBOARD_INCLUDE_DIR
    NAMES libclipboard.h
    NAMES ${PC_LIBCLIPBOARD_INCLUDE_DIRS}
)
mark_as_advanced(LIBCLIPBOARD_INCLUDE_DIR)

find_library(LIBCLIPBOARD_LIBRARY
    names clipboard
    PATHS ${PC_LIBCLIPBOARD_LIBRARY_DIRS}
)
mark_as_advanced(LIBCLIPBOARD_LIBRARY)

# Commits prior to 227588f to the libclipboard git repository did not correctly
# define a version in the libclipboard.pc generated during installation.
# We first attempt to use the version found by pkgconfig.
# If that version string is empty we try the less realiable way of reading the
# two macros LIBCLIPBOARD_VERSION_MAJOR and LIBCLIPBOARD_VERSION_MINOR from
# libclipboard-config.h and combine those two into LIBCLIPBOARD_VERSION.
if (PC_LIBCLIPBOARD_VERSION)
    set(LIBCLIPBOARD_VERSION "${PC_LIBCLIPBOARD_VERSION}")
elseif (LIBCLIPBOARD_INCLUDE_DIR)
    set(_LIBCLIPBOARD_CONFIG_HEADER "${LIBCLIPBOARD_INCLUDE_DIR}/libclipboard-config.h")

    if (EXISTS "${_LIBCLIPBOARD_CONFIG_HEADER}")
        set(_S "[\\n\\r\\t ]")

        set(_LIBCLIPBOARD_VERSION_MAJOR_REGEX "^#define${_S}+LIBCLIPBOARD_VERSION_MAJOR${_S}+")
        set(_LIBCLIPBOARD_VERSION_MINOR_REGEX "^#define${_S}+LIBCLIPBOARD_VERSION_MINOR${_S}+")

        file(STRINGS "${_LIBCLIPBOARD_CONFIG_HEADER}" _LIBCLIPBOARD_VERSION_MAJOR REGEX "${_LIBCLIPBOARD_VERSION_MAJOR_REGEX}")
        string(REGEX REPLACE "${_LIBCLIPBOARD_VERSION_MAJOR_REGEX}" "" _LIBCLIPBOARD_VERSION_MAJOR "${_LIBCLIPBOARD_VERSION_MAJOR}")

        file(STRINGS "${_LIBCLIPBOARD_CONFIG_HEADER}" _LIBCLIPBOARD_VERSION_MINOR REGEX "${_LIBCLIPBOARD_VERSION_MINOR_REGEX}")
        string(REGEX REPLACE "${_LIBCLIPBOARD_VERSION_MINOR_REGEX}" "" _LIBCLIPBOARD_VERSION_MINOR "${_LIBCLIPBOARD_VERSION_MINOR}")

        set(LIBCLIPBOARD_VERSION "${_LIBCLIPBOARD_VERSION_MAJOR}.${_LIBCLIPBOARD_VERSION_MINOR}")
    endif ()
endif ()
mark_as_advanced(LIBCLIPBOARD_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibClipboard
    FOUND_VAR     LIBCLIPBOARD_FOUND
    REQUIRED_VARS LIBCLIPBOARD_INCLUDE_DIR
                  LIBCLIPBOARD_LIBRARY
    VERSION_VAR   LIBCLIPBOARD_VERSION
)

if (LIBCLIPBOARD_FOUND)
    set(LIBCLIPBOARD_INCLUDE_DIRS "${LIBCLIPBOARD_INCLUDE_DIR}")
    set(LIBCLIPBOARD_LIBRARIES    "${LIBCLIPBOARD_LIBRARY}")

    mark_as_advanced(
        LIBCLIPBOARD_INCLUDE_DIRS
        LIBCLIPBOARD_LIBRARIES
    )
endif ()

if (LIBCLIPBOARD_FOUND AND NOT TARGET LibClipboard::LibClipboard)
    add_library(LibClipboard::LibClipboard STATIC IMPORTED)
    set_target_properties(LibClipboard::LibClipboard PROPERTIES
        IMPORTED_LOCATION             "${LIBCLIPBOARD_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS     "${PC_LIBCLIPBOARD_CFLAGS_OTHER}"
        INTERFACE_INCLUDE_DIRECTORIES "${LIBCLIPBOARD_INCLUDE_DIRS}"
    )
endif ()
