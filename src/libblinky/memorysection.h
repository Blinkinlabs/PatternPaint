#ifndef MEMORYSECTION_H
#define MEMORYSECTION_H

#include "libblinkyglobal.h"

#include <QMetaType>
#include <QString>
#include <QByteArray>
#include <QList>

// Single contiguous secton of memory, eg flash
struct LIBBLINKY_EXPORT MemorySection {
    MemorySection();

    /// Create a new flash section
    /// @param address Address in the flash memory where the data should be stored
    /// @param data Data to store
    MemorySection(const QString& name, unsigned int address, const QByteArray& data);

    QString name;       /// Section name, for debugging
    unsigned int address;        /// Address to store the data
    QByteArray data;    /// Data to store

    unsigned int extent() const; /// Calculate the address of the last byte in the section
    bool overlaps(const MemorySection &rhs) const;
};

Q_DECLARE_METATYPE(MemorySection)

#endif // MEMORYSECTION_H
