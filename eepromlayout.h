#ifndef EEPROMLAYOUT_H
#define EEPROMLAYOUT_H

#include <Arduino.h>

/**
 * Class for handling data in EEPromLayout
 *
 * Class EEPromLayout handles the data stored in EEPROM. Each item is of fixed
 * length, is identified by a four byte magic number, and is stored at a
 * specific position in the EEPROM. The table of known items is stored in this
 * class.
 */
class EEPromLayout
{
public:
    /// Type definition of a magic item identifier
    typedef char Magic[4];
    /// Definition of a data item stored in EEPROM
    struct ItemInfo
    {
        /// Identifier for the item
        const Magic magic;
        /// Offset of the start of the item in EEPROM
        int offset;
        /// The (maximum) number of data bytes in the item. This excludes the identifier
        int len;
    };

    /// Look up the data item location associated with identifier \a magic.
    static const ItemInfo* find(const char* magic);
    /// Read the data item \a magic from EEPROM into buffer \a data of size \a len.
    static int read(const char* magic, byte* data, int len);
    /// Write \a len bytes of data in \a data into the EEPROM item \a magic
    static bool write(const char* magic, const byte* data, int len);

private:
    /// Number if known data items
    static const int _layout_size = 1;
    /// Lookup table for item locations
    static const ItemInfo _layout[_layout_size];
};

#endif // EEPROMLAYOUT_H