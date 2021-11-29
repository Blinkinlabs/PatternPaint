#ifndef DEFAULTS
#define DEFAULTS

#define OSX_RELEASE_APPCAST_URL \
    "https://software.blinkinlabs.com/patternpaint/patternpaint-osx.xml"
#define WINDOWS_RELEASE_APPCAST_URL \
    "https://software.blinkinlabs.com/patternpaint/patternpaint-windows.xml"

#define COLOR_BACKGROUND_UNCHECKED  QColor(0x10, 0x10, 0x10, 0xFF)
#define COLOR_BACKGROUND_CHECKED    QColor(0xA, 0x50, 0xCE, 0xFF)
#define COLOR_FRAME_SELECTED        QColor(0xFE, 0xD7, 0x28, 255)
#define COLOR_FRAME_UNSELECTED      QColor(250, 250, 250, 255)

#define PATTERN_SPEED_MINIMUM_VALUE 1                           /// Minimum pattern speed, in FPS
#define PATTERN_SPEED_MAXIMUM_VALUE 100                         /// Maximum pattern speed, in FPS

#define PATTERN_REPEAT_COUNT_MINIMUM_VALUE 1                    /// Minimum number of times a pattern can repeat before advancing
#define PATTERN_REPEAT_COUNT_MAXIMUM_VALUE 65535                /// Maximum number of times a pattern can repeat before advancing

#define DRAWING_SIZE_MINIMUM_VALUE 1                            /// Minimum drawing tool size, in pixels
#define DRAWING_SIZE_MAXIMUM_VALUE 20                           /// Maximum drawing tool size, in pixels
#define DRAWING_SIZE_DEFAULT_VALUE 1                            /// Default drawing tool size

#define COLOR_CANVAS_DEFAULT    QColor(0, 0, 0, 255)            /// Default tool color
#define COLOR_TOOL_DEFAULT    QColor(255, 255, 255, 255)        /// Default pattern backgroud color

#define DEFAULT_FIXTURE_TYPE "Linear"                           /// Default fixture type
#define DEFAULT_FIXTURE_WIDTH 1                                 /// Default fixture width for new fixtures
#define DEFAULT_FIXTURE_HEIGHT 60                               /// Default fixture height for new fixtures
#define DEFAULT_FRAME_COUNT 8                                   /// Default frame count for new patterns

#define CONNECTION_SCANNER_INTERVAL 1000                        /// Interval between controller connection scans, in ms

#define DEFAULT_LANGUAGE "<System Language>"                    /// Default language of Pattern Paint

#endif // DEFAULTS
