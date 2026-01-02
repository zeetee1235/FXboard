cmake_minimum_required(VERSION 3.15)

project(FXBoard VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE 프레임워크 추가 (audio-only, no GUI)
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.9
)
FetchContent_MakeAvailable(JUCE)

# 소스 파일 수집
set(SOURCES
    src/main.cpp
    src/core/Application.cpp
    src/app/ConfigManager.cpp
    src/input/KeyHook.cpp
    src/audio/AudioEngine.cpp
    src/audio/SampleManager.cpp
    src/audio/Mixer.cpp
    src/audio/FX.cpp
)

# 헤더 파일 경로
set(HEADERS
    src/core/Application.h
    src/core/EventQueue.h
    src/core/Smoother.h
    src/core/KeyEvent.h
    src/app/ConfigManager.h
    src/input/KeyHook.h
    src/input/KeyState.h
    src/audio/AudioEngine.h
    src/audio/SampleManager.h
    src/audio/Mixer.h
    src/audio/FX.h
)

# 실행 파일 생성 (console app, not GUI)
juce_add_console_app(FXBoard
    PRODUCT_NAME "FXBoard"
)

target_sources(FXBoard PRIVATE ${SOURCES} ${HEADERS})

# JUCE 모듈 추가 (audio only, no GUI modules)
target_link_libraries(FXBoard
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# 인클루드 디렉토리
target_include_directories(FXBoard PRIVATE src)

# 컴파일 정의
target_compile_definitions(FXBoard
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

# 플랫폼별 설정
if(UNIX AND NOT APPLE)
    # No X11 needed for console-only app
    target_link_libraries(FXBoard PRIVATE pthread)
endif()

# Install target
install(TARGETS FXBoard
    RUNTIME DESTINATION bin
)
