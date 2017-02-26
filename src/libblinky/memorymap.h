#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include "memorysection.h"

// Collection of multiple blocks of memory, with checking to
// ensure they are all in bounds and do not overlap
class MemoryMap {
public:
    MemoryMap(unsigned int start, unsigned int size);

    // Add a new memory section to the map. Fails if the new section
    // overlaps an existing section
    bool addSection(const MemorySection& section);

    QList<MemorySection> memorySections;     ///< List of memory sections

    unsigned int start;
    unsigned int size;
};

#endif // MEMORYMAP_H
