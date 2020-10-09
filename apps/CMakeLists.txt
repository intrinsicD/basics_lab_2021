#add_subdirectory(app_xyz) for each app inside this folder

if (BCG_APPS)
    #add_subdirectory(app_xyz)
    #...

endif (BCG_APPS)

if (BCG_OPENGL)
    add_subdirectory(bcg_viewer)
endif (BCG_OPENGL)
