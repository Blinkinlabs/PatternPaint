#include "memorysection.h"

MemorySection::MemorySection(const QString& name, unsigned int address, const QByteArray& data) :
    name(name),
    address(address),
    data(data)
{
}

unsigned int MemorySection::extent() const {
    // Special case- zero-length sections are degenerate
    if(data.length() == 0) {
        return address;
    }

    return address + data.length() - 1;
}

bool MemorySection::overlaps(const MemorySection &rhs) const
{
    // Special case- zero-length sections do not overlap
    if((data.length() == 0) || (rhs.data.length() == 0)) {
        return false;
    }

    return((extent() >= rhs.address) && (address <= rhs.extent()));
}

MemoryMap::MemoryMap(unsigned int start, unsigned int size) :
    start(start),
    size(size)
{

}
