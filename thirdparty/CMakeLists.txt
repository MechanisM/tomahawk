ADD_SUBDIRECTORY( qxt )
ADD_SUBDIRECTORY( liblastfm2 )
IF( WITH_BREAKPAD )
    ADD_SUBDIRECTORY( breakpad )
ENDIF()
IF( APPLE )
    ADD_SUBDIRECTORY( SPMediaKeyTap )
ENDIF()
