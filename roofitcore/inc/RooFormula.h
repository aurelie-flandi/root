/*****************************************************************************
 * Project: RooFit                                                           *
 * Package: RooFitCore                                                       *
 *    File: $Id: RooFormula.rdl,v 1.26 2002/09/05 04:33:27 verkerke Exp $
 * Authors:                                                                  *
 *   WV, Wouter Verkerke, UC Santa Barbara, verkerke@slac.stanford.edu       *
 *   DK, David Kirkby,    UC Irvine,         dkirkby@uci.edu                 *
 *                                                                           *
 * Copyright (c) 2000-2002, Regents of the University of California          *
 *                          and Stanford University. All rights reserved.    *
 *                                                                           *
 * Redistribution and use in source and binary forms,                        *
 * with or without modification, are permitted according to the terms        *
 * listed in LICENSE (http://roofit.sourceforge.net/license.txt)             *
 *****************************************************************************/
#ifndef ROO_FORMULA
#define ROO_FORMULA

#include "Rtypes.h"
#include "TFormula.h"
#include "RooFitCore/RooAbsReal.hh"
#include "RooFitCore/RooArgSet.hh"
#include "RooFitCore/RooPrintable.hh"
#include "RooFitCore/RooLinkedList.hh"

class RooFormula : public TFormula, public RooPrintable {
public:
  // Constructors etc.
  RooFormula() ;
  RooFormula(const char* name, const char* formula, const RooArgList& varList);
  RooFormula(const RooFormula& other, const char* name=0) ;
  virtual TObject* Clone(const char* newName=0) const { return new RooFormula(*this) ; }
  virtual ~RooFormula();
	
  // Dependent management
  RooArgSet& actualDependents() const ;
  Bool_t changeDependents(const RooAbsCollection& newDeps, Bool_t mustReplaceAll, Bool_t nameChange) ;

  inline RooAbsArg* getParameter(const char* name) const { return (RooAbsArg*) _useList.FindObject(name) ; }
  inline RooAbsArg* getParameter(Int_t index) const { return (RooAbsArg*) _origList.At(index) ; }

  // Function value accessor
  inline Bool_t ok() { return _isOK ; }
  Double_t eval(const RooArgSet* nset=0) ;

  // Debugging
  void dump() ;
  Bool_t reCompile(const char* newFormula) ;

  // Printing interface (human readable)
  virtual void printToStream(ostream& os, PrintOption opt= Standard, TString indent= "") const;
  inline virtual void Print(Option_t *options= 0) const {
    printToStream(defaultStream(),parseOptions(options));
  }

protected:
  
  RooFormula& operator=(const RooFormula& other) ;
  void initCopy(const RooFormula& other) ;

  // Interface to TFormula engine
  Int_t DefinedVariable(TString &name) ;
  Double_t DefinedValue(Int_t code) ;

  RooArgSet* _nset ;
  mutable Bool_t    _isOK ;
  RooLinkedList     _origList ; //! Original list of dependents
  RooLinkedList _useList ;      //! List of actual dependents 
  mutable RooArgSet _actual;    //! Set of actual dependents
  RooLinkedList _labelList ;    //  List of label names for category objects  
  mutable Bool_t    _compiled ; //  Flag set if formula is compiled

  ClassDef(RooFormula,1)     // TFormula derived class interfacing with RooAbsArg objects
};

#endif
