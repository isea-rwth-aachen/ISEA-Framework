Parameter objects    {#xmlobject}
================

Parameter objects are used for the parameterization of electrical objects. This makes it possible for elements of the ECM to change during the simulation depending on the state of the battery. Each xml element describing a parameter object has the attribute "class" to determine the object type. To make object values dependent on the battery state, [object referencing](xmlsprache.html#xml_object_refs) is used.

Constant value
==========================
An object with constant value is added by setting the "class" attribute to **ConstObj**.

XML Tag | Type   | Explanation                                                                                 
--------|--------|----------------------------------------
Value   | double | constant value of the parameter object

Definition of a constant parameter object:
\htmlinclude constobj_color.xml

1D lookup parameter object with state binding
==========
XML Tag                 | Type                              | Default value       | Explanation
------------------------|-----------------------------------|---------------------|----------------------------------------------------
Attribute LookupType    | [Lookup type](xmllookuptype.html) | LinearInterpolation | type of interpolation between data points
State                   | [State object](xmlstate.html)     |                     | state that determines the position in the 1D lookup
LookupData              | comma-separated doubles           |                     | values of the lookup
MeasurementPoints       | comma-separated doubles           |                     | measurement points of the lookup

Definition of a 1D parameter object with state binding:

\htmlinclude 1dlookup_color.xml

2D lookup parameter object with state binding
==========
XML Tag                 | Type                              | Default value       | Explanation
------------------------|-----------------------------------|---------------------|----------------------------------------------------
Attribute LookupType    | [Lookup type](xmllookuptype.html) | LinearInterpolation | type of interpolation between data points
RowState                | [State object](xmlstate.html)     |                     | state that is contant along each row of the table
ColState                | [State object](xmlstate.html)     |                     | state that is constant along each column of the table
LookupData              | comma-separated doubles           |                     | values of the lookup table, rows of the table are seperated by semicolons
MeasurementPointsRow    | comma-separated doubles           |                     | measurement points of the lookup table columns
MeasurementPointsColumn | comma-separated doubles           |                     | measurement points of the lookup table rows

Definition of a 2D parameter object with state binding:

\htmlinclude 2dlookup_color.xml

Expression object
=================
This object can be used to get a value from a mathematical expression.

XML Tag                 | Type                                   | Explanation
------------------------|----------------------------------------|----------------------------------------------------
Expression              | mathematical expression                | expression that is evaluated each time the object value is needed
Rescale                 | rescale (see below)                    | rescaling of the calculated value
Parameters              | list of [state objects](xmlstate.html) | list of states that are used as parameters in the expression
- Attribute name        | string                                 | variable name used in the expression. If this attribute is not set, but the attribute cacheref is used, that name is used instead 
- Rescale               | rescale (see below)                    | rescaling of the parameter before the expression is evaluated

To evaluate the expressions, the library [exprtk](https://github.com/ArashPartow/exprtk) is used. The documentation provides a list of operations that can be used in the expressions.

\htmlinclude expression_obj_color.xml

Rescaling
--------------

The input parameters as well as the calculated output value of the expression object can be rescaled by specifying an input range that is mapped to an output range. This is done by adding an element with the tag Rescale that contains the following children:

XML Tag            | Type   | Explanation
-------------------|--------|-------------------------------
InputRangeMinimum  | double | lower end of the input range
InputRangeMaximum  | double | upper end of the input range
OutputRangeMinimum | double | lower end of the output range
OutputRangeMaximum | double | upper end of the output range

The value is rescaled according to the formula
\f{eqnarray*}{
f(x) = (x - InputRangeMinimum) \frac{OutputRangeMaximum - OutputRangeMinimum}{InputRangeMaximum - InputRangeMinimum} + OutputRangeMinimum
\f}

\htmlinclude expression_obj_rescale_color.xml

Combination of objects
==========================
The values of multiple objects can be combined to produce a single value.

XML Tag                 | Type                              | Explanation
------------------------|-----------------------------------|----------------------------------------------------
Attribute Operation     | Add, Multiply or Divide           | specifies how the object values are combined
Children                | list of parameter objects         | objects that are combined

\htmlinclude multiobj_color.xml