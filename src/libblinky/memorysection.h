#ifndef FLASHSECTION_H
#define FLASHSECTION_H

#include <QMetaType>
#include <QString>
#include <QByteArray>
#include <QList>

// Single contiguous secton of memory, eg flash
struct MemorySection {
    MemorySection() {}

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


// Collection of multiple blocks of memory, with checking to
// ensure they are all in bounds and do not overlap
class MemoryMap {
public:
    MemoryMap(unsigned int start, unsigned int size);

//    bool addSection(const memorySection& section);

    QList<MemorySection> memorySections;     ///< List of memory sections

    unsigned int start;
    unsigned int size;
};

#endif // FLASHSECTION_H
