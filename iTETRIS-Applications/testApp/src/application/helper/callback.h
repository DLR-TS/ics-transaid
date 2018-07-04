/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2005,2006 INRIA
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
/****************************************************************************************
 * Edited by Federico Caselli <f.caselli@unibo.it>
 * University of Bologna 2015
 * FP7 ICT COLOMBO project under the Framework Programme,
 * FP7-ICT-2011-8, grant agreement no. 318622.
***************************************************************************************/
#ifndef CALLBACK_H
#define CALLBACK_H

#include "empty.h"
#include <typeinfo>
#include <string>

namespace protocol
{
namespace application
{
/**
 * \ingroup core
 * \defgroup callback Callbacks
 */
/**
 * \ingroup callback
 */
/**@{*/
/**
 * Trait class to convert a pointer into a reference,
 * used by MemPtrCallBackImpl
 */
template<typename T>
struct CallbackTraits;

/**
 * Trait class to convert a pointer into a reference,
 * used by MemPtrCallBackImpl
 */
template<typename T>
struct CallbackTraits<T *>
{
  /**
   * \param p object pointer
   * \return a reference to the object pointed to by p
   */
  static T & GetReference(T * const p)
  {
    return *p;
  }
};
/**@}*/

/**
 * \ingroup callback
 * \defgroup callbackimpl CallbackImpl
 * CallbackImpl classes
 */
/**
 * \ingroup callbackimpl
 * Abstract base class for CallbackImpl
 * Provides reference counting and equality test.
 */
class CallbackImplBase
{
public:
  /** Virtual destructor */
  virtual ~CallbackImplBase()
  {
  }
  /**
   * Equality test
   *
   * \param other Callback Ptr
   * \return true if we are equal
   */
  virtual bool IsEqual(const CallbackImplBase* other) const = 0;
};

/**
 * \ingroup callbackimpl
 * The unqualified CallbackImpl class
 */
template<typename R, typename T1, typename T2, typename T3, typename T4>
class CallbackImpl;

/**
 * \ingroup callbackimpl
 * CallbackImpl classes with varying numbers of argument types
 */
/**@{*/
/** CallbackImpl class with no arguments. */
template<typename R>
class CallbackImpl<R, empty, empty, empty, empty> : public CallbackImplBase
{
public:
  virtual ~CallbackImpl()
  {
  }
  virtual R operator()(void) = 0;      //!< Abstract operator
};
/** CallbackImpl class with one argument. */
template<typename R, typename T1>
class CallbackImpl<R, T1, empty, empty, empty> : public CallbackImplBase
{
public:
  virtual ~CallbackImpl()
  {
  }
  virtual R operator()(T1) = 0;        //!< Abstract operator
};
/** CallbackImpl class with two arguments. */
template<typename R, typename T1, typename T2>
class CallbackImpl<R, T1, T2, empty, empty> : public CallbackImplBase
{
public:
  virtual ~CallbackImpl()
  {
  }
  virtual R operator()(T1, T2) = 0;    //!< Abstract operator
};
/** CallbackImpl class with three arguments. */
template<typename R, typename T1, typename T2, typename T3>
class CallbackImpl<R, T1, T2, T3, empty> : public CallbackImplBase
{
public:
  virtual ~CallbackImpl()
  {
  }
  virtual R operator()(T1, T2, T3) = 0;  //!< Abstract operator
};
/** CallbackImpl class with four arguments. */
template<typename R, typename T1, typename T2, typename T3, typename T4>
class CallbackImpl: public CallbackImplBase
{
public:
  virtual ~CallbackImpl()
  {
  }
  virtual R operator()(T1, T2, T3, T4) = 0;  //!< Abstract operator
};

/**
 * \ingroup callback
 * CallbackImpl for pointer to member functions
 */
template<typename OBJ_PTR, typename MEM_PTR, typename R, typename T1, typename T2, typename T3, typename T4>
class MemPtrCallbackImpl: public CallbackImpl<R, T1, T2, T3, T4>
{
public:
  /**
   * Construct from an object pointer and member function pointer
   *
   * \param objPtr the object pointer
   * \param memPtr the object class member function
   */
  MemPtrCallbackImpl(OBJ_PTR const&objPtr, MEM_PTR memPtr) :
      m_objPtr(objPtr), m_memPtr(memPtr)
  {
  }
  virtual ~MemPtrCallbackImpl()
  {
  }
  /**
   * Functor with varying numbers of arguments
   * @{
   */
  /** \return Callback value */
  R operator()(void)
  {
    return ((CallbackTraits<OBJ_PTR>::GetReference(m_objPtr)).*m_memPtr)();
  }
  /**
   * \param a1 first argument
   * \return Callback value
   */
  R operator()(T1 a1)
  {
    return ((CallbackTraits<OBJ_PTR>::GetReference(m_objPtr)).*m_memPtr)(a1);
  }
  /**
   * \param a1 first argument
   * \param a2 second argument
   * \return Callback value
   */
  R operator()(T1 a1, T2 a2)
  {
    return ((CallbackTraits<OBJ_PTR>::GetReference(m_objPtr)).*m_memPtr)(a1, a2);
  }
  /**
   * \param a1 first argument
   * \param a2 second argument
   * \param a3 third argument
   * \return Callback value
   */
  R operator()(T1 a1, T2 a2, T3 a3)
  {
    return ((CallbackTraits<OBJ_PTR>::GetReference(m_objPtr)).*m_memPtr)(a1, a2, a3);
  }
  /**
   * \param a1 first argument
   * \param a2 second argument
   * \param a3 third argument
   * \param a4 fourth argument
   * \return Callback value
   */
  R operator()(T1 a1, T2 a2, T3 a3, T4 a4)
  {
    return ((CallbackTraits<OBJ_PTR>::GetReference(m_objPtr)).*m_memPtr)(a1, a2, a3, a4);
  }
  /**@}*/
  /**
   * Equality test.
   *
   * \param other Callback Ptr
   * \return true if we have the same object and member function
   */
  virtual bool IsEqual(const CallbackImplBase* other) const
  {
    MemPtrCallbackImpl<OBJ_PTR, MEM_PTR, R, T1, T2, T3, T4> const *otherDerived = dynamic_cast<MemPtrCallbackImpl<
        OBJ_PTR, MEM_PTR, R, T1, T2, T3, T4> const *>(other);
    if (otherDerived == 0)
    {
      return false;
    }
    else if (otherDerived->m_objPtr != m_objPtr || otherDerived->m_memPtr != m_memPtr)
    {
      return false;
    }
    return true;
  }
private:
  OBJ_PTR const m_objPtr;               //!< the object pointer
  MEM_PTR m_memPtr;                     //!< the member function pointer
};

/**
 * \ingroup callback
 * Base class for Callback class.
 * Provides pimpl abstraction.
 */
class CallbackBase
{
public:
  CallbackBase() :
      m_impl()
  {
  }
  void Delete()
  {
    delete m_impl;
    delete this;
  }

  /** \return the impl pointer */
  CallbackImplBase* GetImpl(void) const
  {
    return m_impl;
  }
protected:
  /**
   * Construct from a pimpl
   * \param impl the CallbackImplBase Ptr
   */
  CallbackBase(CallbackImplBase* impl) :
      m_impl(impl)
  {
  }
  CallbackImplBase* m_impl;         //!< the pimpl
};

/**
 * \ingroup callback
 * \brief Callback template class
 *
 * This class template implements the Functor Design Pattern.
 * It is used to declare the type of a Callback:
 *  - the first non-optional template argument represents
 *    the return type of the callback.
 *  - the remaining (optional) template arguments represent
 *    the type of the subsequent arguments to the callback.
 *  - up to nine arguments are supported.
 *
 * Callback instances are built with the \ref MakeCallback
 * template functions. Callback instances have POD semantics:
 * the memory they allocate is managed automatically, without
 * user intervention which allows you to pass around Callback
 * instances by value.
 *
 * Sample code which shows how to use this class template 
 * as well as the function templates \ref MakeCallback :
 * \include src/core/examples/main-callback.cc
 *
 * \internal
 * This code was originally written based on the techniques 
 * described in http://www.codeproject.com/cpp/TTLFunction.asp
 * It was subsequently rewritten to follow the architecture
 * outlined in "Modern C++ Design" by Andrei Alexandrescu in 
 * chapter 5, "Generalized Functors".
 *
 * This code uses:
 *   - default template parameters to saves users from having to
 *     specify empty parameters when the number of parameters
 *     is smaller than the maximum supported number
 *   - the pimpl idiom: the Callback class is passed around by 
 *     value and delegates the crux of the work to its pimpl
 *     pointer.
 *   - two pimpl implementations which derive from CallbackImpl
 *     FunctorCallbackImpl can be used with any functor-type
 *     while MemPtrCallbackImpl can be used with pointers to
 *     member functions.
 *   - a reference list implementation to implement the Callback's
 *     value semantics.
 *
 * This code most notably departs from the alexandrescu 
 * implementation in that it does not use type lists to specify
 * and pass around the types of the callback arguments.
 * Of course, it also does not use copy-destruction semantics
 * and relies on a reference list rather than autoPtr to hold
 * the pointer.
 */
template<typename R, typename T1 = empty, typename T2 = empty, typename T3 = empty, typename T4 = empty>
class Callback: public CallbackBase
{
public:
  Callback()
  {
  }

  /**
   * Construct a member function pointer call back.
   *
   * \param objPtr pointer to the object
   * \param memPtr  pointer to the member function
   */
  template<typename OBJ_PTR, typename MEM_PTR>
  Callback(OBJ_PTR const &objPtr, MEM_PTR memPtr) :
      CallbackBase(new MemPtrCallbackImpl<OBJ_PTR, MEM_PTR, R, T1, T2, T3, T4>(objPtr, memPtr))
  {
  }

  /**
   * Check for null implementation
   *
   * \return true if I don't have an implementation
   */
  bool IsNull(void) const
  {
    return (DoPeekImpl() == 0) ? true : false;
  }
  /** Discard the implementation, set it to null */
  void Nullify(void)
  {
    m_impl = 0;
  }

  /**
   * Functor with varying numbers of arguments
   * @{
   */
  /** \return Callback value */
  R operator()(void) const
  {
    return (*(DoPeekImpl()))();
  }
  /**
   * \param a1 first argument
   * \return Callback value
   */
  R operator()(T1 a1) const
  {
    return (*(DoPeekImpl()))(a1);
  }
  /**
   * \param a1 first argument
   * \param a2 second argument
   * \return Callback value
   */
  R operator()(T1 a1, T2 a2) const
  {
    return (*(DoPeekImpl()))(a1, a2);
  }
  /**
   * \param a1 first argument
   * \param a2 second argument
   * \param a3 third argument
   * \return Callback value
   */
  R operator()(T1 a1, T2 a2, T3 a3) const
  {
    return (*(DoPeekImpl()))(a1, a2, a3);
  }
  /**
   * \param a1 first argument
   * \param a2 second argument
   * \param a3 third argument
   * \param a4 fourth argument
   * \return Callback value
   */
  R operator()(T1 a1, T2 a2, T3 a3, T4 a4) const
  {
    return (*(DoPeekImpl()))(a1, a2, a3, a4);
  }

  /**
   * Equality test.
   *
   * \param other Callback
   * \return true if we are equal
   */
  bool IsEqual(const CallbackBase &other) const
  {
    return m_impl->IsEqual(other.GetImpl());
  }

  /**
   * Check for compatible types
   *
   * \param other Callback Ptr
   * \return true if other can be dynamic_cast to my type
   */
  bool CheckType(const CallbackBase & other) const
  {
    return DoCheckType(other.GetImpl());
  }
  /**
   * Adopt the other's implementation, if type compatible
   *
   * \param other Callback
   */
  void Assign(const CallbackBase &other)
  {
    DoAssign(other.GetImpl());
  }
private:
  /** \return the pimpl pointer */
  CallbackImpl<R, T1, T2, T3, T4> *DoPeekImpl(void) const
  {
    return static_cast<CallbackImpl<R, T1, T2, T3, T4> *>(m_impl);
  }
  /**
   * Check for compatible types
   *
   * \param other Callback Ptr
   * \return true if other can be dynamic_cast to my type
   */
  bool DoCheckType(const CallbackImplBase* other) const
  {
    if (other != 0 && dynamic_cast<const CallbackImpl<R, T1, T2, T3, T4> *>(other) != 0)
    {
      return true;
    }
    else if (other == 0)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  /**
   * Adopt the other's implementation, if type compatible
   *
   * \param other Callback Ptr to adopt from
   */
  void DoAssign(const CallbackImplBase* other)
  {
    if (!DoCheckType(other))
    {
      std::terminate();
//      NS_FATAL_ERROR(
//          "Incompatible types. (feed to \"c++filt -t\" if needed)" << std::endl << "got="
//              << Demangle(typeid (*other).name()) << std::endl << "expected="
//              << Demangle(typeid(CallbackImpl<R, T1, T2, T3, T4, T5, T6, T7, T8, T9> *).name()));
    }
    m_impl = const_cast<CallbackImplBase *>(other);
  }
};

/**
 * Inequality test.
 *
 * \param a Callback
 * \param b Callback
 *
 * \return true if the Callbacks are not equal
 */
template<typename R, typename T1, typename T2, typename T3, typename T4>
bool operator !=(Callback<R, T1, T2, T3, T4> a, Callback<R, T1, T2, T3, T4> b)
{
  return !a.IsEqual(b);
}

/**
 * \ingroup callback
 * \defgroup makecallbackmemptr MakeCallback from member function pointer
 *
 * Build Callbacks for class method members which take varying numbers of arguments
 * and potentially returning a value.
 */
/**
 * \ingroup makecallbackmemptr
 * @{
 */
/**
 * \param memPtr class method member pointer
 * \param objPtr class instance
 * \return a wrapper Callback
 * 
 * Build Callbacks for class method members which take varying numbers of arguments
 * and potentially returning a value.
 */
template<typename T, typename OBJ, typename R>
Callback<R> MakeCallback(R (T::*memPtr)(void), OBJ objPtr)
{
  return Callback<R>(objPtr, memPtr);
}
template<typename T, typename OBJ, typename R>
Callback<R> MakeCallback(R (T::*memPtr)() const, OBJ objPtr)
{
  return Callback<R>(objPtr, memPtr);
}
template<typename T, typename OBJ, typename R, typename T1>
Callback<R, T1> MakeCallback(R (T::*memPtr)(T1), OBJ objPtr)
{
  return Callback<R, T1>(objPtr, memPtr);
}
template<typename T, typename OBJ, typename R, typename T1>
Callback<R, T1> MakeCallback(R (T::*memPtr)(T1) const, OBJ objPtr)
{
  return Callback<R, T1>(objPtr, memPtr);
}
template<typename T, typename OBJ, typename R, typename T1, typename T2>
Callback<R, T1, T2> MakeCallback(R (T::*memPtr)(T1, T2), OBJ objPtr)
{
  return Callback<R, T1, T2>(objPtr, memPtr);
}
template<typename T, typename OBJ, typename R, typename T1, typename T2>
Callback<R, T1, T2> MakeCallback(R (T::*memPtr)(T1, T2) const, OBJ objPtr)
{
  return Callback<R, T1, T2>(objPtr, memPtr);
}
template<typename T, typename OBJ, typename R, typename T1, typename T2, typename T3>
Callback<R, T1, T2, T3> MakeCallback(R (T::*memPtr)(T1, T2, T3), OBJ objPtr)
{
  return Callback<R, T1, T2, T3>(objPtr, memPtr);
}
template<typename T, typename OBJ, typename R, typename T1, typename T2, typename T3>
Callback<R, T1, T2, T3> MakeCallback(R (T::*memPtr)(T1, T2, T3) const, OBJ objPtr)
{
  return Callback<R, T1, T2, T3>(objPtr, memPtr);
}
template<typename T, typename OBJ, typename R, typename T1, typename T2, typename T3, typename T4>
Callback<R, T1, T2, T3, T4> MakeCallback(R (T::*memPtr)(T1, T2, T3, T4), OBJ objPtr)
{
  return Callback<R, T1, T2, T3, T4>(objPtr, memPtr);
}
template<typename T, typename OBJ, typename R, typename T1, typename T2, typename T3, typename T4>
Callback<R, T1, T2, T3, T4> MakeCallback(R (T::*memPtr)(T1, T2, T3, T4) const, OBJ objPtr)
{
  return Callback<R, T1, T2, T3, T4>(objPtr, memPtr);
}

} /* namespace application */
} /* namespace protocol */
#endif /* CALLBACK_H */
