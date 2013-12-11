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

void AnimationTest::testInitializesMembers()
{
//    int expectedFrameDelay = 123;
//    int expectedFrameCount = 34;
//    int expectedLedCount = 12;
//    Animation::Encoding expectedEncoding = Animation::Encoding_RGB24;

//    QImage image = QImage(expectedLedCount,expectedFrameCount,QImage::Format_RGB32);

//    Animation animation = Animation(image, expectedFrameDelay, expectedEncoding);

    QVERIFY2(false, "Failure");
}

QTEST_APPLESS_MAIN(AnimationTest)

#include "tst_animation.moc"
