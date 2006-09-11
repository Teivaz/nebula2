#ifndef ATTR__ATTR_CLASS_TYPES_H
#define ATTR__ATTR_CLASS_TYPES_H
/**
    @file attributeclasses.h

    @brief define type classes

    define empty "type" classes to allow class template for compile time safe
    ID classes (also see class _attridTyped in attridTyped.h).

    (C) 2005 Radon Labs GmbH
*/

namespace attr
{

class VoidT     {};
class BoolT     {};
class IntT      {};
class FloatT    {};
class StringT   {};
class Vector3T  {};
class Vector4T  {};
class Matrix44T {};

} // namespace attr

#endif
