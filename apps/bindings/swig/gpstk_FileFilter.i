//swig interface for FileFilter.hpp

%module gpstk_FileFilter

%{
#include "../../../src/FileFilter.hpp"

using namespace gpstk;
%}

%include "../../../src/FileFilter.hpp"
%include "gpstk_FileFilterFrame.i"
%include "gpstk_FileFilterFrameWithHeader.i"
