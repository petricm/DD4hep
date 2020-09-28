// -*- C++ -*-
// ----------------------------------------------------------------------

#include "Evaluator/Evaluator.h"
#include <limits>

#ifdef DD4HEP_NONE
/// Utility namespace to support TGeo units.
namespace dd4hep {
  //namespace units  {
  static const double radian = 1. ;
  static const double degree = (3.14159265358979323846/180.0)*radian;
  //}
}
#else
#include "Evaluator/DD4hepUnits.h"
#endif
namespace units = dd4hep;

#include <cmath>        // for sqrt and pow
#include <cmath>

using namespace std;

static auto eval_abs  (double a) -> double           { return (a < 0) ? -a : a;        }
static auto eval_fabs (double a) -> double           { return std::fabs(a);            }
static auto eval_fmod (double a, double b) -> double { return std::fmod(a,b);          }
static auto eval_min  (double a, double b) -> double { return (a < b) ?  a : b;        }
static auto eval_max  (double a, double b) -> double { return (a > b) ?  a : b;        }
static auto eval_sqrt (double a) -> double           { return std::sqrt(a);            }
static auto eval_cbrt (double a) -> double           { return ::cbrt(a);               }
static auto eval_pow  (double a, double b) -> double { return std::pow(a,b);           }
static auto eval_sin  (double a) -> double           { return std::sin(a);             }
static auto eval_cos  (double a) -> double           { return std::cos(a);             }
static auto eval_tan  (double a) -> double           { return std::tan(a);             }
static auto eval_asin (double a) -> double           { return std::asin(a);            }
static auto eval_acos (double a) -> double           { return std::acos(a);            }
static auto eval_atan (double a) -> double           { return std::atan(a);            }
static auto eval_atan2(double a, double b) -> double { return std::atan2(a,b);         }
static auto eval_sinh (double a) -> double           { return std::sinh(a);            }
static auto eval_cosh (double a) -> double           { return std::cosh(a);            }
static auto eval_tanh (double a) -> double           { return std::tanh(a);            }
static auto eval_exp  (double a) -> double           { return std::exp(a);             }
static auto eval_exp2 (double a) -> double           { return ::exp2(a);               }
//static double eval_exp10(double a)           { return ::exp10(a);              }
static auto eval_exp10(double a) -> double           { return ::pow(10.,a) ;           }
static auto eval_expm1(double a) -> double           { return ::expm1(a);              }
static auto eval_log  (double a) -> double           { return std::log(a);             }
static auto eval_logb (double a) -> double           { return ::logb(a);               }
static auto eval_log10(double a) -> double           { return std::log10(a);           }
static auto eval_log1p(double a) -> double           { return ::log1p(a);              }
static auto eval_hypot(double a, double b) -> double { return ::hypot(a,b);            }
static auto eval_erf  (double a) -> double           { return ::erf(a);                }
static auto eval_erfc (double a) -> double           { return ::erfc(a);               }
static auto eval_gamma(double a) -> double           { return ::tgamma(a);             }
static auto eval_double(double a) -> double          { return a;                       }
static auto eval_int  (double a) -> double           { return (double)int(a);          }
static auto eval_nint (double a) -> double           { return std::floor(a);           }
static auto eval_floor(double a) -> double           { return std::floor(a);           }
static auto eval_round(double a) -> double           { return ::round(a);              }
static auto eval_ceil (double a) -> double           { return std::ceil(a);            }
static auto eval_isnan(double a) -> double           { return std::isnan(a);           }
static auto eval_isinf(double a) -> double           { return std::isinf(a);           }
static auto eval_nearbyint(double a) -> double       { return ::nearbyint(a);          }

/// Namespace for the AIDA detector description toolkit
namespace dd4hep::tools {

    void Evaluator::setStdMath() {

      //   S E T   S T A N D A R D   C O N S T A N T S

      setVariable("pi",     3.14159265358979323846);
      setVariable("e",      2.7182818284590452354);
      setVariable("gamma",  0.577215664901532861);


      // setVariable("radian", 1.0);
      // setVariable("rad",    1.0);
      // setVariable("degree", 3.14159265358979323846/180.);
      // setVariable("deg",    3.14159265358979323846/180.);
      setVariable("radian", units::radian );
      setVariable("rad",    units::radian );
      setVariable("degree", units::degree );
      setVariable("deg",    units::degree );
      setVariable("int:epsilon",    std::numeric_limits<int>::epsilon());
      setVariable("long:epsilon",   std::numeric_limits<long>::epsilon());
      setVariable("float:epsilon",  std::numeric_limits<float>::epsilon());
      setVariable("double:epsilon", std::numeric_limits<double>::epsilon());

      //   S E T   S T A N D A R D   F U N C T I O N S

      setFunction("abs",        eval_abs);
      setFunction("fabs",       eval_fabs);
      setFunction("fmod",       eval_fmod);
      setFunction("min",        eval_min);
      setFunction("max",        eval_max);
      setFunction("sqrt",       eval_sqrt);
      setFunction("cbrt",       eval_cbrt);
      setFunction("pow",        eval_pow);
      setFunction("sin",        eval_sin);
      setFunction("cos",        eval_cos);
      setFunction("tan",        eval_tan);
      setFunction("asin",       eval_asin);
      setFunction("acos",       eval_acos);
      setFunction("atan",       eval_atan);
      setFunction("atan2",      eval_atan2);
      setFunction("sinh",       eval_sinh);
      setFunction("cosh",       eval_cosh);
      setFunction("tanh",       eval_tanh);
      setFunction("exp",        eval_exp);
      setFunction("exp2",       eval_exp2);
      setFunction("exp10",      eval_exp10);
      setFunction("expm1",      eval_expm1);
      setFunction("log",        eval_log);
      setFunction("logb",       eval_logb);
      setFunction("log10",      eval_log10);
      setFunction("log1p",      eval_log1p);
      setFunction("hypot",      eval_hypot);
      setFunction("erf",        eval_erf);
      setFunction("erfc",       eval_erfc);
      setFunction("gamma",      eval_gamma);
      setFunction("double",     eval_double);
      setFunction("int",        eval_int);
      setFunction("nint",       eval_nint);
      setFunction("floor",      eval_floor);
      setFunction("round",      eval_round);
      setFunction("ceil",       eval_ceil);
      setFunction("isnan",      eval_isnan);
      setFunction("isinf",      eval_isinf);
      setFunction("nearbyint",  eval_nearbyint);
    }

  }  // namespace dd4hep
