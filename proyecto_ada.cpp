#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <string>
using namespace std;

//Divide un cadena
vector<string> split(string cadena, char simbolo)
{
    vector<string> vec;
    string tmp;
    for(int i=0; i<cadena.size(); i++)
    {
        if(cadena[i]!=simbolo)
        {
            tmp.push_back(cadena[i]);
        }
        else
        {
            vec.push_back(tmp);
            tmp.clear();
        }
    }
    vec.push_back(tmp);

    return vec;
}

//ABRE Y SEPARA EL .TXT EN PALABRAS
vector <string> lecturaParrafo(string dir)
{
    string v;
    vector <string> palabras;
    ifstream f;
    f.open(dir);
    while(!f.eof())
    {
        getline(f, v); //toda la linea
        palabras.push_back(v);
        v="";
    }
    f.close();

    return palabras;
}

//IMPRIME VECTOR
void imprimirVector(vector <string> vectorsote)
{
    for(int i=0; i<vectorsote.size(); i++)
    {
        cout<<vectorsote[i]<<" ";
    }
}

//========================= CONTEXTO ============================
class Contexto
{
    string etiqueta;
    string valor;
public:
    Contexto();
    Contexto(string,string);
    void imprimirContexto();
    friend class EarleyParser;
};

Contexto::Contexto()
{
    etiqueta = "";
    valor = "";
}

Contexto::Contexto(string _etiqueta, string _valor)
{
    etiqueta = _etiqueta;
    valor = _valor;
}

void Contexto::imprimirContexto()
{
    cout<<etiqueta<<"="<<valor;
}

class EarleyState;

//========================= NODO ==========================
class Nodo
{
    string etiqueta;
    vector<Contexto*> contextos;
public:
    Nodo();
    Nodo(string);
    void insertarContexto(string,string);
    void imprimirContextos();
    void imprimirNodo();
    bool esTerminal();
    Nodo& operator = (const Nodo &original);
    friend class Produccion;
    friend class EarleyState;
    friend class EarleyParser;
    friend void imprimirVector(vector <EarleyState*>);
    friend bool busquedaString(string, vector <EarleyState*>);
};

Nodo::Nodo()
{
    etiqueta = "";
}

Nodo::Nodo(string _etiqueta)
{
    etiqueta = _etiqueta;
}

void Nodo::insertarContexto(string etiqueta, string valor)
{
    Contexto *contexto = new Contexto(etiqueta,valor);
    contextos.push_back(contexto);
}

void Nodo::imprimirContextos()
{
    for(int i=0; i<contextos.size(); i++)
    {
        contextos[i]->imprimirContexto();
    }
}

void Nodo::imprimirNodo()
{
    cout<<etiqueta;
    cout<<"[";
    imprimirContextos();
    cout<<"]";
}

Nodo& Nodo::operator = (const Nodo &original)
{
    etiqueta = original.etiqueta;
    for (int i=0; i<original.contextos.size(); i++)
        contextos.push_back(original.contextos[i]);
    return *this;
}

bool Nodo::esTerminal()
{
    if(contextos.size() == 0)
    {
        return true;
    }
    return false;
}

//============================= PRODUCCION =============================
class Produccion
{
public:
    // Ej. aAb -> aBb + aAcb
    Nodo *first;            // parte izquierda
    vector<Nodo*> second;   // parte derecha
    Produccion(Nodo* f, vector<Nodo*> s)
    {
        first = f;
        for(int i=0; i<s.size(); i++)
        {
            second.push_back(s[i]);
        }
    };
    Produccion() {};
    Produccion& operator = (Produccion &original);
    void imprimirProduccion();
    friend class EarleyParser;
};

void Produccion::imprimirProduccion()
{
    first->imprimirNodo();
    cout<<"->";
    for(int i=0; i<second.size(); i++)
    {
        second[i]->imprimirNodo();
    }
}

Produccion& Produccion::operator = (Produccion &original)
{
    first = original.first;
    for(int i=0; i<original.second.size(); i++)
    {
        second.push_back(original.second[i]);
    }
    return *this;
}

//============================= EARLEY STATE =============================
class EarleyState
{
    Produccion produccion_actual;   // ej. Verb -> Sust + Adj
    int pos_punto;
    int pos_chart;
    int estado_chart;
    int referencia_int;             // profe dijo hacerlo puntero y entero
    string contexto;
public:
    EarleyState* referencia_ptr;
    EarleyState(Produccion, int, int, int, int, string);
    void imprimirEarleyState();
    friend class EarleyParser;
    friend bool busquedaChar(string, vector <EarleyState*>);
    friend void imprimirVector(vector <EarleyState*>);
    friend bool busquedaString(string,vector <EarleyState*>);
};

EarleyState::EarleyState(Produccion prod, int pp, int pc, int ec, int r, string context)
{
    produccion_actual = prod;
    pos_punto = pp;     // pos del punto en la expresion
    pos_chart = pc;     // linea del chart actual
    estado_chart = ec;  // 0(inicio,predecir), 1(escanear,completar), 2(escanear,predecir), 3(... )
    referencia_int = r; // de donde viene la produccion actual
    contexto = context;
}

//IMPRIME VECTOR
void imprimirVector(vector <EarleyState*> vectorsote)
{
    for(int i=0; i<vectorsote.size(); i++)
    {
        cout<<vectorsote[i]->produccion_actual.first->etiqueta<<" ";
    }
}

void imprimirVector(vector <Nodo*> vectorsote)
{
    for(int i=0; i<vectorsote.size(); i++)
    {
        vectorsote[i]->imprimirNodo();
    }
}
void EarleyState::imprimirEarleyState()
{
    cout << pos_chart << ", ";
    produccion_actual.imprimirProduccion();
    cout << ", " << pos_punto << ", " << estado_chart << ", " << referencia_int << ", " << contexto << endl;
}

//============================= EARLEY PARSER =============================

class EarleyParser
{
    vector<Produccion> P;   // vector de producciones
    vector<string> SNT;     // simbolos no terminales
    vector<string> ST;      // simbolos terminales
    string inicial;         // estado inicial
    vector<string> expresion;
    int pos_ch;
    int estado_ch;

    vector<EarleyState*> chart; // Chart es un arreglo de EarlyState

public:
    EarleyParser(vector <string>);
    Nodo* estructurarNodo(string);
    void recibirEntrada(vector<string>);
    void predecir();
    void completar();
    void escanear();
    void actualizarUnContexto(string, string, string);
    bool comprobarContexto(Produccion*);
    void actualizarContextosRec();
    bool evaluacion_final();
    void imprimirGramatica();
    void imprimirChart();

};

EarleyParser::EarleyParser(vector <string> entrada)
{
    recibirEntrada(entrada);

    pos_ch = 0;         // inicio pos_chart
    estado_ch = 0;      // inicio estado_chart
    for(int i = 0; i < P.size(); i++)
    {
        if (P[i].first->etiqueta == inicial)
        {
            EarleyState* oEarley = new EarleyState(P[i], 0, pos_ch, estado_ch, 0, "---");
            chart.push_back(oEarley);
            pos_ch++;
        }
    }

    int pos_punto_actual;
    vector<Nodo*> second_actual;
    int pos_final_produccion;


    while (true)
    {
        int tam_ini = chart.size();

        EarleyState* estado_final = chart[chart.size()-1];      // ultimo estado del chart
        pos_punto_actual    = estado_final->pos_punto;
        second_actual       = estado_final->produccion_actual.second;
        pos_final_produccion= second_actual.size()-1;

        if (pos_punto_actual != pos_final_produccion+1)         // cuando el punto esta al final de la produccion
        {
            if (!second_actual[pos_punto_actual]->esTerminal())
            {
                predecir();  // no terminal
            }
            else
            {
                if(chart.empty())
                {
                    break;
                }
                else
                {
                    cout << endl;
                    imprimirGramatica();
                    cout<<endl;
                    imprimirChart();
                    escanear();
                }
            }
        }
        else
        {
            completar();
        }
        int tam_fin = chart.size();
        if(tam_ini == tam_fin)
            break;

    }
    cout << endl;
    imprimirGramatica();
    cout<<endl;
    imprimirChart();

    if(evaluacion_final())
    {
        cout<<"\nSi pertenece a la gramatica."<<endl;
    }
    else
    {
        cout<<"\nNo pertenece a la gramatica."<<endl;
    }
}

void EarleyParser::recibirEntrada(vector<string> entrada)
{
    // entrada guardar en expresion
    expresion = split(entrada[0],' ');
    inicial = entrada[1];

    string palabra;
    for(int i=0; i<entrada[2].size(); i++)
    {
        if(entrada[2][i]!=' ')
        {
            palabra.push_back(entrada[2][i]);
        }
        else
        {
            SNT.push_back(palabra);
            palabra.clear();
        }
    }
    SNT.push_back(palabra);
    palabra.clear();

    for(int i=0; i<entrada[3].size(); i++)
    {
        if(entrada[3][i]!=' ')
        {
            palabra.push_back(entrada[3][i]);
        }
        else
        {
            ST.push_back(palabra);
            palabra.clear();
        }
    }
    ST.push_back(palabra);
    palabra.clear();

    int cant = atoi(entrada[4].c_str());
    int fin = 5 + cant;
    vector<string> strSecond;
    for(int i = 5; i < fin; i++)
    {
        Produccion* tmp = new Produccion;
        for(int j=0; j<entrada[i].size(); j++)
        {
            if(entrada[i][j]!= ':')
            {
                palabra.push_back(entrada[i][j]);
            }
            else
            {
                tmp->first = estructurarNodo(palabra);
                palabra.clear();
            }
        }
        strSecond = split(palabra,' ');
        Nodo* ptrSecond;
        for(int j=0; j<strSecond.size(); j++)
        {
            ptrSecond = estructurarNodo(strSecond[j]);
            tmp->second.push_back(ptrSecond);

        }
        P.push_back(*tmp);
        palabra.clear();
        strSecond.clear();
    }

    cout<<"DATOS RECIBIDOS"<<endl;
    cout<<"Expresion      : ";
    imprimirVector(expresion);
    cout<<endl;
    cout<<"Estado inicial : "<<inicial<<endl;
    cout<<"No Terminales  : ";
    imprimirVector(SNT);
    cout<<endl;
    cout<<"Terminales     : ";
    imprimirVector(ST);
    cout<<endl<<endl;
    cout<<"PRODUCCIONES"<<endl;
    for(int i=0; i<P.size(); i++)
    {
        P[i].imprimirProduccion();
        cout << endl;
    }
    cout << endl;
}

void EarleyParser::imprimirGramatica()
{
    cout<<"\nImprimir gramatica"<<endl;
    for(int i=0; i<P.size(); i++)
    {
        P[i].imprimirProduccion();
        cout<<endl;
    }
}

void EarleyParser::imprimirChart()
{
    cout << "============== CHART ==============" << endl;
    cout << "pos_chart, produc_actual, pos_punto, estado, referencia, contexto\n\n";
    for(int i = 0; i < chart.size(); i++)
    {
        cout << chart[i]->pos_chart << ", ";
        chart[i]->produccion_actual.imprimirProduccion();
        cout << ", " << chart[i]->pos_punto << ", " << chart[i]->estado_chart << ", " << chart[i]->referencia_int
             << chart[i]->contexto << endl;
    }
}

bool busquedaString(string letra, vector <string> vec)
{
    for(int i=0; i<vec.size(); i++)
    {
        if(vec[i] == letra)
        {
            return true;
        }
    }
    return false;
}

bool busquedaString(string letra, vector <EarleyState*> vec)
{
    for(int i=0; i<vec.size(); i++)
    {
        if(vec[i]->produccion_actual.first->etiqueta == letra)
        {
            return true;
        }
    }
    return false;
}

string char_to_string(char A)
{
    string tmp;
    tmp.push_back(A);
    return tmp;
}

Nodo* EarleyParser::estructurarNodo(string strNodo)
{
    string etNodo;
    string strContextos;
    Nodo* nodito = new Nodo();

    int flag = 0;
    for(int i=0; i<strNodo.size(); i++)
    {
        if(flag==0)
        {
            etNodo.push_back(strNodo[i]);
            if(strNodo[i+1]=='[')
            {
                i++;
                flag = 1;
                nodito->etiqueta=etNodo;
            }
        }
        else if(flag==1)
        {
            strContextos.push_back(strNodo[i]);
            if(strNodo[i+1]==']')
            {
                break;
            }
        }
    }

    //en el caso de que sea un nodo terminal
    if(flag == 0)
    {
        nodito->etiqueta=etNodo;
        return nodito;
    }

    string etContexto;
    string valContexto;
    vector<string> vecContextos;
    vecContextos = split(strContextos,' ');
    for(int i=0; i<vecContextos.size(); i++)
    {
        flag = 0;
        for(int j=0; j<vecContextos[i].size(); j++)
        {
            if(flag == 0)
            {
                etContexto.push_back(vecContextos[i][j]);
                if(vecContextos[i][j+1]=='=')
                {
                    j++;
                    flag = 1;
                }
            }
            else if(flag == 1)
            {
                valContexto.push_back(vecContextos[i][j]);
                if(vecContextos[i][j+1]==']')
                {
                    break;
                }
            }
        }
        nodito->insertarContexto(etContexto,valContexto);
        etContexto.clear();
        valContexto.clear();
    }

    return nodito;
}

void EarleyParser::actualizarContextosRec()
{
    for(int i=0; i<chart.size(); i++)
    {
        bool b = comprobarContexto(&chart[i]->produccion_actual);
        if(b==true)
        {

            string etN = chart[i]->produccion_actual.first->etiqueta;
            string etC = chart[i]->produccion_actual.first->contextos[0]->etiqueta;
            string valC = chart[i]->produccion_actual.first->contextos[0]->valor;
            actualizarUnContexto(etN,etC,valC);
            cout<<"HOLa"<<endl;
            actualizarContextosRec();
        }
    }
}

bool EarleyParser::comprobarContexto(Produccion* prod)
{
    string var = "?n";

    // inicializar var
    for(int i=0; i<prod->second.size(); i++)                    // itera por el second de la prod
    {
        for(int j=0; j<prod->second[i]->contextos.size(); j++)
        {
            if(prod->second[i]->contextos[j]->valor != "?n")
            {
                var = prod->second[i]->contextos[j]->valor;
            }
        }
    }

    //comprobacion de la etiqueta de los hijos
    bool flag = true;
    if(var!= "?n")
    {
        for(int i=0; i<prod->second.size(); i++)
        {
            for(int j=0; j<prod->second[i]->contextos.size(); j++)
            {
                if(prod->second[i]->contextos[j]->valor != var)
                {
                    flag = false;
                    if(prod->second[i]->contextos[j]->valor != "?n")
                    {
                        exit(1);
                    }
                }
            }
        }
    }

    //si los hijos tienen la misma etiqueta se le asigna el padre
    if(flag == true && prod->first->contextos[0]->valor == "?n" && prod->first->contextos[0]->valor !=var)  //
    {
        prod->first->contextos[0]->valor = var;
        return true;
    }
    else
    {
        return false;
    }

}

void EarleyParser::actualizarUnContexto(string etNodo, string etContext, string valContext)
{
    //SECOND CHART
    for(int i=0; i<chart.size(); i++)                                   // itera por cada earley state del chart
    {
        vector<Nodo*> ptrSecond = chart[i]->produccion_actual.second;
        for(int k=0; k<ptrSecond.size(); k++)                           // itera por cada nodo del second
        {
            if(ptrSecond[k]->etiqueta == etNodo)
            {
                for(int j=0; j<ptrSecond[k]->contextos.size(); j++)     // recorre el vec de contextos de second
                {
                    if(ptrSecond[k]->contextos[j]->etiqueta == etContext && ptrSecond[k]->contextos[j]->valor == "?n")
                    {
                        ptrSecond[k]->contextos[j]->valor = valContext; // actualiza
                    }
                }
            }
        }
    }

    //second Gramatica
    for(int i=0; i<P.size(); i++)                                       // itera por cada earley state del chart
    {
        vector<Nodo*> ptrSecond = P[i].second;
        for(int k=0; k<ptrSecond.size(); k++)                           // itera por cada nodo del second
        {
            if(ptrSecond[k]->etiqueta == etNodo)
            {
                for(int j=0; j<ptrSecond[k]->contextos.size(); j++)     // recorre el vec de contextos de second
                {
                    if(ptrSecond[k]->contextos[j]->etiqueta == etContext && ptrSecond[k]->contextos[j]->valor == "?n")
                    {
                        ptrSecond[k]->contextos[j]->valor = valContext; // actualiza
                    }
                }
            }
        }
    }

}


bool EarleyParser::evaluacion_final()
{
    for(int i=0; i<chart.size(); i++)
    {
        if(chart[i]->estado_chart == estado_ch)                                     // buscar en el ultimo estado
        {
            if(chart[i]->pos_punto == chart[i]->produccion_actual.second.size())    // verifica que pos-punto este al final de su produccion
            {
                if(chart[i]->referencia_int == 0)                                   // verifica que su state_refence sea 0
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void EarleyParser::predecir()
{
    vector <string> vec;
    int pos = chart[chart.size()-1]->pos_punto;
    string tmp;
    tmp = chart[chart.size()-1]->produccion_actual.second[pos]->etiqueta;

    vec.push_back(tmp);
    int pos_vec=0;

    while(true)  // ITERA POR EL VEC
    {
        for(int i=0; i<P.size(); i++)  // ITERA POR LAS PRODUCCIONES DE LA GRAMATICA
        {
            if(P[i].first->etiqueta == vec[pos_vec])   // COMPARA LA LETRA EN LA POS_PUNTO CON LA GRAMATICA
            {
                EarleyState* oEarley = new EarleyState(P[i], 0, pos_ch, estado_ch, estado_ch, "---"); //1° estado_ch = estado actual; 2° = ref
                chart.push_back(oEarley);
                pos_ch++;
                if(!busquedaString(P[i].second[0]->etiqueta, vec))
                {
                    vec.push_back(P[i].second[0]->etiqueta);
                }
            }
        }
        pos_vec++;
        if(pos_vec == vec.size())
            break;
    }

}

void EarleyParser::escanear()
{
    estado_ch++;
    string palabraTest = expresion[0];

    for(int i=0; i<chart.size(); i++) // itera en el chart
    {
        if(chart[i]->estado_chart == estado_ch-1)
        {
            int pos = chart[i]->pos_punto;
            if(pos!=chart[i]->produccion_actual.second.size())
            {
                string tmp = chart[i]->produccion_actual.second[pos]->etiqueta;
                if(palabraTest == tmp)
                {
                    // itera en los contextos del first
                    for(int j=0; j<chart[i]->produccion_actual.first->contextos.size(); j++)
                    {
                        string etN = chart[i]->produccion_actual.first->etiqueta;
                        string etC = chart[i]->produccion_actual.first->contextos[j]->etiqueta;
                        string valC = chart[i]->produccion_actual.first->contextos[j]->valor;

                        actualizarUnContexto(etN, etC, valC);
                        actualizarContextosRec();
                    }

                    EarleyState* oEarley = new EarleyState(chart[i]->produccion_actual,
                                                pos + 1, pos_ch, estado_ch, chart[i]->referencia_int, "---");

                    chart.push_back(oEarley);
                    pos_ch++;
                }
            }
        }
    }

    expresion.erase(expresion.begin());

}

void EarleyParser::completar()
{

    // hay que ver si hace un paso de mas corregir
    vector <EarleyState*> vec;
    vec.push_back(chart[chart.size()-1]);

    int pos_vec = 0;
    int var_ref;
    string first_busqueda; // extrae palabra del vec

    for(int i = 0; i < chart.size(); i++) //itera por el chart
    {
        first_busqueda = vec[pos_vec]->produccion_actual.first->etiqueta;
        var_ref        = vec[pos_vec]->referencia_int;

        int pos_tmp = chart[i]->pos_punto;

        if(pos_tmp!= chart[i]->produccion_actual.second.size())
        {
            string palabra_actual = chart[i]->produccion_actual.second[pos_tmp]->etiqueta;

            if(chart[i]->estado_chart == var_ref && palabra_actual == first_busqueda)
            {
                EarleyState* oEarley = new EarleyState(chart[i]->produccion_actual,
                                                       pos_tmp + 1, pos_ch, estado_ch, chart[i]->referencia_int, "---");
                chart.push_back(oEarley);
                pos_ch++;

                if( !busquedaString(chart[i]->produccion_actual.first->etiqueta, vec) )
                {
                    vec.push_back(chart[i]);
                }
            }
        }
    }
    pos_vec++;
}


int main()
{
    vector <string> entrada;
    entrada = lecturaParrafo("entradacontexto.txt");

    EarleyParser earley(entrada);


    return 0;
}
