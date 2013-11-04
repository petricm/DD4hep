// $Id: Geant4Hits.h 513 2013-04-05 14:31:53Z gaede $
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_DDG4_GEANT4CALLBACK_H
#define DD4HEP_DDG4_GEANT4CALLBACK_H

// C/C++ include files
#include <algorithm>
#include <typeinfo>
#include <vector>

/*
 *   DD4hep namespace declaration
 */
namespace DD4hep {

  /**@class Callback  Callback.h CPP/Callback.h
   *
   *  Definition of the generic callback structure for member functions
   *
   * @author  M.Frank
   * @date    01/03/2013
   * @version 0.1
   */
  struct Callback {
    typedef unsigned long (*func_t)(void* obj, const void* fun, const void* args[]);
    typedef struct { void *first, *second; } mfunc_t;
    typedef unsigned long ulong;

    void*   par;
    func_t  call;
    mfunc_t func;
    Callback() : par(0), call(0) { func.first = func.second = 0; }
    Callback(void* p) : par(p), call(0) { func.first = func.second = 0; }
    Callback(void* p, void* mf, func_t c) : par(p), call(c) { func = *(mfunc_t*)mf; }

    operator bool() const { return (call && par && func.first); }
    unsigned long execute(const void* user_param[]) const { return (*this) ? call(par, &func, user_param) : 0; }

    template <typename T> static T* cast(void* p) { return (T*)p; }
    template <typename T> static const T* c_cast(const void* p) { return (const T*)p; }
    template <typename T> struct Wrapper {
    public:
      typedef T pmf_t;
      union Functor {
        mfunc_t ptr;
        pmf_t   pmf;
        Functor(const void* f) { ptr = *(mfunc_t*)f; }
        Functor(pmf_t f) { pmf = f; }
      };
      static mfunc_t pmf(pmf_t f) {
        const Functor func(f);
        return func.ptr;
      }
    };
    //
    // Callback with no arguments
    //
    template <typename T> const Callback& _make(ulong (*fptr)(void* o, const void* f, const void* u[]), T pmf) {
      typename Wrapper<T>::Functor f(pmf);
      func = f.ptr;
      call = fptr;
      return *this;
    }
    template <typename R, typename T> const Callback& make(R (T::*pmf)()) {
      typedef R (T::*pfunc_t)();
      struct _Wrapper : public Wrapper<pfunc_t> {
        static ulong call(void* o, const void* f, const void* []) {
          return (ulong)(cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))();
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    template <typename R, typename T> const Callback& make(R (T::*pmf)() const) {
      typedef R (T::*pfunc_t)() const;
      struct _Wrapper : public Wrapper<pfunc_t> {
        static ulong call(void* o, const void* f, const void* []) {
          return (ulong)(cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))();
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    template <typename T> const Callback& make(void (T::*pmf)()) {
      typedef void (T::*pfunc_t)() const;
      struct _Wrapper : public Wrapper<pfunc_t> {
        static ulong call(void* o, const void* f, const void* []) {
          (cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))();
          return 1;
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    template <typename T> const Callback& make(void (T::*pmf)() const) {
      typedef void (T::*pfunc_t)() const;
      struct _Wrapper : public Wrapper<pfunc_t> {
        static ulong call(void* o, const void* f, const void* []) {
          (cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))();
          return 1;
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    //
    // Callback with 1 argument
    //
    template <typename R, typename T, typename A> const Callback& make(R (T::*pmf)(A)) {
      typedef R (T::*pfunc_t)(A);
      struct _Wrapper : public Wrapper<pfunc_t> {
        static ulong call(void* o, const void* f, const void* u[]) {
          return (ulong)(cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))((A)u[0]);
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    template <typename R, typename T, typename A> const Callback& make(R (T::*pmf)(A) const) {
      typedef R (T::*pfunc_t)(A) const;
      struct _Wrapper : public Wrapper<pfunc_t> {
        static ulong call(void* o, const void* f, const void* u[]) {
          return (ulong)(cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))((A)u[0]);
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    template <typename T, typename A> const Callback& make(void (T::*pmf)(A)) {
      typedef void (T::*pfunc_t)(const A);
      struct _Wrapper : public Wrapper<pfunc_t> {
        static ulong call(void* o, const void* f, const void* u[]) {
          (cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))((A)u[0]);
          return 1;
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    template <typename T, typename A> const Callback& make(void (T::*pmf)(A) const) {
      typedef void (T::*pfunc_t)(const A) const;
      struct _Wrapper : public Wrapper<pfunc_t> {
        static ulong call(void* o, const void* f, const void* u[]) {
          (cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))((A)u[0]);
          return 1;
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    //
    // Callback with 3 arguments
    //
    template <typename R, typename T, typename A0, typename A1> const Callback& make(R (T::*pmf)(A0, A1)) {
      typedef R (T::*          pfunc_t)(A0, A1);
      typedef Wrapper<pfunc_t> _W;
      struct _Wrapper : public _W {
        static ulong call(void* o, const void* f, const void* u[]) {
          return (ulong)(cast<T>(o)->*(typename _W::Functor(f).pmf))((A0)u[0], (A1)u[1]);
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    template <typename R, typename T, typename A0, typename A1> const Callback& make(R (T::*pmf)(A0, A1) const) {
      typedef R (T::*          pfunc_t)(A0, A1);
      typedef Wrapper<pfunc_t> _W;
      struct _Wrapper : public _W {
        static ulong call(void* o, const void* f, const void* u[]) {
          return (ulong)(cast<T>(o)->*(typename _W::Functor(f).pmf))((A0)u[0], (A1)u[1]);
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    template <typename T, typename A0, typename A1> const Callback& make(void (T::*pmf)(A0, A1)) {
      typedef void (T::*       pfunc_t)(A0, A1);
      typedef Wrapper<pfunc_t> _W;
      struct _Wrapper : public _W {
        static ulong call(void* o, const void* f, const void* u[]) {
          (cast<T>(o)->*(typename _W::Functor(f).pmf))((A0)u[0], (A1)u[1]);
          return 1;
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    template <typename T, typename A0, typename A1> const Callback& make(void (T::*pmf)(A0, A1) const) {
      typedef void (T::*       pfunc_t)(A0, A1);
      typedef Wrapper<pfunc_t> _W;
      struct _Wrapper : public _W {
        static ulong call(void* o, const void* f, const void* u[]) {
          (cast<T>(o)->*(typename _W::Functor(f).pmf))((A0)u[0], (A1)u[1]);
          return 1;
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    //
    // Callback with 3 arguments
    //
    template <typename R, typename T, typename A0, typename A1, typename A2> const Callback& make(R (T::*pmf)(A0, A1, A2)) {
      typedef R (T::*          pfunc_t)(A0, A1);
      typedef Wrapper<pfunc_t> _W;
      struct _Wrapper : public _W {
        static ulong call(void* o, const void* f, const void* u[]) {
          return (ulong)(cast<T>(o)->*(typename _W::Functor(f).pmf))((A0)u[0], (A1)u[1], (A2)u[2]);
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    template <typename R, typename T, typename A0, typename A1, typename A2>
    const Callback& make(R (T::*pmf)(A0, A1, A2) const) {
      typedef R (T::*          pfunc_t)(A0, A1);
      typedef Wrapper<pfunc_t> _W;
      struct _Wrapper : public _W {
        static ulong call(void* o, const void* f, const void* u[]) {
          return (ulong)(cast<T>(o)->*(typename _W::Functor(f).pmf))((A0)u[0], (A1)u[1], (A2)u[2]);
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    template <typename T, typename A0, typename A1, typename A2> const Callback& make(void (T::*pmf)(A0, A1, A2)) {
      typedef void (T::*       pfunc_t)(A0, A1);
      typedef Wrapper<pfunc_t> _W;
      struct _Wrapper : public _W {
        static ulong call(void* o, const void* f, const void* u[]) {
          (cast<T>(o)->*(typename _W::Functor(f).pmf))((A0)u[0], (A1)u[1], (A2)u[2]);
          return 1;
        }
      };
      return _make(_Wrapper::call, pmf);
    }
    template <typename T, typename A0, typename A1, typename A2> const Callback& make(void (T::*pmf)(A0, A1, A2) const) {
      typedef void (T::*       pfunc_t)(A0, A1);
      typedef Wrapper<pfunc_t> _W;
      struct _Wrapper : public _W {
        static ulong call(void* o, const void* f, const void* u[]) {
          (cast<T>(o)->*(typename _W::Functor(f).pmf))((A0)u[0], (A1)u[1], (A2)u[2]);
          return 1;
        }
      };
      return _make(_Wrapper::call, pmf);
    }

    template <typename T> static Callback make(void* p, T pmf) { return Callback(p).make(pmf); }

    template <typename P, typename R, typename T> static T* dyn_cast(P* p, R (T::*)()) { return dynamic_cast<T*>(p); }
    template <typename P, typename R, typename T> static const T* dyn_cast(const P* p, R (T::*)() const) {
      return dynamic_cast<const T*>(p);
    }

    template <typename P, typename R, typename T, typename A> static T* dyn_cast(P* p, R (T::*)(A)) {
      return dynamic_cast<T*>(p);
    }
    template <typename P, typename R, typename T, typename A> static const T* dyn_cast(const P* p, R (T::*)(A) const) {
      return dynamic_cast<const T*>(p);
    }
  };

  struct CallbackSequence {
    typedef std::vector<Callback> Callbacks;
    Callbacks                     callbacks;
    CallbackSequence() {}
    CallbackSequence(const CallbackSequence& c) : callbacks(c.callbacks) {}
    //template <typename TYPE, typename R, typename OBJECT>
    //  CallbackSequence(const std::vector<TYPE*>& objects, R (TYPE::value_type::*pmf)())  {
    //}
    bool empty() const { return callbacks.empty(); }
    void clear() { callbacks.clear(); }
    void add(const Callback&    cb) { callbacks.push_back(cb); }
    void                        operator()() const;
    template <typename A0> void operator()(A0 a0) const;
    template <typename A0, typename A1> void operator()(A0 a0, A1 a1) const;
    template <typename A0, typename A1, typename A2> void operator()(A0 a0, A1 a1, A2 a2) const;
    /// Check the compatibility of two typed objects. The test is the result of a dynamic_cast
    static void checkTypes(const std::type_info& typ1, const std::type_info& typ2, void* test);

    template <typename TYPE, typename R, typename OBJECT> void add(TYPE* pointer, R (OBJECT::*pmf)()) {
      checkTypes(typeid(TYPE), typeid(OBJECT), dynamic_cast<OBJECT*>(pointer));
      add(Callback(pointer).make(pmf));
    }
    template <typename TYPE, typename R, typename OBJECT, typename A> void add(TYPE* pointer, R (OBJECT::*pmf)(A)) {
      checkTypes(typeid(TYPE), typeid(OBJECT), dynamic_cast<OBJECT*>(pointer));
      add(Callback(pointer).make(pmf));
    }
    template <typename TYPE, typename R, typename OBJECT, typename A1, typename A2>
    void add(TYPE* pointer, R (OBJECT::*pmf)(A1, A2)) {
      checkTypes(typeid(TYPE), typeid(OBJECT), dynamic_cast<OBJECT*>(pointer));
      add(Callback(pointer).make(pmf));
    }
  };

  inline void CallbackSequence::operator()() const {
    if (!callbacks.empty()) {
      const void* args[1] = {0};
      for (Callbacks::const_iterator i = callbacks.begin(); i != callbacks.end(); ++i)
        (*i).execute(args);
    }
  }
  template <typename A0> inline void CallbackSequence::operator()(A0 a0) const {
    if (!callbacks.empty()) {
      const void* args[1] = {a0};
      for (Callbacks::const_iterator i = callbacks.begin(); i != callbacks.end(); ++i)
        (*i).execute(args);
    }
  }
  template <typename A0, typename A1> inline void CallbackSequence::operator()(A0 a0, A1 a1) const {
    if (!callbacks.empty()) {
      const void* args[2] = {a0, a1};
      for (Callbacks::const_iterator i = callbacks.begin(); i != callbacks.end(); ++i)
        (*i).execute(args);
    }
  }
  template <typename A0, typename A1, typename A2> inline void CallbackSequence::operator()(A0 a0, A1 a1, A2 a2) const {
    if (!callbacks.empty()) {
      const void* args[3] = {a0, a1, a2};
      for (Callbacks::const_iterator i = callbacks.begin(); i != callbacks.end(); ++i)
        (*i).execute(args);
    }
  }

}  // End namespace DD4hep
#endif  // DD4HEP_DDG4_GEANT4CALLBACK_H
