/* -.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.
* File Name : texExport.h
* Creation Date : 23-06-2017
* Last Modified :
* Created By : Friedrich Hust
_._._._._._._._._._._._._._._._._._._._._.*/
#ifndef _TEXEXPORT_
#define _TEXEXPORT_
#include <string>

namespace symbolic
{
/// DESCRIPTION
struct TexExportStruct
{
    // Exportfunctions for exporting to latex
    public:
    bool FallThrough() const { return false; };
    const std::string newline{"\\\\\n"};
    const std::string spacer{" & "};
    std::string PreTextOutput( const std::string name ) const
    {
        return "\\begin{equation}\n" + name + " = \\begin{pmatrix}";
    };
    std::string PostTextOutput( const std::string /*name*/ ) const { return "\\end{pmatrix}\n\\end{equation}\n"; };

    void OutputPhiAussen( std::string & /*input*/ ) const {};
    void OutputPhiInnen( std::string & /*input*/ ) const {};
    void TauFactor( std::string & /*input*/ ) const {};
    void OhmicResistance( std::string & /*input*/ ) const {};
    void VoltageSource( std::string & /*input*/ ) const {};
    void Capacity( std::string & /*input*/ ) const {};
    void Tau( std::string &input ) const { input = "\\tau"; };
    void Obj( std::string & /*input*/ ) const {};
    void MP( std::string &input ) const { input = "\\MP"; };
    void CT( std::string &input ) const { input = "\\CT"; };
    void TwoPort( std::string & /*input*/ ) const {};
    void Current( std::string &input ) const { input += "_{base}"; };
    void RealName( std::string &ID_Number, std::string &TypeName ) const { TypeName += "_{" + ID_Number + "}"; }

    void AddOperator( std::string &add ) const { add = " + "; };
    void SubOperator( std::string &sub ) const { sub = " - "; };
    void MulOperator( std::string &mul ) const { mul = " \\cdot "; };
    void DivOperator( std::string &operand1, std::string &operand2 ) const
    {
        operand1 = " \\frac{" + operand1 + "}{" + operand2 + "} ";
    };
    void PowOperator( std::string &operand1, std::string &operand2 ) const
    {
        operand1 = " " + operand1 + "^{" + operand2 + "} ";
    };
    void DoubleRule( std::string &op, std::string &operand1, std::string &operand2 ) const
    {
        op = "( " + operand1 + op + operand2 + " )";
    }
    void SqrtRule( std::string &operand ) const { operand = "\\sqrt{" + operand + "}"; }
    void TanhRule( std::string &operand ) const { operand = "tanh(" + operand + ")"; }
    void NegRule( std::string &operand ) const { operand = "-(" + operand + ")"; }
    void State( std::string &returnString ) const { returnString = "x_{" + returnString + "}"; }
};
}


#endif /* _TEXEXPORT_ */
