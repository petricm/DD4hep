#include "DDSegmentation/BitField64.h"

#include <cmath>
#include <algorithm>
#include <stdexcept>

namespace dd4hep::DDSegmentation {
  

  auto BitField64::valueString() const -> std::string {

    std::stringstream  os ;

    for(unsigned i=0;i<size();i++){
      
      if( i != 0 )   os << "," ;

      os << _coder->fields()[i].name() <<  ":" << _coder->get( _value , i ) ;

    }
    return os.str() ;
  }
  

  auto operator<<(std::ostream& os, const BitField64& b) -> std::ostream&{

    os << " bitfield:  0x" << std::hex // << std::ios::width(16) << std::ios::fill('0') <<
       << b._value << std::dec << std::endl ;


    for(unsigned i=0;i<b._coder->size();i++){
      
      const BitFieldElement& bv = b._coder->fields()[i] ;
      
      os << "  " <<  bv.name()
	 << " [" <<  bv.offset()  << ":"  ;
      
      if(  bv.isSigned()  )  os << "-" ;

      os << bv.width() << "]  : "  ;
      
      os << b._coder->get( b._value , i)
	 << std::endl ;
      
    }
  
    return os ;
  } 

} // namespace
