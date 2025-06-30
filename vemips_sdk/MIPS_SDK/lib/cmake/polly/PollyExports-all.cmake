set_target_properties(PollyISL PROPERTIES
        IMPORTED_LOCATION_RELEASE "${POLLY_INSTALL_PREFIX}/lib/PollyISL.lib")
set_target_properties(Polly PROPERTIES
        IMPORTED_LOCATION_RELEASE "${POLLY_INSTALL_PREFIX}/lib/Polly.lib")
# Compute the installation prefix from this LLVMConfig.cmake file location.
get_filename_component(POLLY_INSTALL_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(POLLY_INSTALL_PREFIX "${POLLY_INSTALL_PREFIX}" PATH)
get_filename_component(POLLY_INSTALL_PREFIX "${POLLY_INSTALL_PREFIX}" PATH)
get_filename_component(POLLY_INSTALL_PREFIX "${POLLY_INSTALL_PREFIX}" PATH)

