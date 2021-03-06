project(vspData)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

set(vspData_Sources
    # Service core
    vsSourceFactoryInterface.cxx
    vsSourceService.cxx
    # Data sources
    vsDataSource.cxx
    vsVideoSource.cxx
    vsSimpleSourceFactory.cxx
    vsSourceFactory.cxx
    vsStaticSourceFactory.cxx
    # Live descriptors
    vsLiveDescriptor.cxx
    # Supporting code
    vsContour.cxx
    vsDescriptorInput.cxx
    vsEventInfo.cxx
    vsLibDebug.cxx
    vsTrackInfo.cxx
    vtkVsTrackInfo.cxx
)

set(vspData_MocHeaders
    # Live descriptors
    vsLiveDescriptor.h
    vsLiveDescriptorPrivate.h
    # Data sources
    vsDataSource.h
    vsVideoSource.h
    vsVideoSourcePrivate.h
    vsTrackSource.h
    vsDescriptorSource.h
)

set(vspDataInstallHeaders
  vsArchiveSourceInfo.h
  vsArchiveSourceInterface.h
  vsContour.h
  vsDataSource.h
  vsDescriptor.h
  vsDescriptorInput.h
  vsDescriptorSource.h
  vsDisplayInfo.h
  vsEvent.h
  vsEventInfo.h
  vsFactoryAction.h
  vsLiveDescriptor.h
  vsLiveDescriptorPrivate.h
  vsSimpleSourceFactory.h
  vsSourceFactory.h
  vsSourceFactoryInterface.h
  vsSourceService.h
  vsStaticSourceFactory.h
  vsTrackClassifier.h
  vsTrackData.h
  vsTrackId.h
  vsTrackInfo.h
  vsTrackSource.h
  vsTrackState.h
  vsVideoSource.h
  vsVideoSourcePrivate.h
  vtkVsTrackInfo.h
)

qt4_wrap_cpp(vspData_MocSources ${vspData_MocHeaders})

add_library(${PROJECT_NAME}
  ${vspData_Sources}
  ${vspData_MocSources}
)

vg_add_dependencies(${PROJECT_NAME}
  PUBLIC_INTERFACE_TARGETS
  qtExtensions
  qtVgCommon
  vgVideo
  vtkVgCore
  vvIO
  vgVtkVideo
  vvVtkWidgets
  LINK_LIBRARIES
  LINK_PUBLIC
  ${QT_LIBRARIES}
)

install_library_targets(${PROJECT_NAME})
install_headers(${vspDataInstallHeaders} TARGET ${PROJECT_NAME}
                DESTINATION include/VspData)
