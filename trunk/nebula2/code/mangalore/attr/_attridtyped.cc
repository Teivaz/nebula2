//------------------------------------------------------------------------------
//  attr/_attrid.cc
//  (C) 2005 Radon Labs GmbH
//------------------------------------------------------------------------------
#include "attr/_attridtyped.h"

// explicitly instantiate template classes for all possible types:

template class _attridTyped<attr::VoidT>;
template class _attridTyped<attr::BoolT>;
template class _attridTyped<attr::IntT>;
template class _attridTyped<attr::FloatT>;
template class _attridTyped<attr::StringT>;
template class _attridTyped<attr::Vector3T>;
template class _attridTyped<attr::Vector4T>;
template class _attridTyped<attr::Matrix44T>;
