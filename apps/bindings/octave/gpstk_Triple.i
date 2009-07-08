// swig file for Triple.cpp

%module gpstk_Triple

%{

#include "../../../src/Triple.hpp"

using namespace std;
using namespace gpstk;

%}

// Position Group
//
// Triple
// Rename below to solve "shadowed by..." warning
%rename (operator_bracket_const) gpstk::Triple::operator[](size_t const) const;
// Under Triple.hpp, friend std::ostream operator<< will
//    clash with Xvt's def of operator<<, result in redefine
%rename (Triple_streamRead) operator<<;
%include "../../../src/Triple.hpp"
%rename (streamRead) operator<<;

