# MemberIterator

MemberIterators are used to iterate over a DocView that contains an object. 

## next

    `void spino_member_iterator_next(SpinoMemberIterator* iter)`

Moves the iterator along to the next value in the object.

## get_name

    `gchar* spino_member_iterator_get_name(SpinoMemberIterator* iter)`

Gets the key name of the field that the iterator points to.

## get_view

    `SpinoDocView* spino_member_iterator_get_view(SpinoMemberIterator* iter)`

Returns a DocView of the member that the iterator points to. The DocView can then be used to check the type and extract the value of the member.

## compare

    `gboolean spino_member_iterator_compare(SpinoMemberIterator* iter, SpinoMemberIterator* other)`

Returns True if iter and other point to the same member, otherwise False

## is_not

    `gboolean spino_member_iterator_is_not(SpinoMemberIterator* iter, SpinoMemberIterator* other)`

Return True if iter and other do not point to the same item, return false if they do


