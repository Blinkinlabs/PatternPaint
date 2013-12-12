#include <QString>
#include <QtTest>
#include <QDebug>

#include <PatternPaint/animation.h>

class AnimationTest : public QObject
{
    Q_OBJECT

public:
    AnimationTest();

private Q_SLOTS:
    void testInitializesMembers();
};

AnimationTest::AnimationTest()
{
}

// Really just to see if the test framework works
void AnimationTest::testInitializesMembers()
{
    int expectedFrameDelay = 123;
    int expectedFrameCount = 34;
    int expectedLedCount = 12;
    Animation::Encoding expectedEncoding = Animation::RGB24;

    QImage image = QImage(expectedFrameCount,expectedLedCount,QImage::Format_RGB32);

    Animation animation = Animation(image, expectedFrameDelay, expectedEncoding);

    QVERIFY(animation.frameDelay == expectedFrameDelay);
    QVERIFY(animation.frameCount == expectedFrameCount);
    QVERIFY(animation.ledCount == expectedLedCount);
    QVERIFY(animation.encoding == expectedEncoding);


}

QTEST_APPLESS_MAIN(AnimationTest)

#include "tst_animation.moc"
