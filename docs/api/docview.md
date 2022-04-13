# DocView

## DocView Description

DocView only exists in the GObject bindings. It isn't neccesary for the NodeJS bindings since Javascript is already highly optimised for processing JSON. 

A DocView is a 'read-only' peek at the document inside SpinoDBs DOM. DocViews do not copy memory or do any string processing, and are a very efficient way to get information out of the document. DocView is also quite a convenient and simple API for traversing the JSON DOM in C / Vala. 

DocViews are created by cursors through calls to spino_cursor_next_view(), and by collections with spino_collection_find_one_view()

```
SpinoDocView* view = spino_cursor_next_view(cursor);

SpinoDocView* view = spino_collection_find_one_view(col);
```

The view represents a JSON value. It could be a numeric value, an object, an array, string, etc, however spino_cursor_next_view() is gauranteed to return an object because documents are objects. 

Members of the object can be read directly using these functions

```
int spino_docview_get_int_member(SpinoDocView* docview, const gchar* name);
guint spino_docview_get_uint_member(SpinoDocView* docview, const gchar* name);
gboolean spino_docview_get_bool_member(SpinoDocView* docview, const gchar* name);
double spino_docview_get_double_member(SpinoDocView* docview, const gchar* name);
const gchar* spino_docview_get_string_member(SpinoDocView* docview, const gchar* name);
```

Sub-object and array members must projected into a DocView of their own using spino_docview_get_member()
```
SpinoDocView* spino_docview_get_member(SpinoDocView* self, const gchar* name);
```

## MemberIterators

Objects can be traversed using MemberIterators. This example prints every field name in the object.

```
SpinoMemberIterator* iter = spino_docview_member_begin(docview);
while(spino_member_iterator_is_not(iter, spino_docview_member_end(docview)) == False) {
    printf("%s\n", spino_member_iterator_get_name(iter));
    spino_member_iterator_next(iter);
}
```

The value of an iter can be obtained by getting a DocView of the iter contents.

```
SpinoDocView* memberview = spino_member_iterator_get_view(iter);

if(spino_docview_get_value_type(memberview) == SPINO_VALUETYPE_INT) {
    int value = spino_docview_get_int(memberview);
}
```


## ValueIterators

Arrays are traversed using ValueIterators. ValueIterators have the same API as MemberIterators, only they do not have a 'get_name' function.

```
if(spino_docview_get_value_type(docview) == SPINO_VALUETYPE_ARRAY) {
    SpinoValueIterator* iter = spino_docview_begin(docview);
    while(spino_value_iterator_is_not(iter, spino_docview_end(docview)) == False) {
        SpinoDocView* memberview = spino_value_iterator_get_view(iter);
        // extract value of memberview here

        spino_value_iterator_next(iter);
    }
}
```

