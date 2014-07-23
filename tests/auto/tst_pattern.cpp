#include <QString>
#include <QtTest>
#include <QDebug>

#include <PatternPaint/pattern.h>

class PatternTest : public QObject
{
    Q_OBJECT

public:
    PatternTest();

private Q_SLOTS:
    void testInitializesMembers();
};

PatternTest::PatternTest()
{
}

// Really just to see if the test framework works
void PatternTest::testInitializesMembers()
{
    int expectedFrameDelay = 123;
    int expectedFrameCount = 34;
    int expectedLedCount = 12;
    Pattern::Encoding expectedEncoding = Pattern::RGB24;

    QImage image = QImage(expectedFrameCount,expectedLedCount,QImage::Format_RGB32);

    Pattern pattern = Pattern(image, expectedFrameDelay, expectedEncoding);

    QVERIFY(pattern.frameDelay == expectedFrameDelay);
    QVERIFY(pattern.frameCount == expectedFrameCount);
    QVERIFY(pattern.ledCount == expectedLedCount);
    QVERIFY(pattern.encoding == expectedEncoding);


}

QTEST_APPLESS_MAIN(PatternTest)

#include "tst_pattern.moc"
