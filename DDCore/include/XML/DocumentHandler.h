// $Id$
//====================================================================
//  AIDA Detector description implementation
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================
#ifndef DD4HEP_XML_DOCUMENTHANDLER_H
#define DD4HEP_XML_DOCUMENTHANDLER_H

// Framework include files
#include "XML/XMLElements.h"

// C/C++ include files
#include <memory>

/// Namespace for the AIDA detector description toolkit
namespace DD4hep {

  /// Namespace containing utilities to parse XML files using XercesC or TinyXML
  namespace XML {

    // Forward declarations
    struct DocumentErrorHandler;

    /// Class supporting to read and parse XML documents.
    /** 
     *  Wrapper object around the document parser.
     *  Supports both, XercesC and TiXml.
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    class DocumentHandler {
    protected:
      /// Reference to the error handler object
      std::auto_ptr<DocumentErrorHandler> m_errHdlr;

    public:
      /// Default constructor
      DocumentHandler();
      /// Default destructor
      virtual ~DocumentHandler();
      // Create new XML document by parsing empty xml buffer
      Document create(const char* tag, const char* comment = 0) const;
      /// Load XML file and parse it.
      virtual Document load(const std::string& fname) const;
      /// Load secondary XML file with relative addressing with respect to handle
      virtual Document load(Handle_t base, const XmlChar* fname) const;
      /// Parse a standalong XML string into a document.
      virtual Document parse(const char* doc_string, size_t length) const;
      /// Write xml document to output file (stdout if file name empty)
      virtual int output(Document doc, const std::string& fname) const;
    };
  }
} /* End namespace DD4hep            */
#endif /* DD4HEP_XML_DOCUMENTHANDLER_H    */
