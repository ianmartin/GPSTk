//swig interface for ConfDataReader.hpp

%module gpstk_ConfDataReader

%{
#include "../../../src/ConfDataReader.hpp"

using namespace std;
using namespace gpstk;
%}

%include "../../../src/ConfDataReader.hpp"
