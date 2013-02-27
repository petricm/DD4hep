if(DD4HEP_USE_XERCESC)
  add_definitions(-DDD4HEP_USE_XERCESC)
  include_directories(${XERCESC_INCLUDE_DIR})
  set(XML_LIBRARIES ${XERCESC_LIBRARIES})
else()
  add_definitions(-DDD4HEP_USE_TINYXML)
  set(XML_LIBRARIES)
endif()
