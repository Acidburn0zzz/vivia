project(vgGuiFramework)

include(${qtExtensions_SOURCE_DIR}/CMake/Modules/UseQtExtensions.cmake)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(vgGuiFrameworkSources
  vgfContextMenu.cxx
  vgfItemModel.cxx
  vgfNoteTreeModel.cxx
  vgfUtil.cxx
)

set(vgGuiFrameworkAMUI
  contextMenu.ui
)

set(vgGuiFrameworkUI ${vgGuiFrameworkAMUI})

set(vgGuiFrameworkMocHeaders
  vgfContextMenu.h
  vgfItemModel.h
  vgfNoteTreeModel.h
)

set(vgGuiFrameworkInstallHeaders
  vgfContextMenu.h
  vgfItemModel.h
  vgfItemReference.h
  vgfNamespace.h
  vgfNoteTreeModel.h
)

set(vgGuiFrameworkWrapObjects
  vgfContextMenu
  vgfItemModel
  vgfNoteTreeModel
  BY_VALUE
  vgfItemReference
)

qt4_wrap_ui(vgGuiFrameworkUiSources ${vgGuiFrameworkUI})
qte_amc_wrap_ui(vgGuiFrameworkAmSources ActionManagerDialog
  ${vgGuiFrameworkAMUI}
)
qt4_wrap_cpp(vgGuiFrameworkMocSources ${vgGuiFrameworkMocHeaders})

add_library(${PROJECT_NAME}
  ${vgGuiFrameworkSources}
  ${vgGuiFrameworkUiSources}
  ${vgGuiFrameworkAmSources}
  ${vgGuiFrameworkMocSources}
)

vg_add_dependencies(${PROJECT_NAME}
  PRIVATE_INTERFACE_TARGETS
  qtVgCommon
  vgCommon
  qtExtensions
  LINK_LIBRARIES
  LINK_PUBLIC
  ${QT_LIBRARIES}
)

install_library_targets(${PROJECT_NAME})
install_headers(${vgGuiFrameworkInstallHeaders} TARGET ${PROJECT_NAME}
                DESTINATION include/VgGuiFramework)

vg_wrap_library(${PROJECT_NAME}
  TYPESYSTEM typesystem.xml
  OBJECTS ${vgGuiFrameworkWrapObjects}
  HEADERS ${vgGuiFrameworkInstallHeaders}
  DEPENDS PySide:Core PySide:Gui
)

vg_add_documentation(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR} Qt)
