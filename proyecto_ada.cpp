#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
using namespace std;

//ABRE Y SEPARA EL PARRAFO EN PALABRAS
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

struct Produccion
{
    // Ej. aAb -> aBb + aAcb
    string first; // parte izquierda
    string second; // parte derecha
    Produccion(string f, string s)
    {
        first = f;
        second = s;
    };
    Produccion()
    {
        first = "";
        second = "";
    };

    void imprimir();
};

void Produccion::imprimir()
{
    cout<<"[f]"<<first<<" "<<"[s]"<<second;
}

class EarleyState
{
    Produccion produccion_actual; // ej. Verb -> Sust + Adj
    int pos_punto;
    int pos_chart;
    int estado_chart;
    int referencia_int; // profe dijo hacerlo puntero y entero
    EarleyState* referencia_ptr;
public:
    EarleyState(Produccion, int, int, int, int);
    friend class EarleyParser;
    friend bool busquedaChar(string, vector <EarleyState*>);
    friend void imprimirVector(vector <EarleyState*>);
};

EarleyState::EarleyState(Produccion prod, int pp, int pc, int ec, int r)
{
    produccion_actual.first = prod.first;
    produccion_actual.second = prod.second;
    pos_punto = pp;     // pos del punto en la expresion
    pos_chart = pc;     // linea del chart actual
    estado_chart = ec;  // 0(inicio,predecir), 1(escanear,completar), 2(escanear,predecir), 3(... )
    referencia_int = r; // de donde viene la produccion actual
}

//IMPRIME VECTOR
void imprimirVector(vector <EarleyState*> vectorsote)
{
    for(int i=0; i<vectorsote.size(); i++)
    {
        cout<<vectorsote[i]->produccion_actual.first<<" ";
    }
}


class EarleyParser
{
    vector<Produccion> P;   // vector de producciones
    vector<string> SNT;     // simbolos no terminales
    vector<string> ST;      // simbolos terminales
    string inicial;         // estado inicial
    string expresion;
    int pos_ch;
    int estado_ch;

    vector<EarleyState*> chart; // Chart es un arreglo de EarlyState
    // (...)
public:
    EarleyParser(vector <string>);
    void recibirEntrada(vector<string>);
    void imprimirChart();
    void predecir();
    void completar();
    void escanear();
    bool evaluacion_final();

};

EarleyParser::EarleyParser(vector <string> entrada)
{
    recibirEntrada(entrada);

    pos_ch = 0;         // inicio pos_chart
    estado_ch = 0;  // inicio estado_chart
    for(int i = 0; i < P.size(); i++)
    {
        if (P[i].first == inicial)
        {
            EarleyState* oEarley = new EarleyState(P[i], 0, pos_ch, estado_ch, 0);
            chart.push_back(oEarley);
            pos_ch++;
        }
    }

    while(true)
    {
        predecir();
        if(expresion.size() == 0)
        {
            imprimirChart();
            if(evaluacion_final())
            {
                cout<<"\nSi pertenece a la gramatica."<<endl;
            }
            else
            {
                cout<<"\nNo pertenece a la gramatica."<<endl;
            }
            break;
        }
        escanear();
        completar();
        if(expresion.size() == 0)
        {
            imprimirChart();
            if(evaluacion_final())
            {
                cout<<"\nSi pertenece a la gramatica."<<endl;
            }
            else
            {
                cout<<"\nNo pertenece a la gramatica."<<endl;
            }
            break;
        }
        escanear();
    }

}

bool EarleyParser::evaluacion_final()
{
    for(int i=0; i<chart.size(); i++)
    {
        if(chart[i]->estado_chart == estado_ch-1)  //buscar en el ultimo estado
        {
            if(chart[i]->pos_punto == chart[i]->produccion_actual.second.size())  //verifica que pos-punto este al final de su produccion
            {
                if(chart[i]->referencia_int == 0)  //verifica que su staterefence sea 0
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void EarleyParser::recibirEntrada(vector<string> entrada)
{
    expresion = entrada[0];
    inicial = entrada[1];
    string palabra;
    for(int i=0; i<entrada[2].size(); i++)
    {
        if(entrada[2][i]!=',')
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
        if(entrada[3][i]!=',')
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
    for(int i = 5; i < fin; i++)
    {
        Produccion* tmp = new Produccion;
        for(int j=0; j<entrada[i].size(); j++)
        {
            if(entrada[i][j]!= '=')
            {
                palabra.push_back(entrada[i][j]);
            }
            else
            {
                tmp->first = palabra;
                palabra.clear();
            }
        }
        tmp->second = palabra;
        P.push_back(*tmp);
        palabra.clear();

    }

    cout<<"DATOS RECIBIDOS"<<endl;
    cout<<"Expresion      : "<<expresion<<endl;
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
        P[i].imprimir();
        cout << endl;
    }
    cout << endl;
}

void EarleyParser::imprimirChart()
{
    cout << "============== CHART ==============" << endl;
    cout << "pos_chart, produc_actual, pos_punto, estado, referencia\n\n";
    for(int i = 0; i < chart.size(); i++)
    {
        cout << chart[i]->pos_chart << ", ";
        chart[i]->produccion_actual.imprimir();
        cout << ", " << chart[i]->pos_punto << ", " << chart[i]->estado_chart << ", " << chart[i]->referencia_int << endl;
    }
}

bool busquedaChar(string letra, vector <string> vec)
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

bool busquedaChar(string letra, vector <EarleyState*> vec)
{
    for(int i=0; i<vec.size(); i++)
    {
        if(vec[i]->produccion_actual.first == letra)
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

void EarleyParser::predecir()
{
    vector <string> vec;
    int pos = chart[chart.size()-1]->pos_punto;
    string tmp;
    tmp.push_back(chart[chart.size()-1]->produccion_actual.second[pos]);

    vec.push_back(tmp);
    int pos_vec=0;

    while(true)  // ITERA POR EL VEC
    {
        for(int i=0; i<P.size(); i++)  // ITERA POR LAS PRODUCCIONES DE LA GRAMATICA
        {
            if(P[i].first == vec[pos_vec])   // COMPARA LA LETRA EN LA POS_PUNTO CON LA GRAMATICA
            {
                EarleyState* oEarley = new EarleyState(P[i], 0, pos_ch, estado_ch, estado_ch); //1� estado_ch = estado actual; 2� = ref
                chart.push_back(oEarley);
                pos_ch++;
                if(!busquedaChar(char_to_string(P[i].second[0]), vec))
                {
                    vec.push_back(char_to_string(P[i].second[0]));
                }
            }
        }
        pos_vec++;
        if(pos_vec == vec.size())
            break;
    }

    estado_ch++;
}

void EarleyParser::escanear()
{
    char letraTest = expresion[0];

    for(int i=0; i<chart.size(); i++)
    {
        if(chart[i]->estado_chart == estado_ch-1)
        {
            int pos = chart[i]->pos_punto;
            char tmp = chart[i]->produccion_actual.second[pos];
            if(letraTest == tmp)
            {
                EarleyState* oEarley = new EarleyState(chart[i]->produccion_actual, pos + 1, pos_ch, estado_ch, chart[i]->referencia_int);
                chart.push_back(oEarley);
                pos_ch++;
            }
        }
    }
    expresion.erase(0,1);
}

void EarleyParser::completar()
{
    vector <EarleyState*> vec;
    vec.push_back(chart[chart.size()-1]);

    int pos_vec = 0;
    int var_ref;
    string first_busqueda; // extrae letra del vec

    while(true)  // itera por el vector
    {
        first_busqueda = vec[pos_vec]->produccion_actual.first;
        var_ref        = vec[pos_vec]->referencia_int;
        for(int i = 0; i < chart.size(); i++) //itera por el chart
        {
            int pos_tmp = chart[i]->pos_punto;

            // letra de la produccion en la posicion pos_punto
            string letra_actual = char_to_string(chart[i]->produccion_actual.second[pos_tmp]);

            if(chart[i]->estado_chart == var_ref && letra_actual == first_busqueda)
            {
                EarleyState* oEarley = new EarleyState(chart[i]->produccion_actual, pos_tmp + 1, pos_ch, estado_ch, chart[i]->referencia_int);
                chart.push_back(oEarley);
                pos_ch++;

                if(!busquedaChar(chart[i]->produccion_actual.first, vec))
                {
                    vec.push_back(chart[i]);
                }
            }
        }
        pos_vec++;

        if(pos_vec == vec.size())
            break;
    }
    estado_ch++;
}

int main()
{
    vector <string> entrada;
    entrada = lecturaParrafo("entrada.txt");

    EarleyParser earley(entrada);

    return 0;
}
