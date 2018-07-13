#--------------------------------------------------------
# The CMakeLists.txt for:                       pGenPath
# Author(s):                              Jonathan B Schwartz
#--------------------------------------------------------

SET(SRC
  GenPath2.cpp
  GenPath2_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pGenPath2 ${SRC})

TARGET_LINK_LIBRARIES(pGenPath2
   ${MOOS_LIBRARIES}
   mbutil
   m
   pthread
   geometry)

