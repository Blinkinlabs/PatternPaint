// Qt Multiple tests sceme for running many unit tests:
// https://github.com/e-j/qt-multiple-tests

#ifndef MULTITESTS_H
#define MULTITESTS_H

#if QT_VERSION >= 0x050000
#include <QTest>
#else
#include <QtTest/QtTest>
#endif

#include <QList>
#include <QString>
#include <QSharedPointer>
#include <QMetaType>
#include <QMetaObject>
#include <QMetaMethod>

#ifndef nullptr
// If C++ is not C++11 compatible
#define nullptr 0
#endif

namespace MultiTests {
    typedef QList<QObject*> TestCasesList;

    inline TestCasesList& allTestCases() {
        static TestCasesList list;
        return list;
    }

    /**
     * @brief Try to find Test object into the list of registered ones
     * @param object - The object to find
     * @return TRUE if object was already registered, FALSE else
     */
    inline bool findObject(QObject* object) {
        TestCasesList& list = allTestCases();
        if (list.contains(object)) {
            return true;
        }
        foreach (QObject* test, list){
            if (test->metaObject()->className() == object->metaObject()->className()) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Try to find a test object in list of registered one by it's name
     * @param name - The test name to look upon for
     * @return A pointer to the QObject* test object if found, or nullptr if no object match that name
     */
    inline QObject* findTestByName(QString name){
        TestCasesList& list = allTestCases();
        foreach (QObject* test, list){
            if (test->metaObject()->className() == name) {
                return test;
            }
        }
        return nullptr;
    }

    /**
     * @brief Add a test object to the list of test to run
     * @param object - Pointer to object to add to list
     */
    inline void addTest(QObject* object) {
        if (!findObject(object)) {
            allTestCases().append(object);
        }
    }
    /**
     * @brief Know if run arguments of program contain an arguments
     * @param args - List of arguments
     * @param argToFind - The argument to find into all arguments
     * @param argIndex - Reference to retrieve the index of list where argument was found
     * @param startIndex - The index to start the search, default is 0
     * @return TRUE if arguments is find, FALSE else
     */
    inline bool argContain(QStringList args,QString argToFind,int& argIndex,
                            int startIndex = 0){

        int tempIndex = args.indexOf(argToFind,startIndex);
        if( tempIndex!=-1 ){
            argIndex = tempIndex;
            return true;
        }
        return false;
    }


    /**
     * @brief Know if a method of an object is a test slot
     * @param sl - Reference to QMetaMethod of method to test
     * @return TRUE if that method is a test slot, FALSE else
     */
    inline bool isValidTestSlot(const QMetaMethod &sl) {
        if (sl.access() != QMetaMethod::Private || sl.parameterTypes().size() != 0
                /*|| sl.returnType() != QMetaType::Void*/ || sl.methodType() != QMetaMethod::Slot)
            return false;
        QByteArray name;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        name = QByteArray::fromRawData(sl.signature(),2048);
#else
        name = sl.methodSignature();
#endif
        if (name.isEmpty() || name.size()<2 )
            return false;
        name.remove(name.size()-2,2);
        if (name.endsWith("_data"))
            return false;
        if (name == "initTestCase" || name == "cleanupTestCase" || name == "cleanup" || name == "init")
            return false;
        return true;
    }
    /**
     * @brief List all test function of a case
     * @param testObj - Pointer to the test object to list
     */
    inline void listTestFunctionsFromCase(QObject * testObj){
        qDebug() << "==" << qPrintable(testObj->metaObject()->className()) <<"==";
        for (int i = 0; i < testObj->metaObject()->methodCount(); ++i) {
            QMetaMethod sl = testObj->metaObject()->method(i);
            if (isValidTestSlot(sl)) {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
                qDebug() << sl.signature();
#else
                qDebug() << sl.methodSignature();
#endif
            }
        }
    }

    /**
     * @brief Transform the C arguments list style to a QStringList
     * @param argc - Count of arguments
     * @param argv - Array of arguments
     * @return A QStringList of arguments
     */
    inline QStringList argumentsToList(int argc, char *argv[]) {
        QStringList argumentsList;
        for(int argIndex=0;argIndex<argc;argIndex++ ){
            argumentsList.append( argv[argIndex] );
        }
        return argumentsList;
    }

    /**
     * @brief Select the cases to run this session (based on the -case arguments)
     * @param arguments - Reference to the list of arguments
     * @return The TestCaseList of cases to run
     */
    inline TestCasesList selectTestCasesToRun(QStringList& arguments){
        TestCasesList selectedCases;
        int argCaseIndex = 0;
        if( argContain(arguments,"-case",argCaseIndex) && (argCaseIndex+1)<arguments.size() ){
            // At least one Argument "-case" specify a test case for run only it
            while( argContain(arguments,"-case",argCaseIndex) && (argCaseIndex+1)<arguments.size() ){
                QString testNameToRun = arguments.at(argCaseIndex+1);
                QObject * specifiedTest = findTestByName(testNameToRun);
                if( specifiedTest!=nullptr ){
                    arguments.removeAt(argCaseIndex);
                    arguments.removeAt(argCaseIndex);
                    selectedCases.append( specifiedTest );
                }
                else{
                    qCritical() << "Cannot find any test case called "<<testNameToRun;
                    qCritical() << "Available test cases are : ";
                    foreach (QObject* test,allTestCases() ){
                        qCritical() << "- " << test->metaObject()->className();
                    }
                    return TestCasesList();
                }
            }

        }
        else{
            // Run all registered tests
            selectedCases = allTestCases();
        }
        return selectedCases;
    }

    /**
     * @brief Update the output filename (-o or --output argument)
     * @param arguments - Reference to the list of arguments
     * @param caseIndex - Index of current case test
     */
    inline void updateOutputFile(QStringList& arguments,int caseIndex){
        int argIndex = 0;
        while( argContain(arguments,"-o",argIndex,argIndex) &&
                                        (argIndex+1)<arguments.size() ){
            QString oldFilename = arguments.at(argIndex+1);
            QString newFilename;
            if( oldFilename.contains(".") ){
                // Insert suffix before file extension
                newFilename = oldFilename.left( oldFilename.lastIndexOf(".") );
            }
            if( newFilename.contains(QRegExp(".*_\\d+$")) ){
                // File seem to already have a digital suffix
                newFilename = newFilename.left( newFilename.lastIndexOf("_") );
            }
            // Add new suffix
            newFilename.append(QString("_%1")
                            .arg(caseIndex,4,10,QChar('0')));
            if( oldFilename.contains(".") ){
                // Add back the extension
                newFilename.append(oldFilename.mid( oldFilename.lastIndexOf(".") ) );
            }
            // Update the filename
            arguments[argIndex+1] = newFilename;
            argIndex+=2;
        }
    }

    /**
     * @brief Run tests cases depending on some command lines options
     */
    inline int run(int argc, char *argv[]) {
        int ret = 0;
        int caseIndex = 0;
        QStringList failedTestCase;
        TestCasesList casesToRun;
        QStringList arguments = argumentsToList(argc,argv);

#if QT_VERSION >= 0x050000
        qApp->setAttribute(Qt::AA_Use96Dpi, true);
#endif


        QDateTime start = QDateTime::currentDateTime();
        casesToRun = selectTestCasesToRun(arguments);

        // If option -functions, need a global running
        if( arguments.contains("-functions") ){
            foreach (QObject* test, casesToRun){
                listTestFunctionsFromCase(test);
            }
            return 0;
        }

        foreach (QObject* test,casesToRun){
            try {
                int currentNbError = QTest::qExec(test,arguments);
                if( currentNbError>0 ){
                    ret+= currentNbError;
                    failedTestCase.append( test->metaObject()->className() );
                }
            }
            catch(...){
                ret++;
                failedTestCase.append( test->metaObject()->className() );
            }
            caseIndex++;
            updateOutputFile(arguments,caseIndex);
        }

        int nbMsecs = start.msecsTo( QDateTime::currentDateTime() );
        int nbSecs = start.secsTo( QDateTime::currentDateTime() );

        if( ret>0 ){
            qCritical() << "========================================";
            qCritical() << "========================================";
            qCritical() << "======= ERRORS during tests !!!   =======";
            qCritical() << qPrintable(
                                QString("%1 error in %2 case(s), over a total of %3 tests cases")
                                .arg(ret).arg(failedTestCase.size()).arg(casesToRun.size()));
            qCritical() << "Case(s) that failed : " << qPrintable(failedTestCase.join(" / "));
        }
        else{
            qWarning() << qPrintable(
                    QString("======= All tests succeed (%1 tests cases)  =======").arg(casesToRun.size()));
        }
        qWarning() << qPrintable(
                QString("Executed in %1 seconds (%2 ms)").arg(nbSecs).arg(nbMsecs)
                );

        return ret;
    }
}



template<class T>
class MultiTests_Case
{
public:
    QSharedPointer<T> child;

    MultiTests_Case(const QString& name) :
            child(new T) {
        child->setObjectName(name);
        MultiTests::addTest(child.data());
    }
};

#define TEST_DECLARE(className) static MultiTests_Case<className> autoT_##className(#className);

#define MULTI_TESTS_MAIN_NOAPP \
int main(int argc, char *argv[]) \
{ \
    srand (time(NULL)); \
    return MultiTests::run(argc, argv); \
}


#define MULTI_TESTS_MAIN \
int main(int argc, char *argv[]) \
{ \
    QApplication app(argc, argv); \
    srand (time(NULL)); \
    return MultiTests::run(argc, argv); \
}

#include <typeinfo>

#ifndef QVERIFY_EXCEPTION_THROWN
// Add to the test function a test on raised exception (and type of it)
// If the function (func) don't raise an exception of (exceptionClass) or a derived,
// Test fail as QFAIL with (description)
#define QVERIFY_EXCEPTION_THROWN( func,exceptionClass ) \
{ \
    bool caught = false; \
    try { \
        func; \
    } catch ( exceptionClass& e ) { \
        if ( typeid( e ) == typeid( exceptionClass ) ) { \
            /*qDebug() << "Caught";*/ \
        } else { \
            /*qDebug() << "Derived exception caught";*/ \
        } \
        caught = true; \
    } catch ( ... ) {} \
    if ( !caught ) { QFAIL(qPrintable(QString("Nothing thrown where expected") )); } \
}
#endif

#endif // MULTITESTS_H
