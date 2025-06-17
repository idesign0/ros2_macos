find_package(Doxygen 1.8 COMPONENTS doxygen)

if(NOT Doxygen_FOUND)
    return()
endif()

file(DOWNLOAD
     https://raw.githubusercontent.com/jothepro/doxygen-awesome-css/v2.1.0/doxygen-awesome.css
     ${CMAKE_CURRENT_BINARY_DIR}/doxygen-awesome.css)

set(DOXYGEN_DISABLE_INDEX NO)
set(DOXYGEN_FULL_SIDEBAR NO)
set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_HTML_EXTRA_STYLESHEET ${CMAKE_CURRENT_BINARY_DIR}/doxygen-awesome.css)
set(DOXYGEN_STRIP_FROM_PATH ${PROJECT_SOURCE_DIR}/include)
set(DOXYGEN_WARN_AS_ERROR YES)
set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
doxygen_add_docs(docs ${PROJECT_SOURCE_DIR}/include)
