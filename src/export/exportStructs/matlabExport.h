/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : matlabExport.h
* Creation Date : 23-06-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _MATLABEXPORTSTRUCT_
#define _MATLABEXPORTSTRUCT_
/// DESCRIPTION
#include <string>

#include <iostream>
namespace symbolic
{

struct MatlabExportStruct
{
    public:
    bool FallThrough() const { return false; };
    const std::string newline{"...\n"};
    const std::string spacer{" , "};
    std::string PreTextOutput( const std::string name ) const { return name + " = ["; };
    std::string PostTextOutput( const std::string /*name*/ ) const { return "];\n"; };

    void OutputPhiAussen( std::string & /*input*/ ) const {};
    void OutputPhiInnen( std::string & /*input*/ ) const {};
    void TauFactor( std::string & /*input*/ ) const {};
    void OhmicResistance( std::string & /*input*/ ) const {};
    void VoltageSource( std::string & /*input*/ ) const {};
    void Capacity( std::string & /*input*/ ) const {};
    void Tau( std::string & /*input*/ ) const {};
    void Obj( std::string & /*input*/ ) const {};

    void MP( std::string & /*input*/ ) const {};
    void CT( std::string & /*input*/ ) const {};

    void TwoPort( std::string & /*input*/ ) const {};
    void Current( std::string &input ) const { input += "_base"; };
    void RealName( std::string &ID_Number, std::string &TypeName ) const { TypeName += ID_Number; }

    void AddOperator( std::string &add ) const { add = " + "; };
    void SubOperator( std::string &sub ) const { sub = " - "; };
    void MulOperator( std::string &mul ) const { mul = " * "; };
    void DivOperator( std::string &operand1, std::string &operand2 ) const
    {
        operand1 = " (" + operand1 + ") / (" + operand2 + ") ";
    };
    void PowOperator( std::string &operand1, std::string &operand2 ) const
    {
        operand1 = " (" + operand1 + ")^(" + operand2 + ") ";
    };
    void DoubleRule( std::string &op, std::string &operand1, std::string &operand2 ) const
    {
        op = "(" + operand1 + ")" + op + "(" + operand2 + ")";
    }
    void SqrtRule( std::string &operand ) const { operand = "sqrt(" + operand + ")"; }
    void TanhRule( std::string &operand ) const { operand = "tanh(" + operand + ")"; }
    void NegRule( std::string &operand ) const { operand = "-(" + operand + ")"; }
    void State( std::string &returnString ) const { returnString = "x(" + returnString + ")"; }
};
}


#endif /* _MATLABEXPORTSTRUCT_ */
