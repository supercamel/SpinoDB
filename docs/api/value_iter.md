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


