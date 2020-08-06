INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_VSG60 vsg60)

FIND_PATH(
    VSG60_INCLUDE_DIRS
    NAMES vsg60/api.h
    HINTS $ENV{VSG60_DIR}/include
        ${PC_VSG60_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    VSG60_LIBRARIES
    NAMES gnuradio-vsg60
    HINTS $ENV{VSG60_DIR}/lib
        ${PC_VSG60_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VSG60 DEFAULT_MSG VSG60_LIBRARIES VSG60_INCLUDE_DIRS)
MARK_AS_ADVANCED(VSG60_LIBRARIES VSG60_INCLUDE_DIRS)

