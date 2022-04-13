# DocView

DocView is used to traverse JSON DOM objects and to retrieve values from the DOM without copying or modifying it. 

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
* SPINO_VALUETYPE_UNDEFINED

In Vala and Python, the types are Spino.ValueType.INT, Spino.ValueType.UINT etc

## get_int

    `int spino_docview_get_int(SpinoDocView* self)`

Returns the int value of self. The DocView node must be of type int.

## get_uint

    `guint spino_docview_get_uint(SpinoDocView* self)`

Returns the uint value of self. The DocView must be of type uint.

## get_double

    `double spino_docview_get_double(SpinoDocView* self)`

Returns the double value of self. The DocView must be of type double.

## get_bool

    `gboolean spino_docview_get_bool(SpinoDocView* self)`

Returns the boolean value of self. The DocView must be of type bool.

## get_string

    `const gchar* spino_docview_get_string(SpinoDocView* self)`

Returns the value of self as a null terminated string. The DocView must be of type string.

## get_value_type

    `SpinoVALUETYPE spino_docview_get_value_type(SpinoDocView* self)`

Returns the VALUETYPE of the DocView.

## has_member

    `gboolean spino_docview_has_member(SpinoDocView* self, const gchar* name)`

Returns true if the docview is an object and has a member called 'name'

## get_member_type

    `SpinoVALUETYPE spino_docview_get_member_type(SpinoDocView* self, const gchar* name)`

Returns the VALUETYPE of a member. If self is not an object or name does not exist, SPINO_VALUETYPE_UNDEFINED is returned.

## get_int_member

    `int spino_docview_get_int_member(SpinoDocView* self, const gchar* name)`

Gets an integer value from a member. self must be an object, name must exist and be of integer type.

## get_uint_member

    `guint spino_docview_get_uint_member(SpinoDocView* self, const gchar* name)`

Gets a uint value from a member. self must be an object, name must exist and be of type uint.

## get_bool_member

    `guint spino_docview_get_bool_member(SpinoDocView* self, const gchar* name)`

Gets a bool value from a member. self must be an object, name must exist and be of type bool.

## get_double_member

    `guint spino_docview_get_double_member(SpinoDocView* self, const gchar* name)`

Gets a double value from a member. self must be an object, name must exist and be of type double.

## get_string_member

    `const gchar* spino_docview_get_string_member(SpinoDocView* self, const gchar* name)`

Gets a string value from a member. self must be an object, name must exist and be of type string.

## get_member

    `SpinoDocView* spino_docview_get_member(SpinoDocView* self, const gchar* name)`

Gets a member as a DocView. self must be an object, name must exist and can be of any type. 

## member_begin

    `SpinoMemberIterator* spino_docview_member_begin(SpinoDocView* self)`

Returns a MemberIterator to the first member of self. Self must of type object. 

## member_end

    `SpinoMemberIterator* spino_docview_member_end(spinoDocView* self)`

Returns a MemberIterator to the last member of self. Self must of type object.

## get_element

    `SpinoDocView* spino_docview_get_element(SpinoDocView* self, guint index)`

Returns a DocView to an element of an array. self must be an array.

## begin

    `SpinoValueIterator* spino_docview_begin(SpinoDocView* self)`

Returns a ValueIterator to the first element of self. self must be of type array.

## end

    `SpinoValueIterator* spino_docview_end(SpinoDocView* self)`

Returns a ValueIterator to the last element of self. self must be of type array.

## stringify

    `gchar* spino_docview_stringify(SpinoDocView* self)`

Turns the DocView into a JSON string representation.




