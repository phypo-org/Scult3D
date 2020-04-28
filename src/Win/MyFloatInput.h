#ifndef H__MyFloatInput__H
#define H__MyFloatInput__H

#include <sstream>

#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Int_Input.H>

//******************************
class MyFloatInput : public Fl_Float_Input 
{

public :
	MyFloatInput( int pX, int pY, int pW, int pH, const char*pStr )
		:Fl_Float_Input( pX, pY, pW, pH, pStr )
	{;}

	void setFloatValue( double pVal )
	{
		std::stringstream lOs;
    lOs <<  pVal;
		value( lOs.str().c_str() );
	}
};
//******************************
class MyIntInput : public Fl_Int_Input 
{

public :
	MyIntInput( int pX, int pY, int pW, int pH, const char*pStr )
		:Fl_Int_Input( pX, pY, pW, pH, pStr )
	{;}

	void setIntValue( int pVal )
	{
	  std::stringstream lOs;
	  lOs <<  pVal;
	  value( lOs.str().c_str() );
	}
};

//******************************

#endif
