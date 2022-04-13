# DocView

DocView is used to traverse JSON DOM objects and to retrieve values from the DOM without copying or modifying it. 

## ValueTypes

DocView nodes can represent any JSON type. In Spino these types are listed in a struct call SpinoVALUETYPE. 

The possible values are

* SPINO_VALUETYPE_INT
* SPINO_VALUETYPE_UINT
* SPINO_VALUETYPE_DOUBLE
* SPINO_VALUETYPE_BOOL
* SPINO_VALUETYPE_STRING
* SPINO_VALUETYPE_ARRAY
* SPINO_VALUETYPE_OBJECT
* SPINO_VALUETYPE_NULL

In Vala and Python, the types are Spino.ValueType.INT, Spino.ValueType.UINT etc

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

# ValueIterator

ValueIterator is used to iterate over an array type DocView

## next

    `void spino_value_iterator_next(SpinoValueIterator* iter)`

Moves the iterator along to the next element of the array.

## get_view

    `SpinoDocView* spino_value_iterator_get_view(SpinoValueIterator* iter)`

Returns a DocView of the array element that the iterator points to.

## compare

    `gboolean spino_value_iterator_compare(SpinoValueIterator* iter, SpinoValueIterator* other)`

Returns True if iter and other point to the same node, otherwise false

## is_not
    
    `gboolean spino_value_iterator_is_not(SpinoValueIterator* iter, SpinoValueIterator* other)`

Return True if iter and other do not point to the same not. The opposite of compare()

# DocView API

## get_int

    `int spino_docview_get_int(SpinoDocView* self)`

Returns the int value of self. The DocView node must be of type int.

## get_uint

    `guint spino_docview_get_uint(SpinoDocView* self)`


