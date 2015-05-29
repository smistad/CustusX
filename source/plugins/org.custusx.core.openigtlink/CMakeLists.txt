project(org_custusx_core_openigtlink)

set(PLUGIN_export_directive "${PROJECT_NAME}_EXPORT")

set(PLUGIN_SRCS
    cxOpenIGTLinkPluginActivator.cpp

    guiExtenderService/cxOpenIGTLinkGuiExtenderService.cpp
    guiExtenderService/cxOpenIGTLinkConnectionWidget.cpp

    trackingSystemService/cxOpenIGTLinkTrackingSystemService.cpp
    trackingSystemService/cxOpenIGTLinkTool.cpp

    streamerService/cxOpenIGTLinkStreamerService.cpp
    streamerService/cxOpenIGTLinkStreamer.cpp

    openigtlinkClient/cxSocket.cpp
    openigtlinkClient/cxOpenIGTLinkClient.cpp
    openigtlinkClient/cxDialect.cpp
    openigtlinkClient/cxPlusDialect.cpp
    openigtlinkClient/cxCustusDialect.cpp
)

# Files which should be processed by Qts moc
set(PLUGIN_MOC_SRCS
    cxOpenIGTLinkPluginActivator.h

    guiExtenderService/cxOpenIGTLinkGuiExtenderService.h
    guiExtenderService/cxOpenIGTLinkConnectionWidget.h

    trackingSystemService/cxOpenIGTLinkTrackingSystemService.h
    trackingSystemService/cxOpenIGTLinkTool.h

    streamerService/cxOpenIGTLinkStreamerService.h
    streamerService/cxOpenIGTLinkStreamer.h

    openigtlinkClient/cxSocket.h
    openigtlinkClient/cxOpenIGTLinkClient.h
    openigtlinkClient/cxDialect.h
    openigtlinkClient/cxPlusDialect.h
    openigtlinkClient/cxCustusDialect.h
)

# Qt Designer files which should be processed by Qts uic
set(PLUGIN_UI_FORMS
)

# QRC Files which should be compiled into the plugin
set(PLUGIN_resources
)


#Compute the plugin dependencies
ctkFunctionGetTargetLibraries(PLUGIN_target_libraries)
set(PLUGIN_target_libraries 
    ${PLUGIN_target_libraries}
    cxResourceWidgets
    cxResource
    cxPluginUtilities
    cxGrabber
    cxGUIExtenderService
    OpenIGTLink
    )

set(PLUGIN_OUTPUT_DIR "")
if(CX_WINDOWS)
    #on windows we want dlls to be placed with the executables
    set(PLUGIN_OUTPUT_DIR "../")
endif(CX_WINDOWS)

ctkMacroBuildPlugin(
  NAME ${PROJECT_NAME}
  EXPORT_DIRECTIVE ${PLUGIN_export_directive}
  SRCS ${PLUGIN_SRCS}
  MOC_SRCS ${PLUGIN_MOC_SRCS}
  UI_FORMS ${PLUGIN_UI_FORMS}
  RESOURCES ${PLUGIN_resources}
  TARGET_LIBRARIES ${PLUGIN_target_libraries}
  OUTPUT_DIR ${PLUGIN_OUTPUT_DIR}
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    .
    ${CMAKE_CURRENT_BINARY_DIR}
    openigtlinkClient
    trackingSystemService
    streamerService
    guiExtenderService
)


add_subdirectory(testing)
