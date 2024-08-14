#===============================================================================
# @brief    cmake file
# Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
if(DEFINED CONFIG_SAMPLE_SUPPORT_MQTT_CLIENT_SAMPLE)
    add_subdirectory_if_exist(mqtt_client_demo)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_HTTP_SERVER_SAMPLE)
    add_subdirectory_if_exist(http_server_demo)
endif()

if(DEFINED CONFIG_SAMPLE_SUPPORT_HTTP_CLIENT_SAMPLE)
    add_subdirectory_if_exist(http_client_demo)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)