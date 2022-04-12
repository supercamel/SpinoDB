# DocView

DocView only exists in the GObject bindings. It isn't neccesary for the NodeJS bindings since Javascript is already highly optimised for processing JSON. 

A DocView is a 'read-only' object that allows the document to be viewed inside SpinoDBs DOM. This cuts down on needless string processing and copying of memory. It's also quite a clean and simple API for traversing JSON DOMs in C / Vala. 

DocViews are created by calling spino_cursor_next_view()

```
SpinoDocView* view = spino_cursor_next_view(cursor);
```

The view represents a JSON value which could be a numeric value, an object, an array, string, etc. spino_cursor_next_view() is gauranteed to return an object. 

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

Objects can be traversed using MemberIterators. This example prints every field name in the object.

```
SpinoMemberIterator* iter = spino_docview_member_begin(docview);
while(spino_member_iterator_compare(iter, spino_docview_member_end(docview)) == False) {
    printf("%s\n", spino_member_iterator_get_name(iter));
    spino_member_iterator_next(iter);
}
```
