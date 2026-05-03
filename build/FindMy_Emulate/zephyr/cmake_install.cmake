# Install script for directory: /opt/nordic/ncs/v3.2.1/zephyr

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/opt/nordic/ncs/toolchains/322ac893fe/opt/zephyr-sdk/arm-zephyr-eabi/bin/arm-zephyr-eabi-objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/zephyr/arch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/zephyr/lib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/zephyr/soc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/zephyr/boards/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/zephyr/subsys/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/zephyr/drivers/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/nrf/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/hostap/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/mcuboot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/mbedtls/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/trusted-firmware-m/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/cjson/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/azure-sdk-for-c/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/cirrus-logic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/openthread/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/memfault-firmware-sdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/canopennode/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/chre/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/lz4/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/zscilib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/cmsis/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/cmsis-dsp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/cmsis-nn/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/cmsis_6/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/fatfs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/hal_nordic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/hal_st/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/hal_tdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/hal_wurthelektronik/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/liblc3/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/libmetal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/littlefs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/loramac-node/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/lvgl/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/mipi-sys-t/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/nanopb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/nrf_wifi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/open-amp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/percepio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/picolibc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/segger/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/uoscore-uedhoc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/zcbor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/nrfxlib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/nrf_hw_models/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/modules/connectedhomeip/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/zephyr/kernel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/zephyr/cmake/flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/zephyr/cmake/usage/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/Users/angelcasanova/Desktop/School/JHU/25-26/Spring/Embedded_Systems/project/FindMy_Emulate/build/FindMy_Emulate/zephyr/cmake/reports/cmake_install.cmake")
endif()

