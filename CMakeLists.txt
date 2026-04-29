cmake_minimum_required(VERSION 3.24)

project(RocoDexQt VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 Network Test)

if(COMMAND qt_policy)
    qt_policy(SET QTP0004 NEW)
endif()

qt_standard_project_setup(REQUIRES 6.7)
enable_testing()

qt_add_executable(RocoDexQt
    src/main.cpp
    src/models/DexTypes.h
    src/app/DexRepository.h
    src/app/DexRepository.cpp
    src/app/DexRuntime.h
    src/app/DexRuntime.cpp
    src/app/DexController.h
    src/app/DexController.cpp
    src/app/PetListModel.h
    src/app/PetListModel.cpp
)

qt_add_qml_module(RocoDexQt
    URI RocoDex
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/pages/DexPage.qml
        qml/pages/PetDetailPage.qml
        qml/components/AttributeBadge.qml
        qml/components/DataManager.qml
        qml/components/FilterRail.qml
        qml/components/QuickPetStrip.qml
        qml/components/PetListItem.qml
        qml/components/StatBars.qml
        qml/components/SkillCard.qml
        qml/components/MatchupPanel.qml
    RESOURCES
        qml/pages/qmldir
        qml/components/qmldir
)

target_link_libraries(RocoDexQt
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Network
)

target_compile_definitions(RocoDexQt
    PRIVATE
        QT_NO_CAST_FROM_ASCII
        QT_NO_CAST_TO_ASCII
)

add_custom_command(TARGET RocoDexQt POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:RocoDexQt>/resources
)

include(GNUInstallDirs)

install(TARGETS RocoDexQt
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_add_executable(RocoDexQtTests
    tests/DexRuntimeTest.cpp
    src/app/DexRuntime.h
    src/app/DexRuntime.cpp
    src/app/DexRepository.h
    src/app/DexRepository.cpp
    src/models/DexTypes.h
)

target_link_libraries(RocoDexQtTests
    PRIVATE
        Qt6::Core
        Qt6::Test
)

add_test(NAME RocoDexQtTests COMMAND RocoDexQtTests)
