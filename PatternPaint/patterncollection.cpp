#include "patterncollection.h"

#include <QDebug>

PatternCollection::PatternCollection()
{
}

PatternCollection::~PatternCollection()
{
}

PatternCollectionModel *PatternCollection::getModel()
{
    return &model;
}

QList<Pattern *> PatternCollection::patterns() const
{
    QList<Pattern * > patterns;
    for (int i = 0; i < count(); i++)
        patterns.append(at(i));

    return patterns;
}

void PatternCollection::add(QPointer<Pattern> newPattern, int index)
{
    model.insertRow(index);
    model.setData(model.index(index),
                     QVariant::fromValue(newPattern),
                     PatternCollectionModel::PatternPointer);
}

void PatternCollection::remove(int index)
{
    model.removeRow(index);
}

void PatternCollection::clear()
{
    model.removeRows(0,count());
}

QPointer<Pattern> PatternCollection::at(int index) const
{
    return qvariant_cast<QPointer<Pattern> >(model.index(index).data(PatternCollectionModel::
                                                                        PatternPointer));
}

int PatternCollection::count() const
{
    return model.rowCount();
}

bool PatternCollection::isEmpty() const
{
    return count() == 0;
}

bool PatternCollection::getModified() const
{
    for (int i = 0; i < count(); i++)
        if(at(i)->getModified())
            return true;

    return false;
}

bool PatternCollection::load(const QString newFileName)
{
    QFile file(newFileName);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);

    // Read and check the header
    quint32 magic;
    in >> magic;
    if (magic != 0xA0B0C0D0)
        return false;

    // Read the version
    qint32 version;
    in >> version;
    if (version < 1)
        return false;
    if (version > 1)
        return false;

    in.setVersion(QDataStream::Qt_4_0);

    clear();

    // Read the data
    while(!in.atEnd()) {
        qint32 type;
        in >> type;

        if(type == SCROLL_MODEL)
        {
            qDebug() << "Found a scroll model";
            Pattern *pattern = new Pattern(Pattern::Scrolling,
                                           QSize(1,1),0);
            pattern->getModel()->deseriaize(in);
            add(pattern,count());
        }
        else if(type == FRAME_MODEL)
        {
            qDebug() << "Found a frame model";
            Pattern *pattern = new Pattern(Pattern::FrameBased,
                                           QSize(1,1),0);
            pattern->getModel()->deseriaize(in);
            add(pattern,count());
        }
        else
        {
            qDebug() << "Invalid data section type!";
        }
    }

    return true;
}

bool PatternCollection::saveAs(const QString newFileName)
{
    QFile file(newFileName);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    // Write a header with a "magic number" and a version
    out << (quint32)0xA0B0C0D0;
    out << (qint32)1;

    out.setVersion(QDataStream::Qt_4_0);

    // Write the data
    for (int i = 0; i < count(); i++) {
        qDebug() << "Writing out pattern";
        at(i)->getModel()->seriaize(out);
    }

    file.close();

    return true;
}
