ADD_LIBRARY(optparse OBJECT optparse.c optparse.h)

ADD_EXECUTABLE(pub pub.c $<TARGET_OBJECTS:optparse>)
TARGET_LINK_LIBRARIES(pub mqtt)

ADD_EXECUTABLE(sub sub.c $<TARGET_OBJECTS:optparse>)
TARGET_LINK_LIBRARIES(sub mqtt)
