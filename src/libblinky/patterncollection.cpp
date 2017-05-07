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

bool PatternCollection::writePatterns(QDataStream& stream)
{

    // write pattern
    for(int i=0;i<count();i++){

        stream << (qint32)at(i)->getType();
        at(i)->getModel()->writeDataToStream(stream);

        at(i)->setModified(false);
        at(i)->getModified();

    }

    return true;
}

bool PatternCollection::readPatterns(QDataStream& stream)
{

    clear();

    while(!stream.atEnd()) {
            qint32 type;
            stream >> type;

            if(type == Pattern::Scrolling)
            {
                Pattern *pattern = new Pattern(Pattern::Scrolling,
                                                           QSize(1,1),0);
                if(!pattern->getModel()->readDataFromStream(stream))
                    return false;
                add(pattern,count());
            }
            else if(type == Pattern::FrameBased)
            {
                Pattern *pattern = new Pattern(Pattern::FrameBased,
                                                           QSize(1,1),0);
                if(!pattern->getModel()->readDataFromStream(stream))
                    return false;
                add(pattern,count());
            }
            else
            {
                qDebug() << "Invalid data section type!";
                return false;
            }
    }

    return true;

}
