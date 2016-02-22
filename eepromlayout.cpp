#include <EEPROM.h>
#include <string.h>
#include "eepromlayout.h"

const EEPromLayout::ItemInfo EEPromLayout::_layout[_layout_size] = {
    { {'B', 'N', 'O', 'C'}, 0, 22 }
};

const EEPromLayout::ItemInfo* EEPromLayout::find(const char* magic)
{
    for (int i = 0; i < _layout_size; ++i)
    {
        if (!memcmp(_layout[i].magic, magic, 4))
            return _layout + i;
    }
    return nullptr;
}

int EEPromLayout::read(const char* magic, byte* data, int len)
{
    const ItemInfo* info = find(magic);
    if (!info)
        return -1;

    int pos = info->offset;
    // check if item is actually stored by checking signature
    for (int i = 0; i < 4; ++i, ++pos)
    {
        if (EEPROM.read(pos) != magic[i])
            return -1;
    }

    // Ok, signature is rpesent. Read the data
    for (int i = 0; i < min(len, info->len); ++i, ++pos)
        data[i] = EEPROM.read(pos);

    return info->len;
}

bool EEPromLayout::write(const char* magic, const byte* data, int len)
{
    const ItemInfo* info = find(magic);
    if (!info || info->len < len)
        return false;

    int pos = info->offset;
    // write signature
    for (int i = 0; i < 4; ++i, ++pos)
        EEPROM.update(pos, magic[i]);

    // write the data
    for (int i = 0; i < len; ++i, ++pos)
        EEPROM.update(pos, data[i]);

    return true;
}