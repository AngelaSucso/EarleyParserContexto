#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
using namespace std;

//ABRE Y SEPARA EL PARRAFO EN PALABRAS
vector <string> lecturaParrafo(string dir){
    string v;
    vector <string> palabras;
    ifstream f;
    f.open(dir);
    while(!f.eof()){
//        f>>v;
        getline(f, v); //toda la linea
        palabras.push_back(v);
        v="";
    }
//    palabras.pop_back();
    f.close();

    return palabras;
}

//IMPRIME VECTOR
void imprimirVector(vector <string> vectorsote){
    for(int i=0; i<vectorsote.size(); i++){
        cout<<vectorsote[i]<<" ";
    }
}

struct Produccion
{
    // Ej. aAb -> aBb + aAcb
    string first; // parte izquierda
    string second; // parte derecha
    Produccion(string f, string s){
        first = f;
        second = s;
    };
    Produccion(){
        first = "";
        second = "";
    };

    void imprimir();
};

void Produccion::imprimir(){
    cout<<"[f]"<<first<<"   "<<"[s]"<<second<<endl;
}

class EarleyState
{
    Produccion produccion_actual; // ej. Verb -> Sust + Adj
    int pos_punto;
    int pos_chart;  // dudas pa'que?
    int estado_chart;
    int referencia_int; // profe dijo hacerlo puntero y entero
    EarleyState* referencia_ptr;
public:
    EarleyState(Produccion, int, int, int, int);
};

EarleyState::EarleyState(Produccion prod, int pp, int pc, int ec, int r)
{
    produccion_actual.first = prod.first;
    produccion_actual.second = prod.second;
    pos_punto = pp;
    pos_chart = pc;
    estado_chart = ec;
    referencia_int = r;
}

class EarleyParser
{
    vector<Produccion> P;
    vector<string> SNT; // simbolos no terminales
    vector<string> ST;  // simbolos terminales
    string Inicial;
    string expresion;

    vector<EarleyState*> chart; // Chart es un arreglo de EarlyState
    // (...)
public:
    EarleyParser(vector <string>);
};

EarleyParser::EarleyParser(vector <string> entrada){
    expresion = entrada[0];
    Inicial = entrada[1];
    string lexema;
    for(int i=0; i<entrada[2].size(); i++){
        if(entrada[2][i]!=','){
//            cout<<"if "<<entrada[2][i]<<endl;
            lexema.push_back(entrada[2][i]);
//            cout<<"lexema: "<<lexema<<endl;
        }
        else{
            SNT.push_back(lexema);
            lexema.clear();
        }
    }
    SNT.push_back(lexema);    lexema.clear();

    for(int i=0; i<entrada[3].size(); i++){
        if(entrada[3][i]!=','){
            lexema.push_back(entrada[3][i]);
        }
        else{
            ST.push_back(lexema);
            lexema.clear();
        }
    }
    ST.push_back(lexema);
    lexema.clear();

    int cant = atoi(entrada[4].c_str());
    int fin = 5 + cant;
    for(int i = 5; i < fin; i++){
        Produccion* tmp = new Produccion;
        for(int j=0; j<entrada[i].size(); j++){
            if(entrada[i][j]!= '='){
                lexema.push_back(entrada[i][j]);
            }
            else{
                tmp->first = lexema;
                lexema.clear();
            }
        }
        tmp->second = lexema;
        P.push_back(*tmp);
        lexema.clear();

    }

    cout<<"DATOS RECIBIDOS"<<endl;
    cout<<"Expresion      : "<<expresion<<endl;
    cout<<"Estado Inicial : "<<Inicial<<endl;
    cout<<"No Terminales  : ";
    imprimirVector(SNT);
    cout<<endl;
    cout<<"Terminales     : ";
    imprimirVector(ST);
    cout<<endl<<endl;
    cout<<"PRODUCCIONES"<<endl;
    for(int i=0; i<P.size(); i++){
        P[i].imprimir();
    }
}

// hacer el input - identificar la gramatica y la expresion a evaluar
int main()
{
    vector <string> entrada;
    entrada = lecturaParrafo("entrada.txt");

    EarleyParser earley(entrada);

    return 0;
}
