#include "Register.h"

#include <QMessageBox>
#include <QLineEdit>
#include <QCompleter>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLocale>
#include <QLabel>
#include <iostream>
#include "MyTextEdit.h"
#include "MyHighLighter.h"
#include <QStandardItemModel>

Register::Register()
{    
    parser=new exprtk::parser<VariableType>;
    current_compiled_expression=new exprtk::expression<VariableType>;
    symbolsTable=new exprtk::symbol_table<VariableType>;

    clear();
    current_compiled_expression->register_symbol_table(*symbolsTable);

    setDataPtr(nullptr);
}

Register::~Register()
{
    //std::cout<<"Delete register3"<<std::endl;
    variables_names.clear();

    //std::cout<<"Delete register4"<<std::endl;
    variables_expressions.clear();

    //std::cout<<"Delete register5"<<std::endl;
    variables.clear();

    //std::cout<<"Delete register6"<<std::endl;
    symbolsTable->clear();

    //std::cout<<"Delete register7"<<std::endl;
    delete parser;

    //std::cout<<"Delete register8"<<std::endl;
    delete current_compiled_expression;

    //std::cout<<"Delete register9"<<std::endl;
    delete symbolsTable;

    //std::cout<<"Delete register done"<<std::endl;
}

void Register::swapVariables(int ida,int idb)
{
    std::swap(variables[ida],variables[idb]);
    variables_names.swap(ida,idb);
    variables_expressions.swap(ida,idb);
}

void Register::moveVariable(int ida,int idb)
{
    if (ida<idb)
    {
        for (int k=ida; k<idb; k++)
        {
            swapVariables(k,k+1);
        }
    }
    else if (ida>idb)
    {
        for (int k=ida; k>idb; k--)
        {
            swapVariables(k,k-1);
        }
    }
}

void Register::dispVariables()
{
    for (int i=0; i<variables_names.size(); i++)
    {
        std::cout<<variables_names[i].toLocal8Bit().data()<<"="<<variables_expressions[i].toLocal8Bit().data()<<"="<<*(variables[i])<<" ("<<(variables[i]) <<") ";
    }
    std::cout<<std::endl;
}

void Register::clear()
{

    variables_names.clear();
    variables_expressions.clear();
    variables.clear();
    symbolsTable->clear();

    symbolsTable->add_constant("i",VariableType(0,1));
    symbolsTable->add_constant("pi",VariableType(M_PI));

    //Table
    symbolsTable->add_variable("Rows",numberRows);
    symbolsTable->add_variable("Cols",numberCols);
    symbolsTable->add_variable("Row",activeRow);
    symbolsTable->add_variable("Col",activeCol);
    symbolsTable->add_function("data",cf_data);
    symbolsTable->add_function("index",cf_id);


    //Noise
    symbolsTable->add_function("uniform"  ,  cf_uniform);
    symbolsTable->add_function("normal"  ,  cf_normal);

    //specials functions
    symbolsTable->add_function("gamma"  ,  cf_gamma);
    symbolsTable->add_function("zeta"  ,  cf_zeta);
    symbolsTable->add_function("xsi"  ,  cf_xsi);
    symbolsTable->add_function("lin"  ,  cf_lin);
    symbolsTable->add_function("clin"  ,  cf_clin);
    symbolsTable->add_function("solveNewton"  ,  cf_solveNewton);

    cf_id.setVariablesNamesPtr(&variables_names);
    cf_lin.setNumberOfRowsPtr(&numberRows);
    cf_lin.setCurrentRowPtr(&activeRow);

    cf_clin.setNumberOfRowsPtr(&numberRows);
    cf_clin.setCurrentRowPtr(&activeRow);
}

bool Register::newVariable(QString varname,QString varexpr)
{
    if(!isValidExpression(varexpr))
    {
        return false;
    }

    if(!isValidVariable(varname))
    {
        return false;
    }

    VariableType * p_variable=new VariableType(0.0);
    variables.push_back(p_variable);
    symbolsTable->add_variable(varname.toStdString(),*p_variable);

    variables_names.push_back(varname);
    variables_expressions.push_back(varexpr);

    return true;
}

void Register::delVariable(QString varname)
{
    int index=variables_names.indexOf(varname);
    variables_names.removeAt(index);
    variables_expressions.removeAt(index);
    variables.erase(variables.begin()+index);
    symbolsTable->remove_variable(varname.toStdString());
}

bool Register::renameVariable(QString old_varname,QString new_varname,QString oldExpression,QString newExpression)
{
    int index=variables_names.indexOf(old_varname);

    if (oldExpression!=newExpression)
    {
        if(isValidExpression(newExpression))
        {
            variables_expressions[index]=newExpression;
        }
        else
        {
            return false;
        }
    }

    if (old_varname!=new_varname)
    {
        if(isValidVariable(new_varname))
        {
            variables_names[index]=new_varname;
            symbolsTable->add_variable(new_varname.toStdString(),symbolsTable->variable_ref(old_varname.toStdString()));
            symbolsTable->remove_variable(old_varname.toStdString());
        }
        else
        {
            return false;
        }
    }

    return true;
}


bool Register::isValidExpression(QString variableExpression)
{
    if (variableExpression.isEmpty())
    {
        return true;
    }
    else if (variableExpression.startsWith("$"))
    {
        return true;
    }
    else
    {
        exprtk::expression<VariableType> current_expression;
        current_expression.register_symbol_table(*symbolsTable);
        bool ok=parser->compile(variableExpression.toStdString(),current_expression);

        if (!ok)
        {
            error("Invalid formula",variableExpression+QString("\nError : ")+QString::fromStdString(parser->error()));
        }

        return ok;
    }
}

int Register::countVariable(QString variableName)
{
    unsigned int cpt=0;
    for(int i=0;i<variables_names.size();i++)
    {
        if(variables_names[i].contains(variableName))
        {
            cpt++;
        }
    }
    return cpt;
}

bool Register::existVariable(QString variableName)
{
    if (symbolsTable->symbol_exists(variableName.toStdString()))
    {
        return true;
    }

    return false;
}

bool Register::isValidVariable(QString variableName)
{
    if (variableName.isEmpty())
    {
        return false;
    }

    if (variableName.begin()->isDigit())
    {
        error("Variable",QString("%1 : Variables names can't start with a number").arg(variableName));
        return false;
    }

    if (variableName.contains(" "))
    {
        error("Variable",QString("%1 : Variables names can't have any space").arg(variableName));
        return false;
    }

    if (!symbolsTable->valid_symbol(variableName.toStdString()))
    {
        error("Variable",QString("%1 : Invalid variable name.\nVariables names can't have any of these characters :\n+ - / * ^ > < | & ...etc").arg(variableName));
        return false;
    }

    if (symbolsTable->symbol_exists(variableName.toStdString()))
    {
        error("Variable",QString("%1 : This variable name is already used").arg(variableName));
        return false;
    }

    return true;
}

void Register::error(QString title,QString msg)
{
    QMessageBox::information(nullptr,QString("Error : ")+title,msg);
    std::cout<<"Error : "<<msg.toLocal8Bit().data()<<std::endl;
}

const QStringList & Register::variablesNames() const
{
    return variables_names;
}

const QStringList & Register::variablesExpressions() const
{
    return variables_expressions;
}

QString Register::getSaveVariablesExpression(int i) const
{
    QString exp=variables_expressions[i];
    exp.replace('\n','#');
    exp.replace(';','@');
    return exp;
}

QString Register::getLoadVariableExpression(QString exp)
{
    exp.replace('#','\n');
    exp.replace('@',';');
    return exp;
}

int Register::size()
{
    return variables.size();
}

exprtk::symbol_table<Register::VariableType> &Register::symbols()
{
    return *symbolsTable;
}

void Register::setVariable(int i,Register::VariableType value)
{
    if(i<variables.size())
    {
        *(variables[i])=value;
    }
}

bool Register::compileExpression(int id)
{

//Compilation options
//    static const std::size_t compile_options =
//            exprtk::parser<double>::settings_store::settings_compilation_options::e_replacer         +
//            exprtk::parser<double>::settings_store::settings_compilation_options::e_joiner            +
//            exprtk::parser<double>::settings_store::settings_compilation_options::e_numeric_check     +
//            exprtk::parser<double>::settings_store::settings_compilation_options::e_bracket_check     +
//            exprtk::parser<double>::settings_store::settings_compilation_options::e_sequence_check    +
//            exprtk::parser<double>::settings_store::settings_compilation_options::e_commutative_check +
//            exprtk::parser<double>::settings_store::settings_compilation_options::e_strength_reduction;
//    parser->settings().load_compile_options(compile_options);
//    std::cout<<"replacer_enabled="<<parser->settings().replacer_enabled()<<std::endl;
//    std::cout<<"commutative_check_enabled="<<parser->settings().commutative_check_enabled()<<std::endl;
//    std::cout<<"joiner_enabled="<<parser->settings().joiner_enabled()<<std::endl;
//    std::cout<<"numeric_check_enabled="<<parser->settings().numeric_check_enabled()<<std::endl;
//    std::cout<<"bracket_check_enabled="<<parser->settings().bracket_check_enabled()<<std::endl;
//    std::cout<<"sequence_check_enabled="<<parser->settings().sequence_check_enabled()<<std::endl;
//    std::cout<<"collect_variables_enabled="<<parser->settings().collect_variables_enabled()<<std::endl;
//    std::cout<<"collect_functions_enabled="<<parser->settings().collect_functions_enabled()<<std::endl;
//    std::cout<<"collect_assignments_enabled="<<parser->settings().collect_assignments_enabled()<<std::endl;




    return parser->compile(variables_expressions[id].toStdString(),*current_compiled_expression);
}

Register * Register::copy()
{
    Register * copy_reg=new Register;
    copy_reg->setDataPtr(data_ptr);
    for(int i=0;i<variables.size();i++)
    {
        VariableType * p_variable =new VariableType;
        copy_reg->variables.push_back(p_variable);
        copy_reg->symbolsTable->add_variable(variables_names[i].toStdString(),*p_variable);
    }
    copy_reg->variables_names=variables_names;
    copy_reg->variables_expressions=variables_expressions;

    return copy_reg;
}

void Register::currentCompiledExpressionValue(MyVariant & variant)const
{
    variant=current_compiled_expression->value();
}

QStringList Register::getVariablesList()
{
    QStringList qlist;
    std::vector<std::string> list;
    symbolsTable->get_variable_list(list);
    for(int i=0;i<list.size();i++)
    {
        qlist.append(QString::fromStdString(list[i]));
    }

    return qlist;
}

QStringList Register::getFunctionsList()
{
    QStringList qlist;
    std::vector<std::string> list;
    symbolsTable->get_function_list(list);
    for(int i=0;i<list.size();i++)
    {
        qlist.append(QString::fromStdString(list[i]));
    }
    qlist.append("index");
    qlist.append("solveNewton");

    return qlist;
}

QStringList Register::getReservedList()
{
    QStringList qlist;

    for(int i=0;i<exprtk::details::reserved_symbols_size;i++)
    {
        qlist.append(QString::fromStdString(exprtk::details::reserved_symbols[i]));
    }

    return qlist;
}

void custom_scanDir(QDir dir,QStringList filters,QString name,QString& result,int depth)
{
    if (depth==0)
    {
        return;
    }

    dir.setNameFilters(filters);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    QStringList fileList = dir.entryList();
    for ( int i=0; i<fileList.size(); i++)
    {
        if ( fileList[i] .contains(name) )
        {
            result=fileList[i];
            return;
        }
    }

    dir.setFilter(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    QStringList dirList = dir.entryList();
    for (int i=0; i<dirList.size(); ++i)
    {
        QString newPath = QString("%1/%2").arg(dir.absolutePath()).arg(dirList.at(i));
        custom_scanDir(QDir(newPath),filters,name,result,depth-1);
    }
}

//bool Register::customExpressionParse(unsigned int id,QString& result,std::function<QString(int,int)> at,int currentRow)
//{
//    QString expression=variables_expressions[id];
//    expression.remove('$');
//    QStringList args=expression.split(" ");
//    if (args.size()>0)
//    {

//        if (args[0]=="uniform" && args.size()==1)
//        {
//            result=fromNumber((*noise_uniform)(generator));
//            return true;
//        }
//        else if (args[0]=="normal" && args.size()==1)
//        {
//            result=fromNumber((*noise_normal)(generator));
//            return true;
//        }
//        else if (args[0]=="search" && args.size()>=5)
//        {
//            //0      1    2      3  4     5      6      i
//            //search what filter depth where0 where1 where2 wherei...

//            QString search_for_name;
//            int index1=variables_names.indexOf(args[1]);
//            if (index1>=0)
//            {
//                search_for_name=at(currentRow,index1);
//            }
//            else
//            {
//                return false;
//            }

//            result="none";
//            for (int i=4; i<args.size(); i++)
//            {
//                QStringList filters;
//                filters<<args[2];
//                custom_scanDir(args[i],filters,search_for_name,result,args[4].toInt());
//            }

//            return true;
//        }
//        else if (args[0]=="str" && args.size()==3)
//        {
//            QStringList sub_args1=args[2].split(',');
//            int index1=variables_names.indexOf(sub_args1[0]);

//            if (index1>=0)
//            {
//                result=args[1];
//                if (sub_args1.size()==2 )
//                {
//                    if (sub_args1[1]=="#" )
//                    {
//                        result=result.arg(at(currentRow,index1));
//                    }
//                    else if (sub_args1[1]=="#")
//                    {
//                        result=result.arg(at(currentRow,index1));
//                    }
//                    else
//                    {
//                        result=result.arg(*(variables[index1]),sub_args1[1].toInt(),'g',-1,'0');
//                    }
//                }
//                else if (sub_args1.size()==1)
//                {
//                    result=result.arg(*(variables[index1]));
//                }

//                return true;
//            }
//        }
//        else if (args[0]=="str" && args.size()==4)
//        {
//            QStringList sub_args1=args[2].split(',');
//            QStringList sub_args2=args[3].split(',');
//            int index1=variables_names.indexOf(sub_args1[0]);
//            int index2=variables_names.indexOf(sub_args2[0]);


//            if (index1>=0 && index2>=0)
//            {
//                result=args[1];

//                if (sub_args1.size()==2 && sub_args2.size()==2)
//                {
//                    if (sub_args1[1]=="#" && sub_args2[1]=="#")
//                    {
//                        result=result.arg(at(currentRow,index1)).arg(at(currentRow,index2));
//                    }
//                    else if (sub_args1[1]=="#")
//                    {
//                        result=result.arg(at(currentRow,index1)).arg(*(variables[index2]),sub_args2[1].toInt(),'g',-1,'0');
//                    }
//                    else if (sub_args2[1]=="#")
//                    {
//                        result=result.arg(*(variables[index1]),sub_args1[1].toInt(),'g',-1,'0').arg(at(currentRow,index2));
//                    }
//                    else
//                    {
//                        result=result.arg(*(variables[index1]),sub_args1[1].toInt(),'g',-1,'0').arg(*(variables[index2]),sub_args2[1].toInt(),'g',-1,'0');
//                    }
//                }
//                else if (sub_args1.size()==2 && sub_args2.size()==1)
//                {
//                    if (sub_args1[1]=="#")
//                    {
//                        result=result.arg(*(variables[index1]),sub_args1[1].toInt(),'g',-1,'0').arg(*(variables[index2]));
//                    }
//                    else
//                    {
//                        result=result.arg(at(currentRow,index1)).arg(*(variables[index2]));
//                    }
//                }
//                else if (sub_args1.size()==1 && sub_args2.size()==2)
//                {
//                    if (sub_args2[1]=="#")
//                    {
//                        result=result.arg(*(variables[index1])).arg(at(currentRow,index2));
//                    }
//                    else
//                    {
//                        result=result.arg(*(variables[index1])).arg(*(variables[index2]),sub_args2[1].toInt(),'g',-1,'0');
//                    }
//                }
//                else if (sub_args1.size()==1 && sub_args2.size()==1)
//                {
//                    result=result.arg(*(variables[index1])).arg(*(variables[index2]));
//                }

//                return true;
//            }
//            else
//            {
//                error("Bad variable indexes",QString("Indexes : %1 %2").arg(index1).arg(index2));
//                return false;
//            }
//        }
//        else
//        {
//            error("Bad custom expression",QString("Expression : ")+expression);
//            return false;
//        }
//    }
//    else
//    {
//        error("Bad custom expression",QString("Expression : ")+expression);
//        return false;
//    }

//    return false;
//}




QString getHelp(QString name)
{
    //Variables
    if(name=="Rows"){return QString("<b>Rows</b><p>Return the number of rows.</p>");}
    else if(name=="Cols"){return QString("<b>Cols</b><p>Return the number of columns.</p>");}
    else if(name=="Row"){return QString("<b>Row</b><p>Return the current row.</p>");}
    else if(name=="Col"){return QString("<b>Col</b><p>Return the current column.</p>");}
    else if(name=="var"){return QString("<b>var</b><p>Define a variable.</p><b>Example:</b><p>var x:=1+i;</p>");}

    //functions
    else if(name=="data"){return QString("<b>data</b>(a,b)<p>Provide direct acces to data.This return the data stored at row a column b.</p><b>Example:</b><p>data(1,0)</p>");}
    else if(name=="clin"){return QString("<b>clin</b>(A,B)<p>Generate a linear regular grid interpolation between A and B where A and B represent the complexes numbers at the bottom-left corner and B at the top-left corner. The grid is composed of exactly Rows points, that is, sqrt(Rows)xsqrt(Rows).</p><b>Example:</b><p>lin(0,1)</p>");}
    else if(name=="lin"){return QString("<b>lin</b>(A,B)<p>Generate a linear interpolation between A and B.</p><b>Example:</b><p>lin(0,1)</p>");}
    else if(name=="uniform"){return QString("<b>uniform</b>(A,B)<p>Generate a random number with uniform distribution on interval [A,B].</p><b>Example:</b><p>uniform(0,1)</p>");}
    else if(name=="normal"){return QString("<b>normal</b>(A,B)<p>Generate a random number with normal distribution with mean A and standard deviation B.</p><b>Example:</b><p>normal(0,1)</p>");}
    else if(name=="zeta"){return QString("<b>zeta</b>(s)<p>The Riemann zeta function valid on the entire complex plane.</p>");}
    else if(name=="xsi"){return QString("<b>xsi</b>(s)<p>The Riemann xsi function valid on the entire complex plane.</p>");}
    else if(name=="gamma"){return QString("<b>gamma</b>(s)<p>The gamma function valid on the entire complex plane.</p>");}
    else if(name=="roundn"){return QString("<b>roundn</b>(x,n)<p>Round x to n decimal places.</p><b>Example:</b><p>roundn(1.2345678,4)</p>");}
    else if(name=="round"){return QString("<b>round</b>(x)<p>Round x to the nearest integer.</p>");}
    else if(name=="ceil"){return QString("<b>ceil</b>(x)<p>maps x to the least integer greater than or equal to x</p>");}
    else if(name=="floor"){return QString("<b>floor</b>(x)<p>maps x to the least integer lower than or equal to x</p>");}
    else if(name=="sinc"){return QString("<b>sinc</b>(s)<p>The sinus cardinal function valid on the entire complex plane.</p>");}
    else if(name=="root"){return QString("<b>root</b>(s,n)<p>Nth-Root of x. where n is a positive integer.</p>");}
    else if(name=="sqrt"){return QString("<b>sqrt</b>(s)<p>The square root function valid on the entire complex plane.</p>");}
    else if(name=="csc"){return QString("<b>csc</b>(s)<p>The cosecant function valid on the entire complex plane.</p>");}
    else if(name=="sec"){return QString("<b>sec</b>(s)<p>The secant function valid on the entire complex plane.</p>");}
    else if(name=="cot"){return QString("<b>cot</b>(s)<p>The cotangent function valid on the entire complex plane.</p>");}
    else if(name=="tan"){return QString("<b>tan</b>(s)<p>The tangent function valid on the entire complex plane.</p>");}
    else if(name=="cos"){return QString("<b>cos</b>(s)<p>The cosinus function valid on the entire complex plane.</p>");}
    else if(name=="sin"){return QString("<b>sin</b>(s)<p>The sinus function valid on the entire complex plane.</p>");}
    else if(name=="tanh"){return QString("<b>tan</b>(s)<p>The hyperbolic tangent function valid on the entire complex plane.</p>");}
    else if(name=="cosh"){return QString("<b>cosh</b>(s)<p>The hyperbolic cosinus function valid on the entire complex plane.</p>");}
    else if(name=="sinh"){return QString("<b>sinh</b>(s)<p>The hyperbolic sinus function valid on the entire complex plane.</p>");}
    else if(name=="atan"){return QString("<b>atan</b>(s)<p>The arc tangent function valid on the entire complex plane.</p>");}
    else if(name=="acos"){return QString("<b>acos</b>(s)<p>The arc cosinus function valid on the entire complex plane.</p>");}
    else if(name=="asin"){return QString("<b>asin</b>(s)<p>The arc sinus function valid on the entire complex plane.</p>");}
    else if(name=="atanh"){return QString("<b>atan</b>(s)<p>The hyperbolic arc tangent function valid on the entire complex plane.</p>");}
    else if(name=="acosh"){return QString("<b>acosh</b>(s)<p>The hyperbolic arc cosinus function valid on the entire complex plane.</p>");}
    else if(name=="asinh"){return QString("<b>asinh</b>(s)<p>The hyperbolic arc sinus function valid on the entire complex plane.</p>");}
    else if(name=="exp"){return QString("<b>exp</b>(s)<p>The exponential function valid on the entire complex plane.</p><b>Example:</b><p>10*exp(i*pi)</p>");}
    else if(name=="log"){return QString("<b>log</b>(s)<p>Natural logarithm function valid on the entire complex plane.</p>");}
    else if(name=="log10"){return QString("<b>log10</b>(s)<p>Base 10 logarithm function valid on the entire complex plane.</p>");}
    else if(name=="log2"){return QString("<b>log2</b>(s)<p>Base 2 logarithm function valid on the entire complex plane.</p>");}
    else if(name=="logn"){return QString("<b>logn</b>(s,n)<p>Base n logarithm function valid on the entire complex plane.</p>");}
    else if(name=="log1p"){return QString("<b>log1p</b>(s)<p>Natural logarithm function of 1+s, valid on the entire complex plane.</p>");}
    else if(name=="pow"){return QString("<b>pow</b>(s,z)<p>Return s power z, valid on the entire complex plane.</p>");}
    else if(name=="imag"){return QString("<b>imag</b>(s)<p>Return imaginary part of s.</p>");}
    else if(name=="real"){return QString("<b>real</b>(s)<p>Return real part of s.</p>");}
    else if(name=="abs"){return QString("<b>abs</b>(s)<p>Return module of s.</p>");}
    else if(name=="arg"){return QString("<b>arg</b>(s)<p>Return argument of s in interval [-pi,pi].</p>");}
    else if(name=="erf"){return QString("<b>erf</b>(s)<p>Error function of x.</p>");}
    else if(name=="erfc"){return QString("<b>erfc</b>(s)<p>Complimentary error function of x.</p>");}
    else if(name=="max"){return QString("<b>max</b>(a,b)<p>Return a if a is greater than b else return b.</p>");}
    else if(name=="min"){return QString("<b>min</b>(a,b)<p>Return a if a is lower than b else return b.</p>");}
    else if(name=="atan2"){return QString("<b>atan2</b>(y,x)<p>Return the single value of theta in [-pi,pi] such that x=cos(theta) and y=sin(theta).</p>");}
    else if(name=="min"){return QString("<b>min</b>(a,b,c,...)<p>Minimum of all the inputs.</p>");}
    else if(name=="max"){return QString("<b>max</b>(a,b,c,...)<p>Maximum of all the inputs.</p>");}
    else if(name=="avg"){return QString("<b>avg</b>(a,b,c,...)<p>Average of all the inputs.</p>");}
    else if(name=="sum"){return QString("<b>sum</b>(a,b,c,...)<p>Sum of all the inputs.</p>");}
    else if(name=="mul"){return QString("<b>mul</b>(a,b,c,...)<p>Product of all the inputs.</p>");}
    else if(name=="sgn"){return QString("<b>sgn</b>(x)<p>Sign of x, -1 where x &lt; 0, +1 where x &gt; 0, else zero.</p>");}
    else if(name=="ncdf"){return QString("<b>ncdf</b>(x)<p>Normal cumulative distribution function.</p>");}
    else if(name=="clamp"){return QString("<b>clamp</b>(a,x,b)<p>Clamp x in range between a and b, where a &lt; b.</p><b>Example:</b><p>clamp(0,x,1)</p>");}
    else if(name=="iclamp"){return QString("<b>clamp</b>(a,x,b)<p>Inverse-clamp x outside of the range a and b. Where a &lt; b. If x is within the range it will snap to the closest bound.</p><b>Example:</b><p>clamp(0,x,1)</p>");}

    //Constants
    else if(name=="i"){return QString("<b>i</b><p>The unit complex number.</p><b>Example:</b><p>i^2=-1</p>");}
    else if(name=="pi"){return QString("<b>pi</b><p>The number pi.</p>");}
    else if(name=="true"){return QString("<b>true</b><p>The true value</p>");}
    else if(name=="false"){return QString("<b>false</b><p>The false value</p>");}

    //Structural
    else if(name=="for"){return QString("<b>for</b><p>The for loop structure.</p><b>Example:</b><p>var t:=0;<br>for(var k:=0;k&lt;10;k+=1)<br>{<br>t+=1;<br>}</p>");}
    else if(name=="if"){return QString("<b>if</b><p>The if/else condition structure.</p><b>Example:</b><p>if(Row%2==0)<br>{<br>1<br>}<br>else<br>{<br>0<br>}</p>");}
    else if(name=="else"){return QString("<b>else</b><p>The if/else condition structure.</p><b>Example:</b><p>if(Row%2==0)<br>{<br>1<br>}<br>else<br>{<br>0<br>}</p>");}

    //Logic
    else if(name=="and"){return QString("<b>and</b><p>The and logic operator</p>");}
    else if(name=="mand"){return QString("<b>mand</b><p>The mand logic operator</p>");}
    else if(name=="mor"){return QString("<b>mor</b><p>The mor logic operator</p>");}
    else if(name=="nand"){return QString("<b>nand</b><p>The nand logic operator</p>");}
    else if(name=="nor"){return QString("<b>nor</b><p>The nor logic operator</p>");}
    else if(name=="nor"){return QString("<b>nor</b><p>The nor logic operator</p>");}
    else if(name=="not"){return QString("<b>not</b><p>The not logic operator</p>");}
    else if(name=="or"){return QString("<b>or</b><p>The or logic operator</p>");}
    else if(name=="shl"){return QString("<b>shl</b><p>The shl logic operator</p>");}
    else if(name=="shr"){return QString("<b>shr</b><p>The shr logic operator</p>");}
    else if(name=="xnor"){return QString("<b>xnor</b><p>The xnor logic operator</p>");}
    else if(name=="xor"){return QString("<b>xor</b><p>The xor logic operator</p>");}

    //Misc
    else if(name=="solveNewton"){
        return QString("<b>solveNewton</b>('f(z)','f'(z)'|epsilon,z0,[a],[it])"\
                                                 "<p>Basic newton solver.</p>"\
                                                 "<p>Perform newton's algorithm, searching zeros of the expression f(z)."\
                                                 "If you know the derivative you can specifie it in the second argument 'f'(z)'."\
                                                 "The algorithm start at z0 then perform 'it' iterations z=z-a*f(z)/f'(z)</p>");
    }


    return QString();
}

void checkHelp(QStringList names)
{
    for(int i=0;i<names.size();i++)
    {
        if(getHelp(names[i]).isEmpty())
        {
            std::cout<<"Missing help for :"<<names[i].toStdString()<<std::endl;
        }
    }
}

int Register::getVarExpDialog(QString currentName, QString currentExpression, QString & newName, QString & newExpression)
{
    QLineEdit* le_variableName=new QLineEdit(newName);
    MyTextEdit* le_variableExpression=new MyTextEdit(newExpression);
    QScopedPointer<MyHighLighter> highlighter(new MyHighLighter(getVariablesList(),getFunctionsList()+getReservedList(),le_variableExpression->document()) );

    //--------------------------------------------------------
    QStringList completerList=getVariablesList()+getFunctionsList()+getReservedList();
    checkHelp(completerList);
    QStandardItemModel* model = new QStandardItemModel();

    // initialize the model
    int rows = completerList.count();
    model->setRowCount(rows);
    model->setColumnCount(1);

    // load the items
    int row = 0;
    for(int i=0;i<completerList.size();i++)
    {
        QStandardItem* item = new QStandardItem(completerList[i]);

        if(getVariablesList().contains(completerList[i]))
        {
            item->setForeground(Qt::blue);
        }

        //item->setIcon(QIcon(":/img/icons/points_cloud.png"));
        item->setToolTip(getHelp(completerList[i]));
        model->setItem(row, 0, item);
        row++;
    }
    //-----------------------------------------------------------

    QCompleter * completer= new QCompleter(model, nullptr);
    le_variableExpression->setCompleter(completer);

    QDialog* dialog=new QDialog;
    dialog->setLocale(QLocale("C"));
    dialog->setWindowTitle((currentName.isEmpty())?"Add variable":"Edit variable");

    dialog->setMinimumWidth(400);
    QGridLayout* gbox = new QGridLayout();

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    QObject::connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    gbox->addWidget(le_variableName,0,1);
    gbox->addWidget(le_variableExpression,1,1);
    gbox->addWidget(new QLabel("Variable name"),0,0);
    gbox->addWidget(new QLabel("Variable formula"),1,0);
    gbox->addWidget(buttonBox,2,0,1,2);

    dialog->setLayout(gbox);
    int result=dialog->exec();
    if (result == QDialog::Accepted)
    {
        newName=le_variableName->text();
        newExpression=le_variableExpression->textContent();

        if (currentName.isEmpty()) //add New
        {
            if (!newVariable(newName,newExpression) )
            {
                return -1;
            }
        }
        else //modify
        {
            if( !renameVariable(currentName,newName,currentExpression,newExpression) )
            {
                return -1;
            }
        }

        emit sig_modified();
        return 1;
    }
    else
    {
        return 0;
    }
}

bool Register::editVariableAndExpression(int currentIndex)
{
    QString currentName,currentExpression;
    QString newName,newExpression;

    if (currentIndex<this->size() && currentIndex>=0) //fetch variable and expression
    {
        currentName=variables_names[currentIndex];
        currentExpression=variables_expressions[currentIndex];
    }
    else
    {
        currentName.clear();
        currentExpression.clear();
    }
    newName=currentName;
    newExpression=currentExpression;

    int ret=0;
    do
    {
        ret=getVarExpDialog(currentName,currentExpression, newName, newExpression);
    }
    while(ret==-1);

    return bool(ret);

}

